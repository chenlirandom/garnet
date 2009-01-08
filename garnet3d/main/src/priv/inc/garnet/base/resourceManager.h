#ifndef __GN_BASE_RESOURCEMANAGER_H__
#define __GN_BASE_RESOURCEMANAGER_H__
// *****************************************************************************
/// \file
/// \brief   general resource manager: map name to instance
/// \author  chenlee (2005.8.17)
// *****************************************************************************

namespace GN
{
    namespace detail
    {
        ///
        /// Singleton selector
        ///
        template<class C, int>
        struct SingletonSelector
        {
            typedef Singleton<C> type; ///< singleton type
        };

        ///
        /// Singleton selector
        ///
        template<class C>
        struct SingletonSelector<C,0>
        {
            ///
            /// non-singleton type
            ///
            struct type {};
        };
    }

    ///
    /// Resource manager template. Used to do mapping among name and ID and instance.
    ///
    /// RES must support default constructor and assignment operation.
    ///
    /// ��Դ������ʹ��4���ص�������������Դ�Ĵ�����ɾ��:
    /// - Creator: ��������
    ///   - ����������Դʵ��. ��������һ��ȫ�ֵĴ�������, �����е���Դʹ��. ͬʱ, ÿ����Դ
    ///     Ҳ����ӵ���Լ��ض��Ĵ�������.
    /// - Deletor: ɾ������
    ///   - ����ɾ����Դʵ��. ���������ҽ���һ��ȫ�ֵ�ɾ������, ��������Դʹ��.
    /// - Nullor: �պ���
    ///   - ����Դ����ʧ��(Creator����null)ʱ, �����������Nullor������һ����ν��"��"����,
    ///     ��������Ǹ�ʧ�ܵ���Դ.
    ///   - ��Creator����, ��������һ��ȫ�ֵĿպ���, ��������Դʹ��. ͬʱ, ÿ����ԴҲ������
    ///     �Լ��ض��Ŀպ���.
    ///   - ʹ�ÿն����Ŀ�ľ���: ��֤��Դ���������ǻ᷵����Ч����Դʵ��. ������ʹ���ߵĴ�����
    ///     �Ϳ��Ա������µĴ���:
    ///         MyResource * ptr = myResourceMgr.getResource( theHandle );
    ///         if( ptr )
    ///             do_something_normal();
    ///         else
    ///             report_error();
    ///   - �ն���Ӧ������������ʹ���ߵ�ע��, �Ҳ�������������.
    ///     - ��������ô���ɫ��1x1��ͼ��Ϊ����ͼ, ��һ���󷽿���Ϊ��mesh.
    /// - NameResolver:
    ///   - A single resource may have several different names (a.k.a aliases). Name resolver is responsible
    ///     for converting aliase to real resource name.
    ///   - A example of name resolver is resolving relative resource path into absolute resource path.
    ///   - By default, name resolver does nothing.
    /// - NameChecker: ���ּ�麯��
    ///   - ���û���ͼ����������һ�������ڵ���Դʱ, �����������NameCheker�����������ֵ���Ч��,
    ///     ������Ч�������Զ�������Դ��������(�μ�getResourceHandle()�Ĵ���).
    ///   - һ�����õ�NameChecker���Ǽ��������Ƿ��Ӧһ����Ч�Ĵ����ļ�. ����, ���û���ͼ����һ��
    ///     ������Դ��������, �������ڴ����ϵ���Դʱ, ����Դ�ͻᱻ�Զ�������Դ������.
    ///
    template<typename RES, typename HANDLE=UInt32, bool SINGLETON=false>
    class ResourceManagerTempl : public detail::SingletonSelector<ResourceManagerTempl<RES,HANDLE,SINGLETON>,SINGLETON>::type
    {
    public:

        typedef HANDLE HandleType; ///< resource Handle. 0 means invalid handle

        typedef RES ResType; ///< resource type

        typedef Delegate3<bool,RES&,const StrA &,void*> Creator; ///< Resource creation functor

        typedef Delegate2<void,RES&,void*> Deletor; ///< Resource deletion functor

        typedef Delegate2<void,StrA&,const StrA&> NameResolver; ///< Resource name resolver.

        typedef Delegate1<bool,const StrA&> NameChecker; ///< Resource name checker.

        ///
        /// Default constructor
        ///
        ResourceManagerTempl(
            const Creator & creator = Creator(),
            const Deletor & deletor = Deletor(),
            const Creator & nullor = Creator(),
            const NameResolver & resolver = NameResolver(),
            const NameChecker & checker = NameChecker() )
            : mCreator(creator)
            , mDeletor(deletor)
            , mNullor(nullor)
            , mNameResolver(resolver)
            , mNameChecker(checker)
            , mNullInstance(0)
        {
            mLRUHead.prev = NULL;
            mLRUHead.next = &mLRUTail;
            mLRUTail.prev = &mLRUHead;
            mLRUTail.next = NULL;
        }

        ///
        /// Default destructor
        ///
        ~ResourceManagerTempl() { clear(); }

        ///
        /// Get global creator.
        ///
        const Creator & getCreator() const { return mCreator; }

        ///
        /// Get global resource deletor.
        ///
        const Deletor & getDeletor() const
        {
            if( !empty() )
            {
                GN_WARN(sLogger)( "It is not safe to change delete functor, while resource manager is not empty." );
            }
            return mDeletor;
        }

        ///
        /// Get global NULL instance creator
        ///
        const Creator & getNullor() const { return mNullor; }

        ///
        /// Get global resource name resolver
        ///
        const NameResolver & getNameResolver() const { return mNameResolver; }

        ///
        /// Get global resource name checker
        ///
        const NameChecker & getNameChecker() const { return mNameChecker; }

        ///
        /// Set global creator
        ///
        void setCreator( const Creator & c ) { mCreator = c; }

        ///
        /// Set global NULL instance creator
        ///
        void setNullor( const Creator & n )
        {
            deleteNullInstance();
            mNullor = n;
        }

        ///
        /// Set global deletor
        ///
        void setDeletor( const Deletor & d ) { mDeletor = d; }

        ///
        /// Set global resource name resolver
        ///
        void setNameResolver( const NameResolver & s ) { mNameResolver = s; }

        ///
        /// Set global resource name checker
        ///
        void setNameChecker( const NameChecker & s ) { mNameChecker = s; }

        ///
        /// Clear all resources.
        ///
        void clear()
        {
            GN_GUARD;

            // dispose all resources
            disposeAll();

            // delete resource descriptions
            HandleType h = mResHandles.first();
            while( h )
            {
                GN_ASSERT( mResHandles[h] );
                delete mResHandles[h];
                h = mResHandles.next( h );
            }

            // clear handles and names
            mResHandles.clear();
            mResNames.clear();

            GN_UNGUARD;
        }

        ///
        /// Is resource manager empty or not?
        ///
        bool empty() const
        {
            GN_ASSERT( mResHandles.size() == mResNames.size() );
            return mResHandles.empty() && NULL == mNullInstance;
        }

        ///
        /// Return true for valid resource handle
        ///
        bool validResourceHandle( HandleType h ) const { return mResHandles.validHandle( h ); }

        ///
        /// Return true for valid resource name
        ///
        bool validResourceName( const StrA & n ) const
        {
            StrA realname;
            return mResNames.end() != mResNames.find( resolveName(realname,n) );
        }

        ///
        /// Get resource by handle.
        ///
        bool getResource( RES & result, HandleType handle )
        {
            GN_GUARD_SLOW;
            return getResourceImpl( result, handle, 0 );
            GN_UNGUARD_SLOW;
        }

        ///
        /// Get resource by handle.
        ///
        /// If failed, return default constructed resource instance.
        ///
        RES getResource( HandleType handle )
        {
            GN_GUARD_SLOW;
            RES res;
            if( getResource( res, handle ) ) return res;
            else return RES();
            GN_UNGUARD_SLOW;
        }

        ///
        /// Get resource by name
        ///
        /// \sa getResourceHandle()
        ///
        bool getResource( RES & result, const StrA & name, bool autoAddNewName = true )
        {
            GN_GUARD_SLOW;
            StrA realname;
            HandleType h = getResourceHandle( resolveName(realname,name), autoAddNewName );
            return getResourceImpl( result, h, realname.cptr() );
            GN_UNGUARD_SLOW;
        }

        ///
        /// Get resource by name.
        ///
        /// If failed, return default constructed resource instance.
        ///
        /// \sa getResourceHandle()
        ///
        RES getResource( const StrA & name, bool autoAddNewName = true )
        {
            GN_GUARD_SLOW;
            RES res;
            StrA realname;
            if( getResource( res, resolveName(realname,name), autoAddNewName ) ) return res;
            else return RES();
            GN_UNGUARD_SLOW;
        }

        ///
        /// Get resource handle
        ///
        /// \param name
        ///     User specified resource name.
        /// \param autoAddNewName
        ///     - If true, when the resource name that is not in manager currently but pass name-checker,
        ///       it'll be add to manager automatically, and a valid handle will be return.
        ///     - If false, return 0 for non-exist resource name.
        ///
        HandleType getResourceHandle( const StrA & name, bool autoAddNewName = true )
        {
            GN_GUARD_SLOW;
            StrA realname;
            typename StringMap::const_iterator iter = mResNames.find( resolveName(realname,name) );
            if( mResNames.end() != iter ) return iter->second;
            if( autoAddNewName && ( !mNameChecker || mNameChecker(realname) ) ) return addResource( realname );
            return 0; // failed
            GN_UNGUARD_SLOW;
        }

        ///
        /// Get resource name
        ///
        const StrA & getResourceName( HandleType handle ) const
        {
            GN_GUARD_SLOW;
            if( validResourceHandle(handle) )
            {
                GN_ASSERT( mResHandles.get(handle) );
                return mResHandles.get(handle)->name;
            }
            else return StrA::EMPTYSTR;
            GN_UNGUARD_SLOW;
        }

        ///
        /// Add new resource item to manager
        ///
        HandleType addResource(
            const StrA & name,
            void * userData = 0,
            const Creator & creator = Creator(),
            const Creator & nullor  = Creator(),
            bool overrideExistingResource = false )
        {
            GN_GUARD;

            HandleType h;
            ResDesc * item;
            StrA realname;
            typename StringMap::const_iterator ci = mResNames.find( resolveName(realname,name) );
            if( mResNames.end() != ci )
            {
                if( !overrideExistingResource )
                {
                    GN_ERROR(sLogger)( "resource '%s' already exist!", realname.cptr() );
                    return 0;
                }
                GN_ASSERT( mResHandles.validHandle(ci->second) );
                h = ci->second;
                item = mResHandles.get( h );
                doDispose( item ); // dispose existing resource
            }
            else
            {
                item = new ResDesc;
                h = mResHandles.add( item );
                if( 0 == h )
                {
                    GN_ERROR(sLogger)( "Fail to create new resource item!" );
                    delete item;
                    return 0;
                }
                mResNames[realname] = h;
            }
            GN_ASSERT( mResNames.size() == mResHandles.size() );
            GN_ASSERT( mResHandles.validHandle(h) && item );
            item->creator = creator;
            item->nullor = nullor;
            item->name = realname;
            item->userData = userData;
            item->disposed = true;
            return h;

            GN_UNGUARD;
        }

        ///
        /// Remove resource from manager
        ///
        void removeResourceByHandle( HandleType handle )
        {
            GN_GUARD;
            if( validResourceHandle(handle) )
                removeResourceByName( mResHandles[handle]->name );
            else
                GN_ERROR(sLogger)( "invalid resource handle: %d", handle );
            GN_UNGUARD;
        }

        ///
        /// Remove resource from manager (unimplemented)
        ///
        void removeResourceByName( const StrA & name )
        {
            GN_GUARD;

            // find the resource
            StrA realname;
            typename StringMap::iterator iter = mResNames.find( resolveName(realname,name) );
            if( mResNames.end() == iter )
            {
                GN_ERROR(sLogger)( "invalid resource name: %s", realname.cptr() );
                return;
            }

            // get the resource handle and pointer
            HandleType h = iter->second;
            ResDesc * r = mResHandles[h];
            GN_ASSERT( r );

            // dispose it
            doDispose( r );

            // remove it from handle and name manager
            mResHandles.remove( h );
            mResNames.erase( iter );

            // remove it from LRU list
            if( r->prev )
            {
                GN_ASSERT( r->next );
                r->prev->next = r->next;
                r->next->prev = r->prev;
            }

            // delete it
            delete r;

            GN_UNGUARD;
        }

        ///
        /// Dispose specific resource
        ///
        void disposeResourceByHandle( HandleType h )
        {
            GN_GUARD;
            if( !validResourceHandle( h ) )
            {
                GN_ERROR(sLogger)( "invalid resource handle: %d", h );
                return;
            }
            doDispose( mResHandles[h] );
            GN_UNGUARD;
        }

        ///
        /// Dispose specific resource
        ///
        void disposeResourceByName( const StrA & name )
        {
            GN_GUARD;
            StrA realname;
            typename StringMap::const_iterator iter = mResNames.find( resolveName(realname,name) );
            if( mResNames.end() == iter )
            {
                GN_ERROR(sLogger)( "invalid resource name: %s", realname.cptr() );
                return;
            }
            disposeResourceByHandle( iter->second );
            GN_UNGUARD;
        }

        ///
        /// Release all resource instances. But keep resource manager itself unchanged.
        ///
        void disposeAll()
        {
            GN_GUARD;
            HandleType h = mResHandles.first();
            while( h )
            {
                doDispose( mResHandles.get(h) );
                h = mResHandles.next( h );
            }
            deleteNullInstance();
            GN_UNGUARD;
        }

        ///
        /// Preload all resources
        ///
        bool preload()
        {
            GN_GUARD;
            RES res;
            bool ok = true;
            HandleType h;
            for( h = mResHandles.first(); h != 0; h = mResHandles.next(h) )
            {
                ok &= getResource( res, h );
            }
            return ok;
            GN_UNGUARD;
        }

        ///
        /// Set user data for specfic resource
        ///
        void setUserData( HandleType h, void * data )
        {
            if( !validResourceHandle(h) )
            {
                GN_ERROR(sLogger)( "invalid resource handle: %d", h );
                return;
            }
            GN_ASSERT( mResHandles[h] );
            mResHandles[h]->userData = data;
        }

        // *****************************
        // private variables
        // *****************************

    private:

        struct ResDesc
        {
            Creator creator;
            Creator nullor; // Use to create per-resource "NULL" instance.
            RES     res;
            StrA    name;
            void *  userData;
            bool    disposed;

            // LRU list
            ResDesc * prev;
            ResDesc * next;

            ResDesc() : userData(0), prev(0), next(0) {}
        };

        typedef std::map<StrA,HandleType>          StringMap;
        typedef HandleManager<ResDesc*,HandleType> ResHandleMgr;

        ResHandleMgr mResHandles;
        StringMap    mResNames;

        ResDesc      mLRUHead;
        ResDesc      mLRUTail;

        // global resource manipulators
        Creator      mCreator;
        Deletor      mDeletor;
        Creator      mNullor; // Use to create default "NULL" instance.
        NameResolver mNameResolver;
        NameChecker  mNameChecker;

        RES   * mNullInstance;
        Deletor mNullDeletor;

        static Logger * sLogger;

        // *****************************
        // private methods
        // *****************************

    private:

        StrA & resolveName( StrA & out, const StrA & in ) const
        {
            if( mNameResolver ) mNameResolver( out, in );
            else out = in;
            return out;
        }

        bool getResourceImpl( RES & res, HandleType handle, const char * name )
        {
            GN_GUARD_SLOW;

            if( !validResourceHandle(handle) )
            {
                if( name )
                    GN_ERROR(sLogger)( "Resource '%s' is invalid. Fall back to null instance...", name );
                else
                    GN_ERROR(sLogger)( "Resource handle '%d' is invalid. Fall back to null instance...", handle );

                if( 0 == mNullInstance )
                {
                    RES * tmp = new RES;
                    if( !mNullor || !mNullor( *tmp, name, 0 ) )
                    {
                        if( name )
                            GN_ERROR(sLogger)( "Fail to create null instance for resource '%s'.", name );
                        else
                            GN_ERROR(sLogger)( "Fail to create null instance for resource handle '%d'.", handle );
                        delete tmp;
                        return false;
                    }
                    mNullInstance = tmp;
                    mNullDeletor = mDeletor;
                }
                GN_ASSERT( mNullInstance );
                res = *mNullInstance;
                return true;
            }

            ResDesc * item = mResHandles.get( handle );

            GN_ASSERT( item );

            if( item->disposed )
            {
                bool ok = false;

                if( item->creator )
                {
                    ok = item->creator( item->res, item->name, item->userData );
                }
                else if( mCreator )
                {
                    ok = mCreator( item->res, item->name, item->userData );
                }

                if( !ok )
                {
                    GN_WARN(sLogger)( "Fall back to null instance for resource '%s'.", item->name.cptr() );
                    if( item->nullor )
                    {
                        ok = item->nullor( item->res, item->name, item->userData );
                    }
                    if( !ok && mNullor )
                    {
                        ok = mNullor( item->res, item->name, item->userData );
                    }
                    if( !ok )
                    {
                        GN_ERROR(sLogger)( "Fail to create NULL instance for resource '%s'.", item->name.cptr() );
                        return false;
                    }
                }

                GN_ASSERT( ok );
                item->disposed = false;
            }

            // adjust access queue: move item to head of LRU list.
            if( item->prev )
            {
                GN_ASSERT( item->next );
                item->prev->next = item->next;
                item->next->prev = item->prev;
            }
            item->next = mLRUHead.next;
            item->prev = &mLRUHead;
            mLRUHead.next = item;

            // success
            res = item->res;
            return true;

            GN_UNGUARD_SLOW;
        }

        void doDispose( ResDesc * item )
        {
            GN_ASSERT( item );
            if( !item->disposed )
            {
                if( mDeletor ) mDeletor( item->res, item->userData );
                item->disposed = true;
            }
        }

        void deleteNullInstance()
        {
            if( mNullInstance )
            {
                if( mNullDeletor )
                {
                    mNullDeletor( *mNullInstance, 0 );
                    mNullDeletor.clear();
                }
                delete mNullInstance;
                mNullInstance = 0;
            }
            GN_ASSERT( !mNullDeletor && !mNullInstance );
        }
    };

    template<typename RES, typename HANDLE, bool SINGLETON>
    GN::Logger * ResourceManagerTempl<RES,HANDLE,SINGLETON>::sLogger = getLogger("GN.base.ResourceManagerTempl");

}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_RESOURCEMANAGER_H__
