#include "pch.h"
#include "garnet/gfx/effect.h"

using namespace GN;
using namespace GN::gfx;

class Scene
{
    AutoRef<Shader> ps1, ps2;

    uint32_t tex0;

    effect::Effect eff0;

    Matrix44f world, view, proj;

    bool loadEffect()
    {
        GN_GUARD;

        // create effect 0
        {
            effect::EffectDesc desc;
            effect::ShaderDesc & vs = desc.shaders["vs"];
            effect::ShaderDesc & ps = desc.shaders["ps"];
            vs.type = VERTEX_SHADER;
            ps.type = PIXEL_SHADER;
            effect::TechniqueDesc & tech = desc.techniques["ffp"];
            tech.passes.resize(1);
            effect::PassDesc & pass0 = tech.passes[0];
            pass0.shaders[VERTEX_SHADER] = "vs";
            pass0.shaders[PIXEL_SHADER] = "ps";
            if( !eff0.init( desc ) ) return false;
        }

        // success
        return true;

        GN_UNGUARD;
    }

public:

    Scene() {}

    ~Scene() { quit(); }

    bool init()
    {
        Renderer & r = gRenderer;

        // create pixel shaders
        if( r.supportShader( PIXEL_SHADER, LANG_D3D_ASM ) )
        {
            static const char * code =
                "ps_1_1\n"
                "mov r0, c0";
            ps1.attach( r.createPxlShader( LANG_D3D_ASM, code ) );
            if( !ps1 ) return false;
            ps1->setUniformByNameV( "c0", Vector4f(0,1,0,1) );
        }
        else if( r.supportShader( PIXEL_SHADER, LANG_OGL_ARB ) )
        {
            static const char * code =
                "!!ARBfp1.0 \n"
                "PARAM white = program.local[0]; \n"
                "OUTPUT oClr = result.color; \n"
                "MOV oClr, white; \n"
                "END";
            ps1.attach( r.createPxlShader( LANG_OGL_ARB, code ) );
            if( !ps1 ) return false;
            ps1->setUniformByNameV( "l0", Vector4f(0,1,0,1) );
        }
        if( r.supportShader( PIXEL_SHADER, LANG_D3D_HLSL ) )
        {
            static const char * code =
                "float4 diffuse; \n"
                "float4 psMain() : COLOR0 \n"
                "{ \n"
                "   return diffuse; \n"
                "} \n";
            ps2.attach( r.createPxlShader( LANG_D3D_HLSL, code, "psMain" ) );
            if( !ps2 ) return false;
            ps2->setUniformByNameV( "diffuse", Vector4f(1,0,0,1) );
        }
        else if( r.supportShader( PIXEL_SHADER, LANG_OGL_GLSL ) )
        {
            static const char * code =
                "uniform vec4 diffuse; \n"
                "void main() \n"
                "{ \n"
                "   gl_FragColor = diffuse; \n"
                "} \n";
            ps2.attach( r.createPxlShader( LANG_OGL_GLSL, code ) );
            if( !ps2 ) return false;
            ps2->setUniformByNameV( "diffuse", Vector4f(1,0,0,1) );
        }

        // get texture handle
        tex0 = gTexDict.getResourceHandle( "texture/rabit.png" );

        // create the effect
        if( !loadEffect() ) return false;

        // initialize matrices
        world.identify();
        view.lookAtRh( Vector3f(200,200,200), Vector3f(0,0,0), Vector3f(0,1,0) );
        gRenderer.composePerspectiveMatrix( proj, 1.0f, 4.0f/3.0f, 1.0f, 1000.0f );

        // success
        return true;
    }

    void quit()
    {
        eff0.quit();
        ps1.clear();
		ps2.clear();
    }

    void update()
    {
        // update world matrix
        static float angle = 0.0f;
        angle += deg2rad(0.2f);
        world.rotateY( angle );
    }

    void draw()
    {
        Renderer & r = gRenderer;

        r.bindTexture( 0, gTexDict.getResource(tex0) );
        r.draw2DTexturedQuad( 0, 0, 0, 0.5, 0.5 );
        if( ps1 )
        {
            r.bindShaders( 0, ps1 );
            r.draw2DTexturedQuad( DQ_USE_CURRENT_PS, 0.5, 0.0, 1.0, 0.5 );
        }
        if( ps2 )
        {
            r.bindShaders( 0, ps2 );
            r.draw2DTexturedQuad( DQ_USE_CURRENT_PS, 0.0, 0.5, 0.5, 1.0 );
        }

        // the a line box
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
                ubyte4ToRGBA32(255,255,0,255),
                world, view, proj );
        }
    }
};

//!
//! GFX module test application
//!
class GfxTestApp : public app::SampleApp
{
    Scene * mScene;
public:

    GfxTestApp() : mScene(0) {}

    bool onRendererCreate()
    {
        mScene = new Scene;
        return mScene->init();
    }

    void onRendererDestroy()
    {
        safeDelete(mScene);
    }

    void onKeyPress( input::KeyEvent ke )
    {
        app::SampleApp::onKeyPress( ke );
        if( input::KEY_RETURN == ke.code && ke.status.down && ke.status.altDown() )
        {
            // toggle fullscreen mode
            RendererOptions ro = gRenderer.getOptions();
            ro.fullscreen = !ro.fullscreen;
            if( !gRenderer.changeOptions(ro) ) postExistEvent();
        }
    }

    void onUpdate()
    {
        mScene->update();
    }

    void onRender()
    {
        Renderer & r = gRenderer;
        
        r.clearScreen( Vector4f(0,0,0,1) ); // clear to pure blue.

        // draw scene
        GN_ASSERT( mScene );
        mScene->draw();

        // draw mouse position on screen
        StrA mousePos;
        int x = 1, y = 1;
        gInput.getMousePosition( x, y );
        mousePos.format( "Mouse: %d, %d", x, y );
        r.drawDebugTextA( mousePos.cstr(), 0, 100 );//*/
    }
};

int main( int argc, const char * argv[] )
{
    GfxTestApp app;
    return app.run( argc, argv );
}
