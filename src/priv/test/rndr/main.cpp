#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::input;
using namespace GN::win;
using namespace GN::util;

RendererContext rc;

const char * hlsl_vscode =
    "uniform float4x4 transform; \n"
    "struct VSOUT { float4 pos : POSITION0; float2 uv : TEXCOORD; }; \n"
    "VSOUT main( in float4 pos : POSITION ) { \n"
    "   VSOUT o; \n"
    "   o.pos = mul( transform, pos ); \n"
    "   o.uv  = pos.xy; \n"
    "   return o; \n"
    "}";

const char * hlsl_pscode =
    "sampler t0; \n"
    "struct VSOUT { float4 pos : POSITION0; float2 uv : TEXCOORD; }; \n"
    "float4 main( in VSOUT i ) : COLOR0 { \n"
    "   return tex2D( t0, i.uv ); \n"
    "}";

const char * glsl_vscode =
    "varying vec2 texcoords; \n"
    "uniform mat4 transform; \n"
    "void main() { \n"
    "   gl_Position = transform * gl_Vertex; \n"
    "   texcoords.xy = gl_Vertex.xy; \n"
    "}";

const char * glsl_pscode =
    "uniform sampler2D t0; \n"
    "varying vec2 texcoords; \n"
    "void main() { \n"
    "   gl_FragColor = texture2D( t0, texcoords ); \n"
    "}";

bool init( Renderer & rndr )
{
    rc.clear();

    // create GPU program
    GpuProgramDesc gpd;
    if( RendererAPI::OGL == rndr.getOptions().api )
    {
        gpd.lang = GpuProgramLanguage::GLSL;
        gpd.vs.source = glsl_vscode;
        gpd.ps.source = glsl_pscode;
    }
    else
    {
        gpd.lang = GpuProgramLanguage::HLSL9;
        gpd.vs.source = hlsl_vscode;
        gpd.ps.source = hlsl_pscode;
        gpd.vs.entry  = "main";
        gpd.ps.entry  = "main";
    }
    rc.gpuProgram.attach( rndr.createGpuProgram( gpd ) );
    if( !rc.gpuProgram ) return false;

    // create uniform
    rc.uniforms.resize( 1 );
    rc.uniforms[0].attach( rndr.createUniform( sizeof(Matrix44f) ) );
    if( !rc.uniforms[0] ) return false;

    // setup vertex format
    rc.vtxfmt.numElements = 1;
    rc.vtxfmt.elements[0].bindTo( "position", 0 );
    rc.vtxfmt.elements[0].format = ColorFormat::FLOAT4;
    rc.vtxfmt.elements[0].offset = 0;
    rc.vtxfmt.elements[0].stream = 0;

    // create texture
    rc.textures[0].texture.attach( loadTextureFromFile( rndr, "media::texture\\earth.jpg" ) );

    // create vertex buffer
    static float vertices[] =
    {
        0,0,0,1,
        1,0,0,1,
        1,1,0,1,
        0,1,0,1,
    };
    VtxBufDesc vbd = {
        sizeof(vertices),
        false,
    };
    rc.vtxbufs[0].vtxbuf.attach( rndr.createVtxBuf( vbd ) );
    if( NULL == rc.vtxbufs[0].vtxbuf ) return false;
    rc.vtxbufs[0].vtxbuf->update( 0, 0, vertices );

    // create index buffer
    UInt16 indices[] = { 0, 1, 3, 2 };
    IdxBufDesc ibd = { 4, false, false };
    rc.idxbuf.attach( rndr.createIdxBuf( ibd ) );
    if( !rc.idxbuf ) return false;
    rc.idxbuf->update( 0, 0, indices );

    return true;
}

void quit( Renderer & )
{
    rc.clear();
}

void draw( Renderer & r )
{
    Matrix44f m;

    // DRAW_UP: triangle at left top corner
    static float vertices[] =
    {
        0,0,0,1,
        1,0,0,1,
        1,1,0,1,
        0,1,0,1,
    };
    m.translate( -1.0f, -0.0f, 0 );
    rc.uniforms[0]->update( m );
    r.bindContext( rc );
    r.drawUp( PrimitiveType::TRIANGLE_LIST, 3, vertices, 4*sizeof(float) );

    // DRAW_INDEXED_UP : triangle at left bottom
    static UInt16 indices[] = { 0, 1, 3 };
    m.translate( -1.0f, -1.0f, 0 );
    rc.uniforms[0]->update( m );
    r.bindContext( rc );
    r.drawIndexedUp( PrimitiveType::TRIANGLE_STRIP, 3, 4, vertices, 4*sizeof(float), indices );

    // DRAW: triangle at right top corner
    m.identity();
    rc.uniforms[0]->update( m );
    r.bindContext( rc );
    r.draw( PrimitiveType::TRIANGLE_LIST, 3, 0 );

    // DRAW_INDEXED : quad at right bottom corner
    m.translate( 0.5f, -1.5f, 0 );
    rc.uniforms[0]->update( m );
    r.bindContext( rc );
    r.drawIndexed( PrimitiveType::TRIANGLE_STRIP, 4, 0, 0, 4, 0 );
}

int run( Renderer & rndr )
{
    if( !init( rndr ) ) { quit( rndr ); return -1; }

    bool gogogo = true;

    FpsCalculator fps;
    getLogger("GN.util.fps")->setLevel( Logger::LL_VERBOSE ); // enable FPS logger

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
        draw( rndr );
        rndr.present();

        fps.onFrame();
    }

    quit( rndr );

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
