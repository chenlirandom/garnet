#include "pch.h"
#include "windowX11.h"

#if GN_POSIX

#include <X11/Xlib.h>
#include <GL/glx.h>

static GN::Logger * sLogger = GN::getLogger("GN.core.win");

// *****************************************************************************
// local functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
static int sXErrorHandler( Display * d, XErrorEvent * e )
{
    GN_GUARD;

    static char buf[4096];
    XGetErrorText( d, e->error_code, buf, 4095 );

    GN_ERROR(sLogger)(
        "X error : %s"
        "   Major opcode of failed request:  %d\n"
        "   Minor opcode of failed request:  %d\n"
        "   Serial number of failed request:  %d\n"
        "   Resource ID:  0x%X",
        buf,
        e->request_code,
        e->minor_code,
        e->serial,
        e->resourceid );

    return 0;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sIsWindow( Display * disp, Window win )
{
    // TODO: use real X function to check window handle.
    return 0 != disp && 0 != win;
}

//
//
// -----------------------------------------------------------------------------
static int sGetScreenNumber( Display * disp, Screen * screen )
{
    GN_ASSERT( disp && screen );

    int n = ScreenCount(disp);
    for( int i = 0; i < n; ++i )
    {
        if( screen == ScreenOfDisplay( disp, i ) ) return i;
    }

    GN_ERROR(sLogger)( "Fail to get screen number out of screen pointer." );
    return -1;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::win::WindowX11::init(const WindowAttachingParameters & wap) {
    GN_GUARD;

    if( !initDisplay( (Display*)wap.display ) ) return false;

    if( !sIsWindow( (Display*)wap.display, (::Window)wap.window ) )
    {
        GN_ERROR(sLogger)( "External render window is invalid!" );
        return false;
    }

    mWindow = (::Window)wap.window;
    mUseExternalWindow = true;

    // success
    GN_ASSERT( mDisplay && mWindow );
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::win::WindowX11::init(const WindowCreationParameters & wcp) {
    GN_GUARD;

    auto display = (Display*)wcp.display;
    auto monitor = (Screen*)wcp.monitor;
    auto parent  = (::Window)wcp.parent;
    auto width = wcp.clientWidth;
    auto height = wcp.clientHeight;

    // remember screen/monitor pointer
    mScreen = monitor;

    // initialize display
    if( !initDisplay(display) ) return false;

    // get screen number
    mScreenNumber = sGetScreenNumber( display, monitor );
    if( mScreenNumber < 0 ) return false;

    // Choose an appropriate visual
    static int attributeList[] = {
        GLX_RGBA, GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        None
    };
    AutoXPtr<XVisualInfo> vi( glXChooseVisual( display, mScreenNumber, attributeList ) );
    if( 0 == vi )
    {
        GN_ERROR(sLogger)( "Cannot find visual with desired attributes." );
        return false;
    }

    // determine parent window
    if( 0 == parent )
    {
        parent = XDefaultRootWindow( display );
        if( 0 == parent )
        {
            GN_ERROR(sLogger)( "Fail to get default root window." );
            return false;
        }
    }

    // create a colormap
    Colormap cmap = XCreateColormap( display, parent, vi->visual, AllocNone );
    if( 0 == cmap )
    {
        GN_ERROR(sLogger)( "Cannot allocate colormap." );
        return false;
    }

    // set window attributes
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | StructureNotifyMask;
    swa.border_pixel = BlackPixel( display, mScreenNumber );
    swa.background_pixel = BlackPixel( display, mScreenNumber );

    // create the render window.
    mWindow = XCreateWindow(
        display,
        parent,
        0, 0, width, height, // position and size
        0, // border
        vi->depth,
        InputOutput,
        vi->visual,
        CWColormap | CWEventMask | CWBorderPixel | CWBackPixel,
        &swa ); // background
    if( 0 == mWindow )
    {
        GN_ERROR(sLogger)( "XCreateWindow() failed." );
        return false;
    }

    // map window
    GN_X_CHECK_RETURN( XSelectInput( display, mWindow, StructureNotifyMask ), false );
    XEvent e;
    XMapWindow( display, mWindow );
    for(;;)
    {
        XNextEvent( display, &e );
        if( e.type == MapNotify && e.xmap.window == mWindow ) break;
    }

    mUseExternalWindow = false;

    // success
    GN_ASSERT( mDisplay && mWindow );
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::win::WindowX11::quit()
{
    GN_GUARD;

    // Destroy window
    if( mWindow && !mUseExternalWindow )
    {
        GN_ASSERT( mDisplay );
        XDestroyWindow( mDisplay, mWindow );
        mWindow = 0;
    }

    // clear display
    mDisplay = 0;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::Vector2<uint32_t> GN::win::WindowX11::getClientSize() const
{
    GN_GUARD;

    GN_ASSERT( sIsWindow( mDisplay, mWindow ) );

    GN::Vector2<uint32_t> size(0, 0);

    XWindowAttributes attr;
    GN_X_CHECK_RETURN( XGetWindowAttributes( mDisplay, mWindow, &attr ), size );

    size.x = (uint32_t)attr.width;
    size.y = (uint32_t)attr.height;

    // success
    return size;

    GN_UNGUARD;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::win::WindowX11::initDisplay( Display * display )
{
    GN_GUARD;

    // store display pointer
    mDisplay = display;

#if GN_ENABLE_DEBUG
    // Trun on synchronous behavior for debug build.
    XSynchronize( mDisplay, true );
#endif

    // update error handler
    XSetErrorHandler( &sXErrorHandler );

    // success
    return true;

    GN_UNGUARD;
}

#endif // GN_POSIX
