#include "../testCommon.h"

typedef GN::ResourceManager<int> ResMgr;

bool defCreator( int & res, const GN::StrA & name )
{
    return 1 == ::sscanf( name.cstr(), "%d", &res );
}

bool nullCreator( int & res, const GN::StrA & )
{
    res = -1;
    return true;
}

bool failedCreator( int &, const GN::StrA & )
{
    return false;
}

void defDeletor( int & )
{
    // do nothing
}

class ResourceManagerTest : public CxxTest::TestSuite
{
public:

    // no creator
    void testNoCreator()
    {
        ResMgr rm;

        TS_ASSERT( rm.addResource( "1" ) );

        TS_ASSERT_EQUALS( 0, rm.getResource( "1" ) );
    }

    // default nullor
    void testDefaultNullor()
    {
        ResMgr rm;

        TS_ASSERT( rm.addResource( "1" ) );

        TS_ASSERT_EQUALS( 0, rm.getResource( "2" ) );

        // default nullor failure
        rm.setDefaultNullor( GN::makeFunctor(&failedCreator) );
        TS_ASSERT_EQUALS( 0, rm.getResource( "1" ) );
        TS_ASSERT_EQUALS( 0, rm.getResource( "2" ) );

        // default nullor success
        rm.setDefaultNullor( GN::makeFunctor(&nullCreator) );
        TS_ASSERT_EQUALS( -1, rm.getResource( "1" ) );
        TS_ASSERT_EQUALS( -1, rm.getResource( "2" ) );
    }

    // per-resource nullor
    void testNullor()
    {
        ResMgr rm;

        // per-resource nullor failed
        TS_ASSERT( rm.addResource(
            "1",
            ResMgr::Creator(),
            GN::makeFunctor(&failedCreator) ) );
        TS_ASSERT_EQUALS( 0, rm.getResource("1") );

        // per-resource nullor success
        TS_ASSERT( rm.addResource(
            "2",
            ResMgr::Creator(),
            GN::makeFunctor(&nullCreator) ) );
        TS_ASSERT_EQUALS( -1, rm.getResource("2") );
    }

    // default creator
    void testDefaultCreator()
    {
        ResMgr rm;

        TS_ASSERT( rm.addResource( "1" ) );

        // default nullor failure
        rm.setDefaultCreator( GN::makeFunctor(&failedCreator) );
        TS_ASSERT_EQUALS( 0, rm.getResource( "1" ) );

        // default nullor success
        rm.setDefaultCreator( GN::makeFunctor(&defCreator) );
        TS_ASSERT_EQUALS( 1, rm.getResource( "1" ) );
    }

    // per-resource creator
    void testCreator()
    {
        // TODO: implement this test case
    }

    // override existing resource
    void testOverrideExistingResource()
    {
        ResMgr rm;

        TS_ASSERT( rm.addResource( "1", GN::makeFunctor(&defCreator), ResMgr::Creator(), false ) );
        TS_ASSERT_EQUALS( 1, rm.getResource( "1" ) );

        // default is not overriding
        TS_ASSERT( !rm.addResource( "1" ) );
        TS_ASSERT_EQUALS( 1, rm.getResource( "1" ) );

        // override existing
        TS_ASSERT( rm.addResource( "1", GN::makeFunctor(&nullCreator), ResMgr::Creator(), true ) );
        TS_ASSERT_EQUALS( -1, rm.getResource( "1" ) );
    }

    void testName2Handle()
    {
        ResMgr rm;

        // rm should be empty
        TS_ASSERT( rm.empty() );

        ResMgr::ResHandle h1 = rm.addResource( "1" );
        TS_ASSERT( h1 );

        // rm should NOT be empty
        TS_ASSERT( !rm.empty() );

        // handle -> name
        TS_ASSERT_EQUALS( "1", rm.getResourceName(h1) );
        TS_ASSERT_EQUALS( "", rm.getResourceName(h1+1) );

        // name -> handle
        TS_ASSERT_EQUALS( h1, rm.getResourceHandle("1") );
        TS_ASSERT_EQUALS( 0, rm.getResourceHandle("2") );

    }
};
