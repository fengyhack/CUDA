#include <iostream>
#include <Windows.h>
#include <cuda_runtime.h>

using namespace std;

// Keyword: __global__
// Function: A[]+B[] --> C[]
// Where, numElements is the array size of A, B and C
__global__ void vectorAdd(float* A, float* B, float* C, int numElements)
{
	// Each thread(i) does its own work
	int i = blockDim.x * blockIdx.x + threadIdx.x;

	if (i < numElements)
	{
		C[i] = A[i] + B[i];
	}
}

int main(void)
{
	DWORD start = GetTickCount();

	int numElements = 50000;
	size_t size = numElements * sizeof(float);

	cout << "Vector add: A+B-->C, where vector size=" << numElements << endl;

	// Maclloc on the host and randomly initialization
	float *h_A = new float[numElements];
	float *h_B = new float[numElements];
	float *h_C = new float[numElements];
	for (int i = 0; i < numElements; ++i)
	{
		h_A[i] = rand() / (float)RAND_MAX;
		h_B[i] = rand() / (float)RAND_MAX;
	}


	//cudaError err = cudaSuccess;
	//Malloc on the CUDA device
	float *d_A = NULL;
	cudaMalloc((void **)&d_A, size);
	float *d_B = NULL;
	cudaMalloc((void **)&d_B, size);
	float *d_C = NULL;
	cudaMalloc((void **)&d_C, size);

	// COPY A&B: HOST--->CUDA
	cout << "Copy data:  [host memory] ---> [CUDA device]" << endl;
	cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

	// CUDA Compute
	int threadsPerBlock = 256;
	int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;  //¡° ½øÒ»·¨¡±
	cout << "CUDA kernel launch with [" << blocksPerGrid << "(blocks) * "<< threadsPerBlock << "(threads)]." << endl;
	vectorAdd <<<blocksPerGrid, threadsPerBlock >>>(d_A, d_B, d_C, numElements);

	// COPY C: HOST<--CUDA
	cout << "Copy data:  [host memory] <--- [CUDA device] " << endl;
	cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

	// Verify A+B==C
	cout << "Verifying the results [A+B==C] ..." << endl;
	for (int i = 0; i < numElements; ++i)
	{
		if (fabs(h_A[i] + h_B[i] - h_C[i]) > 1e-5)
		{
			cerr << "Result verification failed at element " << i << "!" << endl;
			exit(EXIT_FAILURE);
		}
	}

	cout << "Test PASSED" << endl;

	// Free memory
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
	delete[] h_A;
	delete[] h_B;
	delete[] h_C;

	cudaDeviceReset();

	DWORD finish = GetTickCount();

	cout << "Time taken:" << finish - start << endl;

	system("pause");
	return 0;
}