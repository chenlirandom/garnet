#include "pch.h"
#include "d3d9VtxBuf.h"
#include "d3d9IdxBuf.h"
#include "d3d9Texture.h"

static GN::Logger * sLogger = GN::getLogger( "GN.gfx2.D3D9KernelPort" );

// *****************************************************************************
// D3D9KernelPort
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------

// *****************************************************************************
// D3D9RenderTargetPort
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9RenderTargetPort::D3D9RenderTargetPort( D3D9GraphicsSystem & gs, BaseKernel & k, const StrA & name, UInt32 stage )
    : D3D9KernelPort(gs,k,name)
    , mStage( stage )
{
    mDesc.portType    = D3D9_KERNEL_PORT_RENDER_TARGET;
    mDesc.surfaceType = D3D9_SURFACE_TYPE_RTT_2D;
    GN_UNIMPL_WARNING();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9RenderTargetPort::compatible( const Surface * ) const
{
    GN_UNIMPL_WARNING();
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9RenderTargetPort::bind( const SurfaceView & ) const
{
    GN_UNIMPL_WARNING();
}

// *****************************************************************************
// D3D9DepthBufferPort
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9DepthBufferPort::D3D9DepthBufferPort( D3D9GraphicsSystem & gs, BaseKernel & k, const StrA & name )
    : D3D9KernelPort(gs,k,name)
{
    mDesc.portType    = D3D9_KERNEL_PORT_DEPTH_BUFFER;
    mDesc.surfaceType = D3D9_SURFACE_TYPE_RTS_DEPTH;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9DepthBufferPort::compatible( const Surface * ) const
{
    GN_UNIMPL_WARNING();
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9DepthBufferPort::bind( const SurfaceView & ) const
{
    GN_UNIMPL_WARNING();
}

// *****************************************************************************
// D3D9TexturePort
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9TexturePort::D3D9TexturePort( D3D9GraphicsSystem & gs, BaseKernel & k, const StrA & name, UInt32 stage )
    : D3D9KernelPort(gs,k,name)
    , mStage( stage )
{
    mDesc.portType    = D3D9_KERNEL_PORT_TEXTURE;
    mDesc.surfaceType = D3D9_SURFACE_TYPE_TEX;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9TexturePort::compatible( const Surface * surf ) const
{
    if( 0 == surf ) return true;

    const D3D9KernelPortDesc & portdesc = getDesc();

    const D3D9Surface * d3d9surf = safeCastPtr<const D3D9Surface>(surf);

    const D3D9SurfaceDesc & surfdesc = d3d9surf->getD3D9Desc();

    if( D3D9_SURFACE_TYPE_TEX == portdesc.surfaceType )
    {
        if( D3D9_SURFACE_TYPE_TEX      != surfdesc.type &&
            D3D9_SURFACE_TYPE_TEX_2D   != surfdesc.type &&
            D3D9_SURFACE_TYPE_TEX_3D   != surfdesc.type &&
            D3D9_SURFACE_TYPE_TEX_CUBE != surfdesc.type )
        {
            GN_ERROR(sLogger)( "Incompatible surface type!" );
            return false;
        }
    }
    else if( portdesc.surfaceType != surfdesc.type )
    {
        GN_ERROR(sLogger)( "Incompatible surface type!" );
        return false;
    }

    const SurfaceLayout & layout = surfdesc.layout;

    GN_ASSERT( 1 == layout.format.count );
    GN_ASSERT( 0 == layout.format.attribs[0].offset );
    GN_ASSERT( SurfaceAttributeSemantic::sMake("TEXEL") == layout.format.attribs[0].semantic );

    // check format
    if( layout.format.stride * 8 != gfx::getClrFmtDesc(layout.format.attribs[0].format).bits )
    {
        GN_ERROR(sLogger)( "incorrect stride." );
        return false;
    }
    if( layout.basemap.rowBytes < layout.basemap.width * layout.format.stride )
    {
        GN_ERROR(sLogger)( "incorrect row bytes." );
        return false;
    }
    if( layout.basemap.sliceBytes < layout.basemap.rowBytes * layout.basemap.height )
    {
        GN_ERROR(sLogger)( "incorrect slice bytes." );
        return false;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9TexturePort::bind( const SurfaceView & target ) const
{
    if( target.surf )
    {
        D3D9Texture * tex = safeCastPtr<D3D9Texture>(target.surf);
        d3d9gs().setTexture( mStage, tex->getSurface() );
    }
    else
    {
        d3d9gs().setTexture( mStage, 0 );
    }
}

// *****************************************************************************
// D3D9VtxBufPort
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9VtxBufPort::D3D9VtxBufPort( D3D9GraphicsSystem & gs, BaseKernel & k, const StrA & name, UInt32 stage )
    : D3D9KernelPort(gs,k,name)
    , mStage(stage)
{
    mDesc.portType    = D3D9_KERNEL_PORT_VTXBUF;
    mDesc.surfaceType = D3D9_SURFACE_TYPE_VB;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9VtxBufPort::compatible( const Surface * surf ) const
{
    if( 0 == surf ) return true;

    const D3D9Surface * d3d9surf = safeCastPtr<const D3D9Surface>(surf);

    const D3D9SurfaceDesc & desc = d3d9surf->getD3D9Desc();

    GN_ASSERT( SURFACE_DIMENSION_1D == desc.layout.dim );
    GN_ASSERT( 1 == desc.layout.faces );
    GN_ASSERT( 1 == desc.layout.levels );

    if( D3D9_SURFACE_TYPE_VB != desc.type )
    {
        GN_ERROR(sLogger)( "Vertex buffer port accepts vertex buffer surface only!" );
        return false;
    }

    if( desc.layout.basemap.rowBytes < desc.layout.basemap.width * desc.layout.format.stride )
    {
        GN_ERROR(sLogger)( "rowBytes must >= stride * width." );
        return false;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9VtxBufPort::bind( const SurfaceView & target ) const
{
    IDirect3DDevice9 * dev = d3d9gs().d3ddev();

    GN_ASSERT( target.surf );

    D3D9VtxBuf * vb = safeCastPtr<D3D9VtxBuf>(target.surf);
    const SurfaceDesc & desc = vb->getDesc();
    dev->SetStreamSource( mStage, vb->getSurface(), 0, desc.layout.format.stride );
}


// *****************************************************************************
// D3D9IdxBufPort
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D9IdxBufPort::D3D9IdxBufPort( D3D9GraphicsSystem & gs, BaseKernel & k, const StrA & name )
    : D3D9KernelPort(gs,k,name)
{
    mDesc.portType    = D3D9_KERNEL_PORT_IDXBUF;
    mDesc.surfaceType = D3D9_SURFACE_TYPE_IB;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9IdxBufPort::compatible( const Surface * surf ) const
{
    if( 0 == surf ) return true;

    const D3D9Surface * d3d9surf = safeCastPtr<const D3D9Surface>(surf);

    const D3D9SurfaceDesc & desc = d3d9surf->getD3D9Desc();

    if( D3D9_SURFACE_TYPE_IB != desc.type )
    {
        GN_ERROR(sLogger)( "Vertex buffer port accepts index buffer surface only!" );
        return false;
    }

    const SurfaceLayout & layout = desc.layout;

    GN_ASSERT( SURFACE_DIMENSION_1D == layout.dim );
    GN_ASSERT( 1 == layout.faces );
    GN_ASSERT( 1 == layout.levels );
    GN_ASSERT( 1 == layout.format.count );
    GN_ASSERT( 0 == layout.format.attribs[0].offset );
    GN_ASSERT( SurfaceAttributeSemantic::sMake("INDEX") == layout.format.attribs[0].semantic );

    // check format
    if( gfx::FMT_R_16_UINT != layout.format.attribs[0].format &&
        gfx::FMT_R_32_UINT != layout.format.attribs[0].format )
    {
        GN_ERROR(sLogger)( "incorrect index format." );
        return false;
    }
    if( layout.format.stride * 8 != gfx::getClrFmtDesc(layout.format.attribs[0].format).bits )
    {
        GN_ERROR(sLogger)( "incorrect stride." );
        return false;
    }
    if( layout.basemap.rowBytes != layout.basemap.width * layout.format.stride )
    {
        GN_ERROR(sLogger)( "incorrect row bytes." );
        return false;
    }
    if( layout.basemap.sliceBytes != layout.basemap.rowBytes )
    {
        GN_ERROR(sLogger)( "incorrect slice bytes." );
        return false;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9IdxBufPort::bind( const SurfaceView & target ) const
{
    GN_ASSERT( target.surf );
    D3D9IdxBuf * ib = safeCastPtr<D3D9IdxBuf>(target.surf);
    d3d9gs().d3ddev()->SetIndices( ib->getSurface() );
}
