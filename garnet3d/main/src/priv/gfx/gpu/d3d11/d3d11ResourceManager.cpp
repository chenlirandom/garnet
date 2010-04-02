#include "pch.h"
#include "d3d11Gpu.h"
#include "d3d11Resource.h"
#include "../common/basicShader.h"
#include "d3d11Shader.h"
#include "d3d11Texture.h"
#include "d3d11Buffer.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpu.D3D11");

// *****************************************************************************
// init/shutdown
// *****************************************************************************

//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D11Gpu::resourceInit()
{
    GN_GUARD;

    GN_ASSERT( mResourceList.empty() );

#ifdef HAS_CG_D3D11
    GN_DX_CHECK_RETURN( cgD3D11SetDevice( &getDeviceRefInlined() ), false );
#endif

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D11Gpu::resourceQuit()
{
    GN_GUARD;

    if( !mResourceList.empty() )
    {
        GN_ERROR(sLogger)( "All graphics resouces MUST be released, before destroying GPU!" );

        std::list<D3D11Resource*>::iterator i = mResourceList.begin();
        while( i != mResourceList.end() )
        {
            D3D11Resource * r = *i;
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
GN::gfx::D3D11Gpu::CompileGpuProgram( const GpuProgramDesc & gpd )
{
    AutoRef<SelfContainedGpuProgramDesc> s( new SelfContainedGpuProgramDesc );
    if( !s->Init( gpd ) ) return NULL;

    // success
    return s.Detach();
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::GpuProgram *
GN::gfx::D3D11Gpu::CreateGpuProgram( const void * data, size_t length )
{
    AutoRef<SelfContainedGpuProgramDesc> s( new SelfContainedGpuProgramDesc );
    if( !s->Init( data, length ) ) return NULL;

    const GpuProgramDesc & desc = s->desc();

    if( GpuProgramLanguage::HLSL11 == desc.lang ||
        GpuProgramLanguage::HLSL10 == desc.lang ||
        GpuProgramLanguage::HLSL9 == desc.lang )
    {
        AutoRef<D3D11GpuProgram> prog( new D3D11GpuProgram(*this) );
        if( !prog->Init( desc ) ) return NULL;
        return prog.Detach();
    }
    else
    {
        GN_ERROR(sLogger)( "Unsupported or invalid GPU program language: %d", desc.lang.ToRawEnum() );
        return NULL;
    }
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Uniform *
GN::gfx::D3D11Gpu::CreateUniform( size_t size )
{
    return new SysMemUniform( size );
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Texture *
GN::gfx::D3D11Gpu::CreateTexture( const TextureDesc & desc )
{
    AutoRef<D3D11Texture> p( new D3D11Texture(*this) );
    if( !p->Init( desc ) ) return 0;
    return p.Detach();
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::VtxBuf * GN::gfx::D3D11Gpu::CreateVtxBuf( const VtxBufDesc & desc )
{
    AutoRef<D3D11VtxBuf> buf( new D3D11VtxBuf(*this) );

    if( !buf->Init( desc ) ) return 0;

    return buf.Detach();
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::IdxBuf * GN::gfx::D3D11Gpu::CreateIdxBuf( const IdxBufDesc & desc )
{
    AutoRef<D3D11IdxBuf> buf( new D3D11IdxBuf(*this) );

    if( !buf->Init( desc ) ) return 0;

    return buf.Detach();
}