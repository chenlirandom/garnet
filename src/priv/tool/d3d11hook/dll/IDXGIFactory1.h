// script generated file. DO NOT edit.

// ==============================================================================
// Constructor / Destructor
// ==============================================================================
private:

DXGIObjectHook & _DXGIObject;
DXGIFactoryHook & _DXGIFactory;

protected:

DXGIFactory1Hook(UnknownBase & unknown, DXGIObjectHook & DXGIObject, DXGIFactoryHook & DXGIFactory, IUnknown * realobj)
    : BASE_CLASS(unknown, realobj)
    , _DXGIObject(DXGIObject)
    , _DXGIFactory(DXGIFactory)
{
    Construct(); 
}

~DXGIFactory1Hook() {}

// ==============================================================================
// Factory Utilities
// ==============================================================================
public:

static IUnknown * sNewInstance(void * context, UnknownBase & unknown, IUnknown * realobj)
{
    UNREFERENCED_PARAMETER(context);

    DXGIObjectHook * DXGIObject = (DXGIObjectHook *)unknown.GetHookedObj(__uuidof(IDXGIObject));
    if (nullptr == DXGIObject) return nullptr;

    DXGIFactoryHook * DXGIFactory = (DXGIFactoryHook *)unknown.GetHookedObj(__uuidof(IDXGIFactory));
    if (nullptr == DXGIFactory) return nullptr;

    try
    {
        IUnknown * result = (UnknownBase*)new DXGIFactory1Hook(unknown, *DXGIObject, *DXGIFactory, realobj);
        result->AddRef();
        return result;
    }
    catch(std::bad_alloc&)
    {
        GN_ERROR(GN::getLogger("GN.d3d11hook"))("Out of memory.");
        return nullptr;
    }
}

// ==============================================================================
// Calling to base interfaces
// ==============================================================================
public:

// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE SetPrivateData(
    _In_  REFGUID Name,
    /* [in] */ UINT DataSize,
    _In_reads_bytes_(DataSize)  const void * pData)
{
    return _DXGIObject.SetPrivateData(Name, DataSize, pData);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(
    _In_  REFGUID Name,
    _In_  const IUnknown * pUnknown)
{
    return _DXGIObject.SetPrivateDataInterface(Name, pUnknown);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE GetPrivateData(
    _In_  REFGUID Name,
    _Inout_  UINT * pDataSize,
    _Out_writes_bytes_(*pDataSize)  void * pData)
{
    return _DXGIObject.GetPrivateData(Name, pDataSize, pData);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE GetParent(
    _In_  REFIID riid,
    _Out_  void ** ppParent)
{
    return _DXGIObject.GetParent(riid, ppParent);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE EnumAdapters(
    /* [in] */ UINT Adapter,
    _Out_  IDXGIAdapter ** ppAdapter)
{
    return _DXGIFactory.EnumAdapters(Adapter, ppAdapter);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE MakeWindowAssociation(
    HWND WindowHandle,
    UINT Flags)
{
    return _DXGIFactory.MakeWindowAssociation(WindowHandle, Flags);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE GetWindowAssociation(
    _Out_  HWND * pWindowHandle)
{
    return _DXGIFactory.GetWindowAssociation(pWindowHandle);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CreateSwapChain(
    _In_  IUnknown * pDevice,
    _In_  DXGI_SWAP_CHAIN_DESC * pDesc,
    _Out_  IDXGISwapChain ** ppSwapChain)
{
    return _DXGIFactory.CreateSwapChain(pDevice, pDesc, ppSwapChain);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
    /* [in] */ HMODULE Module,
    _Out_  IDXGIAdapter ** ppAdapter)
{
    return _DXGIFactory.CreateSoftwareAdapter(Module, ppAdapter);
}
// ==============================================================================
// Method Prototypes
// ==============================================================================
public:

// -----------------------------------------------------------------------------
virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
    /* [in] */ UINT Adapter,
    _Out_  IDXGIAdapter1 ** ppAdapter);
NullPtr<void (DXGIFactory1Hook::*)(/* [in] */ UINT &, _Out_  IDXGIAdapter1 ** &)> _EnumAdapters1_pre_ptr;
NullPtr<void (DXGIFactory1Hook::*)(HRESULT, /* [in] */ UINT, _Out_  IDXGIAdapter1 **)> _EnumAdapters1_post_ptr;
// -----------------------------------------------------------------------------
virtual BOOL STDMETHODCALLTYPE IsCurrent();
NullPtr<void (DXGIFactory1Hook::*)()> _IsCurrent_pre_ptr;
NullPtr<void (DXGIFactory1Hook::*)(BOOL)> _IsCurrent_post_ptr;

// ==============================================================================
// The End
// ==============================================================================
private: