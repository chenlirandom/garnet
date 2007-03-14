#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::app;
using namespace GN::util;
using namespace GN::scene;

bool gAnimation = true;

class Scene
{
    SampleApp & app;
    
    UInt32 tex0;

    UInt32 eff0;

    Drawable box;

    Matrix44f world, view, proj;

    ArcBall arcball;

public:

    Scene( SampleApp & a ) : app(a) {}

    ~Scene() { quit(); }

    bool init()
    {
        scene::ResourceManager & rm = gSceneResMgr;

        // load box
        if( !box.loadFromXmlFile( "media::drawable/cube1.xml" ) ) return false;

        // load texture
        tex0 = rm.getResourceId( "media::texture/rabit.png" );
        if( 0 == tex0 ) return false;

        // load effect
        eff0 = rm.getResourceId( "media::effect/sprite.xml" );
        if( 0 == eff0 ) return false;

        // initialize matrices
        world.identity();
        view.lookAtRh( Vector3f(3,3,3), Vector3f(0,0,0), Vector3f(0,1,0) );
        gRenderer.composePerspectiveMatrixRh( proj, 1.0f, 4.0f/3.0f, 1.0f, 10.0f );

        // initialize arcball.
        arcball.setHandness( util::ArcBall::RIGHT_HAND );
        arcball.setViewMatrix( view );
        const DispDesc & dd = gRenderer.getDispDesc();
        arcball.setMouseMoveWindow( 0, 0, (int)dd.width, (int)dd.height );
        arcball.connectToInput();

        // success
        return true;
    }

    void quit()
    {
    }

    void update()
    {
        scene::ResourceManager & rm = gSceneResMgr;

        // update box matrix
        world = arcball.getRotationMatrix();
        Matrix44f pvw = proj * view * world;
        box.uniforms["pvw"].value = pvw;

        // update color
        static int r = 0; static int rr = 1;
        static int g = 80; static int gg = 1;
        static int b = 160; static int bb = 1;
        r += rr; if( 0 == r || 255 == r ) rr = -rr;
        g += gg; if( 0 == g || 255 == g ) gg = -gg;
        b += bb; if( 0 == b || 255 == b ) bb = -bb;
        GN::gfx::UniformValue u;
        u.setV( GN::Vector4f( r/255.0f, g/255.0f, b/255.0f, 1.0f ) );
        rm.getResourceT<Effect>(eff0)->setUniformByName( "color", u );
    }

    void draw()
    {
        Renderer & r = gRenderer;
        scene::ResourceManager & rm = gSceneResMgr;

        // quad 1
        gQuadRenderer.drawBegin( rm.getResourceT<Texture>(tex0), QuadRenderer::OPT_DEPTH_WRITE );
        gQuadRenderer.drawTextured( 0, 0, 0, 0.5f, 0.5f );
        gQuadRenderer.drawEnd();

        // quad 2,3
        gQuadRenderer.drawSolidBegin();
        gQuadRenderer.drawSolid( ubyte4ToBGRA32(0,255,0,255), 0, 0.5, 0.0, 1.0, 0.5 );
        gQuadRenderer.drawSolid( ubyte4ToBGRA32(255,0,0,255), 0, 0.0, 0.5, 0.5, 1.0 );
        gQuadRenderer.drawEnd();

        // quad 4
        RendererContext ctx;
        ctx.clearToNull();
        Effect * eff = rm.getResourceT<Effect>( eff0 );
        for( size_t i = 0; i < eff->getNumPasses(); ++i )
        {
            eff->passBegin( ctx, i );
            eff->commitChanges();
            r.setContext( ctx );
            gQuadRenderer.drawSingleTexturedQuad( 0, QuadRenderer::OPT_USER_CONTEXT, 0, 0.5, 0.5, 1.0, 1.0 );
            eff->passEnd();
        }

        // draw box
        box.draw();

        /* a wireframe box
        {
            static const float E = 80.0f;
            static float lines[] =
            {
                -E, -E, -E,  E, -E, -E,
                 E, -E, -E,  E,  E, -E,
                 E,  E, -E, -E,  E, -E,
                -E,  E, -E, -E, -E, -E,

				-E, -E, -E, -E, -E,  E,
                 E, -E, -E,  E, -E,  E,
                 E,  E, -E,  E,  E,  E,
                -E,  E, -E, -E,  E,  E,

                -E, -E,  E,  E, -E,  E,
                 E, -E,  E,  E,  E,  E,
                 E,  E,  E, -E,  E,  E,
                -E,  E,  E, -E, -E,  E,
			};
            static size_t count = sizeof(lines)/sizeof(float)/6;
            
            r.drawLines(
                0,
                lines,
                sizeof(float)*3,
                count,
                ubyte4ToBGRA32(255,255,0,255),
                world, view, proj );
        }//*/
    }
};

///
/// GFX module test application
///
class GfxTestApp : public SampleApp
{
    Scene * mScene;
public:

    GfxTestApp() : mScene(0) {}

    bool onRendererCreate()
    {
        mScene = new Scene(*this);
        return mScene->init();
    }

    void onRendererDestroy()
    {
        safeDelete(mScene);
    }

    void onUpdate()
    {
        if( gAnimation ) mScene->update();
    }

    void onRender()
    {
        Renderer & r = gRenderer;
        
        r.clearScreen( Vector4f(0,0,0,1) ); // clear to pure black

        // draw scene
        GN_ASSERT( mScene );
        mScene->draw();

        // draw mouse position on screen
        StrA mousePos;
        int x = 1, y = 1;
        gInput.getMousePosition( x, y );
        mousePos.format( "Mouse: %d, %d", x, y );
        gAsciiFont.drawText( mousePos.cptr(), 0, 100 );
    }
};

int main( int argc, const char * argv[] )
{
    GfxTestApp app;
    return app.run( argc, argv );
}
