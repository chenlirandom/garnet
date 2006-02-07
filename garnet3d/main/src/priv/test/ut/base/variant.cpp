#include "../testCommon.h"

class VariantTest : public CxxTest::TestSuite
{
public:

    void testCtor()
    {
        {
            GN::Variant var;
            TS_ASSERT_EQUALS( GN::VARIANT_INVALID, var.getType() );
        }
        {
            GN::Variant var(true);
            TS_ASSERT_EQUALS( GN::VARIANT_BOOL, var.getType() );
            bool v;
            TS_ASSERT( var.getB(v) ); TS_ASSERT_EQUALS( true, v );
        }
        {
            GN::Variant var(1);
            TS_ASSERT_EQUALS( GN::VARIANT_INT, var.getType() );
            int v;
            TS_ASSERT( var.getI(v) ); TS_ASSERT_EQUALS( 1, v );
        }
        {
            GN::Variant var(1.0f);
            TS_ASSERT_EQUALS( GN::VARIANT_FLOAT, var.getType() );
            float v;
            TS_ASSERT( var.getF(v) ); TS_ASSERT_EQUALS( 1.0f, v );
        }
        {
            GN::Variant var((void*)NULL);
            TS_ASSERT_EQUALS( GN::VARIANT_POINTER, var.getType() );
            void * v;
            TS_ASSERT( var.getP(v) ); TS_ASSERT_EQUALS( (void*)NULL, v );
        }
        {
            GN::Variant var(GN::StrA("a"));
            TS_ASSERT_EQUALS( GN::VARIANT_STRING, var.getType() );
            GN::StrA v;
            TS_ASSERT( var.getS(v) ); TS_ASSERT_EQUALS( "a", v );
        }
        {
            GN::Variant var(GN::Vector4f(1.0f,2.0f,3.0f,4.0f));
            TS_ASSERT_EQUALS( GN::VARIANT_VECTOR4, var.getType() );
            GN::Vector4f v;
            TS_ASSERT( var.getV(v) );
            TS_ASSERT_EQUALS( 1.0f, v.x );
            TS_ASSERT_EQUALS( 2.0f, v.y );
            TS_ASSERT_EQUALS( 3.0f, v.z );
            TS_ASSERT_EQUALS( 4.0f, v.w );
        }
    }

    void testFromInvalid()
    {
        GN::Variant var;
        bool          b;
        int           i;
        float         f;
        void *        p;
        GN::StrA      s;
        GN::Vector4f  v;
        GN::Matrix44f m;

        TS_ASSERT_EQUALS( GN::VARIANT_INVALID, var.getType() );

        TS_ASSERT( !var.getB(b) );
        TS_ASSERT( !var.getI(i) );
        TS_ASSERT( !var.getF(f) );
        TS_ASSERT( !var.getP(p) );
        TS_ASSERT( !var.getS(s) );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );
    }

    void testFromBool()
    {
        GN::Variant var;
        bool          b;
        int           i;
        float         f;
        void *        p;
        GN::StrA      s;
        GN::Vector4f  v;
        GN::Matrix44f m;

        var.setB(true);

        TS_ASSERT_EQUALS( GN::VARIANT_BOOL, var.getType() );

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( true, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 1, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 1.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)1, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "yes", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );

        var.setB(false);

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( false, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 0, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 0.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)0, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "no", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );
    }

    void testFromInt()
    {
        GN::Variant var;
        bool          b;
        int           i;
        float         f;
        void *        p;
        GN::StrA      s;
        GN::Vector4f  v;
        GN::Matrix44f m;

        var.setI(1);

        TS_ASSERT_EQUALS( GN::VARIANT_INT, var.getType() );

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( true, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 1, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 1.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)1, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "1", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );

        var.setI(0);

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( false, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 0, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 0.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)0, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "0", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );
    }

    void testFromFloat()
    {
        GN::Variant var;
        bool          b;
        int           i;
        float         f;
        void *        p;
        GN::StrA      s;
        GN::Vector4f  v;
        GN::Matrix44f m;

        var.setF(1.123f);

        TS_ASSERT_EQUALS( GN::VARIANT_FLOAT, var.getType() );

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( true, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 1, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 1.123f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)1, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "1.123000", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );

        var.setF(0.0f);

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( false, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 0, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 0.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)0, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "0.000000", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );
    }

    void testFromString()
    {
        GN::Variant var;
        bool          b;
        int           i;
        float         f;
        void *        p;
        GN::StrA      s;
        GN::Vector4f  v;
        GN::Matrix44f m;

        var.setS("1");

        TS_ASSERT_EQUALS( GN::VARIANT_STRING, var.getType() );

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( true, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 1, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 1.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)1, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "1", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );

        var.setS("0");

        TS_ASSERT( var.getB(b) ); TS_ASSERT_EQUALS( false, b );
        TS_ASSERT( var.getI(i) ); TS_ASSERT_EQUALS( 0, i );
        TS_ASSERT( var.getF(f) ); TS_ASSERT_EQUALS( 0.0f, f );
        TS_ASSERT( var.getP(p) ); TS_ASSERT_EQUALS( (void*)0, p );
        TS_ASSERT( var.getS(s) ); TS_ASSERT_EQUALS( "0", s );
        TS_ASSERT( !var.getV(v) );
        TS_ASSERT( !var.getM(m) );

        var.setS("(1,2,3,4)");
        TS_ASSERT( var.getV(v) );
        TS_ASSERT_EQUALS( 1.0f, v.x );
        TS_ASSERT_EQUALS( 2.0f, v.y );
        TS_ASSERT_EQUALS( 3.0f, v.z );
        TS_ASSERT_EQUALS( 4.0f, v.w );
    }

};
