#include "pch.h"
#include "garnet/GNutil.h"

static GN::Logger * sLogger = GN::getLogger("GN.util.ArcBall");

// *****************************************************************************
// local functions
// *****************************************************************************

///
/// convert window position to unit sphere vector.
///
/// \param hand
///     -1 : left hand
///      1 : right hand
///
static void sWindowPosition2UnitVector( GN::Vector3f & result, float x, float y, float hand )
{
    float k = x*x + y*y;
    if( k > 1.0f )
    {
        k = 1.0f / k;
        result.x = x * k;
        result.y = -y * k;
        result.z = 0.0f;
    }
    else
    {
        result.x = x;
        result.y = -y;
        result.z = hand * sqrt( 1.0f - k );
    }
}

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::util::ArcBall::ArcBall( Handness h )
    : mTranslation( 0, 0, 0 )
    , mQuat( Quaternionf::IDENTITY )
    , mRotation3x3( Matrix33f::IDENTITY )
    , mRotation4x4( Matrix44f::IDENTITY )
    , mTransView( Matrix44f::IDENTITY )
    , mWindowCenter( 0, 0 )
    , mWindowHalfSize( 1, 1 )
    , mHandness( LEFT_HAND == h ? -1.0f : 1.0f )
    , mRolling( false )
    , mMoveSpeed( 1.0f )
    , mMoving( false )
{
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::rotate( float dx, float dy )
{
    if( .0f == dx && .0f == dy ) return;

    dx /= mWindowHalfSize.x;
    dy /= mWindowHalfSize.y;

    Vector3f v1, v2;
    sWindowPosition2UnitVector( v1, 0, 0, mHandness );
    sWindowPosition2UnitVector( v2, dx, dy, mHandness );
    v1 = mTransView.transformVector( v1 );
    v2 = mTransView.transformVector( v2 );

    Quaternionf q;
    q.fromArc( v1, v2 );
    mQuat = q * mQuat;

    mQuat.toMatrix33( mRotation3x3 );
    mRotation4x4.set( mRotation3x3 );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::connectToInput()
{
    gSigKeyPress.connect( this, &ArcBall::onKeyPress );
    gSigAxisMove.connect( this, &ArcBall::onAxisMove );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::disconnectFromInput()
{
    gSigKeyPress.disconnect( this );
    gSigAxisMove.disconnect( this );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::beginRotation( int x, int y )
{
    GN_TRACE(sLogger)( "ArcBall::beginRotation()" );

    mRolling = true;

    float fx = (float)(x - mWindowCenter.x) / mWindowHalfSize.x;
    float fy = (float)(y - mWindowCenter.y) / mWindowHalfSize.y;

    sWindowPosition2UnitVector( mRollBase, fx, fy, mHandness );

    mRollBase = mTransView.transformVector( mRollBase );

    mQuatBase = mQuat;
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::endRotation()
{
    GN_TRACE(sLogger)( "ArcBall::endRotation()" );
    mRolling = false;
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::onRotation( int x, int y )
{
    if( !mRolling ) return;

    GN_TRACE(sLogger)( "ArcBall::onRotation()" );

    float fx = (float)(x - mWindowCenter.x) / mWindowHalfSize.x;
    float fy = (float)(y - mWindowCenter.y) / mWindowHalfSize.y;

    Vector3f v;
    sWindowPosition2UnitVector( v, fx, fy, mHandness );
    v = mTransView.transformVector( v );

    Quaternionf q;
    q.fromArc( mRollBase, v );
    mQuat = q * mQuatBase;

    mQuat.toMatrix33( mRotation3x3 );
    mRotation4x4.set( mRotation3x3 );

    GN_TRACE(sLogger)( "\n%s", mRotation3x3.print().cptr() );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::beginTranslation( int x, int y )
{
    GN_TRACE(sLogger)( "ArcBall::beginTranslation()" );
    mMoving = true;
    mTranslationBase = mTranslation;
    mMoveBase.set( x, y );
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::endTranslation()
{
    GN_TRACE(sLogger)( "ArcBall::endTranslation()" );
    mMoving = false;
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::onTranslation( int x, int y )
{
    if( !mMoving ) return;

    GN_TRACE(sLogger)( "ArcBall::onTranslation()" );

    Vector3f v( (float)( x - mMoveBase.x ), (float)( mMoveBase.y - y ), 0 );

    Matrix44f transWorld; // invtrans( inv(world) ), transform vector from world space to object space
    transWorld = Matrix44f::sTranspose( mRotation4x4 );

    v = mTransView.transformVector( v * mMoveSpeed ); // view space -> world space
    v = transWorld.transformVector( v ); // world space -> model space

    mTranslation = mTranslationBase + v;
}


// *****************************************************************************
// private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::onKeyPress( input::KeyEvent key )
{
    if( input::KEY_MOUSEBTN_0 == key.code )
    {
        if( key.status.down )
        {
            int x, y;
            gInput.getMousePosition( x, y );
            beginRotation( x, y );
        }
        else
        {
            endRotation();
        }
    }
    else if( input::KEY_MOUSEBTN_1 == key.code )
    {
        if( key.status.down )
        {
            int x, y;
            gInput.getMousePosition( x, y );
            beginTranslation( x, y );
        }
        else
        {
            endTranslation();
        }
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::util::ArcBall::onAxisMove( input::Axis, int )
{
    if( !mMoving && !mRolling ) return;

    int x, y;
    gInput.getMousePosition( x, y );

    onTranslation( x, y );
    onRotation( x, y );
}
