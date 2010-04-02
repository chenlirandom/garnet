#include "pch.h"
#include "meshresource.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpures");

// *****************************************************************************
// GN::gfx::VertexFormatProperties - public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::VertexFormatProperties::analyze( const VertexFormat & vf )
{
    memset( this, 0, sizeof(*this) );

    for( size_t i = 0; i < vf.numElements; ++i )
    {
        const VertexElement & e = vf.elements[i];

        if( e.stream > GpuContext::MAX_VERTEX_BUFFERS )
        {
            GN_ERROR(sLogger)( "Invalid stream ID: %d", e.stream );
            return false;
        }

        used[e.stream] = true;

        size_t currentStride = minStrides[e.stream];

        size_t newStride = e.offset + e.format.GetBytesPerBlock();

        if( newStride > currentStride ) minStrides[e.stream] = newStride;
    }

    return true;
}

// *****************************************************************************
// GN::gfx::MeshResource::Impl - public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MeshResource::Impl::Reset( const MeshResourceDesc * desc )
{
    Clear();

    bool ok;
    if( desc && !create( *desc ) )
    {
        Clear();
        ok = false;
    }
    else
    {
        ok = true;
    }

    mOwner.sigMeshChanged( mOwner );

    return ok;
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::Impl::ApplyToContext( GpuContext & context ) const
{
    // vertex format
    context.vtxfmt = mDesc.vtxfmt;

    // verex buffers
    GN_CASSERT( GN_ARRAY_COUNT(context.vtxbufs) == GN_ARRAY_COUNT(mVtxBufs) );
    for( size_t i = 0; i < GN_ARRAY_COUNT(context.vtxbufs); ++i )
    {
        context.vtxbufs[i].vtxbuf = mVtxBufs[i].gpudata;
        context.vtxbufs[i].stride = (UInt16)mDesc.strides[i];
        context.vtxbufs[i].offset = (UInt32)mDesc.offsets[i];
    }

    // index buffers
    context.idxbuf = mIdxBuf.gpudata;
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::Impl::CalculateBoundingBox( Box<float> & box ) const
{
    MeshResourceDesc desc = mDesc;

    DynaArray<UInt8> buffers[GpuContext::MAX_VERTEX_BUFFERS];
    for( size_t i = 0; i < GpuContext::MAX_VERTEX_BUFFERS; ++i )
    {
        if( mVtxBufs[i].gpudata )
        {
            mVtxBufs[i].gpudata->Readback( buffers[i] );
        }

        desc.vertices[i] = buffers[i].ToRawPtr();
    }

    desc.CalculateBoundingBox( box );
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::Impl::CalculateBoundingSphere( Sphere<float> & sphere ) const
{
    MeshResourceDesc desc = mDesc;

    DynaArray<UInt8> buffers[GpuContext::MAX_VERTEX_BUFFERS];
    for( size_t i = 0; i < GpuContext::MAX_VERTEX_BUFFERS; ++i )
    {
        if( mVtxBufs[i].gpudata )
        {
            mVtxBufs[i].gpudata->Readback( buffers[i] );
        }

        desc.vertices[i] = buffers[i].Empty() ? NULL : &buffers[i][0];
    }

    desc.CalculateBoundingSphere( sphere );
}

// *****************************************************************************
// GN::gfx::MeshResource::Impl - private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MeshResource::Impl::create( const MeshResourceDesc & desc )
{
    // analyze vertex format
    VertexFormatProperties vfp;
    if( !vfp.analyze( desc.vtxfmt ) ) return false;

    // store descriptor
    mDesc = desc;

    Gpu & gpu = GetGdb().GetGpu();

    // initialize vertex buffers
    if( desc.numvtx > 0 )
    {
        for( size_t i = 0; i < GpuContext::MAX_VERTEX_BUFFERS; ++i )
        {
            if( !vfp.used[i] ) continue; // ignore unused vertex buffer

            // calculate vertex stride
            size_t stride;
            if( 0 == desc.strides[i] )
            {
                stride = vfp.minStrides[i];
            }
            else if( desc.strides[i] >= vfp.minStrides[i] )
            {
                stride = desc.strides[i];
            }
            else
            {
                GN_ERROR(sLogger)( "stride for stream %u is too small.", i );
                return false;
            }
            mDesc.strides[i] = stride;

            // calculate vertex buffer size
            size_t vbsize;
            vbsize = stride * desc.numvtx;

            // create GPU vertex buffer
            VtxBufDesc vbdesc = { vbsize, desc.dynavb };
            mVtxBufs[i].gpudata.Attach( gpu.CreateVtxBuf( vbdesc ) );
            if( NULL == mVtxBufs[i].gpudata ) return false;

            // copy vertices to vertex buffer
            const void * vertices = desc.vertices[i];
            if( vertices ) mVtxBufs[i].gpudata->Update( 0, 0, vertices );
        }
    }

    // initialize index buffer
    if( desc.numidx > 0 )
    {
        IdxBufDesc ibd = { (UInt32)desc.numidx, desc.idx32, desc.dynaib };
        mIdxBuf.gpudata.Attach( gpu.CreateIdxBuf( ibd ) );
        if( NULL == mIdxBuf.gpudata ) return false;

        if( desc.indices ) mIdxBuf.gpudata->Update( 0, 0, desc.indices );
    }

    // clear data pointers in stored decriptor
    memset( mDesc.vertices, 0, sizeof(mDesc.vertices) );
    mDesc.indices = NULL;

    // success
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::MeshResource::Impl::Clear()
{
    for( size_t i = 0; i < GN_ARRAY_COUNT(mVtxBufs); ++i )
    {
        mVtxBufs[i].gpudata.Clear();
    }

    mIdxBuf.gpudata.Clear();
}

// *****************************************************************************
// GN::gfx::MeshResource
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
class MeshResourceInternal : public MeshResource
{
    MeshResourceInternal( GpuResourceDatabase & db ) : MeshResource( db )
    {
    }

public:

    static GpuResource *
    sCreateInstance( GpuResourceDatabase & db )
    {
        return new MeshResourceInternal( db );
    }
};

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::registerMeshResourceFactory( GpuResourceDatabase & db )
{
    if( db.HasResourceFactory( MeshResource::GetGuid() ) ) return true;

    GpuResourceFactory factory = { &MeshResourceInternal::sCreateInstance };

    return db.RegisterResourceFactory( MeshResource::GetGuid(), "Mesh Resource", factory );
}

// *****************************************************************************
// GN::gfx::MeshResource
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::MeshResource::MeshResource( GpuResourceDatabase & db )
    : GpuResource( db )
    , mImpl(NULL)
{
    mImpl = new Impl(*this);
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::MeshResource::~MeshResource()
{
    delete mImpl;
}

//
//
// -----------------------------------------------------------------------------
const Guid & GN::gfx::MeshResource::GetGuid()
{
    static const Guid MESH_GUID = { 0x892f15d5, 0x8e56, 0x4982, { 0x83, 0x1a, 0xc7, 0x1a, 0x11, 0x20, 0x4e, 0x4a } };
    return MESH_GUID;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<MeshResource>
GN::gfx::MeshResource::LoadFromFile(
    GpuResourceDatabase & db,
    const char          * filename )
{
    if( NULL == filename )
    {
        GN_INFO(sLogger)( "Null filename string." );
        return AutoRef<MeshResource>::NULLREF;
    }

    // Reuse existing resource, if possible
    AutoRef<MeshResource> m( db.FindResource<MeshResource>( filename ) );
    if( m ) return m;

    // convert to full (absolute) path
    StrA abspath = fs::ResolvePath( fs::GetCurrentDir(), filename );
    filename = abspath;

    // Try search for existing resource again with full path
    m = db.FindResource<MeshResource>( filename );
    if( m ) return m;

    MeshResourceDesc desc;
    AutoRef<Blob> blob = desc.LoadFromFile( filename );
    if( !blob ) return AutoRef<MeshResource>::NULLREF;

    m = db.CreateResource<MeshResource>( abspath );
    if( !m || !m->Reset( &desc ) ) AutoRef<MeshResource>::NULLREF;

    return m;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MeshResource::Reset( const MeshResourceDesc * desc )
{
    return mImpl->Reset( desc );
}


//
//
// -----------------------------------------------------------------------------
const MeshResourceDesc & GN::gfx::MeshResource::GetDesc() const
{
    return mImpl->GetDesc();
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::ApplyToContext( GpuContext & context ) const
{
    mImpl->ApplyToContext( context );
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::CalculateBoundingBox( Box<float> & box ) const
{
    return mImpl->CalculateBoundingBox( box );
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::CalculateBoundingSphere( Sphere<float> & sphere ) const
{
    return mImpl->CalculateBoundingSphere( sphere );
}