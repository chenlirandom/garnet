#include "pch.h"
#include "oglRenderer.h"
#include <algorithm>

// for GLEW multi-context support
#define glewGetContext() r.getGLEWContext()

// ****************************************************************************
// local types/variables/functions
// ****************************************************************************

//!
//! Split a string into token list
// ------------------------------------------------------------------------
static void
sGetTokens( std::vector<GN::StrA> & tokens, const char * str )
{
    if( GN::strEmpty(str) ) return;
    const char * p1 = str;
    const char * p2 = p1;

    while( *p1 )
    {
        while( *p2 && *p2 != ' ' ) ++p2;

        tokens.push_back( GN::StrA(p1, p2-p1) );

        while( *p2 && *p2 == ' ' ) ++p2;

        p1 = p2;
    }
}

//!
//! function use to determine a extension is supported or not
// ------------------------------------------------------------------------
static GN_INLINE bool
sFindExtension( const std::vector<GN::StrA> & glexts, const char * ext )
{
    return glexts.end() != std::find( glexts.begin(), glexts.end(), ext );
}
//!
//! Check required extensions
// ------------------------------------------------------------------------
static bool sCheckRequiredExtensions( const std::vector<GN::StrA> & extensions )
{
    static const char * sRequiredExtensions[] =
    {
        "GL_EXT_bgra",                    // 1.1
        //"GL_ARB_multitexture",            // 1.3
        //"GL_ARB_texture_env_combine",     // 1.3
        0,
    };
    bool fail = false;
    char const * const * p = sRequiredExtensions;
    while ( *p )
    {
        if( GL_TRUE != sFindExtension( extensions, *p ) )
        {
            GN_ERROR( "Required extension '%s' was not supported!", *p );
            fail = true;
        }
        // next extension
        ++p;
    }
    return !fail;
}

//!
//! initialize opengl extension
// ------------------------------------------------------------------------
#if GN_MSWIN
static bool sGetOGLExtensions( HDC hdc, std::vector<GN::StrA> & result )
#else
static bool sGetOGLExtensions( Display * disp, std::vector<GN::StrA> & result )
#endif
{
    GN_GUARD;

    result.clear();

    // ����OpenGL-Extentions-String
    sGetTokens( result, (const char*)glGetString(GL_EXTENSIONS) );

#if GN_MSWIN
    // ����WGL Extensions
    PFNWGLGETEXTENSIONSSTRINGARBPROC proc;
    proc = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(
        ::wglGetProcAddress("wglGetExtensionsStringARB") );
    if( proc ) sGetTokens( result, (const char *)proc(hdc) );
#elif GN_POSIX
    // ����GLX Extensions
    // TODO: query server extension string
    sGetTokens( result, (const char*)glXGetClientString( disp, GLX_EXTENSIONS) );
#endif

    std::sort( result.begin(), result.end() );

    // success;
    return true;

    GN_UNGUARD;
}

//!
//! output GL implementation info.
// ------------------------------------------------------------------------
static void sOutputOGLInfo( GN::gfx::OGLRenderer & r, GN::HandleType disp, const std::vector<GN::StrA> & glexts )
{
    GN_GUARD;

    GN::StrA info;

    // vendor info.

#if GN_POSIX
    const char * vendor   = (const char *)glXGetClientString( (Display*)disp, GLX_VENDOR );
    const char * version  = (const char *)glXGetClientString( (Display*)disp, GLX_VERSION );
#else
    GN_UNUSED_PARAM( disp );
    const char * vendor   = (const char *)glGetString(GL_VENDOR);
    const char * version  = (const char *)glGetString(GL_VERSION);
#endif
    const char * renderer = (const char *)glGetString(GL_RENDERER);

    info = GN::strFormat(
        "\n\n"
        "===================================================\n"
        "        OpenGL Implementation Informations\n"
        "---------------------------------------------------\n"
        "    OpenGL vendor      :    %s\n"
        "    OpenGL version     :    %s\n"
        "    OpenGL renderer    :    %s\n",
        vendor, version, renderer );

    // caps. info.
    GLint ts, tu;
    GN_OGL_CHECK( glGetIntegerv( GL_MAX_TEXTURE_SIZE, &ts ) );
    if( GLEW_ARB_multitexture )
        GN_OGL_CHECK( glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &tu ) );
    else
        tu = 1;
    info += GN::strFormat(
        "---------------------------------------------------\n"
        "    Max size of texture             :    %d\n"
        "    Max number of texture stages    :    %d\n",
        ts,tu );

    // extension info.
    info +=
        "---------------------------------------------------\n";
    for ( size_t i = 0; i < glexts.size(); ++i )
    {
        info += "     " + glexts[i] + "\n";
    }

    info +=
        "===================================================\n"
        "\n\n";

    GN_INFO( info.cstr() );

    GN_UNGUARD;
}

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif
static void GLAPIENTRY sFake_glActiveTexture(GLenum) {}
static void GLAPIENTRY sFake_glClientActiveTexture(GLenum) {}

// ****************************************************************************
// local functions that initialize individual capability
// ****************************************************************************

//
static uint32_t sCapsInit_MAX_2D_TEXTURE_SIZE( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    GLint result = 0;
    GN_OGL_CHECK( glGetIntegerv( GL_MAX_TEXTURE_SIZE, &result ) );
    return result;
}
//
static uint32_t sCapsInit_MAX_CLIP_PLANES( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    GLint result = 0;
    GN_OGL_CHECK( glGetIntegerv( GL_MAX_CLIP_PLANES, &result ) );
    return result;
}
//
static uint32_t sCapsInit_MAX_RENDER_TARGETS( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    // FIXME: this is only suit for glCopyTexImage, not real PBuffer texture
    return 4;
}
//
static uint32_t sCapsInit_MAX_PRIMITIVES( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    return 0x10000; // no more than 65536 elements in one DIP
}
//
static uint32_t sCapsInit_MAX_TEXTURE_STAGES( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    if( GLEW_ARB_multitexture )
    {
        GLint result;
        GN_OGL_CHECK_RV( glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &result ), 1 );
        return result;
    }
    else
    {
        return 1;
    }
}
//
static uint32_t sCapsInit_PER_STAGE_CONSTANT( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    // OpenGL always supports this.
    return true;
}
//
static uint32_t sCapsInit_PSCAPS( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    uint32_t result = 0;
    if( GLEW_ARB_fragment_program ) result |= GN::gfx::PSCAPS_OGL_ARB1;
    if( GLEW_ARB_shader_objects &&
         GLEW_ARB_fragment_shader &&
         GLEW_ARB_shading_language_100 ) result |= GN::gfx::PSCAPS_OGL_GLSL;
    return result;
}
//
static uint32_t sCapsInit_VSCAPS( GN::gfx::OGLRenderer & r )
{
    GN_UNUSED_PARAM( r );
    uint32_t result = 0;
    if( GLEW_ARB_vertex_program ) result |= GN::gfx::VSCAPS_OGL_ARB1;
    if( GLEW_ARB_shader_objects &&
         GLEW_ARB_vertex_shader &&
         GLEW_ARB_shading_language_100 ) result |= GN::gfx::VSCAPS_OGL_GLSL;
    return result;
}

// for GLEW multi-context support
#undef glewGetContext

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::capsDeviceCreate()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // output opengl implementation info.
    std::vector<StrA> glexts;
#if GN_MSWIN
    if( !sGetOGLExtensions( mDeviceContext, glexts ) )
#else
    if( !sGetOGLExtensions( (Display*)getDispDesc().displayHandle, glexts) )
#endif
    {
        return false;
    }
    sOutputOGLInfo( *this, getDispDesc().displayHandle, glexts );

    // check required extension
    if( !sCheckRequiredExtensions( glexts ) ) return false;

    // ��һ�ĳ�ʼ��ÿһ��caps
    #define GNGFX_CAPS( name ) \
        setCaps( CAPS_##name, sCapsInit_##name( *this ) );
    #include "garnet/gfx/rendererCapsMeta.h"
    #undef GNGFX_CAPS

    // special case for multi-texture
    if( !GLEW_ARB_multitexture )
    {
        glActiveTextureARB = sFake_glActiveTexture;
        glClientActiveTextureARB = sFake_glClientActiveTexture;
    }

    // success;
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::capsDeviceDestroy()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // clear all caps
    resetAllCaps();

    GN_UNGUARD;
}

