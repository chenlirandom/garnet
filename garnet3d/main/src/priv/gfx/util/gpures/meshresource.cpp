#include "pch.h"
#include "meshresource.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.gpures");

// *****************************************************************************
// Local stuff
// *****************************************************************************

struct VertexFormatProperties
{
    /// minimal strides for each stream
    size_t minStrides[GpuContext::MAX_VERTEX_BUFFERS];

    /// true means that stream is referenced by the vertex format.
    bool used[GpuContext::MAX_VERTEX_BUFFERS];

    /// analyze vertex format
    bool analyze( const VertexFormat & vf )
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

            size_t newStride = e.offset + e.format.getBytesPerBlock();

            if( newStride > currentStride ) minStrides[e.stream] = newStride;
        }

        return true;
    }
};

//
//
// -----------------------------------------------------------------------------
const VertexElement * sFindPositionElement( const VertexFormat & vf )
{
    for( size_t i = 0; i < vf.numElements; ++i )
    {
        const VertexElement & e = vf.elements[i];

        if( ( 0 == strCmpI( "position", e.binding ) ||
              0 == strCmpI( "pos", e.binding ) ||
              0 == strCmpI( "gl_vertex", e.binding ) )
            &&
            0 == e.bindingIndex )
        {
            return &e;
        }
    }

    GN_ERROR(sLogger)( "Position semantice is not found in vertex format." );
    return NULL;
}

// *****************************************************************************
// GN::gfx::MeshResource::Impl - public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MeshResource::Impl::reset( const MeshResourceDesc * desc )
{
    clear();

    bool ok;
    if( desc && !create( *desc ) )
    {
        clear();
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
GN::gfx::MeshResource::Impl::applyToContext( GpuContext & context ) const
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
GN::gfx::MeshResource::Impl::calculateAABB( Box<float> & box ) const
{
    box.x = box.y = box.w = box.h = 0.0f;

    const VertexElement * positionElement = sFindPositionElement( mDesc.vtxfmt );
    if( NULL == positionElement ) return;

    std::vector<UInt8> buffer;
    mVtxBufs[positionElement->stream].gpudata->readback( buffer );
    const float * vertices = (const float*)&buffer[0];

    const float * x, * y, * z;
    size_t stride;
    if( ColorFormat::FLOAT1 == positionElement->format )
    {
        x = vertices;
        y = 0;
        z = 0;
        stride = sizeof(float);
    }
    else if( ColorFormat::FLOAT2 == positionElement->format )
    {
        x = vertices;
        y = vertices + 1;
        z = 0;
        stride = sizeof(float) * 2;
    }
    else if( ColorFormat::FLOAT3 == positionElement->format )
    {
        x = vertices;
        y = vertices + 1;
        z = vertices + 2;
        stride = sizeof(float) * 3;
    }
    else if( ColorFormat::FLOAT4 == positionElement->format )
    {
        x = vertices;
        y = vertices + 1;
        z = vertices + 2;
        stride = sizeof(float) * 4;
    }
    else
    {
        GN_ERROR(sLogger)( "AABB calculation failed: unsupported vertex format %s", positionElement->format.toString().cptr() );
        return;
    }

    GN::calculateAABB( box, x, stride, y, stride, z, stride, mDesc.numvtx );
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::Impl::calculateBoundingSphere( Sphere<float> & sphere ) const
{
    sphere.center.set( 0, 0, 0 );
    sphere.radius = 0;

    const VertexElement * positionElement = sFindPositionElement( mDesc.vtxfmt );
    if( NULL == positionElement ) return;

    std::vector<UInt8> buffer;
    mVtxBufs[positionElement->stream].gpudata->readback( buffer );
    const float * vertices = (const float*)&buffer[0];

    const float * x, * y, * z;
    size_t stride;
    if( ColorFormat::FLOAT1 == positionElement->format )
    {
        x = vertices;
        y = 0;
        z = 0;
        stride = sizeof(float);
    }
    else if( ColorFormat::FLOAT2 == positionElement->format )
    {
        x = vertices;
        y = vertices + 1;
        z = 0;
        stride = sizeof(float) * 2;
    }
    else if( ColorFormat::FLOAT3 == positionElement->format )
    {
        x = vertices;
        y = vertices + 1;
        z = vertices + 2;
        stride = sizeof(float) * 3;
    }
    else if( ColorFormat::FLOAT4 == positionElement->format )
    {
        x = vertices;
        y = vertices + 1;
        z = vertices + 2;
        stride = sizeof(float) * 4;
    }
    else
    {
        GN_ERROR(sLogger)( "AABB calculation failed: unsupported vertex format %s", positionElement->format.toString().cptr() );
        return;
    }

    GN::calculateBoundingSphere( sphere, x, stride, y, stride, z, stride, mDesc.numvtx );
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

    Gpu & gpu = database().gpu();

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
            mVtxBufs[i].gpudata.attach( gpu.createVtxBuf( vbdesc ) );
            if( NULL == mVtxBufs[i].gpudata ) return false;

            // copy vertices to vertex buffer
            const void * vertices = desc.vertices[i];
            if( vertices ) mVtxBufs[i].gpudata->update( 0, 0, vertices );
        }
    }

    // initialize index buffer
    if( desc.numidx > 0 )
    {
        IdxBufDesc ibd = { (UInt32)desc.numidx, desc.idx32, desc.dynaib };
        mIdxBuf.gpudata.attach( gpu.createIdxBuf( ibd ) );
        if( NULL == mIdxBuf.gpudata ) return false;

        if( desc.indices ) mIdxBuf.gpudata->update( 0, 0, desc.indices );
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
void GN::gfx::MeshResource::Impl::clear()
{
    for( size_t i = 0; i < GN_ARRAY_COUNT(mVtxBufs); ++i )
    {
        mVtxBufs[i].gpudata.clear();
    }

    mIdxBuf.gpudata.clear();
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
    if( db.hasResourceFactory( MeshResource::guid() ) ) return true;

    GpuResourceFactory factory = { &MeshResourceInternal::sCreateInstance };

    return db.registerResourceFactory( MeshResource::guid(), "Mesh Resource", factory );
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
const Guid & GN::gfx::MeshResource::guid()
{
    static const Guid MESH_GUID = { 0x892f15d5, 0x8e56, 0x4982, { 0x83, 0x1a, 0xc7, 0x1a, 0x11, 0x20, 0x4e, 0x4a } };
    return MESH_GUID;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<MeshResource>
GN::gfx::MeshResource::loadFromFile(
    GpuResourceDatabase & db,
    const char          * filename )
{
    GN_UNUSED_PARAM( db );
    GN_UNUSED_PARAM( filename );
    GN_UNIMPL();

    return AutoRef<MeshResource>::NULLREF;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::MeshResource::reset( const MeshResourceDesc * desc )
{
    return mImpl->reset( desc );
}


//
//
// -----------------------------------------------------------------------------
const MeshResourceDesc & GN::gfx::MeshResource::getDesc() const
{
    return mImpl->getDesc();
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::applyToContext( GpuContext & context ) const
{
    mImpl->applyToContext( context );
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::calculateAABB( Box<float> & box ) const
{
    return mImpl->calculateAABB( box );
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::MeshResource::calculateBoundingSphere( Sphere<float> & sphere ) const
{
    return mImpl->calculateBoundingSphere( sphere );
}