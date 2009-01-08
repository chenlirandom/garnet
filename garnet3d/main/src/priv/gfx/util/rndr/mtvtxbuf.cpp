#include "pch.h"
#include "mtvtxbuf.h"
#include "mtrndrCmd.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.util.rndr.mtvtxbuf");

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MultiThreadVtxBuf::init( VtxBuf * vb )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::MultiThreadVtxBuf, () );

    if( NULL == vb ) return failure();

    mVtxBuf = vb;

    const VtxBufDesc & desc = mVtxBuf->getDesc();

    setDesc( desc );

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::MultiThreadVtxBuf::quit()
{
    GN_GUARD;

    if( mVtxBuf )
    {
        mRenderer.postCommand1( CMD_VTXBUF_DESTROY, mVtxBuf );
        mVtxBuf = NULL;
    }

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
void GN::gfx::MultiThreadVtxBuf::update( size_t offset, size_t length, const void * data, UpdateFlag flag )
{
    if( 0 == length )
    {
        length = getDesc().length - offset;
    }

    void * tmpbuf = heapAlloc( length );
    if( NULL == tmpbuf )
    {
        GN_ERROR(sLogger)( "fail to allocate temporary buffer." );
        return;
    }
    memcpy( tmpbuf, data, length );

    mRenderer.postCommand5( CMD_VTXBUF_UPDATE, mVtxBuf, offset, length, tmpbuf, flag );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::MultiThreadVtxBuf::readback( std::vector<UInt8> & data )
{
    mRenderer.postCommand2( CMD_VTXBUF_READBACK, mVtxBuf, &data );
}

// *****************************************************************************
// Command handlers (called by back end thread)
// *****************************************************************************

namespace GN { namespace gfx
{
    //
    //
    // -------------------------------------------------------------------------
    void func_VTXBUF_DESTROY( Renderer &, void * p, size_t )
    {
        VtxBuf * vb = *(VtxBuf**)p;
        vb->decref();
    }

    //
    //
    // -------------------------------------------------------------------------
    void func_VTXBUF_UPDATE( Renderer &, void * p, size_t )
    {
        struct VtxBufUpdateParam
        {
            VtxBuf   * vtxbuf;
            size_t     offset;
            size_t     length;
            void *     data;
            UpdateFlag flag;
        };

        VtxBufUpdateParam * vbup = (VtxBufUpdateParam*)p;

        vbup->vtxbuf->update( vbup->offset, vbup->length, vbup->data, vbup->flag );

        heapFree( vbup->data );
    }

    //
    //
    // -------------------------------------------------------------------------
    void func_VTXBUF_READBACK( Renderer &, void * p, size_t )
    {
        struct VtxBufReadBackParam
        {
            VtxBuf             * vb;
            std::vector<UInt8> * buf;
        };
        VtxBufReadBackParam * vbrp = (VtxBufReadBackParam*)p;

        vbrp->vb->readback( *vbrp->buf );
     }
}}