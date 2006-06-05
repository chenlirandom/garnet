#include "pch.h"
#include "garnet/GNcore.h"
#include "garnet/GNgfx.h"
#include "garnet/GNinput.h"

// implement global singletons
GN_IMPLEMENT_SINGLETON( GN::PluginManager )
GN_IMPLEMENT_SINGLETON( GN::ProfilerManager )
GN_IMPLEMENT_SINGLETON( GN::input::Input )
GN_IMPLEMENT_SINGLETON( GN::gfx::Renderer )

// implement static renderer data members
GN_IMPLEMENT_RENDERER_STATIC_MEMBERS()

namespace GN
{
    // Global runtime assert behavior flag.
    GN_PUBLIC RuntimeAssertBehavior gRuntimeAssertBehavior = RAB_ASK_USER;

    //
    //
    // -----------------------------------------------------------------------------
    GN_PUBLIC void * heapAlloc( size_t sz )
    {
        //#if GN_MSWIN
        //void * ptr = ::HeapAlloc( ::GetProcessHeap(), 0, sz );
        //#else
        void * ptr = ::malloc( sz );
        //#endif
        if ( 0 == ptr ) GN_ERROR( "out of memory!" );
        return ptr;
    }

    //
    //
    // -----------------------------------------------------------------------------
    GN_PUBLIC void * heapRealloc( void * ptr, size_t sz )
    {
        //#if GN_MSWIN
        //ptr = ::HeapReAlloc( ::GetProcessHeap(), 0, ptr, sz );
        //#else
        ptr = ::realloc( ptr, sz );
        //#endif
        if ( 0 == ptr ) GN_ERROR( "out of memory!" );
        return ptr;
    }

    //
    //
    // -----------------------------------------------------------------------------
    GN_PUBLIC void heapFree( void * ptr )
    {
        //#if GN_MSWIN
        //::HeapFree( ::GetProcessHeap(), 0, ptr );
        //#else
        ::free( ptr );
        //#endif
    }
}
