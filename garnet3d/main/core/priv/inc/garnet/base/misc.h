#ifndef __GN_BASE_MISC_H__
#define __GN_BASE_MISC_H__
// *****************************************************************************
//! \file    misc.h
//! \brief   misc functions and types
//! \author  chenlee (2005.5.4)
// *****************************************************************************

//!
//! Execute only limited times during the entir life of the
//! application, no matter how many time it is called.
//!
#define GN_DO_LIMITED_TIMES(n,X)      \
    if (true)                         \
    {                                 \
        static size_t s_counter = n;  \
        if( s_counter > 0 )           \
        {                             \
            --s_counter;              \
            X;                        \
        }                             \
    } else void(0)

//!
//! Do something only once. ͨ���������ڲ�ѭ�������һЩ���Ժʹ�����Ϣ��
//!
#define GN_DO_ONCE(X) GN_DO_LIMITED_TIMES(1,X)

//!
//! "Use" unused parameter (to avoid compiler warnings)
//!
#define GN_UNUSED_PARAM(X) ((void)(X))

// *************************************************************************
//! \name StdCalss
// *************************************************************************

//@{

namespace GN
{
    //!
    //! ����stdclass����
    //!
    //! \param self    �������������
    //! \param parent  ��������ĸ��������
    //! \note �����л���stdclass���͵�������ʹ�������
    //!
    //! ���������������������MySelf��MyParent���ͱ�׼˽�к����� selfOk()
    //!
    #define GN_DECLARE_STDCLASS(self, parent)                           \
                private : typedef self MySelf; typedef parent MyParent; \
                private : bool selfOk() const { return MySelf::ok(); }


    //!
    //! stdclass���͵ı�׼��ʼ�����̡�
    //!
    //! \param class_name ��ǰ��������������
    //! \param param_list ���磺(param1, param2, param3,...)�ģ�
    //!                   �ҷ��ϸ���init()Ҫ��Ĳ����б����Ų���ʡ�ԣ�
    //! \note �����л���stdclass���͵�init������ʹ�������
    //!
    //! ���磺GN_STDCLASS_INIT( myobject_c, (param1, param2) );
    //! ������Ϣ�μ����������
    #define GN_STDCLASS_INIT( class_name, param_list )     \
        /* check for twice init */                         \
        if( selfOk() )                                     \
        {                                                  \
            GN_ERROR( "u call init() twice!" );            \
            quit(); return selfOk();                       \
        }                                                  \
        /* call parent's init() */                         \
        if( !MyParent::init param_list )  { quit(); return selfOk(); }

    //!
    //! stdclass���͵ı�׼�˳�����
    //!
    //! \note �����л���stdclass���͵�quit()������ʹ�������
    //!
    //! ʹ�÷����μ����������
    #define GN_STDCLASS_QUIT()  { clear(); MyParent::quit(); }


    //!
    //! standard class
    //!
    //! \par
    //! ���� StdClass ��ʹ��˵���Լ�һЩע���������������е�ע�͡�
    //! \par
    //! �κ���Ҫ��Ҫ��ʼ��/�˳��������඼Ӧ���� StdClass �̳С�
    //! \par
    //! StdClassʵ���˱�׼��init/quit�ӿڣ�����ӿڰ����߸���׼������\n
    //! - StdClass()  : ���캯����
    //!   - ͨ��clear()��˽�б�������
    //!   - һ������£����� clear() ���⣬���캯�����ٲ�Ӧ�����������룬
    //!   - �κ��п���ʧ�ܵĳ�ʼ�����붼Ӧ�ŵ� init() �С�
    //! \n
    //! - ~StdClass() : ������������
    //!   - ���� quit() �ͷ���Դ��
    //! \n
    //! - init()      : ʵ��ʼ��������
    //!   - �ɹ��򷵻�1�����򷵻�0��
    //! \n
    //! - quit()      : �麯����
    //!   - �ͷ����е���Դ��ͬʱ��˽�б������㣨ͨ������ clear() ����
    //!   - �����ʵ����deleteʱ�������������Զ����� quit() ��
    //!   - quit() ����Ӧ�����Ա���ȫ�ġ���εĵ��ã�Ҳ����˵��
    //!   - ���ͷ���Դʱ�������ȼ����Դ����Ч�ԡ�
    //! \n
    //! - ok()     : �����麯����
    //!   - ��������Ƿ��Ѿ���ʼ�������ɹ����� init() �󷵻�true��
    //!     ���� quit() �󷵻�false��
    //! \n
    //! - selfOk() : ˽�к�����
    //!   - selfOk() �� ok() �ġ����顱�汾��ǿ�Ƶ��ñ���� ok() ������
    //!   - ���� selfOk() ����ϸʹ�ù���������ע�⡣
    //! \n
    //! - clear()     : ˽�к�����
    //!   - ���ڽ�˽�г�Ա�������㣬�����캯���� quit() ����
    //!
    //! \note  selfOk() ���麯�� ok() �ġ����顱�汾���书����ǿ�Ƶ���
    //!        �����͵� ok() ����������ͨ�����麯����������ʵ��������
    //!        ��������Ӧ�� ok() �� selfOk() ��˽�к�����ֻ���ڱ���ĳ�
    //!        Ա������ʹ�á� �ٸ����ӣ�
    //! \code
    //!    class A : public StdClass
    //!    {
    //!        GN_DECLARE_STDCLASS(A, StdClass);
    //!    public:
    //!        virtual bool ok() const { return 1; }
    //!        bool test()             { return ok(); }
    //!        bool test2()            { return selfOk(); }
    //!    }
    //!    class B : public A
    //!    {
    //!        GN_DECLARE_STDCLASS(B, A);
    //!    public:
    //!        virtual bool ok() const { return 0; }
    //!    }
    //!    A * a;
    //!    B b;
    //!    a = \&b;
    //!    a->test();  // ����aָ�����B���͵ı��������԰���
    //!                // �麯���ĵ��ù��򣬽������B::ok()�����Է���0
    //!    a->test2(); // ʹ��selfOk()���ǿ�Ƶ���A::ok()������1
    //! \endcode
    //!
    class StdClass
    {
    public:
        //!
        //! ctor
        //!
        StdClass() { clear(); }
        //!
        //! dtor
        //!
        virtual ~StdClass() { quit(); }

    public :

        //!
        //! ��ʼ������
        //!
        bool init()
        {
            if( selfOk() )
            {
                GN_ERROR( "u call init() twice!" );
                quit(); return selfOk();
            }

            m_ok = 1;
            return selfOk();
        }

        //!
        //! �˳�����
        //!
        virtual void quit() { clear(); }

        //!
        //! �Ƿ��ʼ������
        //!
        virtual bool ok() const { return m_ok; }

    private :

        //!
        //! initialize data members
        //!
        void clear() { m_ok = 0; }

        //!
        //! private initialization check routine
        //!
        bool selfOk() const { return StdClass::ok(); }

        //!
        //! initialization flag
        //!
        bool m_ok;
    };

    //!
    //! Disable copy semantic of all descendants.
    //!
    class NoCopy
    {
    protected:
        //!
        //! Default constructor
        //!
        NoCopy() {}
        //!
        //! Destructor
        //!
        virtual ~NoCopy() {}
    private:  // emphasize the following members are private
        NoCopy( const NoCopy & );
        const NoCopy & operator = ( const NoCopy& );
    };

    //!
    //! ���ü�����
    //!
    //! �ṩ���������ü������ܣ����ں� AutoRef һ��ʵ���Զ�ָ����
    //!
    //! \note
    //! ��ʹ�� RefCounter ʱ��Ϊ��֤ϵͳ�������������м�������������أ�
    //!     - �����ܵ�ʹ�� AutoRef ������ RefCounter ���ʵ��
    //!     - ������໥���õ�������ڣ�������ֱ�ӵĻ��Ǽ�ӵģ������ͷŶ���ǰ
    //!       �������ȴ����û�����������ڴ�й©�����еı�������Զ�޷��ͷ�
    //!     - ����Щ�ɣ��������Ժ��뵽��˵....
    //!
    class RefCounter : public NoCopy
    {
        // ********************************
        //       reference management
        // ********************************
    public :

        //!
        //! increase reference counter
        //!
        void incref() const  throw() { ++m_ref; }

        //!
        //! decrease reference counter,
        //!
        //! After decreation, if reference counter is "0", call "delete this"
        //!
        void decref() const
        {
            GN_ASSERT( m_ref>0 );
            if( 0 == --m_ref ) delete this;
        }

        //!
        //! get current reference counter value
        //!
        int  getref() const throw() { return m_ref; }

        // ********************************
        //! \name protective ctor/dtor
        // ********************************
    protected:

        //@{

        //!
        //! Constructor
        //!
        RefCounter() : m_ref(0) {}

        //!
        //! Destructor
        //!
        virtual ~RefCounter()
        {
            if( m_ref > 0 )
            {
                GN_ERROR(
                    "destructing a refcounter with its reference counter "
                    "greater then zero!" );
            }
        }

        //@}

        // ********************************
        // private data members
        // ********************************
    private:

        // Make this class NoCopy.
        RefCounter( const RefCounter& );
        const RefCounter& operator=( const RefCounter& );

        //!
        //! reference counter
        //!
        mutable int m_ref;
    };

    //!
    //! �� RefCounter ���ʹ�õ��Զ�ָ����
    //!
    template <class X> class AutoRef
    {
        //!
        //! pointer to class X
        //!
        typedef  X * XPTR;

        //!
        //! reference to class X
        //!
        typedef  X & XREF;

    private:

        XPTR  mPtr; //!< internal pointer

    public :

        //!
        //! Instance of empty reference pointer
        //!
        static AutoRef<X> EMPTYPTR;

        //!
        //! default constructor
        //!
        AutoRef() throw() : mPtr(0) {}

        //!
        //! construct from a normal pointer
        //!
        //! Y should be a class that can be <b>implicitly</b> convert to
        //! X, such as a sub-class of X.
        //!
        //! if p is not NULL, increase its reference counter
        //!
        template <class Y>
        AutoRef<X>( Y * p ) throw()
            : mPtr(p)
        { if(mPtr) mPtr->incref(); }

        //!
        //! copy constructor
        //!
        //! Y should be a class that can be <b>implicitly</b> convert to
        //! X, such as a sub-class of X.
        //!
        template <class Y>
        AutoRef<X>( const AutoRef<Y> & p ) throw()
            : mPtr( p.get() )
        { if(mPtr) mPtr->incref(); }

        //!
        //! destructor
        //!
        ~AutoRef()
        {
            if(mPtr) mPtr->decref();
        }

        //!
        //! ��ֵ���
        //!
        //! Y should be a class that can be <b>implicitly</b> convert to
        //! X, such as a sub-class of X.
        //!
        template <class Y> AutoRef<X> & operator = ( const AutoRef<Y> & rhs )
        {
            reset( rhs.get() );
            return *this;
        }

        //!
        //! �Ƚϲ���
        //!
        bool operator == ( const AutoRef & rhs ) const throw()
        {
            return mPtr == rhs.mPtr;
        }

        //!
        //! �Ƚϲ���
        //!
        bool operator != ( const AutoRef & rhs ) const throw()
        {
            return mPtr != rhs.mPtr;
        }

        //!
        //! �Ƚϲ���
        //!
        bool operator < ( const AutoRef & rhs ) const throw()
        {
            return mPtr < rhs.mPtr;
        }

        //!
        //! dereference operator
        //!
        XREF operator *() const throw()  { GN_ASSERT(mPtr); return *mPtr; }

        //!
        //! arrow operator
        //!
        XPTR operator->() const throw()  { GN_ASSERT(mPtr); return  mPtr; }

        //!
        //! get internal pointer
        //!
        XPTR get() const throw() { return mPtr; }

        //!
        //! reset with new pointer data
        //!
        //! this function will release old pointer, if not NULL; then increase
        //! the reference counter of new pointer, if not NULL.
        //!
        void reset( XPTR p = 0 )
        {
            if( p ) p->incref();
            if( mPtr ) mPtr->decref();
            mPtr = p;
        }

        //!
        //! return true if no pointer is currently being hold
        //!
        bool empty() const throw()
        {
            return 0 == mPtr;
        }


        //!
        //! acquire ownership of a XPTR
        //!
        //! this function will not modify pointer's refcount
        //!
        void acquire( XPTR ptr )
        {
            reset();
            mPtr = ptr;
        }

        //!
        //! release ownership of internal XPTR
        //!
        //! this function will not modify pointer's refcount
        //!
        XPTR dismiss() throw()
        {
            XPTR tmp = mPtr;
            mPtr = 0;
            return tmp;
        }
    };

    template<typename X> AutoRef<X> AutoRef<X>::EMPTYPTR;

    //!
    //! ������
    //!
    template<typename T>
    class Singleton
    {
        GN_PUBLIC static T * msInstance; //!< ָ��singleton��ʵ��

    public:

        //!
        //! Constructor
        //!
        Singleton()
        {
            GN_ASSERT( 0 == msInstance );
            // This is code 64-bit compatible?
            int offset = (int)(T*)1 - (int)(Singleton<T>*)(T*)1;
            msInstance = (T*)((int)this+offset);
        }

        //!
        //! Destructor
        //!
        virtual ~Singleton() { GN_ASSERT(msInstance); msInstance = 0; }

        //!
        //! Get the instance
        //!
        static T & getInstance() { GN_ASSERT(msInstance); return *msInstance; }

        //!
        //! Get the instance pointer (might be NULL)
        //!
        static T * getInstancePtr() { return msInstance; }
    };

    // ʵ�ֵ������еľ�̬����
    template<typename T> T * ::GN::Singleton<T>::msInstance = 0;

    //!
    //! type cast function
    //!
    //! perform dynamic cast in debug build, and static cast in release build.
    // ------------------------------------------------------------------------
    template < class TO, class FROM >
    GN_FORCE_INLINE TO safeCast( FROM from )
    {
    #if GN_DEBUG && ( !GN_MSVC || defined(_CPPRTTI) )
        GN_ASSERT( 0 == from || dynamic_cast<TO>(from) );
        return dynamic_cast<TO>(from);
    #else
        return static_cast<TO>(from);
    #endif
    }

    //!
    //! vector to pointer
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE T * vec2ptr( std::vector<T> & vec )
    {
        return vec.empty() ? 0 : &vec[0];
    }

    //!
    //! vector to pointer
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE const T * vec2ptr( const std::vector<T> & vec )
    {
        return vec.empty() ? 0 : &vec[0];
    }

    //!
    //! ��value�޶���[vmin, vmax]������
    // ------------------------------------------------------------------------
    template < typename T >
    inline void clamp( T & value, const T & vmin, const T & vmax )
    {
        value = vmin > value ? vmin : vmax < value ? vmax : value;
    }

    //!
    //! general safe delLocation routine
    // ------------------------------------------------------------------------
    template < typename T, typename DEALLOC_FUNC >
    GN_FORCE_INLINE void safeDealloc( T * & ptr )
    {
        if( ptr )
        {
            DEALLOC_FUNC( ptr );
            ptr = 0;
        }
    }

    //!
    //! free one object
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE void safeFree( T * & ptr )
    {
        if( ptr )
        {
            memFree( ptr );
            ptr = 0;
        }
    }

    //!
    //! delete one object
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE void safeDelete( T * & ptr )
    {
        if( ptr )
        {
            delete ptr;
            ptr = 0;
        }
    }

    //!
    //! delete object array
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE void safeDeleteArray( T * & ptr )
    {
        if( ptr )
        {
            delete [] ptr;
            ptr = 0;
        }
    }
}

// *****************************************************************************
//                           End of misc.h
// *****************************************************************************
#endif // __GN_BASE_MISC_H__
