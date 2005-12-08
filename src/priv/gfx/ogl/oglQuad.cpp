#include "pch.h"
#include "oglQuad.h"
#include "oglRenderer.h"

struct OGLQuadStruct
{
    GN::Vector2f p0;
    GN::Vector2f t0;

    GN::Vector2f p1;
    GN::Vector2f t1;

    GN::Vector2f p2;
    GN::Vector2f t2;

    GN::Vector2f p3;
    GN::Vector2f t3;
};

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLQuad::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::OGLQuad, () );

    // create render state block
    RenderStateBlockDesc rsbd( RenderStateBlockDesc::RESET_TO_DEFAULT );
    rsbd.rs[RS_DEPTH_TEST] = RSV_TRUE;
    rsbd.rs[RS_DEPTH_WRITE] = RSV_FALSE;
    rsbd.rs[RS_CULL_MODE] = RSV_CULL_NONE;
    mRsb = mRenderer.createRenderStateBlock( rsbd );
    if( 0 == mRsb ) { quit(); return selfOK(); }

    // create vertex binding
    VtxFmtDesc vfd;
    vfd.addAttrib( 0, 0, VTXSEM_COORD, FMT_FLOAT2 );
    vfd.addAttrib( 0, 8, VTXSEM_TEX0, FMT_FLOAT2 );
    mVtxBinding = mRenderer.createVtxBinding( vfd );
    if( 0 == mVtxBinding ) { quit(); return selfOK(); }

    // create vertex buffer
    GN_ASSERT( !mVtxBuf );
    mVtxBuf = mRenderer.createVtxBuf( sizeof(OGLQuadStruct) * MAX_QUADS, true );
    if( 0 == mVtxBuf ) { quit(); return selfOK(); }

    // create index buffer
    GN_ASSERT( !mIdxBuf );
    mIdxBuf = mRenderer.createIdxBuf( MAX_QUADS * 6 );
    if( 0 == mIdxBuf ) { quit(); return selfOK(); }

    // fill index buffer
    uint16_t * ibData = (uint16_t*)mIdxBuf->lock( 0, 0, 0 );
    if( 0 == ibData ) { quit(); return selfOK(); }
    for( uint16_t i = 0; i < MAX_QUADS; ++i )
    {
        ibData[i*6+0] = i*4+0;
        ibData[i*6+1] = i*4+1;
        ibData[i*6+2] = i*4+2;
        ibData[i*6+3] = i*4+0;
        ibData[i*6+4] = i*4+2;
        ibData[i*6+5] = i*4+3;
    }
    mIdxBuf->unlock();

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLQuad::quit()
{
    GN_GUARD;

    safeDecref( mVtxBuf );
    safeDecref( mIdxBuf );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLQuad::drawQuads(
    const Vector2f * positions, size_t posStride,
    const Vector2f * texcoords, size_t texStride,
    size_t count )
{
    GN_GUARD_SLOW;

    GN_ASSERT( selfOK() );

    if( 0 == positions || 0 == texcoords )
    {
        GNGFX_ERROR( "NULL parameter(s)!" );
        return;
    }

    if( 0 == posStride || 0 == texStride )
    {
        GNGFX_ERROR( "stride can't be zero!" );
        return;
    }

    GN_ASSERT( mNextQuad < MAX_QUADS );

    // handle large amount of array
    while( count + mNextQuad > MAX_QUADS )
    {
        size_t n = MAX_QUADS - mNextQuad;
        GN_ASSERT( n > 0 );
        drawQuads( positions, posStride, texcoords, texStride, n );
        positions = (const Vector2f*)( ((const uint8_t*)positions) + n * posStride );
        texcoords = (const Vector2f*)( ((const uint8_t*)texcoords) + n * texStride );
        count -= n;
    }

    // lock vertex buffer
    OGLQuadStruct * vbData;
    if( 0 == mNextQuad )
    {
        vbData = (OGLQuadStruct*)mVtxBuf->lock( 0, 0, LOCK_DISCARD );
    }
    else
    {
        vbData = (OGLQuadStruct*)mVtxBuf->lock(
            sizeof(OGLQuadStruct)*mNextQuad,
            sizeof(OGLQuadStruct)*count,
            LOCK_NO_OVERWRITE );
    }
    if( 0 == vbData ) return;

    // fill vertex data
    for( size_t i = 0; i < count; ++i )
    {
        OGLQuadStruct & v = vbData[i];

        v.p0 = positions[0];
        v.p1 = positions[1];
        v.p2 = positions[2];
        v.p3 = positions[3];
        v.t0 = texcoords[0];
        v.t1 = texcoords[1];
        v.t2 = texcoords[2];
        v.t3 = texcoords[3];

        positions = (const Vector2f*)( ((const uint8_t*)positions) + posStride );
        texcoords = (const Vector2f*)( ((const uint8_t*)texcoords) + texStride );
    }

    // unlock the buffer
    mVtxBuf->unlock();

    if( !( DQ_USE_CURRENT_VS & mDrawOptions ) )
    {
        // push OGL attributes
        GN_OGL_CHECK( glPushAttrib( GL_TRANSFORM_BIT ) );

        // TODO: setup OGL matrices
        if( DQ_WINDOW_SPACE & mDrawOptions )
        {
            // position is in screen space (0,0)->(1,1)
            GN_OGL_CHECK( glMatrixMode( GL_PROJECTION ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadIdentity() );
            GN_OGL_CHECK( glOrtho( 0, 1, 1, 0, 0, 1 ) );

            GN_OGL_CHECK( glMatrixMode( GL_MODELVIEW ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadIdentity() );
        }
        else
        {
            GLdouble vp[4];
            GN_OGL_CHECK( glGetDoublev( GL_VIEWPORT, vp ) );
            
            // position is in screen space (0,0)->(width,height)
            GN_OGL_CHECK( glMatrixMode( GL_PROJECTION ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadIdentity() );
            GN_OGL_CHECK( glOrtho( 0, vp[2], vp[3], 0, 0, 1 ) );

            GN_OGL_CHECK( glMatrixMode( GL_MODELVIEW ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadIdentity() );
        }
    }

    // draw
    mRenderer.drawIndexed(
        TRIANGLE_LIST,
        count * 2,     // numPrim
        mNextQuad * 4, // baseVtx
        0,             // minVtxIdx
        count * 4,     // numVtx
        0 );           // startIdx

    if( !( DQ_USE_CURRENT_VS & mDrawOptions ) )
    {
        // restore OGL matrices
        GN_OGL_CHECK( glMatrixMode( GL_PROJECTION ) );
        GN_OGL_CHECK( glPopMatrix() );
        GN_OGL_CHECK( glMatrixMode( GL_MODELVIEW ) );
        GN_OGL_CHECK( glPopMatrix() );

        // restore OGL attributes
        GN_OGL_CHECK( glPopAttrib() );
    }

    // update mNextQuad
    mNextQuad += count;
    GN_ASSERT( mNextQuad <= MAX_QUADS );
    if( MAX_QUADS == mNextQuad ) mNextQuad = 0;

    GN_UNGUARD_SLOW;
}
