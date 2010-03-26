#include "pch.h"
#include "spatialNode.h"

using namespace GN;
using namespace GN::util;

static GN::Logger * sLogger = GN::GetLogger("GN.util");

// *****************************************************************************
// SpatialNode::Impl public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::util::SpatialNode::Impl::Impl( SpatialNode & owner, SpatialGraph & graph )
    : mOwner( owner )
    , mGraph( graph )
    , mPosition( 0, 0, 0 )
    , mRotation( 0, 0, 0, 1 )
    , mScale( 1, 1, 1 )
    , mBoundingSphere( 0, 0, 0, 0 )
    , mLocal2Parent( Matrix44f::sIdentity() )
    , mParent2Local( Matrix44f::sIdentity() )
    , mLocal2Root( Matrix44f::sIdentity() )
    , mRoot2Local( Matrix44f::sIdentity() )
    , mTransformDirty( false )
{
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::SetParent( SpatialNode * parent, SpatialNode * prevSibling )
{
    // only nodes belong to same graph can be linked with each other.
    GN_ASSERT( NULL == parent || &parent->graph() == &mOwner.graph() );
    GN_ASSERT( NULL == prevSibling || &prevSibling->graph() == &mOwner.graph() );

    if( parent != GetParent() || prevSibling != GetPrevSibling() )
    {
        TreeNodeClass::SetParent( toImplPtr(parent), toImplPtr(prevSibling) );
        invalidateTransformation();
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::setPosition( const Vector3f & p )
{
    if( p != mPosition )
    {
        mPosition = p;
        invalidateTransformation();
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::setRotation( const Quaternionf & q )
{
    if( q != mRotation )
    {
        mRotation = q;
        invalidateTransformation();
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::setScale( const Vector3f & s )
{
    if( s != mScale )
    {
        mScale = s;
        invalidateTransformation();
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::setBoundingSphere( const Spheref & s )
{
    mBoundingSphere = s;
}

//
//
// -----------------------------------------------------------------------------
SpatialNode * GN::util::SpatialNode::Impl::getLastChild() const
{
    GN_UNIMPL();
    return NULL;
}

// *****************************************************************************
// SpatialNode::Impl private methods
// *****************************************************************************

//
// Invalidate transformation of all nodes in subtree
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::invalidateTransformation()
{
    if( mTransformDirty ) return;

    mTransformDirty = true;

    for( SpatialNode * child = GetFirstChild(); child; child = child->GetNextSibling() )
    {
        toImplPtr( child )->invalidateTransformation();
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SpatialNode::Impl::calcTransform()
{
    GN_ASSERT( mTransformDirty );

    Matrix33f r33;
    Matrix44f r, t, s;

    // object will be:
    //
    //  1. scale
    //  2. rotated around its local origin
    //  3. moved to localtion defined "position" in parent space

    mRotation.ToMatrix33( r33 );
    r.Set( r33 );

    t.Translate( mPosition );

    s.Identity();
    s[0][0] = mScale[0];
    s[1][1] = mScale[1];
    s[2][2] = mScale[2];

    mLocal2Parent = t * r * s;
    mParent2Local = Matrix44f::sInverse( mLocal2Parent );

    Impl * parent = (Impl*)( TreeNodeClass::GetParent() );
    if( parent )
    {
        mLocal2Root = parent->getLocal2Root() * mLocal2Parent;
        mRoot2Local = Matrix44f::sInverse( mLocal2Root );
    }
    else
    {
        mLocal2Root = mLocal2Parent;
        mRoot2Local = mParent2Local;
    }

    mTransformDirty = false;
}

// *****************************************************************************
// SpatialNode public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::util::SpatialNode::SpatialNode( Entity & entity, SpatialGraph & graph )
    : NodeBase(entity)
    , mImpl(NULL)
{
    mImpl = new Impl( *this, graph );
}

//
//
// -----------------------------------------------------------------------------
GN::util::SpatialNode::~SpatialNode()
{
    delete mImpl;
}

//
//
// -----------------------------------------------------------------------------
const Guid & GN::util::SpatialNode::guid()
{
    static const Guid SPATIAL_NODE_GUID =
    {
        0xaab1738d, 0x3bc7, 0x424d, { 0x90, 0x35, 0x49, 0x19, 0x47, 0xef, 0xa7, 0x20 },
    };

    return SPATIAL_NODE_GUID;
}

//
//
// -----------------------------------------------------------------------------
SpatialGraph      & GN::util::SpatialNode::graph() const { return mImpl->graph(); }
void                GN::util::SpatialNode::SetParent( SpatialNode * parent, SpatialNode * prevSibling ) { return mImpl->SetParent( parent, prevSibling ); }
void                GN::util::SpatialNode::setPosition( const Vector3f & position ) { return mImpl->setPosition( position ); }
void                GN::util::SpatialNode::setRotation( const Quaternionf & rotation ) { return mImpl->setRotation( rotation ); }
void                GN::util::SpatialNode::setScale( const Vector3f & scale ) { return mImpl->setScale( scale ); }
void                GN::util::SpatialNode::setBoundingSphere( const Spheref & sphere ) { return mImpl->setBoundingSphere( sphere ); }

SpatialNode       * GN::util::SpatialNode::GetParent() const { return mImpl->GetParent(); }
SpatialNode       * GN::util::SpatialNode::GetPrevSibling() const { return mImpl->GetPrevSibling(); }
SpatialNode       * GN::util::SpatialNode::GetNextSibling() const { return mImpl->GetNextSibling(); }
SpatialNode       * GN::util::SpatialNode::GetFirstChild() const { return mImpl->GetFirstChild(); }
SpatialNode       * GN::util::SpatialNode::getLastChild() const { return mImpl->getLastChild(); }

const Vector3f    & GN::util::SpatialNode::getPosition() const { return mImpl->getPosition(); }
const Quaternionf & GN::util::SpatialNode::getRotation() const { return mImpl->getRotation(); }
const Vector3f    & GN::util::SpatialNode::getScale() const { return mImpl->getScale(); }
const Spheref     & GN::util::SpatialNode::getBoundingSphere() const { return mImpl->getBoundingSphere(); }
const Matrix44f   & GN::util::SpatialNode::getLocal2Parent() const { return mImpl->getLocal2Parent(); }
const Matrix44f   & GN::util::SpatialNode::getLocal2Root() const { return mImpl->getLocal2Root(); }

// *****************************************************************************
// SpatialNode factory
// *****************************************************************************

class SpatialNodeInternal : public SpatialNode
{
public:

    SpatialNodeInternal( Entity & entity, SpatialGraph & graph ) : SpatialNode( entity, graph ) {}

    virtual ~SpatialNodeInternal() {}

};

//
//
// -----------------------------------------------------------------------------
SpatialNode * GN::util::newSpatialNode( Entity & entity, SpatialGraph & graph )
{
    return new SpatialNodeInternal( entity, graph );
}
