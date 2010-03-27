#include "pch.h"
#include "xenonGpu.h"
#include "xenonLine.h"

///
/// static primitive map
///
static const D3DPRIMITIVETYPE PRIMITIVE_TO_XENON[GN::gfx::PrimitiveType::NUM_PRIMITIVES] =
{
    D3DPT_POINTLIST,
    D3DPT_LINELIST,
    D3DPT_LINESTRIP,
    D3DPT_TRIANGLELIST,
    D3DPT_TRIANGLESTRIP,
    D3DPT_QUADLIST,
    D3DPT_RECTLIST,
};

///
/// convert Vector4f to D3COLOR
// ------------------------------------------------------------------------
static inline D3DCOLOR sRgba2D3DCOLOR( const GN::Vector4f & c )
{
    D3DCOLOR dc = D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a);
    return dc;
}

// *****************************************************************************
// init/quit
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonGpu::drawInit()
{
    // create line renderer
    mLine = new XenonLine( *this );
    if( !mLine->Init() ) return false;

    // begin scene
    mDevice->BeginScene();
    mSceneBegun = true;

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::drawQuit()
{
    // endScene
    if( mSceneBegun )
    {
        mDevice->EndScene();
        mSceneBegun = false;
    }

    SafeDelete( mLine );
}

// *****************************************************************************
// from Gpu
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::Present()
{
    GN_ASSERT( GetCurrentThreadIdentifier() == mThreadId );

    GN_ASSERT( mSceneBegun );

    // end current scene
    {
        PIXPERF_SCOPE_EVENT( "EndScene" );
        mDevice->EndScene();
        mSceneBegun = false;
    }

    // do present
    mDevice->Present( 0, 0, 0, 0 );

    // begin next scene
    {
        PIXPERF_SCOPE_EVENT( "BeginScene" );
        mDevice->BeginScene();
        mSceneBegun = true;
    }

    ++mFrameCounter;
    mDrawCounter = 0;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::ClearScreen(
    const Vector4f & c, float z, UInt8 s, BitFields flags )
{
    GN_ASSERT( GetCurrentThreadIdentifier() == mThreadId );

    // build d3d Clear flag
    int d3dflag =
        (flags & CLEAR_C ? D3DCLEAR_TARGET : 0) |
        (flags & CLEAR_Z ? D3DCLEAR_ZBUFFER : 0) |
        (flags & CLEAR_S ? D3DCLEAR_STENCIL : 0);

    // do Clear
    mDevice->Clear( 0, 0, d3dflag, sRgba2D3DCOLOR(c), z, s );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::DrawIndexed(
    PrimitiveType prim,
    size_t        numidx,
    size_t        basevtx,
    size_t        startvtx,
    size_t        numvtx,
    size_t        startidx )
{
    // draw indexed primitives
    GN_ASSERT( prim < PrimitiveType::NUM_PRIMITIVES );
    mDevice->DrawIndexedVertices(
        PRIMITIVE_TO_XENON[prim],  // primitive type
        (UINT)basevtx,             // base vertex
        (UINT)startidx,            // base index
        (UINT)numidx );            // primitive count

    ++mDrawCounter;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::Draw(
    PrimitiveType prim, size_t numvtx, size_t startvtx )
{
    GN_ASSERT( GetCurrentThreadIdentifier() == mThreadId );

    mDevice->DrawVertices(
        PRIMITIVE_TO_XENON[prim],    // primitive type
        (UINT)startvtx,            // start vertex
        (UINT)numvtx );            // primitive count

    ++mDrawCounter;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::DrawIndexedUp(
    PrimitiveType  prim,
    size_t         numidx,
    size_t         numvtx,
    const void *   vertexData,
    size_t         strideInBytes,
    const UInt16 * indexData )
{
    PIXPERF_FUNCTION_EVENT();

    GN_ASSERT( GetCurrentThreadIdentifier() == mThreadId );

    // store vertex and index buffer
    AutoComPtr<IDirect3DVertexBuffer9> vb; UINT vbOffset; UINT vbStride;
    AutoComPtr<IDirect3DIndexBuffer9> ib;
    GN_DX_CHECK( mDevice->GetStreamSource( 0, &vb, &vbOffset, &vbStride ) );
    GN_DX_CHECK( mDevice->GetIndices( &ib ) );

    GN_DX_CHECK(
        mDevice->DrawIndexedVerticesUP(
            PRIMITIVE_TO_XENON[prim],
            0, // MinVertexIndex
            (UINT)numvtx,
            (UINT)numidx,
            indexData,
            D3DFMT_INDEX16,
            vertexData,
            (UINT)strideInBytes ) );

    // restore vertex and index buffer
    GN_DX_CHECK( mDevice->SetStreamSource( 0, vb, vbOffset, vbStride ) );
    GN_DX_CHECK( mDevice->SetIndices( ib ) );

    // success
    ++mDrawCounter;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::DrawUp(
    PrimitiveType prim,
    size_t        numvtx,
    const void *  vertexData,
    size_t        strideInBytes )
{
    PIXPERF_FUNCTION_EVENT();

    GN_ASSERT( GetCurrentThreadIdentifier() == mThreadId );

    // store vertex and index buffer
    AutoComPtr<IDirect3DVertexBuffer9> vb; UINT vbOffset; UINT vbStride;
    GN_DX_CHECK( mDevice->GetStreamSource( 0, &vb, &vbOffset, &vbStride ) );

    // do draw
    GN_DX_CHECK( mDevice->DrawVerticesUP(
        PRIMITIVE_TO_XENON[prim],
        (UINT)numvtx,
        vertexData,
        (UINT)strideInBytes ) );

    // restore vertex and index buffer
    GN_DX_CHECK( mDevice->SetStreamSource( 0, vb, vbOffset, vbStride ) );

    // success
    ++mDrawCounter;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpu::DrawLines(
    BitFields         options,
    const void      * positions,
    size_t            stride,
    size_t            numpoints,
    UInt32            rgba,
    const Matrix44f & model,
    const Matrix44f & view,
    const Matrix44f & proj )
{
    PIXPERF_FUNCTION_EVENT();

    GN_ASSERT( GetCurrentThreadIdentifier() == mThreadId );

    mLine->DrawLines(
        options,
        (const float*)positions,
        stride,
        numpoints,
        rgba,
        model, view, proj );

    ++mDrawCounter;
}
