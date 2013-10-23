// script generated file. DO NOT edit.

// ==============================================================================
// Constructor / Destructor
// ==============================================================================
private:

DXGIObjectHook & _DXGIObject;
DXGIAdapterHook & _DXGIAdapter;

protected:

DXGIAdapter1Hook(UnknownBase & unknown, DXGIObjectHook & DXGIObject, DXGIAdapterHook & DXGIAdapter, IUnknown * realobj)
    : BASE_CLASS(unknown, realobj)
    , _DXGIObject(DXGIObject)
    , _DXGIAdapter(DXGIAdapter)
{
    Construct(); 
}

~DXGIAdapter1Hook() {}

// ==============================================================================
// Factory Utilities
// ==============================================================================
public:

static IUnknown * sNewInstance(void * context, UnknownBase & unknown, IUnknown * realobj)
{
    UNREFERENCED_PARAMETER(context);

    DXGIObjectHook * DXGIObject = (DXGIObjectHook *)unknown.GetHookedObj(__uuidof(IDXGIObject));
    if (nullptr == DXGIObject) return nullptr;

    DXGIAdapterHook * DXGIAdapter = (DXGIAdapterHook *)unknown.GetHookedObj(__uuidof(IDXGIAdapter));
    if (nullptr == DXGIAdapter) return nullptr;

    try
    {
        IUnknown * result = (UnknownBase*)new DXGIAdapter1Hook(unknown, *DXGIObject, *DXGIAdapter, realobj);
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
HRESULT STDMETHODCALLTYPE EnumOutputs(
    /* [in] */ UINT Output,
    _Out_  IDXGIOutput ** ppOutput)
{
    return _DXGIAdapter.EnumOutputs(Output, ppOutput);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE GetDesc(
    _Out_  DXGI_ADAPTER_DESC * pDesc)
{
    return _DXGIAdapter.GetDesc(pDesc);
}
// -----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(
    _In_  REFGUID InterfaceName,
    _Out_  LARGE_INTEGER * pUMDVersion)
{
    return _DXGIAdapter.CheckInterfaceSupport(InterfaceName, pUMDVersion);
}
// ==============================================================================
// Method Prototypes
// ==============================================================================
public:

// -----------------------------------------------------------------------------
virtual HRESULT STDMETHODCALLTYPE GetDesc1(
    _Out_  DXGI_ADAPTER_DESC1 * pDesc);
NullPtr<void (DXGIAdapter1Hook::*)(_Out_  DXGI_ADAPTER_DESC1 * &)> _GetDesc1_pre_ptr;
NullPtr<void (DXGIAdapter1Hook::*)(HRESULT, _Out_  DXGI_ADAPTER_DESC1 *)> _GetDesc1_post_ptr;

// ==============================================================================
// The End
// ==============================================================================
private: