#include "pch.h"
#include "d3d11hook.h"
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::SetFeatureMask(
    UINT Mask)
{
    if (_SetFeatureMask_pre_ptr._value) { (this->*_SetFeatureMask_pre_ptr._value)(Mask); }
    HRESULT ret = GetRealObj()->SetFeatureMask(Mask);
    if (_SetFeatureMask_post_ptr._value) { (this->*_SetFeatureMask_post_ptr._value)(ret, Mask); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT STDMETHODCALLTYPE D3D11DebugHook::GetFeatureMask()
{
    if (_GetFeatureMask_pre_ptr._value) { (this->*_GetFeatureMask_pre_ptr._value)(); }
    UINT ret = GetRealObj()->GetFeatureMask();
    if (_GetFeatureMask_post_ptr._value) { (this->*_GetFeatureMask_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::SetPresentPerRenderOpDelay(
    UINT Milliseconds)
{
    if (_SetPresentPerRenderOpDelay_pre_ptr._value) { (this->*_SetPresentPerRenderOpDelay_pre_ptr._value)(Milliseconds); }
    HRESULT ret = GetRealObj()->SetPresentPerRenderOpDelay(Milliseconds);
    if (_SetPresentPerRenderOpDelay_post_ptr._value) { (this->*_SetPresentPerRenderOpDelay_post_ptr._value)(ret, Milliseconds); }
    return ret;
}

// -----------------------------------------------------------------------------
UINT STDMETHODCALLTYPE D3D11DebugHook::GetPresentPerRenderOpDelay()
{
    if (_GetPresentPerRenderOpDelay_pre_ptr._value) { (this->*_GetPresentPerRenderOpDelay_pre_ptr._value)(); }
    UINT ret = GetRealObj()->GetPresentPerRenderOpDelay();
    if (_GetPresentPerRenderOpDelay_post_ptr._value) { (this->*_GetPresentPerRenderOpDelay_post_ptr._value)(ret); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::SetSwapChain(
    IDXGISwapChain * pSwapChain)
{
    if (_SetSwapChain_pre_ptr._value) { (this->*_SetSwapChain_pre_ptr._value)(pSwapChain); }
    HRESULT ret = GetRealObj()->SetSwapChain(pSwapChain);
    if (_SetSwapChain_post_ptr._value) { (this->*_SetSwapChain_post_ptr._value)(ret, pSwapChain); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::GetSwapChain(
    IDXGISwapChain ** ppSwapChain)
{
    if (_GetSwapChain_pre_ptr._value) { (this->*_GetSwapChain_pre_ptr._value)(ppSwapChain); }
    HRESULT ret = GetRealObj()->GetSwapChain(ppSwapChain);
    if (_GetSwapChain_post_ptr._value) { (this->*_GetSwapChain_post_ptr._value)(ret, ppSwapChain); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::ValidateContext(
    ID3D11DeviceContext * pContext)
{
    if (_ValidateContext_pre_ptr._value) { (this->*_ValidateContext_pre_ptr._value)(pContext); }
    HRESULT ret = GetRealObj()->ValidateContext(pContext);
    if (_ValidateContext_post_ptr._value) { (this->*_ValidateContext_post_ptr._value)(ret, pContext); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::ReportLiveDeviceObjects(
    D3D11_RLDO_FLAGS Flags)
{
    if (_ReportLiveDeviceObjects_pre_ptr._value) { (this->*_ReportLiveDeviceObjects_pre_ptr._value)(Flags); }
    HRESULT ret = GetRealObj()->ReportLiveDeviceObjects(Flags);
    if (_ReportLiveDeviceObjects_post_ptr._value) { (this->*_ReportLiveDeviceObjects_post_ptr._value)(ret, Flags); }
    return ret;
}

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE D3D11DebugHook::ValidateContextForDispatch(
    ID3D11DeviceContext * pContext)
{
    if (_ValidateContextForDispatch_pre_ptr._value) { (this->*_ValidateContextForDispatch_pre_ptr._value)(pContext); }
    HRESULT ret = GetRealObj()->ValidateContextForDispatch(pContext);
    if (_ValidateContextForDispatch_post_ptr._value) { (this->*_ValidateContextForDispatch_post_ptr._value)(ret, pContext); }
    return ret;
}

