#include "pch.h"
#include "resourceCache.h"

static GN::Logger * sLogger = GN::getLogger("GN.engine.RenderEngine.ResourceCache");

#pragma warning(disable:4100)

// *****************************************************************************
// local functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
static size_t sEstimateSurfaceSize( const GN::gfx::SurfaceCreationParameter & desc )
{
    using namespace GN::gfx;

    const SurfaceLayout & l = desc.desc.layout;

    size_t basebytes = l.basemap.sliceBytes * l.basemap.depth;
    size_t mipbytes  = basebytes / 3;

    GN_ASSERT( basebytes > 0 );

    return (basebytes + mipbytes) * l.faces;
}

//
//
// -----------------------------------------------------------------------------
static size_t sEstimateResourceSize( const GN::engine::GraphicsResourceDesc & desc )
{
    using namespace GN::engine;

    switch( desc.type )
    {
        case GRT_SURFACE  :
            return sEstimateSurfaceSize( desc.surface.creation );

        case GRT_PARAMETER_SET :
        case GRT_PORT_BINDING  :
        case GRT_KERNEL        :
            return 1024; // we assume that each one of these resources will occupy 1K device memory.

        default :
            GN_UNEXPECTED();
            return 0;
    }
}

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::engine::RenderEngine::ResourceCache::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::engine::RenderEngine::ResourceCache, () );

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::engine::RenderEngine::ResourceCache::quit()
{
    GN_GUARD;

    ScopeMutex<SpinLoop> lock( mResourceMutex );

    // free all resources
    UInt32 id = mResources.first();
    UInt32 id2;
    while( id )
    {
        GraphicsResourceItem * item = mResources.get( id );

        id2 = mResources.next( id );

        mResources.remove( id );
        delete item;

        id = id2;
    }

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
GN::engine::GraphicsResourceItem *
GN::engine::RenderEngine::ResourceCache::createResource(
    const GraphicsResourceDesc & desc )
{
    size_t bytes = sEstimateResourceSize( desc );
    if( 0 == bytes ) return 0;

    ScopeMutex<SpinLoop> lock( mResourceMutex );

    UInt32 id = mResources.newItem();

    GraphicsResourceItem * item = new GraphicsResourceItem( mEngine, id, desc, bytes );
    item->prev = item->next = 0;

    mResources[id] = item;

    return item;
}

//
//
// -----------------------------------------------------------------------------
void GN::engine::RenderEngine::ResourceCache::deleteResource( GraphicsResourceItem * item )
{
    if( 0 == item ) return;

    GN_ASSERT( checkResource( item ) );

    ScopeMutex<SpinLoop> lock( mResourceMutex );

    mResources.remove( item->id );
    delete item;
}

//
//
// -----------------------------------------------------------------------------
void GN::engine::RenderEngine::ResourceCache::deleteAllResources()
{
    ScopeMutex<SpinLoop> lock( mResourceMutex );

    for( UInt32 i = mResources.first(); i != 0; i = mResources.next( i ) )
    {
        GN_ASSERT( mResources[i] );
        delete mResources[i];
    }
    mResources.clear();
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------


