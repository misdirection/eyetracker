#pragma once
#include "eyeTracker.h"
#include <windows.h>
#include <dshow.h>

#pragma comment(lib, "strmiids")

DeviceInformation::DeviceInformation(void)
{
	getData();
}


DeviceInformation::~DeviceInformation(void)
{
}


HRESULT DeviceInformation::EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,  
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}

void DeviceInformation::DisplayDeviceInformation(IEnumMoniker *pEnum)
{
    IMoniker *pMoniker = NULL;

    while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag *pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;  
        } 

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if (SUCCEEDED(hr))
        {
			wstring ws(var.bstrVal, SysStringLen(var.bstrVal));
			string s( ws.begin(), ws.end() );
			_name.push_back(s);
            VariantClear(&var); 
        }

        hr = pPropBag->Write(L"FriendlyName", &var);

        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if (SUCCEEDED(hr))
        {
            // The device path is not intended for display.
 			wstring ws(var.bstrVal, SysStringLen(var.bstrVal));
			string s( ws.begin(), ws.end() );
			_path.push_back(s);
           VariantClear(&var); 
        }

        pPropBag->Release();
        pMoniker->Release();
    }
}

void DeviceInformation::getData()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;

        hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            DisplayDeviceInformation(pEnum);
            pEnum->Release();
        }
        CoUninitialize();
    }
}

string DeviceInformation::getName(int i)
{
	return _name[i];
}

string DeviceInformation::getPath(int i)
{
	return _path[i];
}
