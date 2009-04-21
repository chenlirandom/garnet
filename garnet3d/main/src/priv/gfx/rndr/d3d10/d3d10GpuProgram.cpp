#include "pch.h"
#include "d3d10Shader.h"
#include "d3d10Renderer.h"
#include "d3d10Texture.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.D3D10");

using namespace GN;
using namespace GN::gfx;

// *****************************************************************************
// D3D10GpuProgramParameterDesc
// *****************************************************************************

template<class T>
static void sFreeNameString( T & t )
{
    heapFree( (void*)t.name );
}

template<class T>
class FindByName
{
    const char * mName;

public:

    FindByName( const char * name ) : mName(name)
    {
    }

    bool operator()( const T & t ) const
    {
        return 0 == strCmp( t.name, mName );
    }
};

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D10GpuProgramParameterDesc::D3D10GpuProgramParameterDesc()
{
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D10GpuProgramParameterDesc::~D3D10GpuProgramParameterDesc()
{
    // delete all name string
    std::for_each( mUniforms.begin(), mUniforms.end(), &sFreeNameString<D3D10UniformParameterDesc> );
    std::for_each( mTextures.begin(), mTextures.end(), &sFreeNameString<D3D10TextureParameterDesc> );
    std::for_each( mAttributes.begin(), mAttributes.end(), &sFreeNameString<D3D10AttributeParameterDesc> );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10GpuProgramParameterDesc::buildParameterArrays()
{
    mUniformArray       = mUniforms.cptr();
    mUniformCount       = mUniforms.size();
    mUniformArrayStride = sizeof(mUniforms[0]);

    mTextureArray       = mTextures.cptr();
    mTextureCount       = mTextures.size();
    mTextureArrayStride = sizeof(mTextures[0]);

    mAttributeArray       = mAttributes.cptr();
    mAttributeCount       = mAttributes.size();
    mAttributeArrayStride = sizeof(mAttributes[0]);
}

//
//
// -----------------------------------------------------------------------------
const GN::gfx::D3D10UniformParameterDesc *
GN::gfx::D3D10GpuProgramParameterDesc::findUniform( const char * name ) const
{
    const GN::gfx::D3D10UniformParameterDesc * p = std::find_if(
        mUniforms.begin(),
        mUniforms.end(),
        FindByName<D3D10UniformParameterDesc>(name) );

    return ( p == mUniforms.end() ) ? NULL : p;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D10UniformParameterDesc *
GN::gfx::D3D10GpuProgramParameterDesc::findUniform( const char * name )
{
    GN::gfx::D3D10UniformParameterDesc * p = std::find_if(
        mUniforms.begin(),
        mUniforms.end(),
        FindByName<D3D10UniformParameterDesc>(name) );

    return ( p == mUniforms.end() ) ? NULL : p;
}

//
//
// -----------------------------------------------------------------------------
const GN::gfx::D3D10TextureParameterDesc *
GN::gfx::D3D10GpuProgramParameterDesc::findTexture( const char * name ) const
{
    const GN::gfx::D3D10TextureParameterDesc * p = std::find_if(
        mTextures.begin(),
        mTextures.end(),
        FindByName<D3D10TextureParameterDesc>(name) );

    return ( p == mTextures.end() ) ? NULL : p;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D10TextureParameterDesc *
GN::gfx::D3D10GpuProgramParameterDesc::findTexture( const char * name )
{
    GN::gfx::D3D10TextureParameterDesc * p = std::find_if(
        mTextures.begin(),
        mTextures.end(),
        FindByName<D3D10TextureParameterDesc>(name) );

    return ( p == mTextures.end() ) ? NULL : p;
}

//
//
// -----------------------------------------------------------------------------
const GN::gfx::D3D10AttributeParameterDesc *
GN::gfx::D3D10GpuProgramParameterDesc::findAttribute( const char * name ) const
{
    const GN::gfx::D3D10AttributeParameterDesc * p = std::find_if(
        mAttributes.begin(),
        mAttributes.end(),
        FindByName<D3D10AttributeParameterDesc>(name) );

    return ( p == mAttributes.end() ) ? NULL : p;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D10AttributeParameterDesc *
GN::gfx::D3D10GpuProgramParameterDesc::findAttribute( const char * name )
{
    GN::gfx::D3D10AttributeParameterDesc * p = std::find_if(
        mAttributes.begin(),
        mAttributes.end(),
        FindByName<D3D10AttributeParameterDesc>(name) );

    return ( p == mAttributes.end() ) ? NULL : p;
}

// *****************************************************************************
// D3D10GpuProgram
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
static inline void
sUpdateConstBuffer(
    ID3D10Device & dev,
    ID3D10Buffer & buf,
    const void   * data,
    size_t         size )
{
#if 0
        dev.UpdateSubresource(
            &buf,   // destination buffer
            0,      // sub resource
            0,      // box
            data,   // source data
            size,   // row pitch
            size ); // slice pitch
#else
        GN_UNUSED_PARAM( dev );
        void * dst;
        GN_DX10_CHECK_R( buf.Map( D3D10_MAP_WRITE_DISCARD, 0, &dst ) );
        memcpy( dst, data, size );
        buf.Unmap();
#endif
}

//
//
// -----------------------------------------------------------------------------
static inline void
sUpdateConstData(
    const D3D10UniformParameterDesc & desc,
    const SysMemUniform             & uniform,
    SysMemConstBufferArray          & cbarray,
    size_t                            shaderType, // 0: VS, 1: GS, 2: PS
    bool                            * dirtyFlags )
{
    const D3D10UniformParameterDesc::ShaderSpecificProperties & ssp = desc.ssp[shaderType];

    // do nothing, if the uniform is not used by the shader
    if( !ssp.used ) return;

    DynaArray<UInt8>             & cb = cbarray[ssp.cbidx];
    SafeArrayAccessor<const UInt8> src( (const UInt8*)uniform.getval(), uniform.size() );
    SafeArrayAccessor<UInt8>       dst( cb.cptr(), cb.size() );

    // copy uniform data to system const buffer
    src.copyTo(
        0,          // src offset
        dst,        // dst buffer
        ssp.offset, // dst offset
        desc.size   // size in bytes
        );

    // update dirty flag
    dirtyFlags[ssp.cbidx] = true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10GpuProgram::init( const GpuProgramDesc & desc, bool hlsl9 )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( D3D10GpuProgram, () );

    // covert shader compile options
    D3D10ShaderCompileOptions options;
    options.compileFlags = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR; // use row major matrix all the time.
    if( !desc.optimize ) options.compileFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
    if( !desc.debug ) options.compileFlags |= D3D10_SHADER_DEBUG;
    if( hlsl9 )
        options.compileFlags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
    else
        options.compileFlags |= D3D10_SHADER_ENABLE_STRICTNESS;

    // compile all shaders
    ID3D10Device & dev = getDeviceRef();
    if( !mVs.init( dev, desc.vs, options, mParamDesc ) ) return failure();
    if( !mGs.init( dev, desc.gs, options, mParamDesc ) ) return failure();
    if( !mPs.init( dev, desc.ps, options, mParamDesc ) ) return failure();

    // build parameter array
    mParamDesc.buildParameterArrays();

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10GpuProgram::quit()
{
    GN_GUARD;

    mVs.clear();
    mGs.clear();
    mPs.clear();

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10GpuProgram::applyUniforms(
    const SysMemUniform * const * uniforms,
    size_t                        count,
    bool                          skipDirtyCheck ) const
{
    GN_UNUSED_PARAM( skipDirtyCheck );

    count = math::getmin( count, mParamDesc.uniforms.count() );

    // dirty flags
    bool vscDirty[mVs.constBufs.MAX_SIZE];
    bool gscDirty[mGs.constBufs.MAX_SIZE];
    bool pscDirty[mPs.constBufs.MAX_SIZE];
    ::memset( vscDirty, 0, sizeof(vscDirty) );
    ::memset( gscDirty, 0, sizeof(gscDirty) );
    ::memset( pscDirty, 0, sizeof(pscDirty) );

    // copy uniform data into constant data array
    for( size_t i = 0; i < count; ++i )
    {
        const D3D10UniformParameterDesc & ud = (const D3D10UniformParameterDesc &)mParamDesc.uniforms[i];
        const SysMemUniform             & u  = *uniforms[i];

        sUpdateConstData( ud, u, mVs.constData, 0, vscDirty );
        sUpdateConstData( ud, u, mGs.constData, 1, gscDirty );
        sUpdateConstData( ud, u, mPs.constData, 2, pscDirty );
    }

    ID3D10Device & dev = getDeviceRef();

    // update vertex shader constant buffers
    for( size_t i = 0; i < mVs.constBufs.size(); ++i )
    {
        if( skipDirtyCheck || vscDirty[i] )
        {
            ID3D10Buffer           & buf = *mVs.constBufs[i];
            const DynaArray<UInt8> & data = mVs.constData[i];
            sUpdateConstBuffer( dev, buf, data.cptr(), data.size() );
        }
    }

    // update geometry shader constant buffers
    for( size_t i = 0; i < mGs.constBufs.size(); ++i )
    {
        if( skipDirtyCheck || gscDirty[i] )
        {
            ID3D10Buffer           & buf = *mGs.constBufs[i];
            const DynaArray<UInt8> & data = mGs.constData[i];
            sUpdateConstBuffer( dev, buf, data.cptr(), data.size() );
        }
    }

    // update pixel shader constant buffers
    for( size_t i = 0; i < mPs.constBufs.size(); ++i )
    {
        if( skipDirtyCheck || pscDirty[i] )
        {
            ID3D10Buffer           & buf = *mPs.constBufs[i];
            const DynaArray<UInt8> & data = mPs.constData[i];
            sUpdateConstBuffer( dev, buf, data.cptr(), data.size() );
        }
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10GpuProgram::applyTextures(
    const TextureBinding * bindings,
    size_t                 count,
    bool                   /*skipDirtyCheck*/ ) const
{
    const size_t NUM_STAGES = getRenderer().getCaps().maxTextures;

    // allocate SRV array on stack, clear to zero.
    const size_t SRV_ARRAY_SIZE = sizeof(void*) * NUM_STAGES * 3;
    ID3D10ShaderResourceView ** srvArray = (ID3D10ShaderResourceView **)alloca( SRV_ARRAY_SIZE );
    memset( srvArray, 0, SRV_ARRAY_SIZE );

    // iterate textures
    for( size_t i = 0; i < count; ++i )
    {
        const TextureBinding & tb = bindings[i];

        D3D10Texture * tex = (D3D10Texture*)tb.texture.get();

        if( tex )
        {
            const D3D10TextureParameterDesc * texParam = mParamDesc.findTexture( tb.binding );
            if( texParam )
            {
                for( int i = 0; i < 3; ++i )
                {
                    if( texParam->ssp[i].used )
                    {
                        size_t srvidx = NUM_STAGES * i + texParam->ssp[i].stage;

                        srvArray[srvidx] = tex->getSRView();
                    }
                }
            }
            else
            {
                GN_ERROR(sLogger)( "texture %d is binding to invalid GPU parameter named \"%s\".", i, tb.binding );
            }
        }
    }

    ID3D10Device & dev = getDeviceRef();

    dev.VSSetShaderResources( 0, NUM_STAGES, srvArray );
    dev.GSSetShaderResources( 0, NUM_STAGES, srvArray + NUM_STAGES );
    dev.PSSetShaderResources( 0, NUM_STAGES, srvArray + NUM_STAGES * 2 );
}
