#include "pch.h"
#include "oglRenderer.h"

#if GN_POSIX

// ****************************************************************************
// local functions
// ****************************************************************************

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::dispDeviceCreate()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    GN_ASSERT( !mRenderContext );

    Display * disp = (Display*)getDispDesc().displayHandle;
    Window win = (Window)getDispDesc().windowHandle;
    GN_ASSERT( disp && win );

    int scr = getScreenNumberOfWindow( disp, win );
    if( scr < 0 ) return false;

    // get window attributes
    XWindowAttributes wa;
    GN_X_CHECK_RV( XGetWindowAttributes( disp, win, &wa ), false );

    // fill VisualInfo structure of the window
    XVisualInfo vi;
    vi.visual = wa.visual;
    vi.visualid = wa.visual->visualid;
    vi.screen = scr;
    vi.depth = wa.depth;
    vi.c_class = wa.visual->c_class;
    vi.red_mask = wa.visual->red_mask;
    vi.green_mask = wa.visual->green_mask;
    vi.blue_mask = wa.visual->blue_mask;
    vi.colormap_size = wa.visual->map_entries;
    vi.bits_per_rgb = wa.visual->bits_per_rgb;

    // create a GLX context
    mRenderContext = glXCreateContext( disp, &vi, 0, GL_FALSE );
    if( 0 == mRenderContext )
    {
        GNGFX_ERROR( "Fail to create GLX context." );
        return false;
    }

    // make the context as current render context.
    if( !glXMakeCurrent( disp, win, mRenderContext ) )
    {
        GNGFX_ERROR( "glXMakeCurrent() failed." );
        return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::dispDeviceRestore()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    GN_ASSERT( mRenderContext );

    // successful
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::dispDeviceDispose()
{
    _GNGFX_DEVICE_TRACE();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::dispDeviceDestroy()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    if( mRenderContext )
    {
        Display * disp = (Display*)getDispDesc().displayHandle;
        GN_ASSERT( disp );
        glXMakeCurrent( disp, 0, 0 );
        glXDestroyContext( disp, mRenderContext );
        mRenderContext = 0;
    }

    GN_UNGUARD;
}

#endif // GN_POSIX
