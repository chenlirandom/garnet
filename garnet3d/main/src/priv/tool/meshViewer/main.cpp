#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::input;
using namespace GN::util;

static GN::Logger * sLogger = GN::GetLogger("GN.tool.meshViewer");

class MyApp : public SampleApp
{
    const char * filename;
    ArcBall      arcball; // arcball camera
    float        radius;  // distance from camera to object
    Camera       camera;
    Entity     * light;
    Entity     * scene;

public:

    void updateRadius()
    {
        Gpu & gpu = GetGpu();

        const DispDesc & dd = gpu.GetDispDesc();

        // setup camera
        Matrix44f view, proj;
        view.LookAtRh( Vector3f(0,0,radius), Vector3f(0,0,0), Vector3f(0,1,0) );
        gpu.ComposePerspectiveMatrixRh( proj, GN_PI/4.0f, (float)dd.width/dd.height, radius / 100.0f, radius * 2.0f );
        camera.setViewMatrix( view );
        camera.setProjectionMatrix( proj );

        // setup arcball
        float h = tan( 0.5f ) * radius * 2.0f;
        arcball.setMouseMoveWindow( 0, 0, (int)dd.width, (int)dd.height );
        arcball.setViewMatrix( view );
        arcball.setTranslationSpeed( h / dd.height );

        // setup light as a head light that is in same location as camera.
        light->getNode<SpatialNode>()->setPosition( Vector3f(0,0,radius) );
    }

    bool OnInit()
    {
        World & w = GetWorld();

        light = w.createLightEntity( NULL );
        if( !light ) return false;

        // load scene from file
        //if( !scene->getNode<VisualNode>()->loadModelsFromFile( filename ) ) return false;
        SimpleWorldDesc swd;
        if( !swd.LoadFromFile( filename ) ) return false;
        scene = swd.populateTheWorld( w );
        if( !scene ) return false;

        // update camera stuff
        Spheref bs;
        CalculateBoundingSphereFromBoundingBox( bs, swd.bbox );
        radius = bs.radius * 2.0f;
        if( 0.0f == radius ) radius = 1.0f;

        // initialize arcball
        arcball.setHandness( util::RIGHT_HAND );
        arcball.setTranslation( -bs.center );
        arcball.connectToInput();

        // success
        updateRadius();
        return true;
    }

    void OnQuit()
    {
    }

    void OnRenderWindowResize( HandleType, UInt32 width, UInt32 height )
    {
        arcball.setMouseMoveWindow( 0, 0, (int)width, (int)height );
    }

    void OnAxisMove( Axis a, int d )
    {
        if( Axis::MOUSE_WHEEL_0 == a )
        {
            float speed = radius / 100.0f;
            radius -= speed * d;
            if( radius < 0.1f ) radius = 0.1f;
            updateRadius();
        }
    }

    void OnUpdate()
    {
        SpatialNode * sceneSpatial = scene->getNode<SpatialNode>();
        const Vector3f & position = arcball.getTranslation();
        sceneSpatial->setPosition( position );
        sceneSpatial->setRotation( arcball.getRotation() );
    }

    void OnRender()
    {
        Gpu & gpu = GetGpu();

        gpu.ClearScreen( Vector4f(0,0.5f,0.5f,1.0f) );

        GetWorld().visualGraph().Draw( camera );

        const Vector3f & position = arcball.getTranslation();

        GetFont().DrawText(
            StringFormat(
                L"position : %f,\n"
                L"           %f,\n"
                L"           %f\n"
                L"radius   : %f",
                position.x, position.y, position.z,
                radius ).ToRawPtr(),
            320, 40 );

        DrawXYZCoordinateAxes( camera.getProjectionMatrix() * camera.getViewMatrix() * arcball.getRotationMatrix44() );
    }

    bool OnCheckExtraCmdlineArguments( int argc, const char * const argv [] )
    {
        if( 0 == argc )
        {
            GN_ERROR(sLogger)( "Mesh file name is missing." );
            return false;
        }
        filename = argv[0];
        return true;
    }

    void OnPrintHelpScreen( const char * executableName )
    {
        GN_INFO(sLogger)( "\nUsage: %s [options] meshfile\n", executableName );
        PrintStandardCommandLineOptions();
    }
};

int main( int argc, const char * argv[] )
{
   printf( "\nGarnet mesh viewer V0.1.\n" );

   MyApp app;

   return app.Run( argc, argv );
}
