#include "pch.h"
#include "mtvtxbuf.h"
#include "mtgpuCmd.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.util.gpu.mtvtxbuf");

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MultiThreadVtxBuf::Init( VtxBuf * vb )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::MultiThreadVtxBuf, () );

    if( NULL == vb ) return Failure();

    mVtxBuf = vb;

    const VtxBufDesc & desc = mVtxBuf->GetDesc();

    SetDesc( desc );

    // success
    return Success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::MultiThreadVtxBuf::Quit()
{
    GN_GUARD;

    if( mVtxBuf )
    {
        mGpu.postCommand1( CMD_VTXBUF_DESTROY, mVtxBuf );
        mVtxBuf = NULL;
    }

    // standard Quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// from VtxBuf
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::MultiThreadVtxBuf::Update( size_t offset, size_t length, const void * data, SurfaceUpdateFlag flag )
{
    if( 0 == length )
    {
        length = GetDesc().length - offset;
    }

    void * tmpbuf = HeapMemory::Alloc( length );
    if( NULL == tmpbuf )
    {
        GN_ERROR(sLogger)( "fail to allocate temporary buffer." );
        return;
    }
    memcpy( tmpbuf, data, length );

    mGpu.postCommand5( CMD_VTXBUF_UPDATE, mVtxBuf, offset, length, tmpbuf, flag );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::MultiThreadVtxBuf::Readback( DynaArray<UInt8> & data )
{
    mGpu.postCommand2( CMD_VTXBUF_READBACK, mVtxBuf, &data );
}

// *****************************************************************************
// Command handlers (called by back end thread)
// *****************************************************************************

namespace GN { namespace gfx
{
    //
    //
    // -------------------------------------------------------------------------
    void func_VTXBUF_DESTROY( Gpu &, void * p, size_t )
    {
        VtxBuf * vb = *(VtxBuf**)p;
        vb->DecRef();
    }

    //
    //
    // -------------------------------------------------------------------------
    void func_VTXBUF_UPDATE( Gpu &, void * p, size_t )
    {
        struct VtxBufUpdateParam
        {
            VtxBuf          * vtxbuf;
            size_t            offset;
            size_t            length;
            void            * data;
            SurfaceUpdateFlag flag;
        };

        VtxBufUpdateParam * vbup = (VtxBufUpdateParam*)p;

        vbup->vtxbuf->Update( vbup->offset, vbup->length, vbup->data, vbup->flag );

        HeapMemory::Free( vbup->data );
    }

    //
    //
    // -------------------------------------------------------------------------
    void func_VTXBUF_READBACK( Gpu &, void * p, size_t )
    {
        struct VtxBufReadBackParam
        {
            VtxBuf             * vb;
            DynaArray<UInt8> * buf;
        };
        VtxBufReadBackParam * vbrp = (VtxBufReadBackParam*)p;

        vbrp->vb->Readback( *vbrp->buf );
     }
}}
