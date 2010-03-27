#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::util;

static const Vector4f WATER_COLOR( 0.0f, 0.25f, 0.5f, 1.0f );

static GN::Logger * sLogger = GN::GetLogger("GN.sample.dolphin");

class TestScene
{
    GN::util::SampleApp & mApp;

    AutoRef<TextureResource> mCaustics[32];

    AutoRef<ModelResource> mDolphin, mSeafloor;

public:

     TestScene( GN::util::SampleApp & app ) : mApp(app) {}

    ~TestScene() { }

    bool create()
    {
        GpuResourceDatabase & gdb = mApp.GetGdb();

        // load caustic textures
        for( int i = 0; i < 32; ++i )
        {
            mCaustics[i] = TextureResource::LoadFromFile( gdb, StringFormat("media::dolphin/caust%02d.tga",i) );
            if( 0 == mCaustics[i] ) return false;
        }

        // load dolphin and seafloor
        mDolphin = ModelResource::LoadFromFile( gdb, "media::dolphin/dolphin.model.xml" );
        mSeafloor = ModelResource::LoadFromFile( gdb, "media::dolphin/seafloor.model.xml" );
        if( !mDolphin || !mSeafloor ) return false;

        // success
        return true;
    }

    void Update( float time, const Matrix44f & view, const Matrix44f & proj )
    {
        // update caustic parameters
        Vector4f caustics( 0.05f, 0.05f, sinf(time)/8, cosf(time)/10 );
        UInt32 causticTex = ((UInt32)(time*32))%32;

        // update seafloor effect parameters
        mSeafloor->GetUniformResource("view")->GetUniform()->Update( view );
        mSeafloor->GetUniformResource("proj")->GetUniform()->Update( proj );
        mSeafloor->GetUniformResource("caustic")->GetUniform()->Update( caustics );
        mSeafloor->SetTextureResource( "caustic", mCaustics[causticTex] );

        // Animation attributes for the dolphin
        float fKickFreq    = 2*time;
        float fPhase       = time/3;
        float fBlendWeight = sinf( fKickFreq );

        // Move dolphin in a circle
        Matrix44f scale, trans, rotate1, rotate2;
        scale.Identity(); scale *= 0.01f; scale[3][3] = 1.0f;
        rotate1.RotateZ( -cosf(fKickFreq)/6 );
        rotate2.RotateY( fPhase );
        float radius = 5;
        trans.Translate( -radius*sinf(fPhase), sinf(fKickFreq)/2, (2*radius)-(2*radius)*cosf(fPhase) );
        Matrix44f world = trans * rotate2 * rotate1 * scale;

        // calculate vertex blending weights
        float fWeight1;
        float fWeight2;
        float fWeight3;
        if( fBlendWeight > 0.0f )
        {
            fWeight1 = fabsf(fBlendWeight);
            fWeight2 = 1.0f - fabsf(fBlendWeight);
            fWeight3 = 0.0f;
        }
        else
        {
            fWeight1 = 0.0f;
            fWeight2 = 1.0f - fabsf(fBlendWeight);
            fWeight3 = fabsf(fBlendWeight);
        }
        Vector3f vWeight( fWeight1, fWeight2, fWeight3 );

        // update dolphin effect parameters
        mDolphin->GetUniformResource("pvw")->GetUniform()->Update( proj * view * world );
        mDolphin->GetUniformResource("viewworld")->GetUniform()->Update( view * world );
        mDolphin->GetUniformResource("weights")->GetUniform()->Update( vWeight );
        mDolphin->SetTextureResource( "caustic", mCaustics[causticTex] );
    }

    void render()
    {
        mApp.GetGpu().ClearScreen( WATER_COLOR );
        mSeafloor->Draw();
        mDolphin->Draw();
    }
};

class Dolphin : public GN::util::SampleApp
{
    TestScene * scene;

    float time;
    bool swimming;

    Matrix44f world, view, proj;

public:

    Dolphin() : scene(0)
    {
        // initialize
        time = .0f;
        swimming = true;

        Vector3f eye(0,0,-2.0f);
        Vector3f at(0,0,0);
        Vector3f up(0,1,0);
        view.LookAtLh( eye, at, up );

        world.Identity();
    }

    bool OnPreInit( InitParam & )
    {
        return true;
    }

    bool OnInit()
    {
        Gpu & g = GetGpu();

        UInt32 width = g.GetDispDesc().width;
        UInt32 height = g.GetDispDesc().height;

        float aspect = (float)width / height;
        g.ComposePerspectiveMatrixLh( proj, GN_PI/3, aspect, 1.0f, 1000.0f );

        // create scene
        scene = new TestScene(*this);
        return scene->create();
    }

    void OnQuit()
    {
        SafeDelete( scene );
    }

    void OnKeyPress( input::KeyEvent key )
    {
        GN::util::SampleApp::OnKeyPress( key );

        if( input::KeyCode::SPACEBAR == key.code && key.status.down )
        {
            swimming = !swimming;
        }
    }

    void OnUpdate()
    {
        if( swimming ) time += 1.0f/60.0f;
        scene->Update( time, view, proj );
    }

    void OnRender()
    {
        GN_ASSERT( scene );
        scene->render();
    }
};

int main( int argc, const char * argv[] )
{
    Dolphin app;
    return app.Run( argc, argv );
}
