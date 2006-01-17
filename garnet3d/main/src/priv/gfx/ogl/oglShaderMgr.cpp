#include "pch.h"
#include "oglRenderer.h"
#include "oglShader.h"

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::shaderDeviceDestroy()
{
    GN_GUARD;

    // clear GLSL program map
    GLSLProgramMap::iterator i;
    for( i = mGLSLProgramMap.begin(); i != mGLSLProgramMap.end(); ++i )
    {
        GN_ASSERT( i->second );
        OGLProgramGLSL * prog = (OGLProgramGLSL*)i->second;
        delete prog;
    }
    mGLSLProgramMap.clear();

    GN_UNGUARD;
}

// *****************************************************************************
// interface functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::supportShader( ShaderType type, ShadingLanguage lang )
{
    GN_GUARD;

    makeCurrent();

    // check parameter
    if( 0 > type || type >= NUM_SHADER_TYPES )
    {
        GNGFX_ERROR( "invalid shader usage!" );
        return false;
    }
    if( 0 > lang || lang >= NUM_SHADING_LANGUAGES )
    {
        GNGFX_ERROR( "invalid shading language!" );
        return false;
    }

    switch( lang )
    {
        // ARB shaders
        case LANG_OGL_ARB :
            if( VERTEX_SHADER == type )
            {
                return 0 != GLEW_ARB_vertex_program;
            }
            else
            {
                GN_ASSERT( PIXEL_SHADER == type );
                return 0 != GLEW_ARB_fragment_program;
            }

        // GLSL shaders
        case LANG_OGL_GLSL :
            if( !GLEW_ARB_shader_objects || !GLEW_ARB_shading_language_100 ) return false;
            if( VERTEX_SHADER == type )
            {
                return 0 != GLEW_ARB_vertex_shader;
            }
            else
            {
                GN_ASSERT( PIXEL_SHADER == type );
                return 0 != GLEW_ARB_fragment_shader;
            }

        // DX shaders are always unsupported
        case LANG_D3D_ASM :
        case LANG_D3D_HLSL :
            return false;

        // TODO: Check Cg shader caps
        case LANG_CG : return false;

        default:
            GN_ASSERT_EX( 0, "program should never reach here!" );
            return false;
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Shader *
GN::gfx::OGLRenderer::createVtxShader( ShadingLanguage lang, const StrA & code, const StrA & )
{
    GN_GUARD;

    if( !supportShader( VERTEX_SHADER, lang ) )
    {
        GNGFX_ERROR( "Current renderer do not support vertex shading language '%s'.",
            shadingLanguage2Str(lang) );
        return 0;
    }

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
            GNGFX_ERROR( "unsupport shading language : %s", shadingLanguage2Str(lang) );
            return 0;
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Shader *
GN::gfx::OGLRenderer::createPxlShader( ShadingLanguage lang, const StrA & code, const StrA & )
{
    GN_GUARD;

    if( !supportShader( PIXEL_SHADER, lang ) )
    {
        GNGFX_ERROR( "Current renderer do not support pixel shading language '%s'.",
            shadingLanguage2Str(lang) );
        return 0;
    }

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
            GNGFX_ERROR( "unsupport shading language : %s", shadingLanguage2Str(lang) );
            return 0;
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::bindVtxShader( const Shader * shader )
{
    GN_GUARD_SLOW;

    mCurrentDrawState.bindVtxShader( shader );

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::bindPxlShader( const Shader * shader )
{
    GN_GUARD_SLOW;

    mCurrentDrawState.bindPxlShader( shader );

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::bindShaders( const Shader * vtx, const Shader * pxl )
{
    GN_GUARD_SLOW;

    mCurrentDrawState.bindVtxShader( vtx );
    mCurrentDrawState.bindPxlShader( pxl );

    GN_UNGUARD_SLOW;
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

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::applyShaderState()
{
    GN_GUARD_SLOW;

    bool vsGLSL = false;
    bool psGLSL = false;

    if( mCurrentDrawState.dirtyFlags.vtxShader )
    {
        if( mLastDrawState.vtxShader )
        {
            const OGLBasicShader * sh = safeCast<const OGLBasicShader *>(mLastDrawState.vtxShader.get());
            sh->disable();
        }
        if( mCurrentDrawState.vtxShader )
        {
            if( LANG_OGL_GLSL != mCurrentDrawState.vtxShader->getLang() )
            {
                const OGLBasicShader * sh = safeCast<const OGLBasicShader *>(mCurrentDrawState.vtxShader.get());
                sh->enable();
                sh->apply();
            }
            else
            {
                vsGLSL = true;
            }
        }
    }
    else if( mCurrentDrawState.vtxShader )
    {
        if( LANG_OGL_GLSL != mCurrentDrawState.vtxShader->getLang() )
        {
            const OGLBasicShader * sh = safeCast<const OGLBasicShader *>(mCurrentDrawState.vtxShader.get());
            sh->applyDirtyUniforms();
        }
    }

    if( mCurrentDrawState.dirtyFlags.pxlShader )
    {
        if( mLastDrawState.pxlShader )
        {
            const OGLBasicShader * sh = safeCast<const OGLBasicShader *>(mLastDrawState.pxlShader.get());
            sh->disable();
        }
        if( mCurrentDrawState.pxlShader )
        {
            if( LANG_OGL_GLSL != mCurrentDrawState.pxlShader->getLang() )
            {
                const OGLBasicShader * sh = safeCast<const OGLBasicShader *>(mCurrentDrawState.pxlShader.get());
                sh->enable();
                sh->apply();
            }
            else
            {
                psGLSL = true;
            }
        }
    }
    else if( mCurrentDrawState.pxlShader )
    {
        if( LANG_OGL_GLSL != mCurrentDrawState.pxlShader->getLang() )
        {
            const OGLBasicShader * sh = safeCast<const OGLBasicShader *>(mCurrentDrawState.pxlShader.get());
            sh->applyDirtyUniforms();
        }
    }

    // handle GLSL shader and program in special way
    if( !vsGLSL && !psGLSL ) return;
    const Shader * vs = vsGLSL ? mCurrentDrawState.vtxShader.get() : 0;
    const Shader * ps = psGLSL ? mCurrentDrawState.pxlShader.get() : 0;
    const OGLProgramGLSL * prog;
    GLSLShaders key = { vs, ps };
    GLSLProgramMap::const_iterator i = mGLSLProgramMap.find( key );
    if( mGLSLProgramMap.end() != i )
    {
        // found!
        GN_ASSERT( i->second );
        prog = (const OGLProgramGLSL*)i->second;
    }
    else
    {
        // not found. we have to create a new GLSL program object
        AutoObjPtr<OGLProgramGLSL> newProg( new OGLProgramGLSL(getGLEWContext()) );
        if( !newProg->init(
            safeCast<const OGLBasicShaderGLSL*>(vs),
            safeCast<const OGLBasicShaderGLSL*>(ps) ) ) return                          ;
        mGLSLProgramMap[key] = newProg.get();
        prog = newProg.detach();
    }
    prog->apply();

    GN_UNGUARD_SLOW;
}
