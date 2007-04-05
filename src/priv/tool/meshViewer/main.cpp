#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::scene;
using namespace GN::input;
using namespace GN::util;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.tool.meshViewer");

class MeshViewerApp : public app::SampleApp
{
    StrA mObjName;

    Scene mScene;
    Actor mActor;
    ArcBall mArcBall;

    float mRadius; // distance from camera to object

    Matrix44f mProj, mView;

    void updateRadius()
    {
        mView.lookAtRh( Vector3f(0,0,mRadius), Vector3f(0,0,0), Vector3f(0,1,0) );
        gRenderer.composePerspectiveMatrixRh( mProj, 1.0f, 4.0f/3.0f, mRadius / 100.0f, mRadius * 2.0f );
        mScene.setView( mView );
        mScene.setProj( mProj );
        mScene.light(0).position.set( 0, 0, mRadius ); // head light: same location as camera.
    }

public:

    MeshViewerApp() : mActor( mScene ) {}

    bool onCheckCmdLine( int argc, const char * const argv[] )
    {
        for( int i = 1; i < argc; ++i )
        {
            const char * a = argv[i];
            if( '-' != *a )
            {
                mObjName = a;
                break;
            }
            else GN_WARN(sLogger)( "unknown command line argument: %s", a );
        }
        if( mObjName.empty() )
        {
            mObjName = "media::/cube/cube.actor.xml";
            GN_INFO(sLogger)( "no object specified in comment line. Using default one: %s", mObjName.cptr() );
        }
        return true;
    }

    bool onRendererRestore()
    {
        static bool firstTime = true;
        if( firstTime )
        {
            firstTime = false;

            // initialize actor
            loadFromXmlFile( mActor, mObjName );
            mActor.setPivot( mActor.getBoundingSphere().center );
            mActor.setPosition( -mActor.getBoundingSphere().center );

            // update camera stuff
            mRadius = mActor.getBoundingSphere().radius * 2.0f;
            updateRadius();

            // initialize mArcBall
            mArcBall.setHandness( util::ArcBall::RIGHT_HAND );
            mArcBall.setViewMatrix( mView );
            mArcBall.connectToInput();
        }
        
        // update arcball window
        const DispDesc & dd = gRenderer.getDispDesc();
        mArcBall.setMouseMoveWindow( 0, 0, (int)dd.width, (int)dd.height );

        return true;
    }

    void onAxisMove( Axis a, int d )
    {
        app::SampleApp::onAxisMove( a, d );

        if( AXIS_MOUSE_WHEEL_0 == a )
        {
            float speed = mRadius / 100.0f;
            mRadius -= speed * d;
            if( mRadius < 0.1f ) mRadius = 0.1f;
            updateRadius();
        }
    }

    void onUpdate()
    {
        const int * axises = gInput.getAxisStatus();
        mArcBall.rotate(
            (float)axises[input::AXIS_XB360_THUMB_LX] /  2000.0f,
            (float)axises[input::AXIS_XB360_THUMB_LY] / -2000.0f );

        mActor.setRotation( mArcBall.getRotation() );
    }

    void onRender()
    {
        Renderer & r = gRenderer;
        r.clearScreen();

        static const float X[] = { 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f, 0.0f };
        static const float Y[] = { 0.0f, 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f };
        static const float Z[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10000.0f };

        const Matrix44f & world = mArcBall.getRotationMatrix44();
        r.drawLines( 0, X, 3*sizeof(float), 1, 0xFFFF0000, world, mView, mProj );
        r.drawLines( 0, Y, 3*sizeof(float), 1, 0xFF00FF00, world, mView, mProj );
        r.drawLines( 0, Z, 3*sizeof(float), 1, 0xFF0000FF, world, mView, mProj );

        mActor.draw();

#if 0
        // draw matrices onto screen
        {
            StrW text;
            text.format(
                L"world :\n%S\n"
                L"view  :\n%S\n"
                L"proj  :\n%S\n",
                mWorld.print().cptr(),
                mView.print().cptr(),
                mProj.print().cptr() );
            getFontRenderer().drawText( text.cptr(), 0, 100 );
        }
#endif
    }
};

int main( int argc, const char * argv[] )
{
    MeshViewerApp app;
    return app.run( argc, argv );
}
