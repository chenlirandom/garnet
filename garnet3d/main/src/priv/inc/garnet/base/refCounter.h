#ifndef __GN_BASE_REFCOUNTER_H__
#define __GN_BASE_REFCOUNTER_H__
// *****************************************************************************
/// \file
/// \brief   Reference counting classes
/// \author  chenlee (2005.8.13)
// *****************************************************************************

namespace GN
{
    ///
    /// ���ü�����
    ///
    /// �ṩ���������ü������ܣ����ں� AutoRef һ��ʵ���Զ�ָ����
    ///
    /// \note
    /// ��ʹ�� RefCounter ʱ��Ϊ��֤ϵͳ�������������м�������������أ�
    ///     - �����ܵ�ʹ�� AutoRef ������ RefCounter ���ʵ��
    ///     - ������໥���õ�������ڣ�������ֱ�ӵĻ��Ǽ�ӵģ������ͷŶ���ǰ
    ///       �������ȴ����û�����������ڴ�й©�����еı�������Զ�޷��ͷ�
    ///     - ����Щ�ɣ��������Ժ��뵽��˵....
    ///
    class RefCounter : public NoCopy
    {
        // ********************************
        //       reference management
        // ********************************
    public :

        ///
        /// increase reference counter
        ///
        void incref() const  throw() { atomInc32(&mRef); }

        ///
        /// decrease reference counter, delete the object, if reference count reaches zero.
        ///
        void decref() const
        {
            if( 0 == atomDec32( &mRef ) )
            {
                // delete itself
                delete this;
            }
        }

        ///
        /// get current reference counter value
        ///
        SInt32 getref() const throw() { return atomGet32(&mRef); }

        // ********************************
        //    weak reference management
        // ********************************
    public :

        std::list<void*>::iterator _addWeakRef( void * p ) const
        {
            return mWeakRefList.insert( mWeakRefList.end(), p );
        }

        void _removeWeakRef( const std::list<void*>::iterator & iter ) const
        {
            mWeakRefList.erase( iter );
        }

        // ********************************
        /// \name protective ctor/dtor
        // ********************************
    protected:

        //@{

        ///
        /// Constructor
        ///
        RefCounter() : mRef(1) {}

        ///
        /// Destructor
        ///
        virtual ~RefCounter()
        {
            if( mRef > 0 )
            {
                GN_ERROR(getLogger("GN.base.RefCounter"))(
                    "destructing a refcounter with its reference counter "
                    "greater then zero!" );
            }

            // NULL all weak references
            while( !mWeakRefList.empty() )
            {
                NullifyWeakRef( *mWeakRefList.begin() );
            }
            GN_ASSERT( mWeakRefList.empty() );
        }

        //@}

        // ********************************
        // private methods
        // ********************************
    private:

        static inline void NullifyWeakRef( void * );

        // ********************************
        // private data members
        // ********************************
    private:

        ///
        /// reference counter
        ///
        mutable SInt32 mRef;

        mutable std::list<void*> mWeakRefList;
    };

    ///
    /// �� RefCounter ���ʹ�õ��Զ�ָ����
    ///
    template <class X, class MUTEX=SingleThreadMutex> class AutoRef
    {
        ///
        /// pointer to class X
        ///
        typedef X * XPTR;

        ///
        /// reference to class X
        ///
        typedef X & XREF;

    private:

        XPTR  mPtr; ///< internal pointer

    public :

        ///
        /// Instance of empty reference pointer
        ///
        static AutoRef<X,MUTEX> EMPTYPTR;

        ///
        /// construct from a normal pointer
        ///
        explicit AutoRef( XPTR p = 0 ) throw() : mPtr(p) {}

        ///
        /// copy constructor
        ///
        AutoRef( const AutoRef & p ) throw() : mPtr( p )
        {
            if(mPtr) mPtr->incref();
        }

        ///
        /// copy constructor
        ///
        template <class Y,class MUTEX2>
        AutoRef( const AutoRef<Y,MUTEX2> & p ) throw() : mPtr( p )
        {
            if(mPtr) mPtr->incref();
        }

        ///
        /// destructor
        ///
        ~AutoRef()
        {
            if(mPtr) mPtr->decref();
        }

        ///
        /// ��ֵ���
        ///
        AutoRef & operator = ( const AutoRef & rhs )
        {
            set( rhs );
            return *this;
        }

        ///
        /// ��ֵ���
        ///
        template <class Y,class MUTEX2>
        AutoRef & operator = ( const AutoRef<Y,MUTEX2> & rhs )
        {
            set( rhs );
            return *this;
        }

        ///
        /// Convert to XPTR
        ///
        operator XPTR () const { return mPtr; }

        ///
        /// �Ƚϲ���
        ///
        bool operator == ( const AutoRef & rhs ) const throw()
        {
            return mPtr == rhs.mPtr;
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator != ( const AutoRef & rhs ) const throw()
        {
            return mPtr != rhs.mPtr;
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator < ( const AutoRef & rhs ) const throw()
        {
            return mPtr < rhs.mPtr;
        }

        ///
        /// NOT operator
        ///
        bool operator !() const throw() { return !mPtr; }

        ///
        /// dereference operator.
        ///
        /// TODO: is this thread safe?
        ///
        XREF operator *() const throw()  { GN_ASSERT(mPtr); return *mPtr; }

        ///
        /// arrow operator
        ///
        XPTR operator->() const throw()  { GN_ASSERT(mPtr); return  mPtr; }

        ///
        /// get internal pointer
        ///
        XPTR get() const throw() { return mPtr; }

        ///
        /// get address of internal pointer.
        ///
        /// ���������Ҫ���ڽ�ָ��AutoRef��ָ����ָ��XPTR��ָ�루�������ڴ��е�ӳ����ʵ��һ���ģ���
        /// �������ǿ������ת����ͬ����Ŀ�ģ��������������������һЩ��
        ///
        XPTR const * addr() const throw() { return &mPtr; }

        ///
        /// return true if no pointer is currently being hold
        ///
        bool empty() const throw()
        {
            return 0 == mPtr;
        }

        ///
        /// Clear to empty. Same as set(NULL).
        ///
        void clear()
        {
            MUTEX m;
            m.lock();

            if( mPtr ) mPtr->decref(); mPtr = 0;

            m.unlock();
        }

        ///
        /// set new pointer data
        ///
        /// this function will release old pointer, if not NULL; then increase
        /// the reference counter of new pointer, if not NULL.
        ///
        void set( XPTR p )
        {
            MUTEX m;
            m.lock();

            if( p ) p->incref();
            if( mPtr ) mPtr->decref();
            mPtr = p;

            m.unlock();
        }

        ///
        /// acquire ownership of a XPTR
        ///
        /// this function will not modify pointer's refcount
        ///
        void attach( XPTR ptr )
        {
            if( ptr == mPtr ) return;

            MUTEX m;
            m.lock();

            if( mPtr ) mPtr->decref();
            mPtr = ptr;

            m.unlock();
        }

        ///
        /// release ownership of internal XPTR
        ///
        /// this function will not modify pointer's refcount
        ///
        XPTR detach() throw()
        {
            MUTEX m;
            m.lock();

            XPTR tmp = mPtr;
            mPtr = 0;

            m.unlock();

            return tmp;
        }
    };

    template<typename X,typename M> AutoRef<X,M> AutoRef<X,M>::EMPTYPTR;

    namespace detail
    {
        ///
        /// base class of weak reference
        ///
        class WeakRefBase
        {
        protected:

            const RefCounter         * mPtr;
            std::list<void*>::iterator mIter;

        public:

            /// constructor
            WeakRefBase( const RefCounter * ptr = NULL ) : mPtr(ptr)
            {
                if( mPtr )
                {
                    mIter = mPtr->_addWeakRef( this );
                }
            }

            /// copy constructor
            WeakRefBase( const WeakRefBase & ref ) : mPtr(ref.mPtr)
            {
                if( mPtr )
                {
                    mIter = mPtr->_addWeakRef( this );
                }
            }

            /// destructor
            virtual ~WeakRefBase()
            {
                clear();
            }

            /// is empty reference or not.
            bool empty() const { return NULL == mPtr; }

            /// clear the reference
            void clear()
            {
                if( mPtr )
                {
                    mPtr->_removeWeakRef( mIter );
                    mPtr = NULL;
                }
            }

            /// set the pointer
            void set( const RefCounter * ptr )
            {
                if( mPtr == ptr ) return;

                clear();

                if( ptr )
                {
                    mPtr = ptr;
                    mIter = mPtr->_addWeakRef( this );
                }
            }
        };
    }

    ///
    /// Weak reference class
    ///
    template<class X>
    class WeakRef : public detail::WeakRefBase
    {
        typedef X * XPTR;
        typedef X & XREF;

    public:

        /// constructor
        WeakRef( const RefCounter * ptr = NULL ) : detail::WeakRefBase(ptr)
        {
        }

        /// copy constructor
        WeakRef( const WeakRef & ref ) : detail::WeakRefBase(ref)
        {
        }

        // get the pointer
        XPTR get() const { return (X*)mPtr; }

        ///
        /// Convert to XPTR
        ///
        operator XPTR () const { return mPtr; }

        ///
        /// �Ƚϲ���
        ///
        bool operator == ( const WeakRef & rhs ) const throw()
        {
            return mPtr == rhs.mPtr;
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator != ( const WeakRef & rhs ) const throw()
        {
            return mPtr != rhs.mPtr;
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator < ( const WeakRef & rhs ) const throw()
        {
            return mPtr < rhs.mPtr;
        }

        ///
        /// NOT operator
        ///
        bool operator !() const throw() { return !mPtr; }

        ///
        /// dereference operator.
        ///
        /// TODO: is this thread safe?
        ///
        XREF operator *() const throw()  { GN_ASSERT(mPtr); return *mPtr; }

        ///
        /// arrow operator
        ///
        XPTR operator->() const throw()  { GN_ASSERT(mPtr); return  mPtr; }
    };

    ///
    ///
    // -------------------------------------------------------------------------
    inline void RefCounter::NullifyWeakRef( void * ptr )
    {
        detail::WeakRefBase * ref = (detail::WeakRefBase *)ptr;
        ref->clear();
    }
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_REFCOUNTER_H__
