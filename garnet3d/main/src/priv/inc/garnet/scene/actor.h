#ifndef __GN_SCENE_ACTOR_H__
#define __GN_SCENE_ACTOR_H__
// *****************************************************************************
//! \file    scene/actor.h
//! \brief   Interface of actor class
//! \author  chen@@CHENLI-HOMEPC (2007.4.1)
// *****************************************************************************

namespace GN
{
    ///
    /// general tree structure
    ///
    template<class T>
    class TreeNode
    {
        TreeNode<T> * mParent; ///< to the parent
        TreeNode<T> * mPrev;   ///< to the previous brother
        TreeNode<T> * mNext;   ///< to the next brother
        TreeNode<T> * mChild;  ///< to the first child

    public:

        //@{
        //@}

        //@{
        TreeNode() : mParent(0), mPrev(0), mNext(0), mChild(0) {}
        virtual ~TreeNode() { doDtor(); }
        //@}

        //@{
        TreeNode<T> * getParent() const { return mParent; }
        TreeNode<T> * getPrevSibling() const { return mPrev; }
        TreeNode<T> * getNextSibling() const { return mNext; }
        TreeNode<T> * getFirstChild() const { return mChild; }
        void setParent( TreeNode<T> * newParent, TreeNode<T> * newPrev ) { doSetParent( newParent, newPrev ); }
        //@}

        //@{
        bool isDescendant( const TreeNode<T> * p ) const { return doIsDescendant( p ); }
        size_t calcChildrenCount() const { return doCalcChildrenCount(); }
        //@}

    private:

        void doDtor()
        {
            // detach from parent
            setParent( 0, 0 );
            GN_ASSERT( 0 == mParent );
            GN_ASSERT( 0 == mPrev );
            GN_ASSERT( 0 == mNext );

            // remove all children
            TreeNode<T> * c1 = mChild, * c2;
            while( c1 )
            {
                c2 = c1->mNext;
                c1->setParent( 0, 0 );
                c1 = c2;
            }

            GN_ASSERT( 0 == mChild );
        }

        ///
        /// make sure a valid parent pointer.
        ///
        /// 1. Can be NULL.
        /// 2. Cannot be this
        /// 3. Cannot be in child tree.
        ///
        bool checkParent( const TreeNode<T> * p ) const
        {
            if( this == p )
            {
                static Logger * sLogger = getLogger("GN.scene.TreeNode");
                GN_ERROR(sLogger)( "can't set itself as parent" );
                return false;
            }
            if( isDescendant( p ) )
            {
                static Logger * sLogger = getLogger("GN.scene.TreeNode");
                GN_ERROR(sLogger)( "can't descendant as parent" );
                return false;
            }
            // TODO: traverse child tree to make sure 'p' is not in it.
            return true;
        }

        bool checkPrev( const TreeNode<T> * parent, const TreeNode<T> * prev ) const
        {
            if( 0 == parent ) return true; // prev will be ignored, if parent is NULL.

            if( this == prev )
            {
                static Logger * sLogger = getLogger("GN.scene.TreeNode");
                GN_ERROR(sLogger)( "can't set itself as prev node" );
                return false;
            }

            if( 0 == prev ) return true;

            if( prev->mParent != parent )
            {
                static Logger * sLogger = getLogger("GN.scene.TreeNode");
                GN_ERROR(sLogger)( "prev node belongs to another parent." );
                return false;
            }

            return true;
        }

        void doSetParent( TreeNode<T> * newParent, TreeNode<T> * newPrev )
        {
            if( newParent == mParent ) return;

            GN_ASSERT( checkParent( newParent ) );
            GN_ASSERT( checkPrev( newParent, newPrev ) );

            if( newParent )
            {
                // detach from old parent
                setParent( 0, 0 );

                // attach to new parent
                mParent = newParent;
                if( newPrev )
                {
                    TreeNode<T> * c =  newParent->mChild;
                    while( c )
                    {
                        c = c->mNext;
                        if( c == newPrev )
                        {
                            mPrev = newPrev;
                            mNext = newPrev->mNext;
                            if( mNext ) mNext->mPrev = this;
                            newPrev->mNext = this;
                            break;
                        }
                    }
                    if( 0 == c )
                    {
                        static Logger * sLogger = getLogger("GN.scene.TreeNode");
                        GN_ERROR(sLogger)( "newPrev is not direct child of newParent!" );
                        GN_UNEXPECTED();
                    }
                }
                else
                {
                    mPrev = 0;
                    mNext = newParent->mChild;
                    newParent->mChild = this;
                    if( mNext ) mNext->mPrev = this;
                }
            }
            else
            {
                if( mParent->mChild == this ) mParent->mChild = mNext;

                TreeNode<T> * p = mPrev, * n = mNext;

                if( p ) p->mNext = n;
                if( n ) n->mPrev = p;

                mParent = 0;
                mPrev = 0;
                mNext = 0;
             }
        }

        bool doIsDescendant( const TreeNode<T> * p ) const
        {
            for( TreeNode<T> * c = mChild; c; c = c->mNext )
            {
                if( p == c ) return true;
                if( c->doIsDescendant( p ) ) return true;
            }
            return false;
        }

        size_t doCalcChildrenCount() const
        {
            size_t n = 0;
            for( TreeNode<T> * c = mChild; c; c = c->mNext )
            {
                n += c->doCalcChildrenCount() + 1;
            }
            return n;
        }
    };

    ///
    /// traverse tree structure in pre-order
    ///
    template<class T>
    class TreeTraversePreOrder
    {
        T * mFirstNode;

    public:

        ///
        /// ctor
        ///
        TreeTraversePreOrder( T * root )
        {
            GN_ASSERT( root );
            mFirstNode = root;
        }

        //@{

        void reset( T * root )
        {
            GN_ASSERT( root );
            mFirstNode = root;
        }

        T * first() const { return mFirstNode; }

        T * next( T * current, int * level = 0 ) const
        {
            GN_ASSERT( current );

            // if( has child ) next is child
            T * n = safeCast<T*>( current->getFirstChild() );
            if( n )
            {
                if( level ) ++(*level);
                return n;
            }

            // if( has brother ) next is brother
            n = safeCast<T*>( current->getNextSibling() );
            if( n ) return n;

            // check parent
            T * p = safeCast<T*>( current->getParent() );
            while( p )
            {
                // if( parent has next ) next is parent's next
                n = safeCast<T*>( p->getNextSibling() );
                if( n )
                {
                    if( level ) --(*level);
                    return n;
                }

                // loop one level up
                p = safeCast<T*>( p->getParent() );
            }

            // if( no parent ) done.
            return 0;
        }

        //@}
    };

    ///
    /// traverse tree structure in post-order
    ///
    template<class T>
    class TreeTraversePostOrder
    {
        T * mFirstNode;

    public:

        ///
        /// ctor
        ///
        TreeTraversePostOrder( T * root )
        {
            GN_ASSERT( root );
            T * c;
            while( NULL != ( c = safeCast<T*>( root->getFirstChild() ) ) ) root = c;
            mFirstNode = root;
            GN_ASSERT( root );
        }

        //@{

        T * first() const { return mFirstNode; }

        T * next( T * current ) const
        {
            GN_ASSERT( current );

            T * n = safeCast<T*>( current->getNextSibling() );

            if( n )
            {
                T * c;
                while( NULL != ( c = safeCast<T*>( n->getFirstChild() ) ) ) n = c;
                GN_ASSERT( n );
                return n;
            }
            else
            {
                return safeCast<T*>( current->getParent() );
            }
        }

        //@}
    };
}

namespace GN { namespace scene
{
    ///
    /// actor is the basic/atomic item that you can put into virtual scene.
    ///
    class Actor
    {
        Scene             & mScene;

        TreeNode<Actor>     mNode;

        DynaArray<Drawable> mDrawables;

        Vector3f            mPosition;       ///< position in parent space
        Vector3f            mPivot;          ///< origin of rotation, in local space.
        Quaternionf         mRotation;       ///< rotation in parent space
        Matrix44f           mLocal2Parent;   ///< local->parent space transformation
        Matrix44f           mParent2Local;   ///< parent->local space transformation
        Matrix44f           mLocal2Root;     ///< local->root space transformation
        Matrix44f           mRoot2Local;     ///< root->local transformation
        Spheref             mBoundingSphere; ///< bounding sphere.

        union
        {
            UInt32          mDirtyFlags; ///< all dirty flags as one integer
            struct
            {
                unsigned int mTransformDirty :  1;
                unsigned int                 : 31; // reserved
            };
        };

    public:

        //@{

        explicit Actor( Scene & );
        ~Actor();

        //@}

        //@{

        void setNumDrawables( size_t );
        void setDrawable( size_t, const Drawable & );
        void addDrawable( const Drawable & );
        void setParent( Actor * newParent, Actor * newPrev = 0 );
        void setPosition( const Vector3f & );
        void setPivot( const Vector3f & );
        void setRotation( const Quaternionf & );
        void setBoundingSphere( const Spheref & );

        Scene             & getScene() const { return mScene; }
        size_t              getNumDrawables() const { return mDrawables.size(); }
        const Drawable    & getDrawable( size_t i ) const { return mDrawables[i]; }
        Actor             * getParent() const { return node2actor( mNode.getParent() ); }
        Actor             * getPrevSibling() const { return node2actor( mNode.getPrevSibling() ); }
        Actor             * getNextSibling() const { return node2actor( mNode.getNextSibling() ); }
        Actor             * getFirstChild() const { return node2actor( mNode.getFirstChild() ); }
        Actor             * getLastChild() const;
        const Vector3f    & getPosition() const { return mPosition; }
        const Vector3f    & getPivot() const { return mPivot; }
        const Quaternionf & getRotation() const { return mRotation; }
        const Matrix44f   & getLocal2Parent() const { if( mTransformDirty ) { const_cast<Actor*>(this)->calcTransform(); } return mLocal2Parent; }
        const Matrix44f   & getLocal2Root() const { if( mTransformDirty ) { const_cast<Actor*>(this)->calcTransform(); } return mLocal2Root; }
        const Spheref     & getBoundingSphere() const { return mBoundingSphere; }

        //@}

        //@{

        void clear(); ///< clear to empty
        void copyto( Actor & ) const; ///< make clone, except node properties
        bool loadFromXmlNode( const XmlNode & node, const StrA & basedir );
        virtual void draw();

        //@}

    private:

        void dirtyTransform(); // mark transformation as dirty;

        void calcTransform();

        static inline Actor * node2actor( TreeNode<Actor> * n )
        {
            return n ? (Actor*)((UInt8*)n - GN_FIELD_OFFSET(Actor,mNode)) : 0;
        }

        static inline TreeNode<Actor> * actor2node( Actor * a )
        {
            return a ? (TreeNode<Actor>*)((UInt8*)a + GN_FIELD_OFFSET(Actor,mNode)) : 0;
        }
    };
}}

#include "actor.inl"

// *****************************************************************************
//                           End of actor.h
// *****************************************************************************
#endif // __GN_SCENE_ACTOR_H__
