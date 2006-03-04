#ifndef __GN_BASE_RESOURCEMANAGER_H__
#define __GN_BASE_RESOURCEMANAGER_H__
// *****************************************************************************
//! \file    resourceManager.h
//! \brief   general resource manager: map name to instance
//! \author  chenlee (2005.8.17)
// *****************************************************************************

namespace GN
{
    namespace detail
    {
        //!
        //! Singleton selector
        //!
        template<class C, int>
        struct SingletonSelector
        {
            typedef Singleton<C> type; //!< singleton type
        };

        //!
        //! Singleton selector
        //!
        template<class C>
        struct SingletonSelector<C,0>
        {
            //!
            //! non-singleton type
            //!
            struct type {};
        };
    }

    //!
    //! Resource manager.
    //!
    //! RES must support default constructor and assignment operation.
    //!
    //! ��Դ������ʹ��4���ص�������������Դ�Ĵ�����ɾ��: 
    //! - Creator: ��������
    //!   - ����������Դʵ��. ��������һ��ȫ�ֵĴ�������, �����е���Դʹ��. ͬʱ, ÿ����Դ
    //!     Ҳ����ӵ���Լ��ض��Ĵ�������.
    //! - Deletor: ɾ������
    //!   - ����ɾ����Դʵ��. ���������ҽ���һ��ȫ�ֵ�ɾ������, ��������Դʹ��.
    //! - Nullor: �պ���
    //!   - ����Դ����ʧ��(Creator����null)ʱ, �����������Nullor������һ����ν��"��"����,
    //!     ��������Ǹ�ʧ�ܵ���Դ.
    //!   - ��Creator����, ��������һ��ȫ�ֵĿպ���, ��������Դʹ��. ͬʱ, ÿ����ԴҲ������
    //!     �Լ��ض��Ŀպ���.
    //!   - ʹ�ÿն����Ŀ�ľ���: ��֤��Դ���������ǻ᷵����Ч����Դʵ��. ������ʹ���ߵĴ�����
    //!     �Ϳ��Ա������µĴ���:
    //!         MyResource * ptr = myResourceMgr.getResource( theHandle );
    //!         if( ptr )
    //!             do_something_normal();
    //!         else
    //!             report_error();
    //!   - �ն���Ӧ������������ʹ���ߵ�ע��, �Ҳ�������������.
    //!     - ��������ô���ɫ��1x1��ͼ��Ϊ����ͼ, ��һ���󷽿���Ϊ��mesh.
    //! - NameChecker: ���ּ�麯��
    //!   - ���û���ͼ����������һ�������ڵ���Դʱ, �����������NameCheker�����������ֵ���Ч��,
    //!     ������Ч�������Զ�������Դ��������(�μ�getResourceHandle()�Ĵ���).
    //!   - һ�����õ�NameChecker���Ǽ��������Ƿ��Ӧһ����Ч�Ĵ����ļ�. ����, ���û���ͼ����һ��
    //!     ������Դ��������, �������ڴ����ϵ���Դʱ, ����Դ�ͻᱻ�Զ�������Դ������.
    //!
    template<typename RES, bool SINGLETON=false>
    class ResourceManager : public detail::SingletonSelector<ResourceManager<RES,SINGLETON>,SINGLETON>::type
    {
    public:

        typedef uint32_t HandleType; //!< resource Handle. 0 means invalid handle

        typedef RES ResType; //!< resource type

        typedef Functor3<bool,RES&,const StrA &,void*> Creator; //!< Resource creation functor

        typedef Functor2<void,RES&,void*> Deletor; //!< Resource deletion functor

        typedef Functor1<bool,const StrA&> NameChecker; //!< Resource name checker.

        //!
        //! Default constructor
        //!
        ResourceManager(
            const Creator & creator = Creator(),
            const Deletor & deletor = Deletor(),
            const Creator & nullor = Creator(),
            const NameChecker & checker = NameChecker() )
            : mCreator(creator)
            , mDeletor(deletor)
            , mNullor(nullor)
            , mNameChecker(checker)
            , mNullInstance(0)
        {
        }

        //!
        //! Default destructor
        //!
        ~ResourceManager() { clear(); }

        //!
        //! Get global creator.
        //!
        const Creator & getCreator() const { return mCreator; }

        //!
        //! Get global resource deletor.
        //!
        const Deletor & getDeletor() const
        {
            if( !empty() )
            {
                GN_WARN( "It is not safe to change delete functor, while resource manager is not empty." );
            }
            return mDeletor;
        }

        //!
        //! Get global NULL instance creator
        //!
        const Creator & getNullor() const { return mNullor; }

        //!
        //! Get global resource name checker
        //!
        const NameChecker & getNameChecker() const { return mNameChecker; }

        //!
        //! Set global creator
        //!
        void setCreator( const Creator & c ) { mCreator = c; }

        //!
        //! Set global NULL instance creator
        //!
        void setNullor( const Creator & n )
        {
            deleteNullInstance();
            mNullor = n;
        }

        //!
        //! Set global deletor
        //!
        void setDeletor( const Deletor & d ) { mDeletor = d; }

        //!
        //! Set global resource name checker
        //!
        void setNameChecker( const NameChecker & s ) { mNameChecker = s; }

        //!
        //! Clear all resources.
        //!
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

        //!
        //! Is resource manager empty or not?
        //!
        bool empty() const
        {
            GN_ASSERT( mResHandles.size() == mResNames.size() );
            return mResHandles.empty() && NULL == mNullInstance;
        }

        //!
        //! Return true for valid resource handle
        //!
        bool validResourceHandle( HandleType h ) const { return mResHandles.validHandle( h ); }

        //!
        //! Return true for valid resource name
        //!
        bool validResourceName( const StrA & n ) const { return mResNames.end() != mResNames.find( n ); }

        //!
        //! Get resource by handle.
        //!
        bool getResource( RES & result, HandleType handle )
        {
            GN_GUARD_SLOW;
            return getResourceImpl( result, handle, 0 );
            GN_UNGUARD_SLOW;
        }

        //!
        //! Get resource by handle.
        //!
        //! If failed, return default constructed resource instance.
        //!
        RES getResource( HandleType handle )
        {
            GN_GUARD_SLOW;
            RES res;
            if( getResource( res, handle ) ) return res;
            else return RES();
            GN_UNGUARD_SLOW;
        }

        //!
        //! Get resource by name
        //!
        //! \sa getResourceHandle()
        //!
        bool getResource( RES & result, const StrA & name, bool autoAddNewName = true )
        {
            GN_GUARD_SLOW;
            HandleType h = getResourceHandle( name, autoAddNewName );
            return getResourceImpl( result, h, name.cstr() );
            GN_UNGUARD_SLOW;
        }

        //!
        //! Get resource by name.
        //!
        //! If failed, return default constructed resource instance.
        //!
        //! \sa getResourceHandle()
        //!
        RES getResource( const StrA & name, bool autoAddNewName = true )
        {
            GN_GUARD_SLOW;
            RES res;
            if( getResource( res, name, autoAddNewName ) ) return res;
            else return RES();
            GN_UNGUARD_SLOW;
        }

        //!
        //! Get resource handle
        //!
        //! \param name
        //!     User specified resource name.
        //! \param autoAddNewName
        //!     - If true, when the resource name that is not in manager currently but pass name-checker,
        //!       it'll be add to manager automatically, and a valid handle will be return.
        //!     - If false, return 0 for non-exist resource name.
        //!
        HandleType getResourceHandle( const StrA & name, bool autoAddNewName = true )
        {
            GN_GUARD_SLOW;
            StringMap::const_iterator iter = mResNames.find( name );
            if( mResNames.end() != iter ) return iter->second;
            if( autoAddNewName && ( !mNameChecker || mNameChecker(name) ) ) return addResource( name );
            return 0; // failed
            GN_UNGUARD_SLOW;
        }

        //!
        //! Get resource name
        //!
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

        //!
        //! Add new resource item to manager
        //!
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
            StringMap::const_iterator ci = mResNames.find(name);
            if( mResNames.end() != ci )
            {
                if( !overrideExistingResource )
                {
                    GN_ERROR( "resource '%s' already exist!", name.cstr() );
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
                    GN_ERROR( "Fail to create new resource item!" );
                    mResNames.erase(name);
                    delete item;
                    return 0;
                }
                mResNames[name] = h;
            }
            GN_ASSERT( mResNames.size() == mResHandles.size() );
            GN_ASSERT( mResHandles.validHandle(h) && item );
            item->creator = creator;
            item->nullor = nullor;
            item->name = name;
            item->userData = userData;
            item->disposed = true;
            return h;

            GN_UNGUARD;
        }

        //!
        //! Add a bunch of new resources to manager
        //!
        void addResources(
            const std::vector<StrA> & names,
            const Creator & creator = Creator(),
            const Creator & nullor  = Creator(),
            bool overrideExistingResource = false )
        {
            GN_GUARD;
            std::vector<StrA>::const_iterator i;
            for( i = names.begin(); i != names.end(); ++i )
            {
                addResource( (*i), creator, nullor, overrideExistingResource );
            }
            GN_UNGUARD;
        }

        //!
        //! Remove resource from manager
        //!
        void removeResourceHandle( HandleType handle )
        {
            GN_GUARD;
            if( !validResourceHandle(handle) )
            {
                GN_ERROR( "invalid resource handle: %d", handle );
                return;
            }
            StringMap::iterator iter = mResNames.find( mResHandles[handle]->name );
            GN_ASSERT( iter != mResNames.end() );
            doDispose( mResHandles[handle] );
            mResHandles.remove( handle );
            mResNames.erase( iter );
            GN_UNGUARD;
        }

        //!
        //! Remove resource from manager (unimplemented)
        //!
        void removeResourceName( const StrA & name )
        {
            GN_GUARD;
            StringMap::iterator iter = mResNames.find( name );
            if( mResNames.end() == iter )
            {
                GN_ERROR( "invalid resource name: %s", name.cstr() );
                return;
            }
            HandleType h = iter->second;
            GN_ASSERT( mResHandles.validHandle( h ) );
            doDispose( mResHandles[h] );
            mResHandles.remove( h );
            mResNames.erase( iter );
            GN_UNGUARD;
        }

        //!
        //! Dispose specific resource
        //!
        void disposeResourceHandle( HandleType h )
        {
            GN_GUARD;
            if( !validResourceHandle( h ) )
            {
                GN_ERROR( "invalid resource handle: %d", handle );
                return;
            }
            doDispose( mResHandles[h] );
            GN_UNGUARD;
        }

        //!
        //! Dispose specific resource
        //!
        void disposeResourceName( const StrA & name )
        {
            GN_GUARD;
            StringMap::const_iterator iter = mResNames.find( name );
            if( mResNames.end() == iter )
            {
                GN_ERROR( "invalid resource name: %s", name.cstr() );
                return;
            }
            disposeHandle( iter->second );
            GN_UNGUARD;
        }

        //!
        //! Release all resource instances. But keep resource manager itself unchanged.
        //!
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

        //!
        //! Preload all resources
        //!
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

        //!
        //! Set user data for specfic resource
        //!
        void setUserData( HandleType h, void * data )
        {
            if( !validResourceHandle(h) )
            {
                GN_ERROR( "invalid resource handle: %d", h );
                return;
            }
            GN_ASSERT( mResHandles[h] );
            mResHandles[h]->userData = data;
        }

        // *****************************
        // private variables
        // *****************************

    private:

        typedef ResourceManager<RES,SINGLETON> MyType;

        struct ResDesc
        {
            Creator creator;
            Creator nullor; // Use to create per-resource "NULL" instance.
            RES     res;
            StrA    name;
            void *  userData;
            bool    disposed;
        };

        typedef std::map<StrA,HandleType>          StringMap;
        typedef HandleManager<ResDesc*,HandleType> ResHandleMgr;

        ResHandleMgr mResHandles;
        StringMap    mResNames;

        // global resource manipulators
        Creator      mCreator;
        Deletor      mDeletor;
        Creator      mNullor; // Use to create default "NULL" instance.
        NameChecker  mNameChecker;

        RES   * mNullInstance;
        Deletor mNullDeletor;

        // *****************************
        // private methods
        // *****************************

    private:

        bool getResourceImpl( RES & res, HandleType handle, const char * name )
        {
            GN_GUARD_SLOW;

            if( !validResourceHandle(handle) )
            {
                if( name )
                    GN_ERROR( "Resource '%s' is invalid. Fall back to null instance...", name );
                else
                    GN_ERROR( "Resource handle '%d' is invalid. Fall back to null instance...", handle );

                if( 0 == mNullInstance )
                {
                    RES * tmp = new RES;
                    if( !mNullor || !mNullor( *tmp, name, 0 ) )
                    {
                        if( name )
                            GN_ERROR( "Fail to create null instance for resource '%s'.", name );
                        else
                            GN_ERROR( "Fail to create null instance for resource handle '%d'.", handle );
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
                    GN_WARN( "Fall back to null instance for resource '%s'.", item->name.cstr() );
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
                        GN_ERROR( "Fail to create NULL instance for resource '%s'.", item->name.cstr() );
                        return false;
                    }
                }

                GN_ASSERT( ok );
                item->disposed = false;
            }

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
}

// *****************************************************************************
//                           End of resourceManager.h
// *****************************************************************************
#endif // __GN_BASE_RESOURCEMANAGER_H__
