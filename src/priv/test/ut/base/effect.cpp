#include "../testCommon.h"
#include "garnet/GNengine.h"
#include "garnet/GNscene.h"

class EffectTest : public CxxTest::TestSuite
{
    GN::engine::RenderEngine re;
    GN::engine::EntityManager em;

public:

    void setUp()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;
        //TS_ASSERT( createRenderer( API_FAKE ) );
        //RendererOptions ro;
        //gRenderer.changeOptions( ro );
    }

    void tearDown()
    {
        //GN::gfx::deleteRenderer();
    }

    void testCondExp()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        // empty condition
        EffectDesc::CondExp emptyExp;
        TS_ASSERT( emptyExp.evaluate(re) );

        // comparasion
        EffectDesc::CondExp c0( (SInt32)0 );
        EffectDesc::CondExp c1( 1 );
        TS_ASSERT( (c0< c1).evaluate(re) );
        TS_ASSERT( (c0<=c1).evaluate(re) );
        TS_ASSERT( (c0==c0).evaluate(re) );
        TS_ASSERT( (c0!=c1).evaluate(re) );
        TS_ASSERT( (c1>=c0).evaluate(re) );
        TS_ASSERT( (c1> c0).evaluate(re) );

        // relation
        TS_ASSERT( !(c0&&c1).evaluate(re) );
        TS_ASSERT(  (c1&&c1).evaluate(re) );
        TS_ASSERT( !(c0||c0).evaluate(re) );
        TS_ASSERT(  (c0||c1).evaluate(re) );

        // complex
        EffectDesc::CondExp c2(2);
        EffectDesc::CondExp c3(2);
        TS_ASSERT( ((c0==c1)||(c2==c3)||(c1==c3)).evaluate(re) );
    }

    void initDesc1( GN::engine::EffectDesc & desc )
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        desc.clear();

        // create uniforms
        desc.uniforms["color"].hasDefaultValue = true;
        desc.uniforms["color"].defaultValue.setV( Vector4f(1,0,0,1) );

        desc.uniforms["pvw"].hasDefaultValue = true;
        desc.uniforms["pvw"].defaultValue.setM(
            Matrix44f( 2,  0,  0, -1,
                       0, -2,  0,  1,
                       0,  0,  1,  0,
                       0,  0,  0,  1 ) );

        // create textures
        desc.textures["t0"];
        desc.textures["t1"];

        // create 1 vertex shader
        EffectDesc::ShaderDesc & vs0 = desc.shaders["vs0"];
        vs0.code =
            "vs.1.1\n "
            "dcl_position v0 \n"
            "dcl_texcoord v1 \n"
            "m4x4 oPos, v0, c0 \n"
            "mov oT0, v1 \n"
            "mov oD0, c4";
        vs0.type = SHADER_VS;
        vs0.lang = LANG_D3D_ASM;
        vs0.uniforms["c0"] = "pvw";
        vs0.uniforms["c4"] = "color";

        // create another vertex shader (FFP shader)
        EffectDesc::ShaderDesc & vs1 = desc.shaders["vs1"];
        vs1.type = SHADER_VS;
        vs1.uniforms["FFP_TRANSFORM_VIEW"] = "pvw";

        // create 1 pixel shader
        EffectDesc::ShaderDesc & ps0 = desc.shaders["ps0"];
        ps0.code =
            "ps.1.1 \n"
            "mov r0, c0";
        ps0.type = SHADER_PS;
        ps0.lang = LANG_D3D_ASM;
        ps0.uniforms["c0"] = "color";
        ps0.textures[0] = "t0";
        ps0.textures[1] = "t1";

        // create empty GS
        EffectDesc::ShaderDesc & gs0 = desc.shaders["gs0"];
        gs0.type = SHADER_GS;

        // create 1 technique
        EffectDesc::TechniqueDesc tech0;
        tech0.name = "t0";
        tech0.passes.resize(1);
        EffectDesc::PassDesc & p0 = tech0.passes[0];
        p0.shaders[SHADER_VS] = "vs0";
        p0.shaders[SHADER_PS] = "ps0";
        p0.shaders[SHADER_GS] = "gs0";
        desc.techniques.push_back( tech0 );

        // create another technique
        EffectDesc::TechniqueDesc tech1;
        tech1.name = "t1";
        tech1.passes.resize(1);
        EffectDesc::PassDesc & p1 = tech1.passes[0];
        p1.shaders[SHADER_VS] = "vs1";
        p1.shaders[SHADER_PS] = "ps0";
        p1.shaders[SHADER_GS] = "gs0";
        desc.techniques.push_back( tech1 );
    }

    void doDraw( const GN::engine::Effect & )
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        /*gRenderer.clearScreen();
        if( gRenderer.drawBegin() )
        {
            RendererContext ctx;
            ctx.clearToNull();
            for( size_t i = 0; i < e.getNumPasses(); ++i )
            {
                e.passBegin( ctx, i );
                e.commitChanges();
                gRenderer.setContext( ctx );
                scene::gQuadRenderer.drawSingleSolidQuad( 0xFFFFFFFF, 0 );
                e.passEnd();
            }
            gRenderer.drawEnd();
        }*/
    }

    void testEmptyTechniqueList()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.techniques.clear();

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidShaderType()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["vs0"].type = NUM_SHADER_TYPES;

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidShadingLang()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["vs0"].lang = NUM_SHADING_LANGUAGES;

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidTextureReference()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["ps0"].textures[0] = "haha";

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidTextureStage()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["ps0"].textures[MAX_TEXTURE_STAGES] = "t0";

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidUniformReference()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["vs0"].uniforms["c0"] = "haha";

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidUniformBinding()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["vs0"].uniforms["haha"] = "pvw";

        Effect e(re);
        TS_ASSERT( e.init( desc ) );

        doDraw( e );
    }

    void testInvalidPassRsb()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        EffectDesc::PassDesc & p0 = desc.techniques[0].passes[0];
        GN::RuntimeAssertBehavior rab = GN::setRuntimeAssertBehavior( GN::RAB_LOG_ONLY );
        p0.rsb.set( RS_ALPHA_TEST, NUM_RENDER_STATE_VALUES );
        GN::setRuntimeAssertBehavior( rab );

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidShaderReference()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        EffectDesc::PassDesc & p0 = desc.techniques[0].passes[0];
        p0.shaders[SHADER_VS] = "haha";

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidShaderReference2()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        EffectDesc::PassDesc & p0 = desc.techniques[0].passes[0];
        p0.shaders[SHADER_VS] = "ps0";

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidTechniqueRsb()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        GN::RuntimeAssertBehavior rab = GN::setRuntimeAssertBehavior( GN::RAB_LOG_ONLY );
        desc.techniques[0].rsb.set( RS_ALPHA_TEST, NUM_RENDER_STATE_VALUES );
        GN::setRuntimeAssertBehavior( rab );

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidGlobalRsb()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        GN::RuntimeAssertBehavior rab = GN::setRuntimeAssertBehavior( GN::RAB_LOG_ONLY );
        desc.rsb.set( RS_ALPHA_TEST, NUM_RENDER_STATE_VALUES );
        GN::setRuntimeAssertBehavior( rab );

        Effect e(re);
        TS_ASSERT( !e.init( desc ) );
    }

    void testInvalidShaderCode()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        desc.shaders["vs0"].code = "haha";

        Effect e(re);
        TS_ASSERT( e.init( desc ) );

        doDraw( e );
    }

    void testDesc1()
    {
        using namespace GN;
        using namespace GN::gfx;
        using namespace GN::engine;

        EffectDesc desc;
        initDesc1( desc );

        Effect e(re);
        TS_ASSERT( e.init( desc ) );

        doDraw( e );
    }

    // TODO: same shader, same uniform, multiple bindings.

};
