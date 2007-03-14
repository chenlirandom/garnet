#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::scene;

class Scene
{
    GN::app::SampleApp & mApp;
    
    AutoRef<Texture> mColor;
    AutoRef<Texture> mDepth;

    struct BoxVert
    {
        float x, y, z, nx, ny, nz, u, v;
    };

    VtxFmtHandle mDecl;

    BoxVert mBoxVerts[24];
    UInt16 mBoxIndices[36];

    util::ArcBall mArcBall;

    Matrix44f mModel, mView, mProj;

    ResourceId mTex0;

    AutoRef<Shader> mVs, mPs;

public:

    Scene( GN::app::SampleApp & app ) : mApp(app) {}

    ~Scene() { destroy(); }

    bool create()
    {
        Renderer & r = gRenderer;

        UInt32 w = 1024;
        UInt32 h = 1024;

        // create color texture
        mColor.attach( r.create2DTexture( w, h, 1, FMT_DEFAULT, TEXUSAGE_RENDER_TARGET ) );
        if( mColor.empty() ) return false;

        // create depth texture
        mDepth.attach( r.create2DTexture( w, h, 1, FMT_DEFAULT, TEXUSAGE_DEPTH ) );
        if( mDepth.empty() ) return false;

        // create texture
        mTex0 = gSceneResMgr.getResourceId( "media::/texture/rabit.png" );

        // create decl
        mDecl = r.createVtxFmt( VtxFmtDesc::XYZ_NORM_UV );
        if( 0 == mDecl ) return false;

        // create box
        float edge = 200.0f;
        createBox(
            edge, edge, edge,
            &mBoxVerts[0].x, sizeof(BoxVert),
            &mBoxVerts[0].u, sizeof(BoxVert),
            &mBoxVerts[0].nx, sizeof(BoxVert),
            mBoxIndices, 0 );

        // initialize matrices
        mModel.identity();
        mView.lookAtRh( Vector3f(200,200,200), Vector3f(0,0,0), Vector3f(0,1,0) );
        r.composePerspectiveMatrix( mProj, 1.0f, 4.0f/3.0f, 80.0f, 600.0f );

        // initialize arcball
        mArcBall.setMouseMoveWindow( 0, 0, r.getDispDesc().width, r.getDispDesc().height );
        mArcBall.setViewMatrix( mView );

        // try create shaders
        if( r.supportShader( "vs_1_1" ) )
        {
            mVs.attach( r.createShaderFromFile( SHADER_VS, LANG_D3D_HLSL, "media::depthTexture/d3dVs.txt" ) );
            if( mVs.empty() ) return false;
        }
        if( r.supportShader( "ps_1_1" ) )
        {
            mPs.attach( r.createShaderFromFile( SHADER_PS, LANG_D3D_HLSL, "media::depthTexture/d3dPs.txt" ) );
            if( !mPs ) return false;
        }

        // success
        return true;
    }

    void destroy()
    {
        mColor.clear();
        mDepth.clear();
        mVs.clear();
        mPs.clear();
    }

    void onKeyPress( input::KeyEvent key )
    {
        if( input::KEY_MOUSEBTN_0 == key.code )
        {
            if( key.status.down )
            {
                int x, y;
                gInput.getMousePosition( x, y );
                mArcBall.onMouseButtonDown( x, y );
            }
            else
            {
                mArcBall.onMouseButtonUp();
            }
        }
    }

    void onAxisMove()
    {
        int x, y;
        gInput.getMousePosition( x, y );
        mArcBall.onMouseMove( x, y );
        mModel = mArcBall.getRotationMatrix();
    }

    void update()
    {
    }

    void render()
    {
        Renderer & r = gRenderer;
        ResourceManager & rm = gSceneResMgr;

        // render to depth texture
        r.contextUpdateBegin();
            r.setShaders( 0, 0, 0 );
            r.setRenderState( RS_CULL_MODE, RSV_CULL_NONE );
            r.setDrawToTextures( 1, mColor, 0, 0, 0, mDepth );
            r.setWorld( mModel ); r.setView( mView ); r.setProj( mProj );
            r.setTexture( 0, rm.getResourceT<Texture>( mTex0 ) );
            r.setVtxFmt( mDecl );
        r.contextUpdateEnd();
        r.clearScreen();
        r.drawIndexedUp( TRIANGLE_LIST, 12, 24, mBoxVerts, sizeof(BoxVert), mBoxIndices );

        // render depth texture to screen
        r.setDrawToBackBuf();
        gQuadRenderer.drawSingleTexturedQuad( mDepth, QuadRenderer::OPT_OPAQUE );
        //r.setTexture( 0, mDepth );
        //r.draw2DTexturedQuad( DQ_OPAQUE );
    }
};

class DepthTexture : public GN::app::SampleApp
{
    Scene * mScene;

public:

    DepthTexture() : mScene(0) {}

    bool onRendererCreate()
    {
        mScene = new Scene(*this);
        return mScene->create();
    }

    void onRendererDestroy()
    {
        safeDelete( mScene );
    }

    void onUpdate()
    {
        mScene->update();
    }

    void onKeyPress( input::KeyEvent key )
    {
        GN::app::SampleApp::onKeyPress( key );
        mScene->onKeyPress( key );
    }

    void onAxisMove( input::Axis a, int d )
    {
        GN::app::SampleApp::onAxisMove( a, d );
        mScene->onAxisMove();
    }

    void onRender()
    {
        GN_ASSERT( mScene );
        mScene->render();
    }
};

int main( int argc, const char * argv[] )
{
    DepthTexture app;
    return app.run( argc, argv );
}
