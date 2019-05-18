#include "pch.h"
#include "oglGpu.h"

#if GN_WINPC

using namespace GN;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.gpu.OGL");

// ****************************************************************************
// local functions
// ****************************************************************************

//
//
// ------------------------------------------------------------------------
static int sChoosePixelFormat( HDC hdc )
{
    GN_GUARD;

    PIXELFORMATDESCRIPTOR pfd;

    DWORD required_flags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER   ;

    // flags that can not exist
    DWORD xxx_flags = PFD_NEED_PALETTE; // we're aiming for a RGB device

    GN_VERBOSE(sLogger)( "Enumerating pixelformats..." );
    int num = DescribePixelFormat(hdc, 1, 0, 0);
    GN_VERBOSE(sLogger)( "%d pixelformats in total.", num );

    int candidates[4] =
    {
        0, // ICD
        0, // MCD
        0, // ???
        0, // software
    };

    for ( int i = 1; i <= num; i++ )
    {
        if (!DescribePixelFormat(hdc, i, sizeof(pfd), &pfd))
        {
            GN_ERROR(sLogger)( "can't get the description of the %dth pixelformat!", i );
            return 0;
        }

        // check pfd flags;
        if( (pfd.dwFlags & required_flags) != required_flags ) continue;
        if( (pfd.dwFlags & xxx_flags) != 0 ) continue;

        // check pixel type
        if( PFD_TYPE_RGBA != pfd.iPixelType ) continue;

        // check z-buffer
        if( 0 == pfd.cDepthBits ) continue;

        // check stencil buffer
        if( 0 == pfd.cStencilBits ) continue;

        // check acceleration flag
        if( PFD_GENERIC_ACCELERATED & pfd.dwFlags )
        {
            if( PFD_GENERIC_FORMAT & pfd.dwFlags )
            {
                // mixed device
                if( 0 == candidates[2] ) candidates[2] = i;
            }
            else
            {
                // MCD device
                if( 0 == candidates[1] ) candidates[1] = i;
            }
        }
        else if( PFD_GENERIC_FORMAT & pfd.dwFlags )
        {
            // software device
            if( 0 == candidates[3] ) candidates[3] = i;
        }
        else
        {
            // might be ICD device
            if( 0 == candidates[0] ) candidates[0] = i;
        }

        if( candidates[0] > 0 && candidates[1] > 0 && candidates[2] > 0 )
        {
            // no need to iterate more formats
            break;
        }
    }

    // prefer hardware than mixed, than software
    if( candidates[0] > 0 )
    {
        return candidates[0];
    }
    else if( candidates[1] > 0 )
    {
        return candidates[1];
    }
    else if( candidates[2] > 0 )
    {
        return candidates[2];
    }
    else if( candidates[3] > 0 )
    {
        return candidates[3];
    }

    // error
    GN_ERROR(sLogger)( "no appropriate pixelformat!" );
    return 0;

    GN_UNGUARD;
}

//
//
// ------------------------------------------------------------------------
static bool sSetupPixelFormat( HDC hdc )
{
    GN_GUARD;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd
            1,                     // version number
            PFD_GENERIC_ACCELERATED |
            PFD_DRAW_TO_WINDOW |   // support window
            PFD_SUPPORT_OPENGL |   // support OpenGL
            PFD_DOUBLEBUFFER,      // double buffered
            PFD_TYPE_RGBA,         // RGBA type
            32,                    // 24-bit color depth
            0, 0, 0, 0, 0, 0,      // color bits ignored
            0,                     // no alpha buffer
            0,                     // shift bit ignored
            0,                     // no accumulation buffer
            0, 0, 0, 0,            // accum bits ignored
            32,                    // 32-bit z-buffer
            0,                     // no stencil buffer
            0,                     // no auxiliary buffer
            PFD_MAIN_PLANE,        // main layer
            0,                     // reserved
            0, 0, 0                // layer masks ignored
    };

    // choose pixel format
    int n = sChoosePixelFormat( hdc );
    if( 0 == n ) return false;
    GN_VERBOSE(sLogger)( "select pixelformat #%d.", n );

    // Set the pixel format for the device context
    if (!SetPixelFormat(hdc, n, &pfd))
    {
        GN_ERROR(sLogger)( "SetPixelFormat failed!" );
        return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

// *****************************************************************************
// OpenGL debug output utilities
// *****************************************************************************

class OGLDebugOutputAMD
{
    static const char * category2string( GLenum category )
    {
        switch( category )
        {
            case GL_DEBUG_CATEGORY_API_ERROR_AMD          : return "API Error";
            case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD      : return "Window System";
            case GL_DEBUG_CATEGORY_DEPRECATION_AMD        : return "Deprecation";
            case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD : return "Undefined Behavior";
            case GL_DEBUG_CATEGORY_PERFORMANCE_AMD        : return "Performance";
            case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD    : return "Shader Compiler";
            case GL_DEBUG_CATEGORY_APPLICATION_AMD        : return "Application";
            case GL_DEBUG_CATEGORY_OTHER_AMD              : return "Other";
            default                                       : return "INVALID_CATEGORY";
        }
    }

    static const char * severity2string( GLenum severity )
    {
        switch( severity )
        {
            case GL_DEBUG_SEVERITY_HIGH_AMD   : return "High";
            case GL_DEBUG_SEVERITY_MEDIUM_AMD : return "Medium";
            case GL_DEBUG_SEVERITY_LOW_AMD    : return "Low";
            default                           : return "INVALID_SEVERITY";
        }
    }

    static void APIENTRY messageCallback(
        GLuint id,
        GLenum category,
        GLenum severity,
        GLsizei length,
        const GLchar * message,
        GLvoid * userParam )
    {
        GN_UNUSED_PARAM( length );
        GN_UNUSED_PARAM( userParam );

        // Determine log level
        Logger::LogLevel logLevel;
        switch( category )
        {
            case GL_DEBUG_CATEGORY_API_ERROR_AMD          :
            case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD :
                logLevel = Logger::ERROR_;
                break;

            case GL_DEBUG_CATEGORY_APPLICATION_AMD        :
                logLevel = Logger::INFO;
                break;

            case GL_DEBUG_CATEGORY_DEPRECATION_AMD        :
            case GL_DEBUG_CATEGORY_PERFORMANCE_AMD        :
                switch( severity )
                {
                    case GL_DEBUG_SEVERITY_HIGH_AMD   :
                    case GL_DEBUG_SEVERITY_MEDIUM_AMD :
                        logLevel = Logger::WARN;
                        break;

                    case GL_DEBUG_SEVERITY_LOW_AMD    : // Performance warnings from redundant state changes
                        logLevel = Logger::VVERBOSE;
                        break;

                    default:
                        logLevel = Logger::ERROR_;
                        break;
                }
                break;

            case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD      :
            case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD    :
            case GL_DEBUG_CATEGORY_OTHER_AMD              :
                switch( severity )
                {
                    case GL_DEBUG_SEVERITY_HIGH_AMD   :
                        logLevel = Logger::ERROR_;
                        break;

                    case GL_DEBUG_SEVERITY_MEDIUM_AMD :
                        logLevel = Logger::WARN;
                        break;

                    case GL_DEBUG_SEVERITY_LOW_AMD    : // Performance warnings from redundant state changes
                        logLevel = Logger::VVERBOSE;
                        break;

                    default:
                        logLevel = Logger::ERROR_;
                        break;
                }
                break;

            default:
                // Invalid category
                logLevel = Logger::ERROR_;
                break;
        }

        GN_LOG(sLogger, logLevel)(
            "OpenGL debug output (id=[%d] category=[%s] severity=[%s]): %s",
            id, category2string(category), severity2string(severity), message );
    }

public:

    static bool available()
    {
        return !!GLEW_AMD_debug_output;
    }

    static void initialize()
    {
#if GN_BUILD_DEBUG_ENABLED

        // setup the callback
        glDebugMessageCallbackAMD( &messageCallback, NULL );

        // enable all messages
        GN_OGL_CHECK( glDebugMessageEnableAMD( 0, 0, 0, 0, GL_TRUE ) );
#endif
    }
};

// *****************************************************************************
// OpenGL debug output utilities
// *****************************************************************************

class OGLDebugOutputARB
{
    static const char * source2string( GLenum source )
    {
        switch( source )
        {
            case GL_DEBUG_SOURCE_API_ARB             : return "GL API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB   : return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB : return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY_ARB     : return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION_ARB     : return "Application";
            case GL_DEBUG_SOURCE_OTHER_ARB           : return "Other";
            default                                  : return "INVALID_SOURCE";
        }
    }

    static const char * type2string( GLenum type )
    {
        switch( type )
        {
            case GL_DEBUG_TYPE_ERROR_ARB               : return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB : return "Deprecation";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB  : return "Undefined Behavior";
            case GL_DEBUG_TYPE_PORTABILITY_ARB         : return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE_ARB         : return "Performance";
            case GL_DEBUG_TYPE_OTHER_ARB               : return "Other";
            default                                    : return "INVALID_TYPE";
        }
    }

    static const char * severity2string( GLenum severity )
    {
        switch( severity )
        {
            case GL_DEBUG_SEVERITY_HIGH_ARB   : return "High";
            case GL_DEBUG_SEVERITY_MEDIUM_ARB : return "Medium";
            case GL_DEBUG_SEVERITY_LOW_ARB    : return "Low";
            default                           : return "INVALID_SEVERITY";
        }
    }

    static void APIENTRY messageCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam)
    {
        GN_UNUSED_PARAM( length );
        GN_UNUSED_PARAM( userParam );

        // Determine log level
        Logger::LogLevel logLevel;
        switch( type )
        {
            case GL_DEBUG_TYPE_ERROR_ARB              :
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB :
                logLevel = Logger::ERROR_;
                break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
            case GL_DEBUG_TYPE_PERFORMANCE_ARB        :
            case GL_DEBUG_TYPE_PORTABILITY            :
                switch( severity )
                {
                    case GL_DEBUG_SEVERITY_HIGH_ARB   :
                    case GL_DEBUG_SEVERITY_MEDIUM_ARB :
                        logLevel = Logger::WARN;
                        break;

                    case GL_DEBUG_SEVERITY_LOW_ARB    : // Performance warnings from redundant state changes
                        logLevel = Logger::VVERBOSE;
                        break;

                    default:
                        logLevel = Logger::ERROR_;
                        break;
                }
                break;

            case GL_DEBUG_TYPE_OTHER_ARB              :
                switch( severity )
                {
                    case GL_DEBUG_SEVERITY_HIGH_ARB   :
                        logLevel = Logger::ERROR_;
                        break;

                    case GL_DEBUG_SEVERITY_MEDIUM_ARB :
                        logLevel = Logger::WARN;
                        break;

                    case GL_DEBUG_SEVERITY_LOW_ARB    :
                        logLevel = Logger::VERBOSE;
                        break;

                    case GL_DEBUG_SEVERITY_NOTIFICATION:
                        logLevel = Logger::VVERBOSE;
                        break;

                    default:
                        logLevel = Logger::ERROR_;
                        break;
                }
                break;

            default:
                // Invalid category
                logLevel = Logger::ERROR_;
                break;
        }

        GN_LOG(sLogger, logLevel)(
            "OpenGL debug output (id=[%d] source=[%s] type=[%s] severity=[%s]): %s",
            id, source2string(source), type2string(type), severity2string(severity), message );
    }

public:

    static bool available()
    {
        return !!GLEW_ARB_debug_output;
    }

    static void initialize()
    {
#if GN_BUILD_DEBUG_ENABLED

        // setup the callback
        glDebugMessageCallbackARB( &messageCallback, NULL );

        // enable all messages
        GN_OGL_CHECK( glDebugMessageControlARB(
            GL_DONT_CARE, // source
            GL_DONT_CARE, // type
            GL_DONT_CARE, // severity
            0, // count
            NULL, // ids
            GL_TRUE ) );
#endif
    }
};

class OGLTempContext
{
    HGLRC hrc;

public:

    OGLTempContext() : hrc(0)
    {
    }

    ~OGLTempContext()
    {
        quit();
    }

    bool init( HDC hdc )
    {
        GN_MSW_CHECK_RETURN( hrc = ::wglCreateContext( hdc ), false );

        GN_MSW_CHECK_RETURN( ::wglMakeCurrent( hdc, hrc ), false );

        return true;
    }

    void quit()
    {
        if( hrc )
        {
            ::wglMakeCurrent(0, 0);
            ::wglDeleteContext(hrc);
            hrc = 0;
        }
    }
};

// *****************************************************************************
// device management
// *****************************************************************************

GN::gfx::OGLGpu::WindowMap GN::gfx::OGLGpu::msInstanceMap;

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLGpu::dispInit()
{
    GN_GUARD;

    GN_ASSERT( !mRenderContext && !mDeviceContext );

    HWND hwnd = (HWND)getRenderWindowHandle();
    if( !::IsWindow(hwnd) )
    {
        GN_ERROR(sLogger)( "Invalid render window handle!" );
        return false;
    }

    GN_MSW_CHECK_RETURN( mDeviceContext = ::GetDC(hwnd), false );

    if( !sSetupPixelFormat(mDeviceContext) ) return false;

    // create a temporary context first
    OGLTempContext tempContext;
    if( !tempContext.init( mDeviceContext ) ) return false;

    const GpuOptions & ro = getOptions();

    // Try creating a formal context using wglCreateContextAttribsARB
    PFNWGLCREATECONTEXTATTRIBSARBPROC createContextFunc = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if( createContextFunc )
    {
        GLint attributes[] =
        {
            WGL_CONTEXT_FLAGS_ARB, ro.debug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
            0,
        };

        GN_MSW_CHECK_RETURN(
            mRenderContext = createContextFunc( mDeviceContext, NULL, attributes ),
            false );
    }
    else
    {
        GN_MSW_CHECK_RETURN(
            mRenderContext = ::wglCreateContext( mDeviceContext ),
            false );
    }

    // release the temporary context
    tempContext.quit();

    // activate the formal context
    GN_MSW_CHECK_RETURN( ::wglMakeCurrent(mDeviceContext, mRenderContext), false );

    // init GLEW
    glewInit();

    // Setup the debug output
    if( ro.debug && OGLDebugOutputARB::available() )
    {
        OGLDebugOutputARB::initialize();
    }

    // modify fullscreen render window properties
    if( DisplayMode::FULL_SCREEN == ro.displayMode.mode ) {
        // activate display mode
        if( !activateDisplayMode() ) return false;

        const DispDesc & dd = getDispDesc();
        HWND hwnd = (HWND)dd.windowHandle;
        HMONITOR hmonitor = (HMONITOR)dd.monitorHandle;

        // get monitor information
        MONITORINFOEXA mi;
        mi.cbSize = sizeof(mi);
        GN_MSW_CHECK_RETURN( ::GetMonitorInfoA( hmonitor, &mi ), false );

        // move window to left-top of the monitor.
        GN_TRACE(sLogger)( "Move window to %d, %d", mi.rcWork.left,mi.rcWork.top );
        GN_MSW_CHECK( ::SetWindowPos(
            hwnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            0, 0,
            SWP_NOSIZE ) );

        // trigger a redraw operation
        GN_MSW_CHECK( ::UpdateWindow( hwnd ) );
    }

    // set swap interval
    if( WGLEW_EXT_swap_control )
    {
        if( !wglSwapIntervalEXT( ro.vsync ) )
        {
            GN_WARN(sLogger)( "Fail to adjust SGI swap control" );
        }
    }

    // setup message hook
    msInstanceMap[getRenderWindowHandle()] = this;
    mHook = ::SetWindowsHookExW( WH_CALLWNDPROC, &staticHookProc, 0, GetCurrentThreadId() );
    if( 0 == mHook )
    {
        GN_ERROR(sLogger)( "Fail to setup message hook : %s", getWin32LastErrorInfo() );
        return false;
    }

    // successful
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLGpu::dispQuit()
{
    GN_GUARD;

    // remove message hook
    if( mHook ) ::UnhookWindowsHookEx( mHook ), mHook = 0;
    msInstanceMap.remove(getRenderWindowHandle());

    // restore display mode
    restoreDisplayMode();

    if( mRenderContext )
    {
        ::wglMakeCurrent(0, 0);
        ::wglDeleteContext(mRenderContext);
        mRenderContext = 0;
    }

    if( mDeviceContext )
    {
        HWND hwnd = (HWND)getDispDesc().windowHandle;
        GN_ASSERT( ::IsWindow(hwnd) );
        ::ReleaseDC( hwnd, mDeviceContext );
        mDeviceContext = 0;
    }

    GN_UNGUARD;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLGpu::activateDisplayMode()
{
    GN_GUARD;

    // check for redundant activation
    if( mDisplayModeActivated ) return true;

    // only change display mode if we are in fullscreen mode
    if( DisplayMode::FULL_SCREEN != getOptions().displayMode.mode ) return true;

    const DispDesc & dd = getDispDesc();

    // ignore message hook during this function call
    ScopeBool ignoreHook(mIgnoreMsgHook);

    // get monitor information
    MONITORINFOEXA mi;
    mi.cbSize = sizeof(mi);
    GN_MSW_CHECK_RETURN( ::GetMonitorInfoA( (HMONITOR)dd.monitorHandle, &mi ), false );

    // change display mode
    DEVMODEA dm;
    memset( &dm, 0, sizeof(dm) );
    dm.dmSize             = sizeof(dm);
    dm.dmPelsWidth        = dd.width;
    dm.dmPelsHeight       = dd.height;
    dm.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT;
    if( 0 != dd.depth ) {
        dm.dmFields |= DM_BITSPERPEL;
        dm.dmBitsPerPel = dd.depth;
    }
    if( 0 != dd.refrate ) {
        dm.dmFields |= DM_DISPLAYFREQUENCY;
        dm.dmDisplayFrequency = dd.refrate;
    }

    if( DISP_CHANGE_SUCCESSFUL != ::ChangeDisplaySettingsExA(
        mi.szDevice,
        &dm,
        NULL,
        CDS_FULLSCREEN,
        NULL ) ) {
        GN_ERROR(sLogger)( "Failed to change to specified full screen mode!" );
        return false;
    }
    mDisplayModeActivated = true;
    GN_INFO(sLogger)(
        "Fullscreen mode activated: width(%d), height(%d), depth(%d), refrate(%d).",
        dd.width, dd.height, dd.depth, dd.refrate );

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLGpu::restoreDisplayMode()
{
    GN_GUARD;

    if( mDisplayModeActivated )
    {
        mDisplayModeActivated = false;

        // ignore message hook during this function call
        ScopeBool ignoreHook(mIgnoreMsgHook);

        // restore display mode
        if( DISP_CHANGE_SUCCESSFUL != ::ChangeDisplaySettings(0, 0) )
        {
            GN_ERROR(sLogger)( "Failed to restore display mode: %s!", getWin32LastErrorInfo() );
        }

        GN_INFO(sLogger)( "Display mode restored." );
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLGpu::msgHook( HWND, UINT msg, WPARAM wp, LPARAM )
{
    GN_GUARD;

    //GN_TRACE( "Message(%s), wp(0x%X)", win::msg2str(msg), wp );

    if( DisplayMode::FULL_SCREEN != getOptions().displayMode.mode ) return;

    if( WM_ACTIVATEAPP == msg && !mIgnoreMsgHook )
    {
        if( wp )
        {
            activateDisplayMode();
            ::ShowWindowAsync( (HWND)getDispDesc().windowHandle, SW_NORMAL );
        }
        else
        {
            ::ShowWindowAsync( (HWND)getDispDesc().windowHandle, SW_MINIMIZE );
            restoreDisplayMode();
        }
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
LRESULT CALLBACK
GN::gfx::OGLGpu::staticHookProc( int code, WPARAM wp, LPARAM lp )
{
    GN_GUARD;

    //GN_TRACE( "wnd=0x%X, msg=%s", wnd, win::msg2str(msg) );

    OGLGpu ** pp = msInstanceMap.find( (intptr_t)((CWPSTRUCT*)lp)->hwnd );

    if( NULL != pp )
    {
        // trigger render window message signal.
        CWPSTRUCT * cwp = (CWPSTRUCT*)lp;
        GN_ASSERT( cwp && (*pp) );
        (*pp)->msgHook( cwp->hwnd, cwp->message, cwp->wParam, cwp->lParam );
    }

    return ::CallNextHookEx( 0, code, wp, lp );

    GN_UNGUARD;
}

#endif // GN_WINPC
