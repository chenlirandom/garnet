#include "pch.h"

//
//
// -----------------------------------------------------------------------------
bool GN::getEnv( StrA & result, const char * name )
{
    const char * var;
    if( strEmpty(name) || NULL == (var=::getenv(name)) )
    {
        result.clear();
        return false;
    }
    else
    {
        result.assign( var );
        return true;
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::sleep( uint32_t microSeconds )
{
#if GN_MSWIN
    ::Sleep( microSeconds );
#elif GN_POSIX
    uint32_t s = microSeconds / 1000;
    uint32_t us = microSeconds % 1000;
    ::sleep( s );
    ::usleep( us );
#else
    GN_CASSERT_EX( 0, "Unimplmented" );
#endif
}
