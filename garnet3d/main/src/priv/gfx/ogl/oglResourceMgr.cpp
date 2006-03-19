#include "pch.h"
#include "oglRenderer.h"
#include "oglShader.h"
#include "oglTexture.h"
#include "oglVtxFmt.h"
#include "oglVtxBuf.h"
#include "oglIdxBuf.h"

// *****************************************************************************
// local function
// *****************************************************************************

struct EqualFormat
{
    const GN::gfx::VtxFmtDesc & format;

    EqualFormat( const GN::gfx::VtxFmtDesc & f ) : format(f) {}

    bool operator()( void * p ) const
    {
        GN_ASSERT( p );
        return format == ((GN::gfx::OGLVtxFmt*)p)->getFormat();
    }
};

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::resourceDeviceCreate()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    std::list<OGLResource*>::iterator i = mResourceList.begin();
    while( i != mResourceList.end() )
    {
        if( !(*i)->deviceCreate() ) return false;
        ++i;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::resourceDeviceRestore()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    std::list<OGLResource*>::iterator i = mResourceList.begin();
    while( i != mResourceList.end() )
    {
        if( !(*i)->deviceRestore() ) return false;
        ++i;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::resourceDeviceDispose()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    std::list<OGLResource*>::reverse_iterator i = mResourceList.rbegin();
    while( i != mResourceList.rend() )
    {
        (*i)->deviceDispose();
        ++i;
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::resourceDeviceDestroy()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    std::list<OGLResource*>::reverse_iterator i = mResourceList.rbegin();
    while( i != mResourceList.rend() )
    {
        (*i)->deviceDestroy();
        ++i;
    }

    GN_UNGUARD;
}

// *****************************************************************************
// from Renderer
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::Shader *
GN::gfx::OGLRenderer::createShader( ShaderType type, ShadingLanguage lang, const StrA & code, const StrA & )
{
    GN_GUARD;

    if( !supportShader( type, lang ) )
    {
        GN_ERROR( "Current renderer do not support shading language '%s' for shader type '%s'.",
            shadingLanguage2Str(lang), shaderType2Str(type) );
        return 0;
    }

    switch( type )
    {
        case VERTEX_SHADER :
            switch( lang )
            {
                case LANG_OGL_ARB:
                {
                    AutoRef<OGLVtxShaderARB> p( new OGLVtxShaderARB(*this) );
                    if( !p->init( code ) ) return 0;
                    return p.detach();
                }

                case LANG_OGL_GLSL:
                {
                    AutoRef<OGLVtxShaderGLSL> p( new OGLVtxShaderGLSL(*this) );
                    if( !p->init( code ) ) return 0;
                    return p.detach();
                }

                default:
                    GN_ERROR( "unsupport shading language : %s", shadingLanguage2Str(lang) );
                    return 0;
            }

        case PIXEL_SHADER :
            switch( lang )
            {
                case LANG_OGL_ARB:
                {
                    AutoRef<OGLPxlShaderARB> p( new OGLPxlShaderARB(*this) );
                    if( !p->init( code ) ) return 0;
                    return p.detach();
                }

                case LANG_OGL_GLSL:
                {
                    AutoRef<OGLPxlShaderGLSL> p( new OGLPxlShaderGLSL(*this) );
                    if( !p->init( code ) ) return 0;
                    return p.detach();
                }

                default:
                    GN_ERROR( "unsupport shading language : %s", shadingLanguage2Str(lang) );
                    return 0;
            }

        default:
            GN_UNEXPECTED(); // program should not reach here
            GN_ERROR( "invalid shader type: %d", type );
            return 0;
    }
    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Texture *
GN::gfx::OGLRenderer::createTexture( const TextureDesc & desc,
                                     const TextureLoader & loader )
{
    GN_GUARD;

    AutoRef<OGLTexture> p( new OGLTexture(*this) );
    p->setLoader( loader );
    if( !p->init( desc ) ) return 0;
    return p.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
uint32_t GN::gfx::OGLRenderer::createVtxFmt( const VtxFmtDesc & format )
{
    GN_GUARD;

    uint32_t h = mVtxFmts.findIf( EqualFormat(format) );

    if( 0 == h )
    {
        // create new vertex binding object
        AutoObjPtr<OGLVtxFmt> p( new OGLVtxFmt(*this) );
        if( !p->init( format ) ) return 0;
        h = mVtxFmts.add( p );
        p.detach();
    }

    // success
    return h;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::VtxBuf *
GN::gfx::OGLRenderer::createVtxBuf(
    size_t bytes, bool dynamic, bool sysCopy, const VtxBufLoader & loader )
{
    GN_GUARD;

    if( GLEW_ARB_vertex_buffer_object )
    {
        AutoRef<OGLVtxBufVBO> p( new OGLVtxBufVBO(*this) );
        p->setLoader( loader );
        if( !p->init( bytes, dynamic, sysCopy ) ) return 0;
        return p.detach();
    }
    else
    {
        AutoRef<OGLVtxBufNormal> p( new OGLVtxBufNormal );
        p->setLoader( loader );
        if( !p->init( bytes, dynamic ) ) return 0;
        return p.detach();
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::IdxBuf *
GN::gfx::OGLRenderer::createIdxBuf(
    size_t numIdx, bool dynamic, bool /*sysCopy*/, const IdxBufLoader & loader )
{
    GN_GUARD;

    AutoRef<OGLIdxBuf> p( new OGLIdxBuf );
    p->setLoader( loader );
    if( !p->init( numIdx, dynamic ) ) return 0;

    return p.detach();

    GN_UNGUARD;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::removeGLSLShader( ShaderType st, Shader * sh )
{
    GN_GUARD;

    GN_ASSERT( 0 <= st && st < NUM_SHADER_TYPES );

    GLSLProgramMap::iterator i,t;
    for( i = mGLSLProgramMap.begin(); i != mGLSLProgramMap.end(); )
    {
        if( i->first.shaders[st] == sh )
        {
            t = i;
            ++i;

            // remove the program from program map
            GN_ASSERT( t->second );
            OGLProgramGLSL * prog = (OGLProgramGLSL*)t->second;
            delete prog;
            mGLSLProgramMap.erase( t );
        }
        else
        {
            ++i;
        }
    }

    GN_UNGUARD;
}
