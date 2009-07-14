#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::util;
using namespace GN::input;

static GN::Logger * sLogger = GN::getLogger( "GN.test.RenderToTexture" );

float RT_WIDTH  = 256.0f;
float RT_HEIGHT = 256.0f;

class RenderToTexture
{
    Renderer       & rndr;
    SpriteRenderer   sr;
    AutoRef<Texture> c0, ds;
    AutoRef<Texture> tex0, tex1;
    RendererContext  context;

    // box scene data
    struct BoxVert
    {
        float x, y, z, nx, ny, nz, u, v;
    };
    Matrix44f           proj, view;
    SimpleDiffuseEffect effect;
    Clock               timer;

public:

    RenderToTexture( Renderer & r )
        : rndr(r)
        , sr(r)
    {
    }

    ~RenderToTexture()
    {
        quit();
    }

    bool init()
    {
        // create sprite renderer
        if( !sr.init() ) return false;

        // create render targets
        c0.attach( rndr.create2DTexture( (UInt32)RT_WIDTH, (UInt32)RT_HEIGHT, 1, ColorFormat::RGBA32, TextureUsage::COLOR_RENDER_TARGET ) );
        if( c0.empty() )
        {
            GN_ERROR(sLogger)( "Current graphics hardware does not support render-to-texture at all." );
            return false;
        }

        ds.attach( rndr.create2DTexture( (UInt32)RT_WIDTH, (UInt32)RT_HEIGHT, 1, ColorFormat::UNKNOWN, TextureUsage::DEPTH_RENDER_TARGET ) );
        if( ds.empty() )
        {
            GN_WARN(sLogger)( "Current graphics hardware does not support depth-texture. All tests related depth-texture are disabled." );
        }

        // load textures
        tex0.attach( loadTextureFromFile( rndr, "media::texture/rabit.png" ) );
        tex1.attach( loadTextureFromFile( rndr, "media::texture/earth.jpg" ) );
        if( 0 == tex0 || 0 == tex1 ) return false;

        // create box mesh
        float edge = 200.0f;
        BoxVert vertices[24];
        UInt16  indices[36];
        createBox(
            edge, edge, edge,
            &vertices[0].x, sizeof(BoxVert),
            &vertices[0].u, sizeof(BoxVert),
            &vertices[0].nx, sizeof(BoxVert),
            0, 0, // tangent
            0, 0, // binormal
            indices, 0 );
        MeshDesc md;
        md.vtxfmt      = VertexFormat::XYZ_NORM_UV();
        md.prim        = PrimitiveType::TRIANGLE_LIST;
        md.numvtx      = 24;
        md.numidx      = 36;
        md.vertices[0] = vertices;
        md.indices     = indices;
        Mesh boxmesh( rndr );
        if( !boxmesh.init( md ) ) return false;

        // setup transformation matrices
        view.lookAtRh( Vector3f(200,200,200), Vector3f(0,0,0), Vector3f(0,1,0) );
        rndr.composePerspectiveMatrix( proj, 1.0f, 4.0f/3.0f, 80.0f, 600.0f );

        // initialize the effect
        if( !effect.init( rndr ) ) return false;
        effect.setMesh( boxmesh );
        effect.setLightPos( Vector4f(200,200,200,1) ); // light is at eye position.
        effect.setAlbedoTexture( tex1 );

        return true;
    }

    void quit()
    {
        effect.quit();
        c0.clear();
        ds.clear();
        tex0.clear();
        tex1.clear();
        context.clear();
        sr.quit();
    }

    void drawBox( float speed )
    {
        float angle = speed * fmod( timer.getTimef(), GN_TWO_PI );

        Matrix44f world;
        world.rotateY( angle );

        effect.setTransformation( proj, view, world );

        effect.draw();
    }

    void drawToColorRenderTarget( Texture * tex )
    {
        context.colortargets.resize( 1 );
        context.colortargets[0].texture = c0;
        rndr.bindContext( context );
        rndr.clearScreen( Vector4f(0, 0, 1, 1 ) ); // clear to green
        sr.drawSingleTexturedSprite( tex, GN::gfx::SpriteRenderer::DEFAULT_OPTIONS, 0, 0, RT_WIDTH, RT_HEIGHT );
    }

    void drawToDepthTexture()
    {
        context.colortargets.clear();
        context.depthstencil.texture = ds;
        rndr.bindContext( context );
        rndr.clearScreen();

        RenderTargetTexture rtt;
        rtt.texture = ds;
        effect.setRenderTarget( NULL, &rtt );
        drawBox( 1.0f );
    }

    void drawToBothColorAndDepthTexture()
    {
        context.colortargets.resize( 1 );
        context.colortargets[0].texture = c0;
        context.depthstencil.texture = ds;
        rndr.bindContext( context );
        rndr.clearScreen( Vector4f(0, 0, 1, 1 ) ); // clear to green

        RenderTargetTexture c, d;
        c.texture = c0;
        d.texture = ds;
        effect.setRenderTarget( &c, &d );
        drawBox( -1.0f );
    }

    void drawToBackBuffer( Texture * tex, float x, float y )
    {
        context.colortargets.clear();
        context.depthstencil.clear();
        rndr.bindContext( context );
        sr.drawSingleTexturedSprite( tex, GN::gfx::SpriteRenderer::DEFAULT_OPTIONS, x, y, RT_WIDTH, RT_HEIGHT );
    }

    void render()
    {
        drawToColorRenderTarget( tex0 );
        drawToBackBuffer( c0, 0, 0 );

        drawToDepthTexture();
        drawToBackBuffer( ds, RT_WIDTH, 0 );

        drawToBothColorAndDepthTexture();
        drawToBackBuffer( c0, 0, RT_HEIGHT );
        drawToBackBuffer( ds, RT_WIDTH, RT_HEIGHT );
    }
};

int run( Renderer & rndr )
{
    RenderToTexture scene( rndr );

    if( !scene.init() ) return -1;

    bool gogogo = true;

    FpsCalculator fps;
    getLogger("GN.util.fps")->setLevel( Logger::VERBOSE ); // enable FPS logger

    while( gogogo )
    {
        rndr.processRenderWindowMessages( false );

        Input & in = gInput;

        in.processInputEvents();

        if( in.getKeyStatus( KeyCode::ESCAPE ).down )
        {
            gogogo = false;
        }

        rndr.clearScreen( Vector4f(0,0.5f,0.5f,1.0f) );
        scene.render();
        rndr.present();

        fps.onFrame();
    }

    return 0;
}

struct InputInitiator
{
    InputInitiator( Renderer & r )
    {
        initializeInputSystem( InputAPI::NATIVE );
        const DispDesc & dd = r.getDispDesc();
        gInput.attachToWindow( dd.displayHandle, dd.windowHandle );
    }

    ~InputInitiator()
    {
        shutdownInputSystem();
    }
};

int main( int argc, const char * argv[] )
{
    enableCRTMemoryCheck();

    CommandLineArguments cmdargs( argc, argv );
    switch( cmdargs.status )
    {
        case CommandLineArguments::SHOW_HELP:
            cmdargs.showDefaultHelp();
            return 0;

        case CommandLineArguments::INVALID_COMMAND_LINE:
            return -1;

        case CommandLineArguments::CONTINUE_EXECUTION:
            // do nothing
            break;

        default:
            GN_UNEXPECTED();
            return -1;
    }

    cmdargs.rendererOptions.windowedWidth = (UInt32)RT_WIDTH * 2;
    cmdargs.rendererOptions.windowedHeight = (UInt32)RT_HEIGHT * 2;

    Renderer * r;
    if( cmdargs.useMultiThreadRenderer )
        r = createMultiThreadRenderer( cmdargs.rendererOptions );
    else
        r = createSingleThreadRenderer( cmdargs.rendererOptions );
    if( NULL == r ) return -1;

    InputInitiator ii(*r);

    int result = run( *r );

    deleteRenderer( r );

    return result;
}
