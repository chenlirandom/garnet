#ifndef __GN_BASE_SMARTPTR_H__
#define __GN_BASE_SMARTPTR_H__
// *****************************************************************************
/// \file
/// \brief   Smart pointer classes and utilities
/// \author  chen@@chenli-homepc (2011.4.9)
// *****************************************************************************

namespace GN
{
    // -------------------------------------------------------------------------
    /// Reference counted smart pointer. Support both strong and weak reference.
    class RefCounter : public GN::NoCopy
    {
    public:

        /// Utility class to support weak ref
        struct WeakObject
        {
            void      *    ptr; // pointer to RefCounter object
            GN::Mutex      lock;
            GN::DoubleLink references;

            // return true, only when reference list is empty
            bool deref(GN::DoubleLink & l)
            {
                lock.enter();
                l.detach();
                bool timeToDelete = !references.prev && !references.next;
                lock.leave();
                return timeToDelete;
            }
        };

        // ********************************
        //       reference management
        // ********************************
    public :

        ///
        /// increase reference counter
        ///
        sint32 incref() const  throw() { return GN::atomInc32(&mRef); }

        ///
        /// decrease reference counter, delete the object, if reference count reaches zero.
        ///
        sint32 decref() const
        {
            GN_ASSERT( mRef > 0 );

            mWeakLock.enter();
            sint32 ref = GN::atomDec32( &mRef ) ;
            if (0 == ref && mWeakObj)
            {
                mWeakObj->lock.enter();
                mWeakLink.detach();
                mWeakObj->ptr = NULL;
                bool timeToDelete = !mWeakObj->references.prev && !mWeakObj->references.next;
                mWeakObj->lock.leave();
                if (timeToDelete)
                {
                    delete mWeakObj;
                }
                mWeakObj = NULL;
            }
            mWeakLock.leave();

            if( 0 == ref )
            {
                delete this;
            }

            return ref;
        }

        ///
        /// get current reference counter value
        ///
        sint32 getref() const throw() { return GN::atomGet32(&mRef); }

        ///
        /// Return the weak object associated with this reference counted object.
        ///
        WeakObject * getWeakObj() const
        {
            mWeakLock.enter();
            if (!mWeakObj)
            {
                mWeakObj = new WeakObject();
                mWeakObj->ptr = (void*)this;
                mWeakLink.linkAfter(&mWeakObj->references);
            }
            mWeakLock.leave();
            return mWeakObj;
        }

        ///
        /// For internal use. Do _NOT_ call.
        ///
        WeakObject * __getWeakObjNoLock() const
        {
            return mWeakObj;
        }

        // ********************************
        /// \name protective ctor/dtor
        // ********************************
    protected:

        //@{

        ///
        /// Constructor
        ///
        RefCounter() : mRef(0), mWeakObj(NULL) {}

        ///
        /// Destructor
        ///
        virtual ~RefCounter()
        {
            if( 0 != mRef )
            {
                GN_UNEXPECTED_EX( "Destructing reference counted object with non-zero reference counter usually means memory corruption, thus is not allowed!" );
            }
        }

        //@}

        // ********************************
        // private data members
        // ********************************
    private:

        ///
        /// reference counter
        ///
        mutable volatile sint32 mRef;
        mutable GN::Mutex       mWeakLock;
        mutable WeakObject *    mWeakObj;
        mutable GN::DoubleLink  mWeakLink;
    };

    ///
    /// ��� RefCounter ʹ�õ��Զ�ָ����
    // -------------------------------------------------------------------------
    template <class X> class AutoRef
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
        /// Instance of empty/null reference pointer
        ///
        static AutoRef<X> NULLREF;

        #if 1
        /// construct from a normal pointer
        ///
        explicit AutoRef( XPTR p = 0 ) throw() : mPtr(p)
        {
            // make sure sizeof(AutoRef) == sizeof(XPTR), which ensures that an array of autoref
            // can always be used as array of native pointer.
            GN_CASSERT( sizeof(AutoRef) == sizeof(XPTR) );

            if( p ) p->incref();
        }
        #else
        // default ctor
        AutoRef() throw() : mPtr(NULL)
        {
            // make sure sizeof(AutoRef) == sizeof(XPTR), which ensures that an array of autoref
            // can always be used as array of native pointer.
            GN_CASSERT( sizeof(AutoRef) == sizeof(XPTR) );
        }
        #endif

        ///
        /// copy constructor
        ///
        AutoRef( const AutoRef & p ) throw() : mPtr( p )
        {
            if( mPtr ) mPtr->incref();
        }

        ///
        /// copy constructor
        ///
        template <class Y>
        AutoRef( const AutoRef<Y> & p ) throw() : mPtr( p )
        {
            if( mPtr ) mPtr->incref();
        }

        ///
        /// destructor
        ///
        ~AutoRef()
        {
            if( mPtr ) mPtr->decref();
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
        AutoRef & operator = ( XPTR ptr )
        {
            set( ptr );
            return *this;
        }

        ///
        /// ��ֵ���
        ///
        template <class Y>
        AutoRef & operator = ( const AutoRef<Y> & rhs )
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
        /// get internal raw pointer
        ///
        XPTR rawptr() const throw() { return mPtr; }

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
            if( mPtr ) mPtr->decref();
            mPtr = 0;
        }

        ///
        /// set new pointer data
        ///
        /// this function will release old pointer, if not NULL; then increase
        /// the reference counter of new pointer, if not NULL.
        ///
        void set( XPTR p )
        {
            if( p ) p->incref();
            if( mPtr ) mPtr->decref();
            mPtr = p;
        }

        ///
        /// acquire ownership of a XPTR
        ///
        /// this function will not modify pointer's refcount
        ///
        void attach( XPTR ptr )
        {
            if( ptr == mPtr ) return;
            if( mPtr ) mPtr->decref();
            mPtr = ptr;
        }

        ///
        /// release ownership of internal XPTR
        ///
        /// this function will not modify pointer's refcount
        ///
        XPTR detach() throw()
        {
            XPTR tmp = mPtr;
            mPtr = 0;
            return tmp;
        }
    };

    template<typename X> AutoRef<X> AutoRef<X>::NULLREF;

    ///
    /// Attach to a C pointer without increasing its reference count.
    ///
    template<typename T>
    inline AutoRef<T> attachTo(T * ptr)
    {
        AutoRef<T> result;
        result.attach(ptr);
        return result;
    }

    ///
    /// Reference a C refcount pointer. Increase its ref count by one.
    ///
    template<typename T>
    inline AutoRef<T> referenceTo(T * ptr)
    {
        AutoRef<T> result;
        result.attach(ptr);
        result->incref();
        return result;
    }

    ///
    /// Weak refernce to smart pointer object
    // -------------------------------------------------------------------------
    template<typename X>
    class WeakRef
    {
        typedef X * XPTR;
        typedef X & XREF;

        RefCounter::WeakObject * mObj;
        GN::DoubleLink           mLink;

        typedef AutoRef<X> StrongRef;

    public:

        ///
        /// constructor
        ///
        WeakRef( XPTR ptr = NULL ) : mObj(NULL)
        {
            set( ptr );
        }

        ///
        /// copy constructor
        ///
        WeakRef( const WeakRef & ref ) : mObj(NULL)
        {
            set( ref.promote() );
        }

        ///
        /// Destructor
        ///
        ~WeakRef()
        {
            clear();
        }

        ///
        /// clear the reference
        ///
        void clear()
        {
            if (mObj && mObj->deref(mLink))
            {
                delete mObj;
            }
            mObj = NULL;
        }

        bool empty() const
        {
            bool result = true;
            if (mObj)
            {
                mObj->lock.enter();
                result = NULL == mObj->ptr;
                mObj->lock.leave();
            }
            return result;
        }

        ///
        /// set/reset the pointer. Null pointer is allowed.
        ///
        void set( XPTR ptr )
        {
            if (!ptr)
            {
                clear();
            }
            else
            {
                RefCounter::WeakObject * obj = ptr->getWeakObj();
                if (obj != mObj)
                {
                    obj->lock.enter();
                    mLink.linkAfter( &obj->references );
                    mLink.context = this;
                    obj->lock.leave();
                    mObj = obj;
                }
            }
        }

        ///
        /// promote to strong reference
        ///
        AutoRef<X> promote() const
        {
            AutoRef<X> result;
            if (mObj)
            {
                mObj->lock.enter();
                result.set((XPTR)mObj->ptr);
                mObj->lock.leave();
            }
            return result;
        }

        ///
        /// copy operator
        ///
        WeakRef & operator = ( const WeakRef & rhs )
        {
            set( rhs.promote() ); // TODO: we might be able to do it slightly faster.
            return *this;
        }

        bool operator !() const { return empty(); }

        ///
        /// �Ƚϲ���
        ///
        bool operator == ( const WeakRef & rhs ) const throw()
        {
            return mObj == rhs.mObj;
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator == ( XPTR ptr ) const throw()
        {
            return mObj == (ptr ? ptr->__getWeakObjNoLock() : NULL);
        }

        ///
        /// �Ƚϲ���
        ///
        friend inline bool operator == ( XPTR ptr, const WeakRef & rhs ) throw()
        {
            return rhs.mObj == (ptr ? ptr->__getWeakObjNoLock() : NULL);
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator != ( const WeakRef & rhs ) const throw()
        {
            return mObj != rhs.mObj;
        }

        ///
        /// �Ƚϲ���
        ///
        bool operator < ( const WeakRef & rhs ) const throw()
        {
            return mObj < rhs.mObj;
        }
    };
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_SMARTPTR_H__
