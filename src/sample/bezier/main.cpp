#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::input;
using namespace GN::engine;
using namespace GN::util;

struct BezierVertex
{
    Vector3f p0, p1, p2;
    Vector3f n0, n1, n2;
    Vector2f t0, t1, t2;
    Vector2f bc;

    BezierVertex( float u, float v )
    {
        p0.set( -1, -1, 0 ); p1.set(  1, -1, 0 ); p2.set(    0,  1, 0 );
        n0.set( -1, -1, 1 ); n1.set(  1, -1, 1 ); n2.set(    0,  1, 1 );
        t0.set(  0,  1 );    t1.set(  1,  1 );    t2.set( 0.5f,  0 );
        bc.set(  u,  v );

        n0.normalize();
        n1.normalize();
        n2.normalize();
    }
};

static GN::Logger * sLogger = GN::getLogger("GN.sample.Bezier");

MeshResource * createMesh( GpuResourceDatabase & gdb )
{
    MeshResourceDesc md;

    md.vtxfmt.numElements = 7;
    md.vtxfmt.elements[0].stream = 0;
    md.vtxfmt.elements[0].format = ColorFormat::FLOAT3;
    md.vtxfmt.elements[0].offset = GN_FIELD_OFFSET( BezierVertex, p0 );
    md.vtxfmt.elements[0].setSemantic( "pos0" );
    md.vtxfmt.elements[1].stream = 0;
    md.vtxfmt.elements[1].format = ColorFormat::FLOAT3;
    md.vtxfmt.elements[1].offset = GN_FIELD_OFFSET( BezierVertex, p1 );
    md.vtxfmt.elements[1].setSemantic( "pos1" );
    md.vtxfmt.elements[2].stream = 0;
    md.vtxfmt.elements[2].format = ColorFormat::FLOAT3;
    md.vtxfmt.elements[2].offset = GN_FIELD_OFFSET( BezierVertex, p2 );
    md.vtxfmt.elements[2].setSemantic( "pos2" );
    md.vtxfmt.elements[3].stream = 0;
    md.vtxfmt.elements[3].format = ColorFormat::FLOAT3;
    md.vtxfmt.elements[3].offset = GN_FIELD_OFFSET( BezierVertex, n0 );
    md.vtxfmt.elements[3].setSemantic( "normal0" );
    md.vtxfmt.elements[4].stream = 0;
    md.vtxfmt.elements[4].format = ColorFormat::FLOAT3;
    md.vtxfmt.elements[4].offset = GN_FIELD_OFFSET( BezierVertex, n1 );
    md.vtxfmt.elements[4].setSemantic( "normal1" );
    md.vtxfmt.elements[5].stream = 0;
    md.vtxfmt.elements[5].format = ColorFormat::FLOAT3;
    md.vtxfmt.elements[5].offset = GN_FIELD_OFFSET( BezierVertex, n2 );
    md.vtxfmt.elements[5].setSemantic( "normal2" );
    md.vtxfmt.elements[6].stream = 0;
    md.vtxfmt.elements[6].format = ColorFormat::FLOAT2;
    md.vtxfmt.elements[6].offset = GN_FIELD_OFFSET( BezierVertex, bc );
    md.vtxfmt.elements[6].setSemantic( "bc" );

    //                               v0 (0,0)                        //
    //                              / \                              //
    //                             /   \                             //
    //                            /     \                            //
    //                           /       \                           //
    //                          /         \                          //
    //                 (1/3,0) v1----------v2 (0,1/3)                //
    //                        / \         / \                        //
    //                       /   \       /   \                       //
    //                      /     \     /     \                      //
    //                     /       \   /       \                     //
    //                    /         \ /         \                    //
    //           (2/3,0) v3----------v4----------v5 (0,2/3)          //
    //                  / \         / \(1/3,1/3)/ \                  //
    //                 /   \       /   \       /   \                 //
    //                /     \     /     \     /     \                //
    //               /       \   /       \   /       \               //
    //              /         \ /         \ /         \              //
    //             v6----------v7----------v8---------v9             //
    //           (1,0)     (2/3,1/3)   (1/3,2/3)     (0,1)           //

    float a = 1.0f / 3.0f;
    float b = 2.0f / 3.0f;
    BezierVertex vertices[] =
    {
        BezierVertex( 0, 0 ), // 0
        BezierVertex( a, 0 ), // 1
        BezierVertex( 0, a ), // 2
        BezierVertex( b, 0 ), // 3
        BezierVertex( a, a ), // 4
        BezierVertex( 0, b ), // 5
        BezierVertex( 1, 0 ), // 6
        BezierVertex( b, a ), // 7
        BezierVertex( a, b ), // 8
        BezierVertex( 0, 1 ), // 9
    };
    md.numvtx = GN_ARRAY_COUNT( vertices );
    md.vertices[0] = vertices;
    md.strides[0] = sizeof(BezierVertex);

    uint16 indices[] =
    {
        0, 1, 2,
        1, 3, 4,
        1, 4, 2,
        2, 4, 5,
        3, 6, 7,
        3, 7, 4,
        4, 7, 8,
        4, 8, 5,
        5, 8, 9,
    };
    md.numidx = GN_ARRAY_COUNT( indices );
    md.indices = indices;
    md.prim = PrimitiveType::TRIANGLE_LIST;

    AutoRef<MeshResource> mesh( gdb.createResource<MeshResource>(NULL) );
    if( !mesh || !mesh->reset(&md) ) return 0;

    return mesh.detach();
}

EffectResource *
createEffect( GpuResourceDatabase & gdb )
{
    const char * glslvscode =

        // define bezier control point structure
        "struct BezierControlPoints \n"
        "{                          \n"
        "   vec3 p300;              \n"
        "   vec3 p030;              \n"
        "   vec3 p003;              \n"
        "   vec3 p210;              \n"
        "   vec3 p120;              \n"
        "   vec3 p201;              \n"
        "   vec3 p102;              \n"
        "   vec3 p012;              \n"
        "   vec3 p021;              \n"
        "   vec3 p111;              \n"
        "                           \n"
        "   vec3 n200;              \n"
        "   vec3 n020;              \n"
        "   vec3 n002;              \n"
        "};                         \n"

        // utils to calculate bezier control points
        "BezierControlPoints                                                                     \n"
        "calcBezierControlPoints( vec3 p0, vec3 p1, vec3 p2, vec3 n0, vec3 n1, vec3 n2 )         \n"
        "{                                                                                       \n"
        "	BezierControlPoints cp;                                                              \n"
        "                                                                                        \n"
        "	cp.p300 = p0;                                                                        \n"
        "	cp.p030 = p1;                                                                        \n"
        "	cp.p003 = p2;                                                                        \n"
        "	cp.n200 = n0;                                                                        \n"
        "	cp.n020 = n1;                                                                        \n"
        "	cp.n002 = n2;                                                                        \n"
        "	cp.p210 = ( 2 * cp.p300 + cp.p030 - dot(cp.n200, (cp.p030 - cp.p300)) * cp.n200 ) / 3; \n"
        "	cp.p120 = ( 2 * cp.p030 + cp.p300 - dot(cp.n020, (cp.p300 - cp.p030)) * cp.n020 ) / 3; \n"
        "	cp.p201 = ( 2 * cp.p300 + cp.p003 - dot(cp.n200, (cp.p003 - cp.p300)) * cp.n200 ) / 3; \n"
        "	cp.p102 = ( 2 * cp.p003 + cp.p300 - dot(cp.n002, (cp.p300 - cp.p003)) * cp.n002 ) / 3; \n"
        "	cp.p012 = ( 2 * cp.p003 + cp.p030 - dot(cp.n002, (cp.p030 - cp.p003)) * cp.n002 ) / 3; \n"
        "	cp.p021 = ( 2 * cp.p030 + cp.p003 - dot(cp.n020, (cp.p003 - cp.p030)) * cp.n020 ) / 3; \n"
        "	cp.p111 = ( cp.p210 + cp.p120 + cp.p102 + cp.p201 + cp.p021 + cp.p012 ) / 4 - ( cp.p300 + cp.p030 + cp.p003 ) / 6;  \n"
        "                                                                                        \n"
        "	return cp;                                                                           \n"
        "}                                                                                       \n"

        // type and function to calcuatle single bezier vertex
        "struct BezierVertex                                             \n"
        "{                                                               \n"
        "	vec3 position;                                               \n"
        "	vec3 normal;                                                 \n"
        "};                                                              \n"
        "                                                                \n"
        "BezierVertex                                                    \n"
        "calcBezierVertex(                                               \n"
        "	BezierControlPoints cp,                                      \n"
        "	float u, float v )                                           \n"
        "{                                                               \n"
        "	float u2 = u * u;                                            \n"
        "	float u3 = u2 * u;                                           \n"
        "	float v2 = v * v;                                            \n"
        "	float v3 = v2 * v;                                           \n"
        "	float w  = 1.0 - u - v;                                      \n"
        "	float w2 = w * w;                                            \n"
        "	float w3 = w2 * w;                                           \n"
        "	                                                             \n"
        "	BezierVertex vert;                                           \n"
        "                                                                \n"
        "	vert.position =                                              \n"
        "		u3 * cp.p300 + v3 * cp.p030 + w3 * cp.p003 +             \n"
        "		3.0 * ( u2 * v * cp.p210 + u2 * w * cp.p201 + u * v2 * cp.p120 + v2 * w * cp.p021 + v * w2 * cp.p012 + u * w2 * cp.p102 ) + \n"
        "		6.0 * u * v * w * cp.p111;                               \n"
        "                                                                \n"
        "	vert.normal = cp.n200 * u + cp.n020 * v + cp.n002 * w;       \n"
        "	                                                             \n"
        "	return vert;                                                 \n"
        "}                                                               \n"

        // shader parameters
        "uniform   mat4 pvw; \n"
        "uniform   mat4 world; \n"
        "uniform   mat4 wit; \n"
        "in vec3 pos1; \n"
        "in vec3 pos2; \n"
        "in vec3 nml1; \n"
        "in vec3 nml2; \n"
        "in vec2 bc; \n"
        "varying   vec4 pos_world; // vertex position in world space \n"
        "varying   vec3 nml_world; // vertex normal in world space \n"
        "varying   vec2 texcoords; \n"

        // main function
        "void main() { \n"

        // calculate control points
        "   vec3 pos0 = gl_Vertex.xyz; \n"
        "   vec3 nml0 = gl_Normal.xyz; \n"
        "   BezierControlPoints cp = calcBezierControlPoints( pos0, pos1, pos2, nml0, nml1, nml2 ); \n"

        // calculate output vertex
        "   float u  = bc.x; \n"
        "   float v  = bc.y; \n"
        "   BezierVertex vert = calcBezierVertex( cp, u, v ); \n"

        // test code
        //"   vert.position = pos0 * (1-u-v) + pos1 * u + pos2 * v; \n"
        //"   vert.normal   = nml0 * (1-u-v) + nml1 * u + nml2 * v; \n"

        // translate position
        "   gl_Position = pvw * vec4(vert.position,1); \n"
        "   pos_world   = world * vec4(vert.position,1); \n"

        // translate normal
        "   nml_world = (wit * vec4(vert.normal,0)).xyz; \n"

        // texcoord
        "   vec2 tc0 = vec2(0.0, 0.0); \n"
        "   vec2 tc1 = vec2(1.0, 0.0); \n"
        "   vec2 tc2 = vec2(1.0, 1.0); \n"
        "   texcoords = tc0 * (1-u-v) + tc1 * u + tc2 * v; \n"

        // end of vertex shader
        "}";

    const char * glslpscode =
        "uniform vec4 lightpos; // light positin in world space \n"
        "uniform sampler2D t0; \n"
        "varying vec4 pos_world; // position in world space \n"
        "varying vec3 nml_world; // normal in world space \n"
        "varying vec2 texcoords; \n"
        "void main() { \n"
        "   vec3  L      = normalize( (lightpos - pos_world).xyz ); \n"
        "   vec3  N      = normalize( nml_world ); \n"
        "   float diff   = clamp( dot( L, N ), 0.0, 1.0 ); \n"
        "   vec4  tex    = texture2D( t0, texcoords ); \n"
        "   gl_FragColor = diff * tex; \n"
        "}";

    EffectResourceDesc ed;
    ed.uniforms["MATRIX_PVW"];
    ed.uniforms["MATRIX_WORLD"];
    ed.uniforms["MATRIX_WORLD_IT"]; // used to translate normal from local space into world space
    ed.uniforms["LIGHT0_POSITION"];
    ed.textures["DIFFUSE_TEXTURE"]; // create a texture parameter named "DIFFUSE_TEXTURE"
    ed.gpuprograms["glsl"].gpd.lang = GpuProgramLanguage::GLSL;
    ed.gpuprograms["glsl"].gpd.shaderModels = ShaderModel::GLSL_1_10;
    ed.gpuprograms["glsl"].gpd.vs.source = glslvscode;
    ed.gpuprograms["glsl"].gpd.ps.source = glslpscode;
    ed.gpuprograms["glsl"].uniforms["pvw"] = "MATRIX_PVW";
    ed.gpuprograms["glsl"].uniforms["world"] = "MATRIX_WORLD";
    ed.gpuprograms["glsl"].uniforms["wit"] = "MATRIX_WORLD_IT";
    ed.gpuprograms["glsl"].uniforms["lightpos"] = "LIGHT0_POSITION";
    ed.gpuprograms["glsl"].textures["t0"] = "DIFFUSE_TEXTURE";
    ed.techniques.resize( 1 );
    ed.techniques[0].passes.resize( 1 );
    ed.techniques[0].passes[0].gpuprogram = "glsl";

    AutoRef<EffectResource> e = gdb.createResource<EffectResource>(NULL);
    if( !e || !e->reset( &ed ) ) return NULL;

    return e.detach();
}

class BezierApp : public SampleApp
{
    struct Camera
    {
        Matrix44f proj;
        Matrix44f view;
    };

    ArcBall                arcball; // arcball camera
    float                  radius;  // distance from camera to object
    Camera                 camera;
    AutoObjPtr<StaticMesh> bezier;

    void updateRadius()
    {
        Gpu            & gpu = *engine::getGpu();
        const DispDesc & dd  = gpu.getDispDesc();

        camera.view.lookAtRh( Vector3f(0,0,radius), Vector3f(0,0,0), Vector3f(0,1,0) );
        gpu.composePerspectiveMatrixRh( camera.proj, GN_PI/4.0f, (float)dd.width/dd.height, radius / 100.0f, radius * 2.0f );

        float h = tan( 0.5f ) * radius * 2.0f;
        arcball.setMouseMoveWindow( 0, 0, (int)dd.width, (int)dd.height );
        arcball.setViewMatrix( camera.view );
        arcball.setTranslationSpeed( h / dd.height );

        // setup light as a head light that is in same location as camera.
        Vector4f lightPos(0,0,radius,1);
        engine::getGdb()->setStandardUniform( StandardUniform::Index::LIGHT0_POSITION, lightPos );
    }

    bool onInit()
    {
        GpuResourceDatabase & gdb = *engine::getGdb();

        AutoRef<MeshResource>   mesh;
        AutoRef<EffectResource> effect;
        AutoRef<ModelResource>  model;

        // initialize effect
        effect.attach( createEffect( gdb ) );
        if( !effect ) return false;

        // load meshes
        mesh.attach( createMesh( gdb ) );
        if( !mesh ) return false;

        // initialize model
        model = gdb.createResource<ModelResource>(NULL);
        model->setEffectResource( effect );
        model->setMeshResource( mesh );
        model->setTextureResource( "DIFFUSE_TEXTURE", TextureResource::loadFromFile( gdb, "media::texture/earth.jpg" ) );

        // create entity
        bezier.attach( new StaticMesh() );
        bezier->visual().addModel( model );

        // initialize arcball
        arcball.setHandness( util::RIGHT_HAND );
        arcball.connectToInput();

        // update radius related stuff
        radius = 3.0f;
        updateRadius();

        // success
        return true;
    }

    void onQuit()
    {
        bezier.clear();
    }

    void onRenderWindowResize( intptr_t, uint32 width, uint32 height )
    {
        arcball.setMouseMoveWindow( 0, 0, (int)width, (int)height );
    }

    void onAxisMove( Axis a, int d )
    {
        if( Axis::MOUSE_WHEEL_0 == a )
        {
            float speed = radius / 100.0f;
            radius -= speed * d;
            if( radius < 0.1f ) radius = 0.1f;
            updateRadius();
        }
    }

    void onUpdate()
    {
        SpacialComponent * spacial = bezier->getComponent<SpacialComponent>();
        const Vector3f & position = arcball.getTranslation();
        spacial->setPosition( position );
        spacial->setRotation( arcball.getRotation() );
    }

    void onRender()
    {
        engine::getGpu()->clearScreen( Vector4f(0,0.5f,0.5f,1.0f) );

        const Vector3f & position = arcball.getTranslation();

        bezier->visual().draw( camera.proj, camera.view, &bezier->spacial() );

        drawCoords();

        engine::getDefaultFontRenderer()->drawText(
            str::format(
                L"position : %f, %f, %f\n"
                L"radius   : %f",
                position.x, position.y, position.z,
                radius ).rawptr(),
            0, 320 );
    }

    void drawCoords()
    {
        static const float X[] = { 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f, 0.0f };
        static const float Y[] = { 0.0f, 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f };
        static const float Z[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10000.0f };

        const Matrix44f & world = arcball.getRotationMatrix44();
        const Matrix44f & view  = camera.view;
        const Matrix44f & proj  = camera.proj;
        auto pvw = proj * view * world;

        auto lr = engine::getLineRenderer();
        lr->batchingBegin();
        lr->drawLines(X, 3*sizeof(float), 2, GN_RGBA8(255,0,0,255), pvw);
        lr->drawLines(Y, 3*sizeof(float), 2, GN_RGBA8(0,255,0,255), pvw);
        lr->drawLines(Z, 3*sizeof(float), 2, GN_RGBA8(0,0,255,255), pvw);
        lr->batchingEnd();
    }
};

int main( int argc, const char * argv[] )
{
    printf( "\nBezier triangle sample.\n" );

    BezierApp app;

    return app.run( argc, argv );
}

