#include "../testCommon.h"

namespace GN
{
    //!
    //! object pool
    //!
    template< class T, size_t N=256, size_t ALIGNMENT=4, class A=StandardAllocator<T> >
    class ObjectPool
    {
        GN_CASSERT( N > 0 );
        GN_CASSERT_EX( ALIGNMENT > 0 && 0==(ALIGNMENT&(ALIGNMENT-1)), alignment_must_be_power_of_two );

        typedef ObjectPool<T,N,ALIGNMENT,A> MyType;        
        typedef T ItemType;
        typedef typename A::Rebind<uint8_t>::Other AllocatorType;

        struct Pool;
        struct Item
        {
            union
            {
                Item * nextFree;
                uint8_t _reserved[sizeof(T)]; // make sure Item is large enough to hold one object.
            };
            Pool * pool;
            MyType * owner;

            T * tptr() { return (T*)this; }
        };

        static const size_t ALIGNED_BYTES = 1 << ALIGNMENT;
        static const size_t ITEM_SIZE = ( sizeof(Item) + (ALIGNED_BYTES-1) ) & ~(ALIGNED_BYTES-1);

        struct Pool
        {
            size_t count; // used items in this pool
            uint8_t data[ITEM_SIZE*N];

            Item & getItem( size_t i )
            {
                GN_ASSERT( i < N );
                return (Item*)(data + ITEM_SIZE * i );
            }
        };

        Pool * mAllPools;
        Item * mFreeItems;

    private:

        T * doAlloc()
        {
            if( !mFreeItems )
            {
                // create new pool
                Pool * p = AllocatorType::sAlloc( 1 );
                if( 0 == p ) return 0;

                // format the pool
                p->count = 0;
                for( size_t i = 0; i < N-1; ++i )
                {
                    Item & item = p->getItem( i );
                    item.nextFree = p->getItem( i+1 );
                    item.pool = p;
                    item.owner = this;
                }
                Item & last = p->getItem( N-1 );
                last.nextFree = NULL;
                last.pool = p;
                last.owner = this;
                mFreeItems = &p->getItem( 0 );
            }
            
            // get the first free item
            Item * p = mFreeItems;
            mFreeItems = mFreeItems->nextFree;
            GN_ASSERT( p->pool && p->pool.count < N );
            ++p->pool.count;
            return p->tptr();
        }

        void doDealloc( T * p )
        {
            if( !p ) return;

            Item * item = (Item*)p;

            GN_ASSERT( this == item->owner );

            // call destructor
            p->T::~T();

            // insert to free list
            item->nextFree = mFreeItems;
            mFreeItems = item;

            // adjust pool's item count as well.
            GN_ASSERT( item->pool && item->pool->count > 0 );
            --item->pool->count;
        }

    public:

        //@{
        ObjectPool() : mAllPools(0), mUsedItems(0), mFreeItems(0) {}
        ~ObjectPool() { freeAll(); }
        //@}

        //@{
        T * alloc() { return doAlloc(); }
        void dealloc( T * p ) { doDealloc( p ); }
        void freeAll();
        void recycle();// { doRecycle(); } //!< free unused memory as much as possible.
        size_t getItemCount() const;
        size_t getMemoryFootprint() const;
        //@}
    };
}

class MemPoolTest : public CxxTest::TestSuite
{
    struct Test
    {
        static GN::FixedSizedObjectAllocator<Test> sAllocator;
        inline void * operator new( size_t ) { return sAllocator.alloc(); }
        inline void operator delete( void* p ) { sAllocator.dealloc(p); }
        inline void * operator new( size_t, void * p ) { GN_INFO("placement new"); return p; }
        inline void operator delete( void*, void * ) { GN_INFO("placement delete"); }
    };

    typedef std::vector<char,GN::StlAllocator<char> > CharArray;
    typedef std::vector<Test,GN::StlAllocator<GN::StrA> > TestArray;

public:

    void testCustomAllocator()
	{
        CharArray a1(10);
        TestArray a2(10);
        TS_ASSERT_EQUALS( a1.size(), a2.size() );
    }

    void testPlacementNew()
    {
        uint8_t buf[sizeof(Test)*10];
        Test * a = new(buf) Test;
        TS_ASSERT_EQUALS( a, (Test*)buf );
        a->~Test();
    }

    void testObjectPoolWithPOD()
    {
        GN::FixedSizedObjectAllocator<char> a;
        a.alloc();
    }

    void testClassAllocator()
    {
        using namespace GN;

        for( int i = 0; i < 10000; ++i )
        {
            new Test;
        }
    }

    void testOneByteAllocator()
    {
        using namespace GN;
        FixedSizedMemoryAllocator a(1);

        for( int i = 0; i < 10000; ++i )
        {
            a.alloc();
        }
    }

    void testManyBytesAllocator()
    {
        using namespace GN;
        FixedSizedMemoryAllocator a(128);

        for( int i = 0; i < 10000; ++i )
        {
            a.alloc();
        }
    }
};
GN::FixedSizedObjectAllocator<MemPoolTest::Test> MemPoolTest::Test::sAllocator;
