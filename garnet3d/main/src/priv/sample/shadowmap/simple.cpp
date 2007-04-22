#include "pch.h"
#include "simple.h"

using namespace GN::gfx;
using namespace GN::scene;

GN::Logger * sLogger = GN::getLogger("GN.sample.ShadowMap");

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::SimpleShadowMap::init( const StrA & actorName )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::SimpleShadowMap, () );

    Renderer & r = gRenderer;

    // create depth texture
    mShadowMap.attach( r.create2DTexture( mShadowMapSize.x, mShadowMapSize.y, 1, FMT_DEFAULT, TEXUSAGE_DEPTH ) );
    if( mShadowMap.empty() )
    {
        GN_ERROR(sLogger)( "current hardware does not support depth texture." );
        return failure();
    }

    // load actor
    if( !loadActor( actorName ) ) return failure();

    // intialize camera
    float radius = mShadowProjectors->getBoundingSphere().radius;
    mCamera.setPosition( 0, 0, radius * 2.0f );
    mCamera.setMoveSpeed( radius * 0.5f );

    // initialize scene
    const Matrix44f & view = mCamera.getViewMatrix();
    Matrix44f proj;
    r.composePerspectiveMatrixRh( proj, 1.0f, 4.0f/3.0f, radius / 100.0f, radius * 10.0f );
    mScene.setProj( proj );
    mScene.light(0).position.set( radius * 2.0f, radius * 2.0f, radius * 2.0f );
    mScene.setView( view );

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::SimpleShadowMap::quit()
{
    GN_GUARD;

    mShadowMap.clear();

    releaseActorHiearacy( mShadowProjectors );
    releaseActorHiearacy( mShadowReceivers );
    mShadowProjectors = 0;
    mShadowReceivers = 0;

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::SimpleShadowMap::clear()
{
    mShadowMapSize.set( 1024, 1024 );

    mShadowProjectors = 0;
    mShadowReceivers = 0;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::SimpleShadowMap::update()
{
    mCamera.update( app::SampleApp::UPDATE_INTERVAL );

    mScene.setView( mCamera.getViewMatrix() );

    // update projection matrix
    const Spheref & bs = mShadowProjectors->getBoundingSphere();
    float d = mCamera.getPosition().length();
    float n_min = bs.radius / 10.0f;
    float n = d - bs.radius;
    float f = d + bs.radius;
    if( n < n_min ) n = n_min;
    Matrix44f proj;
    gRenderer.composePerspectiveMatrixRh( proj, 1.0f, 4.0f/3.0f, n, f );
    mScene.setProj( proj );
}

//
//
// -----------------------------------------------------------------------------
void GN::SimpleShadowMap::draw()
{
    Renderer & r = gRenderer;

    r.clearScreen();

    // draw axises
    static const float X[] = { 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f, 0.0f };
    static const float Y[] = { 0.0f, 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f };
    static const float Z[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10000.0f };
    const Matrix44f & view = mScene.getView();
    const Matrix44f & proj = mScene.getProj();
    r.drawLines( 0, X, 3*sizeof(float), 1, GN_RGBA32(255,0,0,255), Matrix44f::IDENTITY, view, proj );
    r.drawLines( 0, Y, 3*sizeof(float), 1, GN_RGBA32(0,255,0,255), Matrix44f::IDENTITY, view, proj );
    r.drawLines( 0, Z, 3*sizeof(float), 1, GN_RGBA32(0,0,255,255), Matrix44f::IDENTITY, view, proj );

    // draw the actor
    mShadowProjectors->draw();
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::SimpleShadowMap::loadActor( const StrA & name )
{
    GN_ASSERT( 0 == mShadowProjectors && 0 == mShadowReceivers );

    mShadowProjectors = mScene.loadActorHiearachyFromXmlFile( name, "root" );
    if( 0 == mShadowProjectors ) return false;

    // place actor to screen center
    Quaternionf r;
    r.fromRotation( Vector3f(1,0,0), -GN_HALF_PI );;
    mShadowProjectors->setRotation( r );
    mShadowProjectors->setPosition( -mShadowProjectors->getBoundingSphere().center );

    // load depth only effect
    ResourceId effid = gSceneResMgr.getResourceId( "media::/effect/depth_only.xml" );
    if( 0 == effid ) return false;
    Effect * eff = gSceneResMgr.getResourceT<Effect>( effid );
    if( 0 == eff ) return false;

    for( Actor * a = mShadowProjectors; a; a = traverseTreePreOrder( a ) )
    {
        for( size_t i = 0; i < a->getNumDrawables(); ++i )
        {
            Drawable d = a->getDrawable( i );
            d.effect = effid;
            d.textures.clear();
            d.uniforms.clear();
            d.uniforms["pvw"].binding = eff->getUniformID( "pvw" );
            a->setDrawable( i, d );
        }
    }

    // create shadow receivers
    mShadowReceivers = cloneActorHiearacy( mShadowProjectors );

    // success
    return true;
}
