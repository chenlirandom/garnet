#ifndef __GN_SCENE_H__
#define __GN_SCENE_H__
// *****************************************************************************
/// \file
/// \brief   Main header for scene management
/// \author  chen@@chenli-homepc (2009.9.6)
// *****************************************************************************

#include "garnet/GNgfx.h"

namespace GN { namespace scene
{
    class NodeBase;

    class SpatialNode;
    class SpatialGraph;

    class VisualNode;
    class LightNode;
    class VisualGraph;

    class World;

    // *************************************************************************
    // Entity and World
    // *************************************************************************

    ///
    /// Represent a entity in the world
    ///
    class Entity : NoCopy
    {
    public:

        //@{

        World                     & world() const;
        gfx::GpuResourceDatabase  & gdb() const;

        int                         id() const;
        const Guid                & type() const;
        const char                * name() const;

        bool                        hasNode( const Guid & nodeType ) const;
        const NodeBase            * getNode( const Guid & nodeType ) const;
        NodeBase                  * getNode( const Guid & nodeType );
        void                        attachNode( const Guid & nodeType, NodeBase * node );
        void                        detachNode( const Guid & nodeType ) { attachNode( nodeType, NULL ); }

        // templated node helpers
        template<class T> const T * getNode() const        { return (const T*)getNode( T::guid() ); }
        template<class T> T       * getNode()              { return (T*)getNode( T::guid() ); }
        template<class T> void      attachNode( T * node ) { attachNode( T::guid(), node ); }

        //@}

    protected:

        //@{

        Entity( World & world, int id );
        virtual ~Entity();

        //@}

    private:

        friend class World;
        class  Impl;
        Impl * mImpl;
    };

    ///
    /// entity factory
    ///
    struct EntityFactory
    {
        /// initialize a newly created entity
        bool (*initializeEntity)( Entity &, const void * factoryParameter );
    };

    ///
    /// world class that manages instance of all entities
    ///
    class World
    {
    public:

        //@{

        World( gfx::GpuResourceDatabase & gdb );
        virtual ~World();

        gfx::GpuResourceDatabase & gdb() const;

        /// delete all entities, unregister all non-built-in factories
        void          clear();

        bool          hasEntityFactory( const Guid & type );
        bool          registerEntityFactory( const Guid & type, const char * desc, EntityFactory factory, const void * factoryParameter );
        void          unregisterEntityFactory( const Guid & type );
        EntityFactory getEntityFactory( const Guid & type );

        Entity      * createEntity( const Guid & type, const char * name = NULL );
        void          deleteEntity( const Guid & type, const char * name );
        void          deleteEntity( int id );
        void          deleteEntity( Entity * entity );
        void          deleteAllEntities();
        Entity      * findEntity( const Guid & type, const char * name );
        Entity      * findEntity( int id );
        Entity      * findOrCreateEntity( const Guid & type, const char * name );

        //@}

    private:

        friend class Entity;
        friend class Entity::Impl;
        class Impl;
        Impl * mImpl;
    };

    ///
    /// base class of all nodes
    ///
    class NodeBase : public NoCopy
    {
    public:

        //@{

        /// public destructor
        virtual ~NodeBase() {}

        /// Get the entity that the node belongs to
        Entity & entity() const { return mEntity; }

        /// Get node type
        const Guid & type() const;

        //@}

    protected:

        //@{

        // protected constructor
        NodeBase( Entity & entity ) : mEntity(entity) {}

        //@}

    private:

        Entity & mEntity;
    };

    /// build-in entity types
    //@{

    extern const Guid SPATIAL_ENTITY; ///< entity that has spatial node only
    extern const Guid VISUAL_ENTITY;  ///< entity that has spatial and visual node
    extern const Guid LIGHT_ENTITY;   ///< entity that has spatial and light node
    extern const Guid CAMERA_ENTITY;  ///< entity that represents a camera in the world. It has spatial and camera node.

    //@}

    // *************************************************************************
    // Camera class
    // *************************************************************************

    ///
    /// camera class
    ///
    class Camera
    {
        // *****************************
        // public methods
        // *****************************
    public:

        //@{

        /// constructor
        Camera();

        /// destructor
        ~Camera();

        //@}

        //@{

        void                 setViewMatrix( const Matrix44f & );
        const Matrix44f    & getViewMatrix() const;

        void                 setProjectionMatrix( const Matrix44f & );
        const Matrix44f    & getProjectionMatrix() const;

        void                 setViewport( const Rect<UInt32> & );
        const Rect<UInt32> & getViewport() const;

        //@}

    private:

        class Impl;
        Impl * mImpl;
    };

    // *************************************************************************
    // Spatial node
    // *************************************************************************

    ///
    /// Basic class that contains node's spatial inforamtion
    ///
    class SpatialNode : public NodeBase
    {
    public:

        //@{

        static const Guid & guid();

        //@}

        //@{

        virtual             ~SpatialNode();

        SpatialGraph      & graph() const;

        void                setParent( SpatialNode * parent, SpatialNode * prevSibling = NULL );
        void                setPosition( const Vector3f & );        ///< set position in parent space.
        void                setPivot( const Vector3f  & );          ///< set pivot in parent space
        void                setRotation( const Quaternionf & );     ///< set node rotation around the pivot point parent space.
        void                setBoundingSphere( const Spheref & s ); /// set bounding sphere, in local space

        SpatialNode       * getParent() const;
        SpatialNode       * getPrevSibling() const;
        SpatialNode       * getNextSibling() const;
        SpatialNode       * getFirstChild() const;
        SpatialNode       * getLastChild() const;

        const Vector3f    & getPosition() const;       ///< get position in parent space
        const Vector3f    & getPivot() const;          ///< get pivot in parent space
        const Quaternionf & getRotation() const;       ///< get orientation, in parent space
        const Spheref     & getBoundingSphere() const; ///< get bounding sphere, in local space
        const Matrix44f   & getLocal2Parent() const;   ///< get local space to parent space transformation matrix
        const Matrix44f   & getLocal2Root() const;     ///< get local space to root space transformation matrix

        //@}

    protected:

        //@{

        SpatialNode( Entity & entity, SpatialGraph & graph );

        //@}

    private:

        class Impl;
        Impl * mImpl;
    };

    ///
    /// manage spatial information of all nodes.
    ///
    /// Q: how to support multiple cameras?
    ///
    class SpatialGraph
    {
    public:

        //@{

        SpatialGraph() {}

        virtual ~SpatialGraph() {}

        //@}
    };

    // *************************************************************************
    // Visual node
    // *************************************************************************

    ///
    /// contains visual information
    ///
    class VisualNode : public NodeBase
    {
    public:

        //@{

        static const Guid & guid();

        //@}

        //@{

        /// public destructor
        virtual ~VisualNode();

        /// get the graph that this visual node belongs to.
        VisualGraph & graph() const;

        /// add new model to the node. return the model ID, or 0 for failure.
        int  addModel( gfx::GpuResource * model );

        /// remove all models that are attached to the node
        void removeAllModels();

        /// load models from file, and attach them to the visual node
        bool loadModelsFromFile( gfx::GpuResourceDatabase & db, const char * filename );

        /// load models from file, and attach them to the visual node
        bool loadModelsFromFile( gfx::GpuResourceDatabase & db, File & fp );

        /// the VisualNode implementation class
        class Impl;

        /// return reference to the instance of implementation class
        Impl & impl() { GN_ASSERT(mImpl); return *mImpl; }

        //@}

    protected:

        //@{

        VisualNode( Entity & entity, VisualGraph & graph );

        //@}

    private:

        Impl * mImpl;
    };

    ///
    /// Light description
    ///
    struct LightDesc
    {
        //@{
        Vector4f  diffuse;
        Vector4f  ambient;
        Vector4f  specular;
        //@}

        /// construct default light
        LightDesc()
            : diffuse(1.0f,1.0f,1.0f,1.0f)
            , ambient(0.2f,0.2f,0.2f,0.2f)
            , specular(0.6f,0.6f,0.6f,0.6f)
        {
        }
    };

    ///
    /// built-in light node that contains standard lighting information
    ///
    class LightNode : public NodeBase
    {
    public:

        //@{

        static const Guid & guid();

        //@}

        //@{

        virtual          ~LightNode();

        const LightDesc & getDesc() const;
        void              setDesc( const LightDesc & desc );

        /// the LightNode implementation class
        class Impl;

        /// return reference to the instance of implementation class
        Impl & impl() { GN_ASSERT(mImpl); return *mImpl; }

        //@}

    protected:

        //@{

        LightNode( Entity & entity, VisualGraph & graph );

        //@}

    private:

        Impl * mImpl;
    };

    ///
    /// Manags all visual and light nodes. Store global rendering data.
    ///
    class VisualGraph
    {
    public:

        //@{

        VisualGraph( gfx::GpuResourceDatabase & gdb );

        virtual ~VisualGraph();

        gfx::GpuResourceDatabase & gdb() const;

        void draw( Camera & camera );

        class Impl;
        Impl & impl() const { GN_ASSERT(mImpl); return *mImpl; }

        //@}

    private:

        Impl * mImpl;
    };
}}

#include "scene.inl"

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_SCENE_SCENE_H__
