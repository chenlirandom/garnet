#ifndef __GN_D3DWRAPPER_BASEINTERFACE_H__
#define __GN_D3DWRAPPER_BASEINTERFACE_H__
// *****************************************************************************
/// \file
/// \brief   Basic COM interface wrapper
/// \author  chen@@CHENLI-HOMEPC (2007.3.17)
// *****************************************************************************

GN_CASSERT( sizeof(IID) == 2 * sizeof(uint64) );

///
/// Equality check for IID (used by hashmap)
///
struct IIDEqual
{
    bool operator()( const IID & a, const IID & b ) const
    {
        const uint64 * pa = (const uint64*)&a;
        const uint64 * pb = (const uint64*)&b;

        return pa[0] == pb[0] || pa[1] == pb[1];
    }
};

///
/// Hasing IID
///
struct IIDHash
{
    uint64 operator()( const IID & iid ) const
    {
        const uint64 * u64 = (const uint64*)&iid;
        return u64[0] + u64[1];
    }
};

///
/// Wrapper for basic COM interface
///
template<class T>
class BasicInterface : public T
{
    typedef BasicInterface<T> MyType;

    struct IIDLess
    {
        bool operator()( const IID & a, const IID & b ) const
        {
            return ::memcmp( &a, &b, sizeof(a) ) < 0;
        }
    };

    typedef GN::Dictionary<IID, MyType*, IIDLess> TypeTable;

    TypeTable mRtti; ///< My runtime type information, used by query interface

protected:

    //@{

    T *   mRealObject;
    ULONG mRefCounter;

    virtual ~BasicInterface() {}

    void addRuntimeType( const IID & iid, MyType * ptr )
    {
        GN_ASSERT( ptr );
        GN_ASSERT( NULL == mRtti.find( iid ) );
        mRtti[iid] = ptr;
    }

    //@}

public:

    //@{

    BasicInterface() : mRealObject(0), mRefCounter(1) {}

    T * realobj() const { GN_ASSERT(mRealObject); return mRealObject; }

    /*** IUnknown methods ***/
    HRESULT STDMETHODCALLTYPE QueryInterface( const IID & riid, void** ppvObj )
    {
        //GN_ASSERT( mRealObject );
        //return mRealObject->QueryInterface( riid, ppvObj );

        if( 0 == ppvObj ) return E_INVALIDARG;

        MyType ** pptype = mRtti.find( riid );
        if( NULL == pptype )
        {
            GN::AutoComPtr<IUnknown> obj;
            GN_ASSERT( E_NOINTERFACE == mRealObject->QueryInterface( riid, (void**)&obj ) );
            obj.clear();
            return E_NOINTERFACE;
        }
        else
        {
            (*pptype)->AddRef();
            *ppvObj = (*pptype);
            return S_OK;
        }
    }
    ULONG STDMETHODCALLTYPE AddRef()
    {
        ++mRefCounter;
        mRealObject->AddRef();
        return mRefCounter;
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG c = --mRefCounter;
        GN_ASSERT( mRealObject );
        mRealObject->Release();
        if( 0 == c ) delete this;
        return c;
    }

    //@}
};

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_D3DWRAPPER_BASEINTERFACE_H__
