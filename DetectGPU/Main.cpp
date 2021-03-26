#include <Windows.h>  
#include <iostream>  
#include <DXGI.h>  
#include <vector>  

using namespace std;

string WStringToString(const wstring& wstr)
{
	string str(wstr.length(), ' ');
	copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

int main()
{
	IDXGIFactory* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	if (FAILED(hr))
	{
		return -1;
	}

	vector <IDXGIAdapter*> adapters;
	IDXGIAdapter* pAdapter = nullptr;
	while (pFactory->EnumAdapters(adapters.size(), &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(pAdapter);
	}

	const int MEGA = 1 << 20;
	const int GIGA = 1 << 30;
	DXGI_ADAPTER_DESC adesc{};
	DXGI_OUTPUT_DESC odesc{};
	IDXGIOutput* pOutput = nullptr;
	vector<IDXGIOutput*> outputs;
	UINT nModes = 0;
	for (auto a : adapters)
	{
		outputs.clear();
		while (a->EnumOutputs(outputs.size(), &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			outputs.push_back(pOutput);
		}
		if (outputs.empty())
		{
			continue;
		}

		a->GetDesc(&adesc);
		cout << "Description: " << WStringToString(adesc.Description) << endl;
		//cout << "  SystemMem: " << (double)adesc.DedicatedSystemMemory / MEGA << "M" << endl;
		if (adesc.DedicatedVideoMemory >= GIGA)
		{
			cout << "   VideoMem: " << (double)adesc.DedicatedVideoMemory / GIGA << " GiB" << endl;
		}
		else if (adesc.DedicatedVideoMemory > 0)
		{
			cout << "   VideoMem: " << (double)adesc.DedicatedVideoMemory / MEGA << " MiB" << endl;
		}
		//cout << "  SharedMem: " << (double)adesc.SharedSystemMemory / MEGA << "M" << endl;

		cout << "    Outputs: " << outputs.size() <<endl;
		for (auto o : outputs)
		{
			o->GetDesc(&odesc);
			cout << " DeviceName: " << WStringToString(odesc.DeviceName) << endl;
			o->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nModes, NULL);
			cout << "      Modes: " << nModes << endl;
			if (nModes > 0)
			{
				DXGI_MODE_DESC* mdescs = new DXGI_MODE_DESC[nModes];
				o->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nModes, mdescs);
				cout << " Resolution: " 
					<< mdescs[nModes - 1].Width 
					<< " * " << mdescs[nModes - 1].Height
					<< " @ "
					<< mdescs[nModes - 1].RefreshRate.Numerator / mdescs[nModes - 1].RefreshRate.Denominator
					<< "Hz" << endl;
			}
			else
			{
				cout << " Resolution: "
					<< odesc.DesktopCoordinates.right - odesc.DesktopCoordinates.left
					<< " * "
					<< odesc.DesktopCoordinates.bottom - odesc.DesktopCoordinates.top
					<< endl;
			}
			cout << endl;
		}
		outputs.clear();
	}

	adapters.clear();
	(void)getchar();
	return 0;

}