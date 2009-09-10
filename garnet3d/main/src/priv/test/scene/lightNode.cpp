#include "pch.h"
#include "lightNode.h"

using namespace GN;
using namespace GN::scene;

static GN::Logger * sLogger = GN::getLogger("GN.scene");

// *****************************************************************************
// LightNode::Impl public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------

// *****************************************************************************
// LightNode::Impl private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------

// *****************************************************************************
// LightNode public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::scene::LightNode::LightNode( Entity & entity, VisualGraph & graph )
    : NodeBase(entity)
    , mImpl(NULL)
{
    mImpl = new Impl( *this, graph );
}

//
//
// -----------------------------------------------------------------------------
GN::scene::LightNode::~LightNode()
{
    delete mImpl;
}

//
//
// -----------------------------------------------------------------------------
const Guid & GN::scene::LightNode::guid()
{
    static const Guid MY_GUID = { 0x5cd50f8d, 0x69fc, 0x4836, { 0xb3, 0x28, 0xea, 0x60, 0xef, 0xcb, 0x21, 0xa8 } };
    return MY_GUID;
}

//
//
// -----------------------------------------------------------------------------

// *****************************************************************************
// LightNode factory
// *****************************************************************************

class LightNodeInternal : public LightNode
{
public:

    LightNodeInternal( Entity & entity, VisualGraph & graph ) : LightNode( entity, graph ) {}

    virtual ~LightNodeInternal() {}

};

//
//
// -----------------------------------------------------------------------------
LightNode * GN::scene::newLightNode( Entity & entity, VisualGraph & graph )
{
    return new LightNodeInternal( entity, graph );
}
