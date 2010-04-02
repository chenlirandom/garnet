#ifndef __GN_UTIL_SCENE_H__
#define __GN_UTIL_SCENE_H__
// *****************************************************************************
/// \file
/// \brief   Scene management utilites
/// \author  chen@@chenli-homepc (2009.9.6)
// *****************************************************************************

#include "garnet/GNgfx.h"

namespace GN { namespace util
{
    class Entity;
    class World;
    class NodeBase;
    class SpatialNode;
    class SpatialGraph;
    class VisualNode;
    class LightNode;
    class VisualGraph;

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
        const char                * Name() const;

        bool                        hasNode( const Guid & nodeType ) const;
        const NodeBase            * getNode( const Guid & nodeType ) const;
        NodeBase                  * getNode( const Guid & nodeType );
        void                        attachNode( const Guid & nodeType, NodeBase * node );
        void                        detachNode( const Guid & nodeType ) { attachNode( nodeType, NULL ); }

        // templated node helpers
        template<class T> const T * getNode() const        { return (const T*)getNode( T::GetGuid() ); }
        template<class T> T       * getNode()              { return (T*)getNode( T::GetGuid() ); }
        template<class T> void      attachNode( T * node ) { attachNode( T::GetGuid(), node ); }

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
    /// world class that manages instance of all entities
    ///
    class World
    {
    public:

        //@{

        World( gfx::GpuResourceDatabase & gdb );
        virtual ~World();

        //@}

        //@{

        gfx::GpuResourceDatabase & gdb() const;
        SpatialGraph             & spatialGraph() const;
        VisualGraph              & visualGraph() const;

        //@}

        void          Reset(); ///< Reset the world to its initial state

        Entity      * createEntity( const char * name = NULL );
        void          deleteEntity( const char * name );
        void          deleteEntity( int id );
        void          deleteEntity( Entity * entity );
        void          deleteAllEntities();
        Entity      * findEntity( const char * name );
        Entity      * findEntity( int id );
        Entity      * findOrCreateEntity( const char * name );

        //@}

        //@{

        /// Create entity that has spatial node only
        Entity * createSpatialEntity( const char * name = NULL );

        /// Create entity that has spatial and visual node
        Entity * createVisualEntity( const char * name = NULL );

        /// Create entity that has spatial and light node
        Entity * createLightEntity( const char * name = NULL );
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
        const Guid & Type() const;

        //@}

    protected:

        //@{

        // protected constructor
        NodeBase( Entity & entity ) : mEntity(entity) {}

        //@}

    private:

        Entity & mEntity;
    };

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

        static const Guid & GetGuid();

        //@}

        //@{

        virtual             ~SpatialNode();

        SpatialGraph      & graph() const;

        void                SetParent( SpatialNode * parent, SpatialNode * prevSibling = NULL );
        void                setPosition( const Vector3f & );        ///< set position in parent space.
        void                setRotation( const Quaternionf & );     ///< set rotation around it's local origin.
        void                setScale( const Vector3f & );           ///< set scaling for each axis.
        void                setBoundingSphere( const Spheref & s ); ///< set bounding sphere for the node itself, in local space.

        SpatialNode       * GetParent() const;
        SpatialNode       * GetPrevSibling() const;
        SpatialNode       * GetNextSibling() const;
        SpatialNode       * GetFirstChild() const;
        SpatialNode       * getLastChild() const;

        const Vector3f    & getPosition() const;       ///< get position in parent space
        const Quaternionf & getRotation() const;       ///< get orientation, in parent space
        const Vector3f    & getScale() const;          ///< get scaling for each axis.
        const Spheref     & getBoundingSphere() const; ///< get bounding sphere for the node itself, in local space
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

        static const Guid & GetGuid();

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
        bool loadModelsFromFile( const char * filename );

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

        static const Guid & GetGuid();

        //@}

        //@{

        virtual          ~LightNode();

        const LightDesc & GetDesc() const;
        void              SetDesc( const LightDesc & desc );

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

        class Impl;

        VisualGraph( gfx::GpuResourceDatabase & gdb );
        virtual ~VisualGraph();

        gfx::GpuResourceDatabase & gdb() const;
        Impl                     & impl() const { GN_ASSERT(mImpl); return *mImpl; }
        void                       Draw( Camera & camera );

        //@}

    private:

        Impl * mImpl;
    };

    ///
    /// A simple world descriptor
    ///
    struct SimpleWorldDesc : public NoCopy
    {
        //@{
        struct SpatialDesc
        {
            StrA        parent;
            Vector3f    position;
            Quaternionf orientation;
            Boxf        bbox; // bounding box of this spatial node itself (children are not included)
        };

        struct EntityDesc
        {
            SpatialDesc     spatial;
            DynaArray<StrA> models;
        };

        StringMap<char,gfx::MeshResourceDesc>  meshes;
        DynaArray<AutoRef<Blob> >              meshdata;
        StringMap<char,gfx::ModelResourceDesc> models;
        StringMap<char,EntityDesc>             entities;
        Boxf                                   bbox;     ///< bounding box of the whole world.
        //@}

        ///
        /// clear the description
        ///
        void Clear();

        ///
        /// load world description from external file
        ///
        bool LoadFromFile( const char * filename );

        ///
        /// Save the world to specific directory
        ///
        bool SaveToFile( const char * filename );

        /// create entities in the world according to the descriptor.
        /// Return the root spatial node represent the whole world.
        Entity * populateTheWorld( World & world ) const;
    };
}}

#include "scene.inl"

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_UTIL_SCENE_H__