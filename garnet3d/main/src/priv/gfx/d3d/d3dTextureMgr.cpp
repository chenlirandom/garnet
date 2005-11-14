#include "pch.h"
#include "d3dRenderer.h"
#include "d3dTexture.h"
#if !GN_ENABLE_INLINE
#include "d3dTextureMgr.inl"
#endif

// *****************************************************************************
// interface functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::Texture *
GN::gfx::D3DRenderer::createTexture( TexType textype,
                                     uint32_t sx, uint32_t sy, uint32_t sz,
                                     uint32_t levels,
                                     ClrFmt format,
                                     uint32_t usages )
{
    GN_GUARD;

    AutoRef<D3DTexture> p( new D3DTexture(*this) );
    if( p->init(textype,sx,sy,sz,levels,format,usages) ) return p.detach();
    return 0;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::Texture *
GN::gfx::D3DRenderer::createTextureFromFile( File & file )
{
    GN_GUARD;

    AutoRef<D3DTexture> p( new D3DTexture(*this) );
    if( p->initFromFile(file) ) return p.detach();
    return 0;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::bindTextures( const Texture * const texlist[],
                                         uint32_t start, uint32_t numtex )
{
    GN_GUARD_SLOW;

    GN_ASSERT( texlist || (0==numtex) );

    // apply texture list
    uint32_t end = start + numtex;
    GN_ASSERT( end < getCaps(CAPS_MAX_TEXTURE_STAGES) );
    for ( uint32_t i = start; i < end; ++i )
    {
        const Texture * tex = texlist[i-start];

        if( 0 == tex )
        {
            mDevice->SetTexture( i, NULL );
        }
        else
        {
            const D3DTexture * p = safeCast<const D3DTexture*>(tex);

            mDevice->SetTexture( i, p->getD3DTexture() );

            // update texture parameters
            updateTextureFilters( i, p->getD3DFilters() );
            updateTextureWraps( i, p->getD3DWraps() );
        }
    }

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DRenderer::textureDeviceRestore()
{
    //GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // reset texture parameters
    for( uint32_t i = 0; i < RenderStateBlockDesc::MAX_STAGES; ++i )
    {
        TexParameters & tp = mTexParameters[i];
        tp.min = tp.mag = tp.mip = D3DTEXF_FORCE_DWORD;
        tp.s = tp.t = tp.r = tp.q = D3DTADDRESS_FORCE_DWORD;
    }

    // successful
    return true;

    //GN_UNGUARD;
}
