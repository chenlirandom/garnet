#include "pch.h"
#include "xenonShader.h"
#include "xenonTexture.h"
#include "xenonRenderer.h"
#include "garnet/GNd3d9.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.xenon");

//
//
// -----------------------------------------------------------------------------
template<class T, class ARRAY>
static T * sFindParameter( ARRAY & array, const char * name )
{
    for( size_t i = 0; i < array.size(); ++i )
    {
        T & t = array[i];

        if( t.name == name ) return &t;
    }

    return NULL;
}


// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonGpuProgramHLSL::init( const GpuProgramDesc & desc )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( XenonGpuProgramHLSL, () );

    GN_ASSERT( GpuProgramLanguage::HLSL9 == desc.lang );

    IDirect3DDevice9 & dev = getRenderer().getDeviceInlined();

    if( desc.vs.source )
    {
        mVs = d3d9::ShaderCompiler<IDirect3DVertexShader9>::compileAndCreate(
            dev,
            desc.vs.source,
            0, // source length
            0, // compile flags
            desc.vs.entry,
            0, // profile
            &mVsConsts );

        if( NULL == mVs ) return failure();

        enumerateConsts( mVsConsts, true );
    }

    if( desc.ps.source )
    {
        mPs = d3d9::ShaderCompiler<IDirect3DPixelShader9>::compileAndCreate(
            dev,
            desc.ps.source,
            0,
            0,
            desc.ps.entry,
            0,
            &mPsConsts );
        if( NULL == mPs ) return failure();

        enumerateConsts( mPsConsts, false );
    }

    // setup parameter descriptor
    buildUnformNameAndSizeArray();

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramHLSL::quit()
{
    GN_GUARD;

    safeRelease( mVs );
    safeRelease( mVsConsts );
    safeRelease( mPs );
    safeRelease( mPsConsts );

    mUniforms.clear();

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// Public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramHLSL::apply() const
{
    IDirect3DDevice9 & dev = getRenderer().getDeviceInlined();
    dev.SetVertexShader( mVs );
    dev.SetPixelShader( mPs );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramHLSL::applyUniforms( const SysMemUniform * const * uniforms, size_t count ) const
{
    IDirect3DDevice9 & dev = getRenderer().getDeviceInlined();

    for( size_t i = 0; i < count; ++i )
    {
        if( i >= mUniforms.size() )
        {
            GN_WARN(sLogger)( "there are more GPU parameters than the shader needs." );
            return;
        }

        const SysMemUniform * u = uniforms[i];

        if( NULL == u )
        {
            GN_ERROR(sLogger)( "Null uniform pointer." );
            continue;
        }

        const XenonUniformParamDesc & d = mUniforms[i];

        GN_ASSERT( !d.name.empty() && (d.vshandle || d.pshandle) && d.size );

        // check parameter size
        if( getRenderer().paramCheckEnabled() )
        {
            if( u->size() != d.size )
            {
                GN_WARN(sLogger)(
                    "parameter %s: value size(%d) differs from size defined in shader code(%d).",
                    d.name.cptr(),
                    u->size(),
                    d.size );
            }
        }

        // apply to D3D
        if( d.vshandle )
        {
            GN_DX9_CHECK( mVsConsts->SetValue( &dev, d.vshandle, u->getval(), (UINT)d.size ) );
        }
        if( d.pshandle )
        {
            GN_DX9_CHECK( mPsConsts->SetValue( &dev, d.pshandle, u->getval(), (UINT)d.size ) );
        }
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramHLSL::applyTextures(
    const TextureBinding * bindings,
    size_t                 count,
    bool                   /*skipDirtyCheck*/ ) const
{
    IDirect3DDevice9 & dev = getRenderer().getDeviceInlined();

    for( size_t i = 0; i < count; ++i )
    {
        const TextureBinding & tb = bindings[i];

        if( '\0' == tb.binding[0] ) continue;

        const XenonTextureParamDesc * param = sFindParameter<const XenonTextureParamDesc>( mTextures, tb.binding );

        if( !param )
        {
            GN_ERROR(sLogger)( "Texture #%d is binding to invalid GPU parameter named \"%s\".", i, tb.binding );
            continue;
        }

        IDirect3DBaseTexture9 * d3dtex = tb.texture ? ((XenonTexture*)tb.texture.get())->getD3DTexture() : NULL;

        if( param->vshandle )
        {
            UINT stage = mVsConsts->GetSamplerIndex( param->vshandle );
            dev.SetTexture( stage, d3dtex );
        }

        if( param->pshandle )
        {
            UINT stage = mPsConsts->GetSamplerIndex( param->pshandle );
            dev.SetTexture( stage, d3dtex );
        }
    }
}

// *****************************************************************************
// Private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonGpuProgramHLSL::enumerateConsts(
    ID3DXConstantTable * constTable,
    bool                 vs )
{
    // get number of constants
    D3DXCONSTANTTABLE_DESC tableDesc;
    GN_DX9_CHECK_RV( constTable->GetDesc( &tableDesc ), false );
    UINT numConstant = tableDesc.Constants;

    // for each const
    for( UINT i = 0; i < numConstant; ++i )
    {
        // get handle and description
        D3DXHANDLE        c = constTable->GetConstant( NULL, i );
        D3DXCONSTANT_DESC cdarray[GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_COUNT];
        size_t            count;
        GN_DX9_CHECK_RV( constTable->GetConstantDesc( c, cdarray, &count ), false );

        GN_ASSERT( count > 0 );

        for( size_t i = 0; i < count; ++i )
        {
            D3DXCONSTANT_DESC & cd = cdarray[0];

            GN_TODO( "check for uniforms with same name" );

            // determin
            switch( cd.Type )
            {
                case D3DXPT_BOOL  :
                case D3DXPT_INT   :
                case D3DXPT_FLOAT :
                {
                    XenonUniformParamDesc * u = sFindParameter<XenonUniformParamDesc>( mUniforms, cd.Name );
                    if( u )
                    {
                        if( u->size != cd.Bytes )
                        {
                            GN_ERROR(sLogger)( "VS and PS are referencing uniform with same name but different type, which is not supported by Xenon renderer." );
                            return false;
                        }

                        if( vs )
                        {
                            u->vshandle = c;
                        }
                        else
                        {
                            u->pshandle = c;
                        }
                    }
                    else
                    {
                        XenonUniformParamDesc u;
                        u.name = cd.Name;
                        if( vs )
                        {
                            u.vshandle = c;
                        }
                        else
                        {
                            u.pshandle = c;
                        }
                        u.size = cd.Bytes;
                        mUniforms.append( u );
                    }
                    break;
                }

                case D3DXPT_TEXTURE1D:
                case D3DXPT_TEXTURE2D:
                case D3DXPT_TEXTURE3D:
                case D3DXPT_TEXTURECUBE:
                    // do nothing, just ignore them.
                    break;

                case D3DXPT_SAMPLER:
                case D3DXPT_SAMPLER1D:
                case D3DXPT_SAMPLER2D:
                case D3DXPT_SAMPLER3D:
                case D3DXPT_SAMPLERCUBE:
                {
                    XenonTextureParamDesc * t = sFindParameter<XenonTextureParamDesc>( mTextures, cd.Name );
                    if( t )
                    {
                        if( vs )
                        {
                            t->vshandle = c;
                        }
                        else
                        {
                            t->pshandle = c;
                        }
                    }
                    else
                    {
                        XenonTextureParamDesc t;
                        t.name = cd.Name;
                        if( vs )
                        {
                            t.vshandle = c;
                        }
                        else
                        {
                            t.pshandle = c;
                        }
                        mTextures.append( t );
                    }
                    break;
                }

                default:
                    GN_WARN(sLogger)( "Unsupport Xenon shader constant type: %d", cd.Type );
            }
        }
    }

    // success
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramHLSL::buildUnformNameAndSizeArray()
{
    if( !mUniforms.empty() )
    {
        size_t count = mUniforms.size();

        GN_ASSERT( count > 0 );

        mParamDesc.mUniformArray       = mUniforms.cptr();
        mParamDesc.mUniformCount       = mUniforms.size();
        mParamDesc.mUniformArrayStride = sizeof(mUniforms[0]);

        for( size_t i = 0; i < count; ++i )
        {
            // UGLY!!! UGLY!!!
            XenonUniformParamDesc          & u1 = mUniforms[i];
            GpuProgramUniformParameterDesc & u2 = mUniforms[i];
            u2.name = u1.name.cptr();
        }
    }

    if( !mTextures.empty() )
    {
        size_t count = mTextures.size();

        GN_ASSERT( count > 0 );

        mParamDesc.mTextureArray       = mTextures.cptr();
        mParamDesc.mTextureCount       = mTextures.size();
        mParamDesc.mTextureArrayStride = sizeof(mTextures[0]);

        for( size_t i = 0; i < count; ++i )
        {
            // UGLY!!! UGLY!!!
            XenonTextureParamDesc          & t1 = mTextures[i];
            GpuProgramTextureParameterDesc & t2 = mTextures[i];
            t2.name = t1.name.cptr();
        }
    }
}
