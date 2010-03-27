#include "pch.h"
#include "d3d10Gpu.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpu.D3D10");

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10Gpu::dispInit()
{
    GN_GUARD;

    const GpuOptions & ro = GetOptions();
    const DispDesc & dd = GetDispDesc();

    UINT flags = D3D10_CREATE_DEVICE_SINGLETHREADED;
    if( ro.debug ) flags |= D3D10_CREATE_DEVICE_DEBUG;

    // setup swap chain descriptor
    GN_CASSERT( D3D10_SDK_VERSION >= 28 );
    DXGI_SWAP_CHAIN_DESC sd;
    ::memset( &sd, 0, sizeof(sd) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = dd.width;
    sd.BufferDesc.Height = dd.height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = (HWND)dd.windowHandle;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = !ro.fullscreen;

    // create device
    GN_DX_CHECK_RETURN(
        D3D10CreateDeviceAndSwapChain(
            mAdapter,
            ro.reference ? D3D10_DRIVER_TYPE_REFERENCE : D3D10_DRIVER_TYPE_HARDWARE,
            NULL, // software module handle
            flags,
            D3D10_SDK_VERSION,
            &sd,
            &mSwapChain,
            &mDevice ),
        false );

    // customize D3D10 debug output
    AutoComPtr<ID3D10InfoQueue> iq;
    if( S_OK == mDevice->QueryInterface( IID_ID3D10InfoQueue, (void**)&iq ) )
    {
        // Break into debugger on D3D errors
		iq->SetBreakOnSeverity( D3D10_MESSAGE_SEVERITY_CORRUPTION, true );
		iq->SetBreakOnSeverity( D3D10_MESSAGE_SEVERITY_ERROR, true );
		iq->SetBreakOnSeverity( D3D10_MESSAGE_SEVERITY_WARNING, false );
		iq->SetBreakOnSeverity( D3D10_MESSAGE_SEVERITY_INFO, false );

        // ignore some expected errors
        D3D10_MESSAGE_ID disabledWarnings[] =
        {
            D3D10_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
            D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
        };
        D3D10_INFO_QUEUE_FILTER filter;
        memset( &filter, 0, sizeof(filter) );
        filter.DenyList.NumIDs  = GN_ARRAY_COUNT( disabledWarnings );
        filter.DenyList.pIDList = disabledWarnings;
        iq->AddStorageFilterEntries( &filter );
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Gpu::dispQuit()
{
    GN_GUARD;

    // switch back to windowed mode, before destroy the swap chain.
    if( mSwapChain && GetOptions().fullscreen )
    {
        mSwapChain->SetFullscreenState( FALSE, NULL );
    }

    SafeRelease( mSwapChain );
    SafeRelease( mDevice );
    SafeRelease( mAdapter );

    GN_UNGUARD;
}
