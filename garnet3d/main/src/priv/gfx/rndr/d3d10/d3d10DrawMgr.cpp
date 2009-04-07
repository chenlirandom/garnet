#include "pch.h"
#include "d3d10Renderer.h"
#include "d3d10RenderTargetMgr.h"

// static primitive map
static D3D10_PRIMITIVE_TOPOLOGY sD3D10PrimMap[GN::gfx::NUM_PRIMITIVES] =
{
    D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
    D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
    D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
    D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, // no quad list
    D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, // no rect list
};

// *****************************************************************************
// interface functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::present()
{
    GN_DX10_CHECK( mSwapChain->Present( getOptions().vsync ? 1 : 0, 0 ) );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::clearScreen(
    const GN::Vector4f & c, float z, UInt8 s, BitFields flags )
{
    GN_GUARD_SLOW;

    if( CLEAR_C & flags )
    {
        for( UInt32 i = 0; i < mRTMgr->getRenderTargetCount(); ++i )
        {
            mDevice->ClearRenderTargetView( mRTMgr->getRenderTargetView( i ) , c );
        }
    }

    ID3D10DepthStencilView * dsview = mRTMgr->getDepthStencilView();
    UInt32 d3dflag = 0;
    if( CLEAR_Z ) d3dflag |= D3D10_CLEAR_DEPTH;
    if( CLEAR_S ) d3dflag |= D3D10_CLEAR_STENCIL;
    if( d3dflag && dsview )
    {
        mDevice->ClearDepthStencilView( dsview, d3dflag, z, s );
    }

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::drawIndexed(
    PrimitiveType prim,
    size_t        numprim,
    size_t        startvtx,
    size_t        , // minvtxidx
    size_t        , // numvtx
    size_t        startidx )
{
    mDevice->IASetPrimitiveTopology( sD3D10PrimMap[prim] );
    mDevice->DrawIndexed(
        (UInt32)calcVertexCount( prim, numprim ),
        (UINT)startidx,
        (INT)startvtx );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::draw(
    PrimitiveType prim,
    size_t        numprim,
    size_t        startvtx )
{
    mDevice->IASetPrimitiveTopology( sD3D10PrimMap[prim] );
    mDevice->Draw( (UInt32)calcVertexCount( prim, numprim ), (UINT)startvtx );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::drawIndexedUp(
    PrimitiveType  /*prim*/,
    size_t         /*numprim*/,
    size_t         /*numvtx*/,
    const void   * /*vertexData*/,
    size_t         /*strideInBytes*/,
    const UInt16 * /*indexData*/ )
{
    GN_UNIMPL_WARNING();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::drawUp(
    PrimitiveType /*prim*/,
    size_t        /*numprim*/,
    const void *  /*vertexData*/,
    size_t        /*strideInBytes*/ )
{
    GN_UNIMPL_WARNING();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::drawLines(
    BitFields         /*options*/,
    const void      * /*positions*/,
    size_t            /*stride*/,
    size_t            /*numPoints*/,
    UInt32            /*rgba*/,
    const Matrix44f & /*model*/,
    const Matrix44f & /*view*/,
    const Matrix44f & /*proj*/ )
{
    GN_UNIMPL_WARNING();
}
