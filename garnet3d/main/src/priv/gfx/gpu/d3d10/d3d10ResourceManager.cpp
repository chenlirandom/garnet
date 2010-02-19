#include "pch.h"
#include "d3d10Gpu.h"
#include "d3d10Shader.h"
#include "d3d10Texture.h"
#include "d3d10Buffer.h"
//#include "d3d10VtxLayout.h"
//#include "d3d10Font.h"
//#include "d3d10Quad.h"
//#include "d3d10Line.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpu.D3D10");

// *****************************************************************************
// init/shutdown
// *****************************************************************************

//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10Gpu::resourceInit()
{
    GN_GUARD;

    GN_ASSERT( mResourceList.empty() );

#ifdef HAS_CG_D3D10
    GN_DX_CHECK_RETURN( cgD3D10SetDevice( &getDeviceRefInlined() ), false );
#endif

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Gpu::resourceQuit()
{
    GN_GUARD;

    if( !mResourceList.empty() )
    {
        GN_ERROR(sLogger)( "All GPU resouces MUST be released, before destroying the GPU!" );

        std::list<D3D10Resource*>::iterator i = mResourceList.begin();
        while( i != mResourceList.end() )
        {
            D3D10Resource * r = *i;
            GN_ERROR(sLogger)( "0x%p", r );
            ++i;
        }
    }

    GN_UNGUARD;
}

// *****************************************************************************
// from Gpu
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::Blob *
GN::gfx::D3D10Gpu::compileGpuProgram( const GpuProgramDesc & gpd )
{
    GN_GUARD;

    AutoRef<SelfContainedGpuProgramDesc> s( new SelfContainedGpuProgramDesc );
    if( !s->Init( gpd ) ) return NULL;

    // success
    return s.Detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::GpuProgram *
GN::gfx::D3D10Gpu::createGpuProgram( const void * data, size_t length )
{
    GN_GUARD;

    AutoRef<SelfContainedGpuProgramDesc> s( new SelfContainedGpuProgramDesc );
    if( !s->Init( data, length ) ) return NULL;

    const GpuProgramDesc & desc = s->desc();

    if( GpuProgramLanguage::HLSL10 == desc.lang ||
        GpuProgramLanguage::HLSL9 == desc.lang )
    {
        AutoRef<D3D10GpuProgram> prog( new D3D10GpuProgram(*this) );
        if( !prog->Init( desc ) ) return NULL;
        return prog.Detach();
    }
    else
    {
        GN_ERROR(sLogger)( "Unsupported or invalid GPU program language: %d", desc.lang.ToRawEnum() );
        return NULL;
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Uniform *
GN::gfx::D3D10Gpu::createUniform( size_t size )
{
    return new SysMemUniform( size );
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Texture *
GN::gfx::D3D10Gpu::createTexture( const TextureDesc & desc )
{
    GN_GUARD;

    AutoRef<D3D10Texture> p( new D3D10Texture(*this) );
    if( !p->Init( desc ) ) return 0;
    return p.Detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::VtxBuf * GN::gfx::D3D10Gpu::createVtxBuf( const VtxBufDesc & desc )
{
    GN_GUARD;

    AutoRef<D3D10VtxBuf> buf( new D3D10VtxBuf(*this) );

    if( !buf->Init( desc ) ) return 0;

    return buf.Detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::IdxBuf * GN::gfx::D3D10Gpu::createIdxBuf( const IdxBufDesc & desc )
{
    GN_GUARD;

    AutoRef<D3D10IdxBuf> buf( new D3D10IdxBuf(*this) );

    if( !buf->Init( desc ) ) return 0;

    return buf.Detach();

    GN_UNGUARD;
}
