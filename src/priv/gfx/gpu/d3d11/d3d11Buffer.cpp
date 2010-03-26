#include "pch.h"
#include "d3d11Buffer.h"
#include "d3d11Gpu.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpu.D3D11.Buffer");

// *****************************************************************************
// init / quit functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D11Buffer::Init( size_t bytes, bool fastCpuWrite, UInt32 bindFlags )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::D3D11Buffer, () );

    // check parameter
    if( 0 == bytes )
    {
        GN_ERROR(sLogger)( "Buffer length can not be zero!" );
        return Failure();
    }

    ID3D11Device & dev = getDeviceRef();

    D3D11_BUFFER_DESC d3ddesc;
    d3ddesc.ByteWidth      = (UInt32)bytes;
    d3ddesc.Usage          = fastCpuWrite ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT ;
    d3ddesc.BindFlags      = bindFlags;
    d3ddesc.CPUAccessFlags = fastCpuWrite ? D3D11_CPU_ACCESS_WRITE : 0;
    d3ddesc.MiscFlags      = 0;

    // create d3d ibuffer
    GN_DX_CHECK_RETURN( dev.CreateBuffer( &d3ddesc, 0, &mD3DBuffer ), Failure() );

    // store buffer parameters
    mBytes        = bytes;
    mFastCpuWrite = fastCpuWrite;

    // success
    return Success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D11Buffer::Quit()
{
    GN_GUARD;

    SafeRelease( mD3DBuffer );

    // standard Quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// interface functions
// *****************************************************************************

static const D3D11_MAP SURFACE_UPDATE_FLAG_TO_D3D11_MAP[] =
{
    D3D11_MAP_WRITE,              // SurfaceUpdateFlag::DEFAULT
    D3D11_MAP_WRITE_DISCARD,      // SurfaceUpdateFlag::DISCARD
    D3D11_MAP_WRITE_NO_OVERWRITE  // SurfaceUpdateFlag::NO_OVERWRITE
};

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D11Buffer::Update( size_t offset, size_t bytes, const void * data, SurfaceUpdateFlag flag )
{
    PIXPERF_FUNCTION_EVENT();

    // parameters should've already been verified by caller.
    GN_ASSERT( ( offset + bytes ) <= mBytes );
    GN_ASSERT( data );
    GN_ASSERT( 0 <= flag && flag < SurfaceUpdateFlag::NUM_FLAGS );

    ID3D11DeviceContext & dev = getDeviceContextRef();

    if( mFastCpuWrite )
    {
        // update dynamic d3d buffer
        D3D11_MAPPED_SUBRESOURCE mapped;
        GN_DX_CHECK_RETURN_VOID( dev.Map( mD3DBuffer, 0, SURFACE_UPDATE_FLAG_TO_D3D11_MAP[flag], 0, &mapped ) );
        UInt8 * dst = (UInt8*)mapped.pData;
        memcpy( dst+offset, data, bytes );
        dev.Unmap( mD3DBuffer, 0 );
    }
    else
    {
        // update non-dynamic d3d buffer
        D3D11_BOX box = { (UInt32)offset, 0, 0, (UInt32)(offset+bytes), 1, 1 };
        dev.UpdateSubresource(
            mD3DBuffer,
            0,   // subresource
            &box,
            data,
            0,   // row pitch
            0 ); // slice pitch
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D11Buffer::Readback( DynaArray<UInt8> & data )
{
    PIXPERF_FUNCTION_EVENT();

    data.Clear();

    GN_UNIMPL();
}
