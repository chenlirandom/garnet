// script generated file. DO NOT edit.

#include "pch.h"
#include "hooks.h"

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE DXGIDeviceSubObjectHook::GetDevice(
    _In_  REFIID riid,
    _Out_  void ** ppDevice)
{
    calltrace::AutoTrace trace(L"DXGIDeviceSubObjectHook::GetDevice");
    if (_GetDevice_pre_ptr._value) { (this->*_GetDevice_pre_ptr._value)(riid, ppDevice); }
    HRESULT ret = GetRealObj()->GetDevice(riid, ppDevice);
    if (_GetDevice_post_ptr._value) { (this->*_GetDevice_post_ptr._value)(ret, riid, ppDevice); }
    return ret;
}

