#include "pch.h"
#include "oglRenderer.h"
#include "oglFont.h"
#include "oglQuad.h"
#include "oglLine.h"
#include "oglVtxFmt.h"
#include "oglVtxBuf.h"
#include "oglIdxBuf.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.OGL");

// *****************************************************************************
// local functions
// *****************************************************************************

//
/// \brief translate garnet primitive to OpenGL primitive
// ------------------------------------------------------------------------
static GN_INLINE
bool sPrimitiveType2OGL( GLenum                 & oglPrim,
                         size_t                 & numIdx,
                         GN::gfx::PrimitiveType   prim,
                         size_t                   numPrims )
{
    switch(prim)
    {
        case GN::gfx::POINT_LIST :
            oglPrim = GL_POINTS;
            numIdx = numPrims;
            break;

        case GN::gfx::LINE_LIST :
            oglPrim = GL_LINES;
            numIdx = numPrims * 2;
            break;

        case GN::gfx::LINE_STRIP :
            oglPrim = GL_LINE_STRIP;
            numIdx = numPrims > 0 ? numPrims + 1 : 0;
            break;

        case GN::gfx::TRIANGLE_LIST :
            oglPrim = GL_TRIANGLES;
            numIdx = numPrims * 3;
            break;

        case GN::gfx::TRIANGLE_STRIP :
            oglPrim = GL_TRIANGLE_STRIP;
            numIdx = numPrims > 0 ? numPrims + 2 : 0;
            break;

        case GN::gfx::QUAD_LIST :
            oglPrim = GL_QUADS;
            numIdx = numPrims * 4;
            break;

        default :
            oglPrim = GL_TRIANGLES;
            numIdx = numPrims * 3;
            GN_ERROR(sLogger)( "invalid primitve type!" );
            return false;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
static inline void sApplyVtxBuf(
    const GN::gfx::OGLVtxFmt & vtxFmt,
    const GN::gfx::RendererContext::VtxBufDesc * vtxBufs,
    size_t startVtx )
{
    GN_GUARD_SLOW;

    using namespace GN;
    using namespace GN::gfx;

    for( size_t i = 0; i < vtxFmt.getNumStreams(); ++i )
    {
        const RendererContext::VtxBufDesc & vbd = vtxBufs[i];

        if( vbd.buffer )
        {
            const UInt8 * data = safeCast<const OGLBasicVtxBuf*>(vbd.buffer)->getVtxData();
            vtxFmt.bindBuffer(
                i,
                data + vbd.offset + startVtx * vbd.stride,
                vbd.stride );
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
bool GN::gfx::OGLRenderer::drawDeviceCreate()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // create font renderer
    GN_ASSERT( !mFont );
    mFont = new OGLFont(*this);
    if( !mFont->init() ) return false;

    // create quad renderer
    GN_ASSERT( !mQuad );
    mQuad = new OGLQuad(*this);
    if( !mQuad->init() ) return false;

    // create line renderer
    GN_ASSERT( !mLine );
    mLine = new OGLLine(*this);
    if( !mLine->init() ) return false;

    // success
    return true;

    GN_UNGUARD
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawDeviceDestroy()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    safeDelete( mFont );
    safeDelete( mQuad );
    safeDelete( mLine );

    GN_UNGUARD
}

// *****************************************************************************
// interface functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::drawBegin()
{
    GN_GUARD_SLOW;

    GN_ASSERT( !mDrawBegan );

    // handle render window size move
    if( !handleRenderWindowSizeMove() ) return false;

    mDrawBegan = 1;
    mNumPrims = 0;
    mNumBatches = 0;
    return true;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawEnd()
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    mDrawBegan = 0;

#if GN_MSWIN
    GN_MSW_CHECK( ::SwapBuffers( mDeviceContext ) );
#else
    const DispDesc & dd = getDispDesc();
    GN_ASSERT( dd.displayHandle && dd.windowHandle );
    glXSwapBuffers( (Display*)dd.displayHandle, (Window)dd.windowHandle );
#endif

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::clearScreen(
    const GN::Vector4f & c, float z, UInt32 s, BitFields flags )
{
    GN_GUARD_SLOW;

    GLbitfield glflag = 0;

    // store GL attributes
    glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // clear color buffer
    if( flags & CLEAR_C )
    {
        glflag |= GL_COLOR_BUFFER_BIT;
        glClearColor( c.r, c.g, c.b, c.a );
        glColorMask( 1,1,1,1 );       // ȷ��COLOR BUFFER��д
    }

    // clean z-buffer
    if( flags & CLEAR_Z )
    {
        glflag |= GL_DEPTH_BUFFER_BIT;
        glClearDepth( z );
        glDepthMask( 1 );             // ȷ��Z-BUFFER��д
    }

    // clearn stencil buffer
    if( flags & CLEAR_S )
    {
        glflag |= GL_STENCIL_BUFFER_BIT;
        glClearStencil( s );
        glStencilMask( 0xFFFFFFFF );  // ȷ��STENCIL BUFFER��д
    }

    // do clear
    GN_OGL_CHECK( glClear( glflag ) );

    // restore GL attributes
    glPopAttrib();

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawIndexed(
    PrimitiveType prim,
    size_t        numPrims,
    size_t        startVtx,
    size_t        minVtxIdx,
    size_t        numVtx,
    size_t        startIdx )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numIdx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numIdx, prim, numPrims ),
        "Fail to map primitive!" );

    // bind vertex buffer based on current startVtx
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxFmt) &&
        mVtxFmts[mContext.vtxFmt] &&
        mVtxFmts[mContext.vtxFmt]->getNumStreams() <= mContext.numVtxBufs );
    sApplyVtxBuf(
        *mVtxFmts[mContext.vtxFmt],
        mContext.vtxBufs,
        startVtx );

    // get current index buffer
    GN_ASSERT( mContext.idxBuf );
    const OGLIdxBuf * ib = safeCast<const OGLIdxBuf*>( mContext.idxBuf );

#if GN_DEBUG_BUILD
    // Verify index buffer
    {
        OGLIdxBuf * testIb = const_cast<OGLIdxBuf*>(ib);
        const UInt16 * idxData = testIb->lock( startIdx, numIdx, LOCK_RO );
        for( size_t i = 0; i < numIdx; ++i, ++idxData )
        {
            GN_ASSERT( minVtxIdx <= *idxData && *idxData < (minVtxIdx+numVtx) );
        }
        testIb->unlock();
    }
#endif

    if( GLEW_EXT_compiled_vertex_array && GLEW_EXT_draw_range_elements )
    {
        GN_OGL_CHECK( glLockArraysEXT( (GLint)minVtxIdx, (GLsizei)numVtx ) );

        // draw indexed primitives
        GN_OGL_CHECK( glDrawRangeElements(
            oglPrim,
            (GLuint)minVtxIdx,
            (GLuint)( minVtxIdx + numVtx ),
            (GLsizei)numIdx,
            GL_UNSIGNED_SHORT,
            ib->getIdxData( startIdx ) ) );
        //GN_OGL_CHECK( glDrawElements( oglPrim, numIdx,
        //    GL_UNSIGNED_SHORT, pib->get_dev_buffer( startIdx ) ) );

        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        GN_OGL_CHECK( glDrawElements(
            oglPrim, (GLsizei)numIdx, GL_UNSIGNED_SHORT, ib->getIdxData( startIdx ) ) );
    }

    // success
    mNumPrims += numPrims;
    ++mNumBatches;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::draw( PrimitiveType prim, size_t numPrims, size_t startVtx )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numIdx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numIdx, prim, numPrims ),
        "Fail to map primitive!" );

    // bind vertex buffer based on current startVtx
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxFmt) &&
        mVtxFmts[mContext.vtxFmt] &&
        mVtxFmts[mContext.vtxFmt]->getNumStreams() <= mContext.numVtxBufs );
    sApplyVtxBuf(
        *mVtxFmts[mContext.vtxFmt],
        mContext.vtxBufs,
        startVtx );

    if( GLEW_EXT_compiled_vertex_array )
    {
        // lock array if GL_EXT_compiled_vertex_array is supported
        GN_OGL_CHECK( glLockArraysEXT( 0, (GLsizei)numIdx ) );

        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numIdx ) );

        // NOTE : �˴���ʹ��GN_AUTOSCOPEH����Ϊ�˼򻯴��룬����ٶ�
        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numIdx ) );
    }

    // success
    mNumPrims += numPrims;
    ++mNumBatches;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawIndexedUp(
    PrimitiveType    prim,
    size_t           numPrims,
    size_t           numVtx,
    const void *     vertexData,
    size_t           strideInBytes,
    const UInt16 * indexData )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numIdx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numIdx, prim, numPrims ),
        "Fail to map primitive!" );

    // bind vertex buffer based on current startVtx
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxFmt) &&
        mVtxFmts[mContext.vtxFmt] &&
        1 == mVtxFmts[mContext.vtxFmt]->getNumStreams() );
    mVtxFmts[mContext.vtxFmt]->bindBuffer(
        0, // stream index
        (const UInt8* )vertexData,
        strideInBytes );

#if GN_DEBUG_BUILD
    // Verify index buffer
    {
        const UInt16 * idxData = indexData;
        for( size_t i = 0; i < numIdx; ++i, ++idxData )
        {
            GN_ASSERT( *idxData < numVtx );
        }
    }
#endif

    if( GLEW_EXT_compiled_vertex_array && GLEW_EXT_draw_range_elements )
    {
        GN_OGL_CHECK( glLockArraysEXT( 0, (GLsizei)numVtx ) );

        // draw indexed primitives
        GN_OGL_CHECK( glDrawRangeElements(
            oglPrim,
            0, // minVtxIdx,
            (GLuint)numVtx,
            (GLsizei)numIdx,
            GL_UNSIGNED_SHORT,
            indexData ) );
        //GN_OGL_CHECK( glDrawElements( oglPrim, numIdx,
        //    GL_UNSIGNED_SHORT, pib->get_dev_buffer( startIdx ) ) );

        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        GN_OGL_CHECK( glDrawElements(
            oglPrim,
            (GLsizei)numIdx,
            GL_UNSIGNED_SHORT,
            indexData ) );
    }

    // success
    mNumPrims += numPrims;
    ++mNumBatches;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawUp(
    PrimitiveType prim,
    size_t        numPrims,
    const void *  vertexData,
    size_t        strideInBytes )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numIdx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numIdx, prim, numPrims ),
        "Fail to map primitive!" );

    // bind vertex buffer based on current startVtx
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxFmt) &&
        mVtxFmts[mContext.vtxFmt] &&
        1 == mVtxFmts[mContext.vtxFmt]->getNumStreams() );
    mVtxFmts[mContext.vtxFmt]->bindBuffer(
        0, // stream index
        (const UInt8* )vertexData,
        strideInBytes );

    if( GLEW_EXT_compiled_vertex_array )
    {
        // lock array if GL_EXT_compiled_vertex_array is supported
        GN_OGL_CHECK( glLockArraysEXT( 0, (GLsizei)numIdx ) );

        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numIdx ) );

        // NOTE : �˴���ʹ��GN_AUTOSCOPEH����Ϊ�˼򻯴��룬����ٶ�
        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numIdx ) );
    }

    // success
    mNumPrims += numPrims;
    ++mNumBatches;

    GN_UNGUARD_SLOW;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawQuads(
    BitFields options,
    const void * positions, size_t posStride,
    const void * texcoords, size_t texStride,
    const void * colors, size_t clrStride,
    size_t count )
{
    GN_GUARD_SLOW;
    GN_ASSERT( mDrawBegan && mQuad );
    contextUpdateBegin();
    if( !(DQ_USE_CURRENT_VS & options) ) setVS( 0 );
    if( !(DQ_USE_CURRENT_PS & options) ) setPS( 0 );
    contextUpdateEnd();
    mQuad->drawQuads(
        options,
        (const float*)positions, posStride,
        (const float*)texcoords, texStride,
        (const UInt32*)colors, clrStride,
        count );
    GN_UNGUARD_SLOW;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawLines(
    BitFields options,
    const void * positions,
    size_t stride,
    size_t count,
    UInt32 color,
    const Matrix44f & model,
    const Matrix44f & view,
    const Matrix44f & proj )
{
    GN_GUARD_SLOW;
    GN_ASSERT( mDrawBegan && mQuad );
    contextUpdateBegin();
    if( !(DL_USE_CURRENT_VS & options) ) setVS( 0 );
    if( !(DL_USE_CURRENT_PS & options) ) setPS( 0 );
    contextUpdateEnd();
    mLine->drawLines( options, (const float*)positions, stride, count, color, model, view, proj );
    GN_UNGUARD_SLOW;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawDebugText( const char * s, int x, int y, const Vector4f & c )
{
    GN_GUARD_SLOW;
    GN_ASSERT( mDrawBegan && mFont );
    setShaders( 0, 0, 0 ); // disable programmable pipeline
    mFont->drawText( s, x, y, c );
    GN_UNGUARD_SLOW;
}
