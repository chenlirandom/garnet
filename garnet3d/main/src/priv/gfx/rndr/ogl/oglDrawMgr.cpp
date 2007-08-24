#include "pch.h"
#include "oglRenderer.h"
#include "oglLine.h"
#include "oglVtxFmt.h"
#include "oglVtxBuf.h"
#include "oglIdxBuf.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.OGL");

// *****************************************************************************
// context dumper
// *****************************************************************************

/*
struct CtxDumper
{
    bool         enabled;
    GN::DiskFile fp;

    CtxDumper() : enabled(true)
    {
        if( enabled )
        {
            fp.open( "GNcontext.xml", "wt" );
        }

        dump( "<?xml version=\"1.0\" standalone=\"yes\"?>" );
    }

    void dump( const char * fmt, ... )
    {
        using namespace GN;

        if( fp )
        {
            StrA s;

            va_list arglist;
            va_start( arglist, fmt );
            s.formatv( fmt, arglist );
            va_end( arglist );

            fprintf( fp, "%s\n", s.cptr() );
        }
    }
};
static CtxDumper sCtxDumper;

//
//
// -----------------------------------------------------------------------------
static void sDumpRendererContext( const GN::gfx::RendererContext & ctx, size_t frame )
{
    using namespace GN;
    using namespace GN::gfx;

    sCtxDumper.dump( "<RendererContext frame=\"%d\">", frame );
    sCtxDumper.dump(
        "    <flags\n"
        "        u32 = \"0x%X\"\n"
        "    />",
        ctx.flags.u32 );
    sCtxDumper.dump( "</RendererContext>" );
}

#define DUMP_STATE() if(sCtxDumper.enabled) sDumpRendererContext( mContext, mFrameCounter );
/*/
#define DUMP_STATE()
//*/

// *****************************************************************************
// local functions
// *****************************************************************************


//
/// \brief translate garnet primitive to OpenGL primitive
// ------------------------------------------------------------------------
static inline
bool sPrimitiveType2OGL( GLenum                 & oglPrim,
                         size_t                 & numidx,
                         GN::gfx::PrimitiveType   prim,
                         size_t                   numprim )
{
    switch(prim)
    {
        case GN::gfx::POINT_LIST :
            oglPrim = GL_POINTS;
            numidx = numprim;
            break;

        case GN::gfx::LINE_LIST :
            oglPrim = GL_LINES;
            numidx = numprim * 2;
            break;

        case GN::gfx::LINE_STRIP :
            oglPrim = GL_LINE_STRIP;
            numidx = numprim > 0 ? numprim + 1 : 0;
            break;

        case GN::gfx::TRIANGLE_LIST :
            oglPrim = GL_TRIANGLES;
            numidx = numprim * 3;
            break;

        case GN::gfx::TRIANGLE_STRIP :
            oglPrim = GL_TRIANGLE_STRIP;
            numidx = numprim > 0 ? numprim + 2 : 0;
            break;

        case GN::gfx::QUAD_LIST :
            oglPrim = GL_QUADS;
            numidx = numprim * 4;
            break;

        default :
            oglPrim = GL_TRIANGLES;
            numidx = numprim * 3;
            GN_ERROR(sLogger)( "invalid primitve type!" );
            return false;
    }

    return true;
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

    ++mFrameCounter;
    mDrawCounter = 0;

    // handle render window size move
    handleRenderWindowSizeMove();

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::clearScreen(
    const GN::Vector4f & c, float z, UInt8 s, BitFields flags )
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
    size_t        numprim,
    size_t        startvtx,
    size_t        minvtxidx,
    size_t        numvtx,
    size_t        startidx )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    DUMP_STATE();

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numidx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numidx, prim, numprim ),
        "Fail to map primitive!" );

    // bind vertex buffer based on current startvtx
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxfmt) &&
        mVtxFmts[mContext.vtxfmt] &&
        mVtxFmts[mContext.vtxfmt]->getNumStreams() <= mContext.numVtxBufs );
    applyVtxBuf(
        *mVtxFmts[mContext.vtxfmt],
        mContext.vtxbufs,
        startvtx );

    // get current index buffer
    GN_ASSERT( mContext.idxbuf );
    const OGLIdxBuf * ib = safeCastPtr<const OGLIdxBuf>( mContext.idxbuf );

#if GN_DEBUG_BUILD
    // Verify index buffer
    {
        OGLIdxBuf * testIb = const_cast<OGLIdxBuf*>(ib);
        const UInt16 * idxData = testIb->lock( startidx, numidx, LOCK_RO );
        for( size_t i = 0; i < numidx; ++i, ++idxData )
        {
            GN_ASSERT( minvtxidx <= *idxData && *idxData < (minvtxidx+numvtx) );
        }
        testIb->unlock();
    }
#endif

    if( GLEW_EXT_compiled_vertex_array && GLEW_EXT_draw_range_elements )
    {
        GN_OGL_CHECK( glLockArraysEXT( (GLint)minvtxidx, (GLsizei)numvtx ) );

        // draw indexed primitives
        GN_OGL_CHECK( glDrawRangeElements(
            oglPrim,
            (GLuint)minvtxidx,
            (GLuint)( minvtxidx + numvtx ),
            (GLsizei)numidx,
            GL_UNSIGNED_SHORT,
            ib->getIdxData( startidx ) ) );
        //GN_OGL_CHECK( glDrawElements( oglPrim, numidx,
        //    GL_UNSIGNED_SHORT, pib->get_dev_buffer( startidx ) ) );

        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        GN_OGL_CHECK( glDrawElements(
            oglPrim, (GLsizei)numidx, GL_UNSIGNED_SHORT, ib->getIdxData( startidx ) ) );
    }

    // success
    mNumPrims += numprim;
    ++mNumBatches;
    ++mDrawCounter;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::draw( PrimitiveType prim, size_t numprim, size_t startvtx )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    DUMP_STATE();

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numidx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numidx, prim, numprim ),
        "Fail to map primitive!" );

    // bind vertex buffer based on current startvtx
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxfmt) &&
        mVtxFmts[mContext.vtxfmt] &&
        mVtxFmts[mContext.vtxfmt]->getNumStreams() <= mContext.numVtxBufs );
    applyVtxBuf(
        *mVtxFmts[mContext.vtxfmt],
        mContext.vtxbufs,
        startvtx );

    if( GLEW_EXT_compiled_vertex_array )
    {
        // lock array if GL_EXT_compiled_vertex_array is supported
        GN_OGL_CHECK( glLockArraysEXT( 0, (GLsizei)numidx ) );

        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numidx ) );

        // NOTE : �˴���ʹ��GN_AUTOSCOPEH����Ϊ�˼򻯴��룬����ٶ�
        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numidx ) );
    }

    // success
    mNumPrims += numprim;
    ++mNumBatches;
    ++mDrawCounter;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawIndexedUp(
    PrimitiveType    prim,
    size_t           numprim,
    size_t           numvtx,
    const void *     vertexData,
    size_t           strideInBytes,
    const UInt16 * indexData )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    DUMP_STATE();

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numidx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numidx, prim, numprim ),
        "Fail to map primitive!" );

    // bind immediate vertex buffer
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxfmt) &&
        mVtxFmts[mContext.vtxfmt] &&
        1 == mVtxFmts[mContext.vtxfmt]->getNumStreams() );
    if( GLEW_ARB_vertex_buffer_object )
    {
        // disable VBO
        GN_OGL_CHECK( glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 ) );
    }
    mVtxFmts[mContext.vtxfmt]->bindBuffer(
        0, // stream index
        (const UInt8* )vertexData,
        strideInBytes );
    mNeedRebindVtxBufs |= 1;

#if GN_DEBUG_BUILD
    // Verify index buffer
    {
        const UInt16 * idxData = indexData;
        for( size_t i = 0; i < numidx; ++i, ++idxData )
        {
            GN_ASSERT( *idxData < numvtx );
        }
    }
#endif

    if( GLEW_EXT_compiled_vertex_array && GLEW_EXT_draw_range_elements )
    {
        GN_OGL_CHECK( glLockArraysEXT( 0, (GLsizei)numvtx ) );

        // draw indexed primitives
        GN_OGL_CHECK( glDrawRangeElements(
            oglPrim,
            0, // minvtxidx,
            (GLuint)numvtx,
            (GLsizei)numidx,
            GL_UNSIGNED_SHORT,
            indexData ) );
        //GN_OGL_CHECK( glDrawElements( oglPrim, numidx,
        //    GL_UNSIGNED_SHORT, pib->get_dev_buffer( startidx ) ) );

        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        GN_OGL_CHECK( glDrawElements(
            oglPrim,
            (GLsizei)numidx,
            GL_UNSIGNED_SHORT,
            indexData ) );
    }

    // success
    mNumPrims += numprim;
    ++mNumBatches;
    ++mDrawCounter;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawUp(
    PrimitiveType prim,
    size_t        numprim,
    const void *  vertexData,
    size_t        strideInBytes )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    DUMP_STATE();

    // map custom primitive to opengl primitive
    GLenum  oglPrim;
    size_t  numidx;
    GN_VERIFY_EX(
        sPrimitiveType2OGL( oglPrim, numidx, prim, numprim ),
        "Fail to map primitive!" );

    // bind immeidate vertex buffer
    GN_ASSERT(
        mVtxFmts.validHandle(mContext.vtxfmt) &&
        mVtxFmts[mContext.vtxfmt] &&
        1 == mVtxFmts[mContext.vtxfmt]->getNumStreams() );
    if( GLEW_ARB_vertex_buffer_object )
    {
        // disable VBO
        GN_OGL_CHECK( glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 ) );
    }
    mVtxFmts[mContext.vtxfmt]->bindBuffer(
        0, // stream index
        (const UInt8* )vertexData,
        strideInBytes );
    mNeedRebindVtxBufs |= 1;

    if( GLEW_EXT_compiled_vertex_array )
    {
        // lock array if GL_EXT_compiled_vertex_array is supported
        GN_OGL_CHECK( glLockArraysEXT( 0, (GLsizei)numidx ) );

        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numidx ) );

        // NOTE : �˴���ʹ��GN_AUTOSCOPEH����Ϊ�˼򻯴��룬����ٶ�
        GN_OGL_CHECK( glUnlockArraysEXT() );
    }
    else
    {
        // draw primitives
        GN_OGL_CHECK( glDrawArrays( oglPrim, 0, (GLsizei)numidx ) );
    }

    // success
    mNumPrims += numprim;
    ++mNumBatches;
    ++mDrawCounter;

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
    UInt32 rgba,
    const Matrix44f & model,
    const Matrix44f & view,
    const Matrix44f & proj )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan && mLine );

    RendererContext ctx;
    ctx.clearToNull();
    if( !(DL_USE_CURRENT_VS & options) ) ctx.setVS( 0 );
    if( !(DL_USE_CURRENT_PS & options) ) ctx.setPS( 0 );
    setContext( ctx );

    DUMP_STATE();

    mLine->drawLines( options, (const float*)positions, stride, count, rgba, model, view, proj );

    ++mDrawCounter;

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
inline void GN::gfx::OGLRenderer::applyVtxBuf(
    const GN::gfx::OGLVtxFmt & vtxfmt,
    const GN::gfx::RendererContext::VtxBufDesc * vtxbufs,
    size_t startvtx )
{
    GN_GUARD_SLOW;

    bool forceRebind = startvtx != mCurrentStartVtx;

    for( size_t i = 0; i < vtxfmt.getNumStreams(); ++i )
    {
        if( forceRebind || ( mNeedRebindVtxBufs & (1<<i) ) )
        {
            const RendererContext::VtxBufDesc & vbd = vtxbufs[i];
            if( vbd.buffer )
            {
                const UInt8 * data = safeCastPtr<const OGLBasicVtxBuf>(vbd.buffer)->getVtxData();
                vtxfmt.bindBuffer(
                    i,
                    data + vbd.offset + startvtx * vbd.stride,
                    vbd.stride );
            }
        }
    }

    // update current vertex offset
    mCurrentStartVtx = startvtx;

    // clear rebind flag
    mNeedRebindVtxBufs = 0;

    GN_UNGUARD_SLOW;
}
