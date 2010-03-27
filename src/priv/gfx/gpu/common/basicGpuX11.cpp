#include "pch.h"
#include "basicGpuX11.h"
#include "garnet/GNwin.h"

#if GN_POSIX

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpu.common");

// ****************************************************************************
// local function
// ****************************************************************************

//
//
// -----------------------------------------------------------------------------
static bool
sGetClientSize( Display * disp, Window win, UInt32 * width, UInt32 * height )
{
    GN_GUARD;

    XWindowAttributes attr;
    GN_X_CHECK_RETURN( XGetWindowAttributes( disp, win, &attr ), false );
    if( width ) *width = (UInt32)attr.width;
    if( height ) *height = (UInt32)attr.height;
    return true;

    GN_UNGUARD;
}

///
/// Determine monitor handle that render window should stay in.
// ----------------------------------------------------------------------------
static GN::HandleType
sDetermineMonitorHandle( Display * disp, const GN::gfx::GpuOptions & ro )
{
    if( 0 == ro.monitorHandle )
    {
        Screen * scr = DefaultScreenOfDisplay( disp );
        GN_ASSERT( scr );
        return (GN::HandleType)scr;
    }
    else
    {
        return ro.monitorHandle;
    }
}

///
/// get current display mode
// ----------------------------------------------------------------------------
static bool
sGetCurrentDisplayMode(
    Display                        * disp,
    const GN::gfx::GpuOptions & ro,
    GN::gfx::DisplayMode           & dm )
{
    GN_GUARD;

    // determine the monitor
    GN::HandleType monitor = sDetermineMonitorHandle( disp, ro );
    if( 0 == monitor ) return false;

    if( (void*)1 == monitor )
    {
        GN_WARN(sLogger)( "No valid screen found. Use hard-coded display mode:: 640x480 32bits" );
        dm.width = 640;
        dm.height = 480;
        dm.depth = 32;
        dm.refrate = 0;
    }
    else
    {
        const Screen * scr = (Screen*)monitor;
        dm.width = WidthOfScreen( scr );
        dm.height = HeightOfScreen( scr );
        dm.depth = DefaultDepthOfScreen( scr );
        dm.refrate = 0;
    }

    // success
    return true;

    GN_UNGUARD;
}

///
/// Determine render window size
// ----------------------------------------------------------------------------
static bool
sDetermineWindowSize(
    Display                        * disp,
    const GN::gfx::GpuOptions & ro,
    const GN::gfx::DisplayMode     & currentDisplayMode,
    UInt32                         & width,
    UInt32                         & height )
{
    GN_GUARD;

    if( ro.useExternalWindow )
    {
        return sGetClientSize( disp, (Window)ro.renderWindow, &width, &height );
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

// *****************************************************************************
//                         BasicGpuX11 init / quit functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::BasicGpuX11::Init( const GpuOptions & o )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( BasicGpuX11, (o) );

    // initialize sub-components one by one
    if( !dispInit(o) ) return Failure();

    // success
    return Success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::BasicGpuX11::Quit()
{
    GN_GUARD;

    // shutdown sub-components in reverse sequence
    dispQuit();

    // standard Quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// from Gpu
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::BasicGpuX11::ProcessRenderWindowMessages( bool blockWhileMinimized )
{
    GN::win::ProcessWindowMessages( mDispDesc.windowHandle, blockWhileMinimized );
}

// ****************************************************************************
// from BasicGpu
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
void
GN::gfx::BasicGpuX11::HandleRenderWindowSizeMove()
{
    mWindow.handleSizeMove();
}

// *****************************************************************************
// private function
// *****************************************************************************

//
//
// ----------------------------------------------------------------------------
bool GN::gfx::BasicGpuX11::dispInit( const GpuOptions & ro )
{
    // determine display
    Display * disp;
    if( 0 == ro.displayHandle )
    {
        StrA dispStr = GetEnv("DISPLAY");
        mDefaultDisplay = XOpenDisplay( dispStr.ToRawPtr() );
        if( 0 == mDefaultDisplay )
        {
            GN_ERROR(sLogger)( "Fail to open display '%s'.", dispStr.ToRawPtr() );
            return false;
        }
        disp = mDefaultDisplay;
    }
    else
    {
        disp = (Display*)ro.displayHandle;
    }
    GN_ASSERT( disp );

    DispDesc desc;

    // determine monitor handle
    desc.monitorHandle = sDetermineMonitorHandle( disp, ro );
    if( 0 == desc.monitorHandle ) return false;

    // setup display mode
    DisplayMode dm;
    if( !sGetCurrentDisplayMode( disp, ro, dm ) ) return false;
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
        if( !sDetermineWindowSize( disp, ro, dm, w, h ) ) return false;
        desc.width = ro.windowedWidth ? ro.windowedWidth : w;
        desc.height = ro.windowedHeight ? ro.windowedHeight : h;
        desc.depth = dm.depth;
        desc.refrate = 0;
    }
    GN_ASSERT( desc.width && desc.height && desc.depth );

    // initialize render window
    if( ro.useExternalWindow )
    {
        if( !mWindow.initExternalRenderWindow( this, disp, (Window)ro.renderWindow ) ) return false;
    }
    else
    {
        if( !mWindow.initInternalRenderWindow( this, disp, (Window)ro.parentWindow, (Screen*)desc.monitorHandle, desc.width, desc.height ) ) return false;
    }
    desc.displayHandle = disp;
    desc.windowHandle = (void*)mWindow.getWindow();

    GN_ASSERT_EX(
        desc.windowHandle && desc.monitorHandle,
        StringFormat( "win(0x%X), monitor(0x%X)", desc.windowHandle, desc.monitorHandle ).ToRawPtr() );

    // success
    mOptions = ro;
    mDispDesc = desc;
    return true;}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::BasicGpuX11::dispQuit()
{
    mWindow.Quit();

    // close default display
    if( mDefaultDisplay )
    {
        XCloseDisplay( mDefaultDisplay );
        mDefaultDisplay = 0;
    }
}

#endif
