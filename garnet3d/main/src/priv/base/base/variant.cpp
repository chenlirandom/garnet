#include "pch.h"

// *****************************************************************************
// local functions
// *****************************************************************************

#define ENCODE_KEY(from,to) ( ((((sint16)from) & 0xFF) << 8) | (((sint16)to) & 0xFF) )

static GN::Logger * sLogger = GN::getLogger("GN.base.Variant");

// *****************************************************************************
// public methods
// *****************************************************************************

//
// set value
// -----------------------------------------------------------------------------
void GN::Variant::setb( bool b ) { mValue = b ? "1" : "0"; }
void GN::Variant::seti( int i ) { mValue.format( "%d", i ); }
void GN::Variant::setf( float f ) { mValue.format( "%f", f ); }
void GN::Variant::setp( void * p ) { if( 0 == p ) mValue = "0"; else mValue.format( "%p", p ); }
void GN::Variant::setv( const Vector4f & v ) { mValue.format( "%f,%f,%f,%f", v.x, v.y, v.z, v.w ); }
void GN::Variant::setm( const Matrix44f & m )
{
    mValue.format(
        "%f,%f,%f,%f,\n"
        "%f,%f,%f,%f,\n"
        "%f,%f,%f,%f,\n"
        "%f,%f,%f,%f",
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3] );
}

//
// get value
// -----------------------------------------------------------------------------
bool GN::Variant::getb( bool & b ) const
{
    int i;
    float f;
    if( 0 == stringCompareI( "yes", mValue.cptr() ) ||
        0 == stringCompareI( "true", mValue.cptr() ) ||
        0 == stringCompareI( "on", mValue.cptr() ) ||
        0 == stringCompare( "1", mValue.cptr() ) )
    {
        b = true;
        return true;
    }
    else if(
        0 == stringCompareI( "no", mValue.cptr() ) ||
        0 == stringCompareI( "false", mValue.cptr() ) ||
        0 == stringCompareI( "off", mValue.cptr() ) ||
        0 == stringCompare( "0", mValue.cptr() ) )
    {
        b = false;
        return true;
    }
    else if( geti( i ) )
    {
        b = 0 != i;
        return true;
    }
    else if( getf( f ) )
    {
        b = .0f != f;
        return true;
    }
    else
    {
        GN_ERROR(sLogger)( "Can't convert string '%s' to boolean.", mValue.cptr() );
        return false;
    }
}
//
bool GN::Variant::geti( int & i ) const
{
    return 0 != string2Integer<int>( i, mValue.cptr() );
}
//
bool GN::Variant::getf( float & f ) const
{
    return 0 != string2Float( f, mValue.cptr() );
}
//
bool GN::Variant::getp( void * & p ) const
{
    bool b = 0 != string2Integer<size_t>( (size_t&)p, mValue.cptr() );
    if( !b ) p = NULL;
    return b;
}
//
bool GN::Variant::getv( Vector4f & v ) const
{
    return 4 == string2FloatArray( v, 4, mValue.cptr(), mValue.size() );
}
//
bool GN::Variant::getm( Matrix44f & m ) const
{
    return 16 == string2FloatArray( m[0], 16, mValue.cptr(), mValue.size() );
}

// *****************************************************************************
// private methods
// *****************************************************************************
