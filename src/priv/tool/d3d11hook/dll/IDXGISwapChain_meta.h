// script generated file. DO NOT edit.

// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, Present, PARAMETER_LIST_2(
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, SyncInterval),
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, Flags)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, GetBuffer, PARAMETER_LIST_3(
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, Buffer),
    DEFINE_METHOD_PARAMETER(_In_  REFIID, riid),
    DEFINE_METHOD_PARAMETER(_Out_  void **, ppSurface)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, SetFullscreenState, PARAMETER_LIST_2(
    DEFINE_METHOD_PARAMETER(/* [in] */ BOOL, Fullscreen),
    DEFINE_METHOD_PARAMETER(_In_opt_  IDXGIOutput *, pTarget)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, GetFullscreenState, PARAMETER_LIST_2(
    DEFINE_METHOD_PARAMETER(_Out_opt_  BOOL *, pFullscreen),
    DEFINE_METHOD_PARAMETER(_Out_opt_  IDXGIOutput **, ppTarget)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, GetDesc, PARAMETER_LIST_1(
    DEFINE_METHOD_PARAMETER(_Out_  DXGI_SWAP_CHAIN_DESC *, pDesc)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, ResizeBuffers, PARAMETER_LIST_5(
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, BufferCount),
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, Width),
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, Height),
    DEFINE_METHOD_PARAMETER(/* [in] */ DXGI_FORMAT, NewFormat),
    DEFINE_METHOD_PARAMETER(/* [in] */ UINT, SwapChainFlags)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, ResizeTarget, PARAMETER_LIST_1(
    DEFINE_METHOD_PARAMETER(_In_  const DXGI_MODE_DESC *, pNewTargetParameters)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, GetContainingOutput, PARAMETER_LIST_1(
    DEFINE_METHOD_PARAMETER(_Out_  IDXGIOutput **, ppOutput)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, GetFrameStatistics, PARAMETER_LIST_1(
    DEFINE_METHOD_PARAMETER(_Out_  DXGI_FRAME_STATISTICS *, pStats)))
// -----------------------------------------------------------------------------
DEFINE_INTERFACE_METHOD(virtual, HRESULT, STDMETHODCALLTYPE, GetLastPresentCount, PARAMETER_LIST_1(
    DEFINE_METHOD_PARAMETER(_Out_  UINT *, pLastPresentCount)))
