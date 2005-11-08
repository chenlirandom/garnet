#include "pch.h"
#include "basicRenderer.h"
#include "ntRenderWindow.h"
#include "xRenderWindow.h"
#include <limits.h>


// ****************************************************************************
// local function
// ****************************************************************************

//!
//! Determine monitor handle that render window should stay in.
// ----------------------------------------------------------------------------
static void *
sDetermineMonitorHandle( const GN::gfx::UserOptions & uo )
{
    GN_GUARD;

    if( 0 == uo.monitorHandle )
    {
#if GN_MSWIN
        HMONITOR monitor;
        if( !::IsWindow( (HWND)uo.parentWindow ) )
        {
            POINT pt = { LONG_MIN, LONG_MIN }; // Make sure primary monitor are returned.
            monitor = ::MonitorFromPoint( pt, MONITOR_DEFAULTTOPRIMARY );
            if( 0 == monitor )
            {
                GN_ERROR( "Fail to get primary monitor handle." );
                return 0;
            }
        }
        else
        {
            monitor = ::MonitorFromWindow( (HWND)uo.renderWindow, MONITOR_DEFAULTTONEAREST );
        }
        GN_ASSERT( monitor );
        return monitor;
#else
        // TODO: not implemented, use hack value.
        return (void*)1;
#endif
    }
    else return uo.monitorHandle;

    GN_UNGUARD;
}

//!
//! get current display mode
// ----------------------------------------------------------------------------
static bool
sGetCurrentDisplayMode( const GN::gfx::UserOptions & uo, GN::gfx::DisplayMode & dm )
{
    GN_GUARD;

    // determine the monitor
    void * monitor = sDetermineMonitorHandle( uo );
    if( 0 == monitor ) return false;

#if GN_MSWIN

    MONITORINFOEXA mi;
    DEVMODEA windm;

    mi.cbSize = sizeof(mi);
    windm.dmSize = sizeof(windm);
    windm.dmDriverExtra = 0;

    GN_MSW_CHECK_RV( ::GetMonitorInfoA( (HMONITOR)monitor, &mi ), false );
    GN_MSW_CHECK_RV( ::EnumDisplaySettingsA( mi.szDevice, ENUM_CURRENT_SETTINGS, &windm ), false );

    GN_ASSERT( (uint32_t) ( mi.rcMonitor.right - mi.rcMonitor.left ) == windm.dmPelsWidth );
    GN_ASSERT( (uint32_t) (mi.rcMonitor.bottom - mi.rcMonitor.top ) == windm.dmPelsHeight );

    dm.width = windm.dmPelsWidth;
    dm.height = windm.dmPelsHeight;
    dm.depth = windm.dmBitsPerPel;
    dm.refrate = windm.dmDisplayFrequency;

#else

    Screen * scr;
    if( (void*)1 == monitor )
    {
        GN_WARN( "No implementation. Use hard-coded value: 640x480." );
        dm.width = 640;
        dm.height = 480;
        dm.depth = 32;
        dm.refrate = 0;
    }
    else
    {
        scr = (Screen*)monitor;
        dm.width = scr->width;
        dm.height = scr->height;
        dm.depth = scr->root_depth;
        dm.refrate = 0;
    }

#endif

    // success
    return true;

    GN_UNGUARD;
}

//!
//! Determine render window size
// ----------------------------------------------------------------------------
static bool
sDetermineWindowSize(
    const GN::gfx::UserOptions & uo,
    const GN::gfx::DisplayMode & currentDisplayMode,
    uint32_t & width,
    uint32_t & height )
{
    GN_GUARD;

    if( uo.useExternalWindow )
    {
        return GN::win::getClientSize( uo.displayHandle, uo.renderWindow, &width, &height );
    }
    else
    {
        if( uo.fullscreen )
        {
            // In fullscreen mode, default window size is determined by current display mode.
            const GN::gfx::DisplayMode & dm = uo.displayMode;
            width = dm.width ? dm.width : currentDisplayMode.width;
            height = dm.height ? dm.height : currentDisplayMode.height;
        }
        else
        {
            // In windowed mode, default window size is 640x480
            width = uo.windowedWidth ? uo.windowedWidth : 640;
            height = uo.windowedHeight ? uo.windowedHeight : 480;
        }

        // success
        return true;
    }

    GN_UNGUARD;
}

// ****************************************************************************
// protected functions
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
bool
GN::gfx::BasicRenderer::processUserOptions( const UserOptions & uo )
{
    GN_GUARD;

    DispDesc desc;

    // setup display mode
    DisplayMode dm;
    if( !sGetCurrentDisplayMode( uo, dm ) ) return false;
    if( uo.fullscreen )
    {
        desc.width = (0==uo.displayMode.width) ? dm.width : uo.displayMode.width;
        desc.height = (0==uo.displayMode.height) ? dm.height : uo.displayMode.height;
        desc.depth = (0==uo.displayMode.depth) ? dm.depth : uo.displayMode.depth;
        desc.refrate = (0==uo.displayMode.refrate) ? dm.refrate : uo.displayMode.refrate;
    }
    else
    {
        uint32_t w, h;
        if( !sDetermineWindowSize( uo, dm, w, h ) ) return false;
        desc.width = (0==uo.displayMode.width) ? w : uo.displayMode.width;
        desc.height = (0==uo.displayMode.height) ? h : uo.displayMode.height;
        desc.depth = dm.depth;
        desc.refrate = 0;
    }
    GN_ASSERT( desc.width && desc.height && desc.depth );

    // (Re)Initialize render window
#if GN_MSWIN
    if( getUserOptions().fullscreen && !uo.fullscreen ) mWinProp.restore();
#endif
    if( uo.useExternalWindow )
    {
        if( !mWindow.initExternalRenderWindow( uo.displayHandle, uo.renderWindow ) ) return false;
    }
    else
    {
        if( !mWindow.initInternalRenderWindow( uo.displayHandle, uo.parentWindow, desc.width, desc.height ) ) return false;
    }
#if GN_MSWIN
    if( !uo.fullscreen && !mWinProp.save( mWindow.getWindow() ) ) return false;
#endif
    desc.displayHandle = mWindow.getDisplay();
    desc.windowHandle = mWindow.getWindow();
    desc.monitorHandle = mWindow.getMonitor();
#if GN_POSIX
    GN_ASSERT( desc.displayHandle );
#endif
    GN_ASSERT( desc.windowHandle && desc.monitorHandle );

    // success
    setUserOptions( uo );
    setDispDesc( desc );
    return true;

    GN_UNGUARD;
}

//
//
// ----------------------------------------------------------------------------
bool
GN::gfx::BasicRenderer::handleRenderWindowSizeMove()
{
    GN_GUARD;

    // handle render window size move
    const UserOptions & uo = getUserOptions();
    if( !uo.fullscreen && // only when we're in windowed mode
        uo.autoBackbufferResizing &&
        mWindow.getSizeChangeFlag() )
    {
        UserOptions newOptions = uo;
        newOptions.monitorHandle = mWindow.getMonitor();
        mWindow.getClientSize( newOptions.windowedWidth, newOptions.windowedHeight );
        if( !changeUserOptions( newOptions, false ) )
        {
            GNGFX_FATAL( "Fail to respond to render window size and position change!" );
            return false;
        }
    }

    // success
    return true;

    GN_UNGUARD;
}
