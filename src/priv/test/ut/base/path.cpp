#include "../testCommon.h"

#if GN_MSWIN
#define PSC '\\'
#define PSS "\\"
#else
#define PSC '/'
#define PSS "/"
#endif

#if GN_MSWIN || GN_CYGWIN
#define APPEXT ".exe"
#else
#define APPEXT ""
#endif


class PathTest : public CxxTest::TestSuite
{
public:

    void testToNative()
    {
        TS_ASSERT_EQUALS( PSS, GN::path::toNative("/") );
        TS_ASSERT_EQUALS( PSS, GN::path::toNative("\\") );

        TS_ASSERT_EQUALS( PSS"a"PSS"b", GN::path::toNative("/a//b/") );
        TS_ASSERT_EQUALS( PSS"a"PSS"b", GN::path::toNative("\\a\\\\b\\") );

        TS_ASSERT_EQUALS( "a:"PSS"b", GN::path::toNative("a:b") );
        TS_ASSERT_EQUALS( "a:"PSS"b", GN::path::toNative("a:b") );

        TS_ASSERT_EQUALS( "a:"PSS, GN::path::toNative("a:") );
        TS_ASSERT_EQUALS( "a:"PSS, GN::path::toNative("a:/") );
        TS_ASSERT_EQUALS( "a:"PSS, GN::path::toNative("a:\\") );

        TS_ASSERT_EQUALS( "a"PSS":", GN::path::toNative("a/:") );
        TS_ASSERT_EQUALS( "a"PSS":", GN::path::toNative("a\\:") );
    }

    void testJoin()
    {
        TS_ASSERT_EQUALS( "", GN::path::join( "", "" ) );
        TS_ASSERT_EQUALS( "a", GN::path::join( "", "a" ) );
        TS_ASSERT_EQUALS( "a", GN::path::join( "a", "" ) );
        TS_ASSERT_EQUALS( "a", GN::path::join( "", "a", "" ) );
        TS_ASSERT_EQUALS( "a"PSS"b", GN::path::join( "a", "b" ) );
        TS_ASSERT_EQUALS( "a"PSS"b", GN::path::join( "", "a", "", "b", "" ) );
        TS_ASSERT_EQUALS( PSS"a"PSS"b"PSS"c", GN::path::join( "\\a\\", "\\b\\", "\\c\\" ) );
        TS_ASSERT_EQUALS( PSS"a"PSS"b"PSS"c", GN::path::join( "/a/", "/b/", "/c/" ) );
    }

    void testPrefix()
    {
        GN::StrA s[3] ={
            GN::path::toNative("app:"),
            GN::path::toNative("startup:"),
            GN::path::toNative("pwd:")
        };

        GN_INFO(sLogger)( "appDir = %s", s[0].cptr() );
        GN_INFO(sLogger)( "startup = %s", s[1].cptr() );
        GN_INFO(sLogger)( "pwd = %s", s[2].cptr() );

        for( size_t i = 0; i < 3; ++i )
        {
            TS_ASSERT( !s[i].empty() );
            if( s[i].size() > 1 )
            {
                TS_ASSERT_DIFFERS( PSC, s[i].last() );
            }
            else
            {
                TS_ASSERT_EQUALS( PSS, s[i] );
            }
        }
    }

    void testExist()
    {
        TS_ASSERT( GN::path::exist("/") );
        TS_ASSERT( !GN::path::exist("haha,heihei,hoho,huhu,mama,papa") );
        TS_ASSERT( GN::path::exist("SConstruct") );

        TS_ASSERT( GN::path::exist("startup:\\SConstruct") );
        TS_ASSERT( GN::path::exist("startup:/SConstruct") );
        TS_ASSERT( GN::path::exist("startup:SConstruct") );

        TS_ASSERT( GN::path::exist("app:GNut"APPEXT) );
        TS_ASSERT( GN::path::exist("app:/GNut"APPEXT) );
        TS_ASSERT( GN::path::exist("app:\\GNut"APPEXT) );

        TS_ASSERT( GN::path::exist("pwd:") );
        TS_ASSERT( GN::path::exist("pwd:/") );
        TS_ASSERT( GN::path::exist("pwd:\\") );
    }

    void testIsDir()
    {
        TS_ASSERT( GN::path::isDir("/") );

        TS_ASSERT( GN::path::isDir("startup:") );
        TS_ASSERT( GN::path::isDir("startup:/") );
        TS_ASSERT( GN::path::isDir("startup:\\") );

        TS_ASSERT( GN::path::isDir("app:") );
        TS_ASSERT( GN::path::isDir("app:/") );
        TS_ASSERT( GN::path::isDir("app:\\") );

        TS_ASSERT( GN::path::isDir("pwd:") );
        TS_ASSERT( GN::path::isDir("pwd:/") );
        TS_ASSERT( GN::path::isDir("pwd:\\") );

        TS_ASSERT( !GN::path::isDir("haha,heihei,hoho,huhu,mama,papa") );
    }

    void testIsFile()
    {
        TS_ASSERT( GN::path::isFile("startup:\\SConstruct") );
        TS_ASSERT( GN::path::isFile("app:GNut"APPEXT) );
        TS_ASSERT( !GN::path::isFile("startup:") );
        TS_ASSERT( !GN::path::isFile("app:") );
        TS_ASSERT( !GN::path::isFile("pwd:") );
        TS_ASSERT( !GN::path::isFile("haha,heihei,hoho,huhu,mama,papa") );
    }

    void testIsAbsPath()
    {
        using namespace GN;
        using namespace GN::path;
        TS_ASSERT( isRelPath("") );
        TS_ASSERT( isRelPath("a/a") );
        TS_ASSERT( isRelPath("a/a:") );
        TS_ASSERT( isAbsPath("/") );
        TS_ASSERT( isAbsPath("/a") );
        TS_ASSERT( isAbsPath("c:a") );
        TS_ASSERT( isAbsPath("c:/a") );
    }

    void testGetParent()
    {
        TS_ASSERT_EQUALS( "a", GN::path::getParent("a/b") );
        TS_ASSERT_EQUALS( "a", GN::path::getParent("a/b/") );
        TS_ASSERT_EQUALS( PSS, GN::path::getParent("/a") );
        TS_ASSERT_EQUALS( PSS, GN::path::getParent("/a/") );
        TS_ASSERT_EQUALS( "", GN::path::getParent("a") );
        TS_ASSERT_EQUALS( "", GN::path::getParent("a/") );
        TS_ASSERT_EQUALS( PSS, GN::path::getParent("/") );
        TS_ASSERT_EQUALS( "", GN::path::getParent("") );
        TS_ASSERT_EQUALS( "startup:"PSS"a", GN::path::getParent("startup:a/b") );
        TS_ASSERT_EQUALS( "startup:"PSS, GN::path::getParent("startup:/a") );
        TS_ASSERT_EQUALS( "startup:"PSS, GN::path::getParent("startup:a") );
        TS_ASSERT_EQUALS( "startup:"PSS, GN::path::getParent("startup:/") );
        TS_ASSERT_EQUALS( "startup:"PSS, GN::path::getParent("startup:") );
    }

    void testGetExt()
    {
        TS_ASSERT_EQUALS( ".c", GN::path::getExt("a.b.c") );
        TS_ASSERT_EQUALS( ".c", GN::path::getExt("a.c") );
        TS_ASSERT_EQUALS( ".c", GN::path::getExt(".c") );
        TS_ASSERT_EQUALS( ".", GN::path::getExt("a.") );
        TS_ASSERT_EQUALS( "", GN::path::getExt("a") );
    }
};
