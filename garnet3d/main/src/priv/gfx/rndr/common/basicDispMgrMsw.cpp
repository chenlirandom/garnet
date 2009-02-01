#include "pch.h"
#include "basicRenderer.h"
#include "renderWindowMsw.h"

#if GN_MSWIN

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.common.BasicRenderer");

//
//
// -----------------------------------------------------------------------------
static bool
sGetClientSize( GN::HandleType disp, GN::HandleType win, UInt32 * width, UInt32 * height )
{
    GN_GUARD;

#if GN_XENON

    GN_UNEXPECTED(); // program should not reach heare
    GN_UNUSED_PARAM(disp);
    GN_UNUSED_PARAM(win);
    GN_UNUSED_PARAM(width);
    GN_UNUSED_PARAM(height);
    GN_ERROR(sLogger)( "Xenon platform does not support this function" );
    return false;

#elif GN_MSWIN

    GN_UNUSED_PARAM( disp );
    RECT rc;
    GN_MSW_CHECK_RV( ::GetClientRect( (HWND)win, &rc ), false );
    if( width ) *width = (UInt32)(rc.right - rc.left);
    if( height ) *height = (UInt32)(rc.bottom - rc.top);
    return true;

#endif

    GN_UNGUARD;
}

// ****************************************************************************
// local function
// ****************************************************************************

///
/// Determine monitor handle that render window should stay in.
// ----------------------------------------------------------------------------
static GN::HandleType
sDetermineMonitorHandle( const GN::gfx::RendererOptions & ro )
{
    if( 0 == ro.monitorHandle )
    {
#if GN_XENON
        return (GN::HandleType)1;
#else
        HMONITOR monitor;
        if( !::IsWindow( (HWND)ro.parentWindow ) )
        {
            POINT pt = { LONG_MIN, LONG_MIN }; // Make sure primary monitor are returned.
            monitor = ::MonitorFromPoint( pt, MONITOR_DEFAULTTOPRIMARY );
            if( 0 == monitor )
            {
                GN_ERROR(sLogger)( "Fail to get primary monitor handle." );
                return 0;
            }
        }
        else
        {
            monitor = ::MonitorFromWindow( (HWND)ro.renderWindow, MONITOR_DEFAULTTONEAREST );
        }
        GN_ASSERT( monitor );
        return monitor;
#endif
    }
    else return ro.monitorHandle;
}

///
/// get current display mode
// ----------------------------------------------------------------------------
static bool
sGetCurrentDisplayMode(
    const GN::gfx::RendererOptions & ro,
    GN::gfx::DisplayMode & dm )
{
    GN_GUARD;

    // determine the monitor
    GN::HandleType monitor = sDetermineMonitorHandle( ro );
    if( 0 == monitor ) return false;

#if GN_XENON

    XVIDEO_MODE xvm;
    XGetVideoMode( &xvm );
    dm.width = xvm.dwDisplayWidth;
    dm.height = xvm.dwDisplayHeight;
    dm.depth = 32;
    dm.refrate = 0;

#else

    MONITORINFOEXA mi;
    DEVMODEA windm;

    mi.cbSize = sizeof(mi);
    windm.dmSize = sizeof(windm);
    windm.dmDriverExtra = 0;

    GN_MSW_CHECK_RV( ::GetMonitorInfoA( (HMONITOR)monitor, &mi ), false );
    GN_MSW_CHECK_RV( ::EnumDisplaySettingsA( mi.szDevice, ENUM_CURRENT_SETTINGS, &windm ), false );

    GN_ASSERT( (UInt32) ( mi.rcMonitor.right - mi.rcMonitor.left ) == windm.dmPelsWidth );
    GN_ASSERT( (UInt32) (mi.rcMonitor.bottom - mi.rcMonitor.top ) == windm.dmPelsHeight );

    dm.width = windm.dmPelsWidth;
    dm.height = windm.dmPelsHeight;
    dm.depth = windm.dmBitsPerPel;
    dm.refrate = windm.dmDisplayFrequency;

#endif

    // success
    return true;

    GN_UNGUARD;
}

///
/// Determine render window size
// ----------------------------------------------------------------------------
static bool
sDetermineWindowSize(
    const GN::gfx::RendererOptions & ro,
    const GN::gfx::DisplayMode & currentDisplayMode,
    UInt32 & width,
    UInt32 & height )
{
    GN_GUARD;

    if( ro.useExternalWindow )
    {
        return sGetClientSize( ro.displayHandle, ro.renderWindow, &width, &height );
    }
    else
    {
        if( ro.fullscreen )
        {
            // In fullscreen mode, default window size is determined by current display mode.
            const GN::gfx::DisplayMode & dm = ro.displayMode;
            width = dm.width ? dm.width : currentDisplayMode.width;
            height = dm.height ? dm.height : currentDisplayMode.height;
        }
        else
        {
            // In windowed mode, default window size is 640x480
            width = ro.windowedWidth ? ro.windowedWidth : 640;
            height = ro.windowedHeight ? ro.windowedHeight : 480;
        }

        // success
        return true;
    }

    GN_UNGUARD;
}

// ****************************************************************************
// init / quit
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
bool GN::gfx::BasicRenderer::dispInit( const RendererOptions & ro )
{
    DispDesc desc;

    // determine monitor handle
    desc.monitorHandle = sDetermineMonitorHandle( ro );
    if( 0 == desc.monitorHandle ) return false;

    // setup display mode
    DisplayMode dm;
    if( !sGetCurrentDisplayMode( ro, dm ) ) return false;
    if( ro.fullscreen )
    {
        desc.width = (0==ro.displayMode.width) ? dm.width : ro.displayMode.width;
        desc.height = (0==ro.displayMode.height) ? dm.height : ro.displayMode.height;
        desc.depth = (0==ro.displayMode.depth) ? dm.depth : ro.displayMode.depth;
        desc.refrate = (0==ro.displayMode.refrate) ? 0 : ro.displayMode.refrate;
    }
    else
    {
        UInt32 w, h;
        if( !sDetermineWindowSize( ro, dm, w, h ) ) return false;
        desc.width = ro.windowedWidth ? ro.windowedWidth : w;
        desc.height = ro.windowedHeight ? ro.windowedHeight : h;
        desc.depth = dm.depth;
        desc.refrate = 0;
    }
    GN_ASSERT( desc.width && desc.height && desc.depth );

    if( getOptions().fullscreen && !ro.fullscreen ) mWinProp.restore();
    if( ro.useExternalWindow )
    {
        if( !mWindow.initExternalRenderWindow( ro.displayHandle, ro.renderWindow ) ) return false;
    }
    else
    {
        if( !mWindow.initInternalRenderWindow( ro.displayHandle, ro.parentWindow, desc.monitorHandle, desc.width, desc.height ) ) return false;
    }
    if( !ro.fullscreen && !mWinProp.save( mWindow.getWindow() ) ) return false;
    desc.displayHandle = mWindow.getDisplay();
    desc.windowHandle = mWindow.getWindow();

    GN_ASSERT_EX(
        desc.windowHandle && desc.monitorHandle,
        strFormat( "win(0x%X), monitor(0x%X)", desc.windowHandle, desc.monitorHandle ).cptr() );

    // success
    mOptions = ro;
    mDispDesc = desc;
    return true;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::BasicRenderer::dispQuit()
{
    mWindow.quit();
}

// ****************************************************************************
// protected functions
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
void
GN::gfx::BasicRenderer::handleRenderWindowSizeMove()
{
    GN_GUARD;

    // handle render window size move
    const RendererOptions & ro = getOptions();
    if( !ro.fullscreen && // only when we're in windowed mode
        mWindow.getSizeChangeFlag() )
    {
        HandleType monitor = mWindow.getMonitor();
        UInt32 width, height;
        mWindow.getClientSize( width, height );

        const DispDesc & dd = getDispDesc();

        if( dd.width != width || dd.height != height || dd.monitorHandle != monitor )
        {
            sigRendererWindowSizeMove( monitor, width, height );
        }
    }

    GN_UNGUARD;
}

#endif
