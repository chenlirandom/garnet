#include "pch.h"
#include "world.h"
#include "spatialNode.h"
#include "visualNode.h"
#include "lightNode.h"

using namespace GN;
using namespace GN::util;

static GN::Logger * sLogger = GN::getLogger("GN.util");

// *****************************************************************************
// EntityInternal class
// *****************************************************************************

class EntityInternal : public Entity
{
public:

    EntityInternal( World & world, int id ) : Entity(world, id) {}

    ~EntityInternal() {}
};

// *****************************************************************************
// World::Impl public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::util::World::Impl::reset()
{
    deleteAllEntities();
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::World::Impl::createEntity( const char * name )
{
    // compose a unique name for unnamed entity
    char unnamed[256];
    if( 0 == name || 0 == *name )
    {
        static int i = 0;
        ++i;
        stringPrintf( unnamed, GN_ARRAY_COUNT(unnamed), "Unnamed entity #%d", i );
        name = unnamed;

        GN_ASSERT( !mEntities.validName( name ) );
    }

    if( mEntities.validName( name ) )
    {
        GN_ERROR(sLogger)( "Entity creation failed: entity named '%s' exists already.", name );
        return NULL;
    }

    // create new ID
    UInt32 id = mEntities.add(name);
    if( 0 == id ) return false;

    // create new entity
    AutoObjPtr<EntityInternal> newEntity( new EntityInternal( mOwner, id ) );
    if( !newEntity )
    {
        mEntities.remove( id );
        return NULL;
    }
    mEntities[id] = newEntity;

    // done
    return newEntity.detach();
}

//
//
// -----------------------------------------------------------------------------
void GN::util::World::Impl::deleteEntity( const char * name )
{
    if( !mEntities.validName( name ) )
    {
        GN_ERROR(sLogger)( "Entity deletion failed: invalid entity named '%s'.", name ? name : "<NULL_NAME>" );
        return;
    }

    UInt32 id = mEntities.name2handle( name );

    delete (EntityInternal*)mEntities[id];

    mEntities.remove( id );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::World::Impl::deleteEntity( int id )
{
    if( !mEntities.validHandle( id ) )
    {
        GN_ERROR(sLogger)( "Entity deletion failed: invalid ID" );
        return;
    }

    delete (EntityInternal*)mEntities[id];

    mEntities.remove( id );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::World::Impl::deleteEntity( Entity * entity )
{
    if( NULL == entity ) return;

    if( this != entity->getWorld().mImpl )
    {
        GN_ERROR(sLogger)( "Entity deletion failed: the entity is from another world." );
        return;
    }

    deleteEntity( entity->id() );
}

//
//
// -----------------------------------------------------------------------------;
void GN::util::World::Impl::deleteAllEntities()
{
    for( UInt32 id = mEntities.first(); id != 0; id = mEntities.next( id ) )
    {
        delete (EntityInternal*)mEntities[id];
    }
    mEntities.clear();
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::World::Impl::findEntity( const char * name )
{
    if( !mEntities.validName( name ) ) return NULL;

    return mEntities[name];
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::World::Impl::findEntity( int id )
{
    if( !mEntities.validHandle( id ) ) return NULL;

    return mEntities[id];
}

//
//
// -----------------------------------------------------------------------------
const char  * GN::util::World::Impl::getEntityName( int id ) const
{
    const char * name = mEntities.handle2name(id);

    if( NULL == name )
    {
        GN_ERROR(sLogger)( "Fail to get reosource name: Invalid ID." );
    }

    return name;
}

// *****************************************************************************
// World::Impl private methods
// *****************************************************************************

// *****************************************************************************
// World public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::util::World::World( gfx::GpuResourceDatabase & gdb ) : mImpl(NULL)
{
    mImpl = new Impl( *this, gdb );
    mImpl->reset();
}

//
//
// -----------------------------------------------------------------------------
GN::util::World::~World()
{
    delete mImpl;
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::World::createSpatialEntity( const char * name )
{
    Entity * e = createEntity( name );
    if( !e ) return NULL;

    AutoObjPtr<SpatialNode> sn( newSpatialNode( *e, spatialGraph() ) );
    if( !sn ) return NULL;

    e->attachNode<SpatialNode>( sn.detach() );

    return e;
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::World::createVisualEntity( const char * name )
{
    Entity * e = createEntity( name );
    if( !e ) return NULL;

    AutoObjPtr<SpatialNode> sn( newSpatialNode( *e, spatialGraph() ) );
    if( !sn ) return false;

    AutoObjPtr<VisualNode> vn( newVisualNode( *e, visualGraph() ) );
    if( !vn ) return false;

    e->attachNode<SpatialNode>( sn.detach() );
    e->attachNode<VisualNode>( vn.detach() );

    return e;
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::World::createLightEntity( const char * name )
{
    Entity * e = createEntity( name );
    if( !e ) return NULL;

    AutoObjPtr<SpatialNode> sn( newSpatialNode( *e, spatialGraph() ) );
    if( !sn ) return false;

    AutoObjPtr<LightNode> ln( newLightNode( *e, visualGraph() ) );
    if( !ln ) return false;

    e->attachNode<SpatialNode>( sn.detach() );
    e->attachNode<LightNode>( ln.detach() );

    return e;
}

//
//
// -----------------------------------------------------------------------------
gfx::GpuResourceDatabase & GN::util::World::getGdb() const { return mImpl->getGdb(); }
SpatialGraph             & GN::util::World::spatialGraph() const { return mImpl->spatialGraph(); }
VisualGraph              & GN::util::World::visualGraph() const { return mImpl->visualGraph(); }
void                       GN::util::World::reset() { return mImpl->reset(); }
Entity                   * GN::util::World::createEntity( const char * name ) { return mImpl->createEntity( name ); }
void                       GN::util::World::deleteEntity( const char * name ) { return mImpl->deleteEntity( name ); }
void                       GN::util::World::deleteEntity( int id ) { return mImpl->deleteEntity( id ); }
void                       GN::util::World::deleteEntity( Entity * entity ) { return mImpl->deleteEntity( entity ); }
void                       GN::util::World::deleteAllEntities() { return mImpl->deleteAllEntities(); }
Entity                   * GN::util::World::findEntity( const char * name ) { return mImpl->findEntity( name ); }
Entity                   * GN::util::World::findEntity( int id ) { return mImpl->findEntity( id ); }
