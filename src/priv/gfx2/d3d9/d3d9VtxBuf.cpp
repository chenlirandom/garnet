#include "pch.h"
#include "d3d9VtxBuf.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx2.D3D9VtxBuf");

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx2::D3D9VtxBuf * GN::gfx2::D3D9VtxBuf::sNewInstance(
    D3D9GraphicsSystem          & gs,
    const SurfaceLayout         & layout,
    int                           access,
    const SurfaceCreationHints  & hints )
{
    GN_GUARD;

    D3D9SurfaceDesc desc;
    desc.type = SURFACE_TYPE_VB;
    desc.layout = layout;
    desc.access = access;

    AutoObjPtr<D3D9VtxBuf> vb( new D3D9VtxBuf(gs,desc,hints) );

    if( !vb->init() ) return 0;

    return vb.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx2::D3D9VtxBuf::~D3D9VtxBuf()
{
    safeRelease( mSurface );
}

#pragma warning(disable:4100)

//
//
// -----------------------------------------------------------------------------
const GN::gfx2::SubSurfaceLayout *
GN::gfx2::D3D9VtxBuf::getSubSurfaceLayout( size_t subsurface ) const
{
    if( 0 == subsurface )
    {
        GN_ERROR(sLogger)( "Vertex buffer has no subsurfaces" );
        return 0;
    }

    return &mSubsurfaceLayout;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx2::D3D9VtxBuf::download(
    size_t                 subsurface,
    const Box<size_t>    & area,
    const void           * source,
    size_t                 srcRowBytes,
    size_t                 srcSliceBytes )
{
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx2::D3D9VtxBuf::upload(
    size_t              subsurface,
    const Box<size_t> & area,
    void              * destination,
    size_t              destRowBytes,
    size_t              destSliceBytes )
{
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx2::D3D9VtxBuf::save( NativeSurfaceData & ) const
{
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx2::D3D9VtxBuf::load( const NativeSurfaceData & )
{
}

// *****************************************************************************
// private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx2::D3D9VtxBuf::D3D9VtxBuf( D3D9GraphicsSystem & gs, const D3D9SurfaceDesc & desc, const SurfaceCreationHints & hints )
    : D3D9Surface( desc )
    , mGraphicsSystem( gs )
    , mSurface( 0 )
    , mHints( hints )
{
    mSubsurfaceLayout.width      = desc.layout.basemap.width;
    mSubsurfaceLayout.height     = desc.layout.basemap.height;
    mSubsurfaceLayout.depth      = desc.layout.basemap.depth;
    mSubsurfaceLayout.rowBytes   = desc.layout.basemap.rowBytes;
    mSubsurfaceLayout.sliceBytes = desc.layout.basemap.sliceBytes;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx2::D3D9VtxBuf::init()
{
    GN_GUARD;

    GN_ASSERT( 0 == mSurface );

    const D3D9SurfaceDesc & desc = getD3D9Desc();

    GN_ASSERT( SURFACE_TYPE_VB == desc.type );

    // check descriptor
    if( SURFACE_DIMENSION_1D != desc.layout.dim )
    {
        GN_ERROR(sLogger)( "vertex buffer surface requires 1D dimension." );
        return false;
    }
    if( 1 != desc.layout.faces )
    {
        GN_ERROR(sLogger)( "vertex buffer surface requires layout.faces == 1." );
        return false;
    }
    if( 1 != desc.layout.levels )
    {
        GN_ERROR(sLogger)( "vertex buffer surface requires layout.levels == 1." );
        return false;
    }
    if( desc.layout.basemap.rowBytes < desc.layout.basemap.width * desc.layout.format.stride )
    {
        GN_ERROR(sLogger)( "vertex buffer creation failed: rowBytes must >= stride * width." );
        return false;
    }

    IDirect3DDevice9 * dev = mGraphicsSystem.d3ddev();

    // create vertex buffer
    GN_TODO( "setup usage and pool from hints" );
    GN_DX9_CHECK_RV( dev->CreateVertexBuffer( desc.layout.basemap.rowBytes, 0, 0, D3DPOOL_DEFAULT, &mSurface, 0 ), false );

    // success
    return true;

    GN_UNGUARD;
}
