#include "pch.h"
#include "oglQuad.h"
#include "oglRenderer.h"

struct OGLQuadStruct
{
    GN::Vector4f p0;
    GN::Vector2f u0;
    GN::Vector2f _reserved0;

    GN::Vector4f p1;
    GN::Vector2f u1;
    GN::Vector2f _reserved1;

    GN::Vector4f p2;
    GN::Vector2f u2;
    GN::Vector2f _reserved2;

    GN::Vector4f p3;
    GN::Vector2f u3;
    GN::Vector2f _reserved3;
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

    GN_ASSERT( !mVtxBuf && !mIdxBuf );

    // create vertex buffer
    mVtxBuf = mRenderer.createVtxBuf( sizeof(OGLQuadStruct) * MAX_QUADS, true );
    if( 0 == mVtxBuf ) { quit(); return selfOK(); }

    // create index buffer
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
// from OGLResource
// *****************************************************************************

#pragma warning(disable:4100) // unused parameters

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLQuad::drawQuads(
    const Vector2f * positions, size_t posStride,
    const Vector2f * texCoords, size_t texStride,
    size_t count, uint32_t options )
{
    //GN_GUARD;

    //GN_UNGUARD;
}
