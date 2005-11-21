#include "pch.h"
#include "oglVtxBuf.h"

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLVtxBufNormal::init( size_t vtxCount, size_t stride, ResourceUsage usage )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( OGLVtxBufNormal, () );

    if( 0 == vtxCount || 0 == stride )
    {
        GNGFX_ERROR( "Vertex count and stride can be zero!" );
        quit(); return selfOK();
    }
    if ( USAGE_STATIC != usage && USAGE_DYNAMIC != usage )
    {
        GNGFX_ERROR( "Vertex buffer usage can be only USAGE_STATIC or USAGE_DYNAMIC!" );
        quit(); return selfOK();
    }

    // store properties
    setProperties( vtxCount, stride, usage );

    mBuffer = (uint8_t*)memAlloc( vtxCount * stride );

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLVtxBufNormal::quit()
{
    GN_GUARD;

    safeMemFree( mBuffer );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// from VtxBuf
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void * GN::gfx::OGLVtxBufNormal::lock( size_t startVtx, size_t /*numVtx*/, uint32_t /*flag*/ )
{
    GN_GUARD_SLOW;

    GN_ASSERT( selfOK() );

    if( mLocked )
    {
        GNGFX_ERROR( "Vertex buffer is already locked!" );
        return 0;
    }
    if( startVtx >= getNumVtx() )
    {
        GNGFX_ERROR( "offset is beyond the end of vertex buffer!" );
        return 0;
    }

    // success
    mLocked = true;
    return &mBuffer[startVtx * getStride()];

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLVtxBufNormal::unlock()
{
    GN_GUARD_SLOW;

    GN_ASSERT( selfOK() );

    if( !mLocked )
    {
        GNGFX_ERROR( "Can't unlock a vertex buffer that is not locked at all!" );
        return;
    }

    mLocked = false;

    GN_UNGUARD_SLOW;
}

