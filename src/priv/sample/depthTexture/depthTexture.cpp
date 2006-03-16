#include "pch.h"

using namespace GN;
using namespace GN::gfx;

struct Scene
{
    virtual ~Scene() {}

    virtual bool create() = 0;
    virtual void destroy() = 0;
    virtual void render() = 0;
};

class SceneNoPs : public Scene
{
    AutoRef<Texture> mDepth;
public:

    bool create()
    {
        Renderer & r = gRenderer;

        // create depth texture
        const DispDesc & dd = r.getDispDesc();
        mDepth.attach( r.create2DTexture( dd.width, dd.height, 1, FMT_DEFAULT, TEXUSAGE_DEPTH ) );
        if( mDepth.empty() ) return false;

        // success
        return true;
    }

    void destroy()
    {
        mDepth.clear();
    }

    void render()
    {
        Renderer & r = gRenderer;

        // render to depth texture
        struct QuadVert
        {
            float x, y, z;
            float u, v;
        } vb[] = 
        {
            { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f },
            { -1.0f,  1.0f, 0.0f, 0.0f, 1.0f },
            {  1.0f,  1.0f, 1.0f, 1.0f, 1.0f },
            {  1.0f, -1.0f, 1.0f, 1.0f, 0.0f },
        };
        r.setRenderState( RS_CULL_MODE, RSV_CULL_NONE );
        r.setTexture( 0, 0 );
        //r.setRenderDepth( mDepth );
        r.clearScreen();
        r.drawQuads( DQ_3D_POSITION, &vb[0].x, &vb[0].u, 0, sizeof(QuadVert), 1 );

        // draw depth texture to screen
        //r.setRenderDepth( 0 );
        r.setTexture( 0, mDepth );
        r.clearScreen();
        r.draw2DTexturedQuad( DQ_OPAQUE );
    }
};

class SceneWithPs : public Scene
{
    uint32_t mVs1, mVs2, mPs1, mPs2;
    AutoRef<Texture> mTarget, mDepth;

    bool createD3DShaders()
    {
        mVs1 = gShaderDict.getResourceHandle( "depthTexture/d3dVs1.txt" );
        mPs1 = gShaderDict.getResourceHandle( "depthTexture/d3dPs1.txt" );
        mVs2 = gShaderDict.getResourceHandle( "depthTexture/d3dVs2.txt" );
        mPs2 = gShaderDict.getResourceHandle( "depthTexture/d3dPs2.txt" );
        return mVs1 && mPs1 && mVs2 && mPs2;
    }

    bool createOGLShaders()
    {
        mVs1 = gShaderDict.getResourceHandle( "depthTexture/oglVs1.txt" );
        mPs1 = gShaderDict.getResourceHandle( "depthTexture/oglPs1.txt" );
        mVs2 = gShaderDict.getResourceHandle( "depthTexture/oglVs2.txt" );
        mPs2 = gShaderDict.getResourceHandle( "depthTexture/oglPs2.txt" );
        return mVs1 && mPs1 && mVs2 && mPs2;
    }

public:

    bool create()
    {
        Renderer & r = gRenderer;

        // create shaders
        if( r.getD3DDevice() && !createD3DShaders() ) return false;
        if( r.getOGLRC() && !createOGLShaders() ) return false;
        
        // create render target textures
        const DispDesc & dd = r.getDispDesc();
        mTarget.attach( r.create2DTexture( dd.width, dd.height, 1, FMT_D3DCOLOR, TEXUSAGE_RENDER_TARGET ) );
        if( mTarget.empty() ) return false;
        mDepth.attach( r.create2DTexture( dd.width, dd.height, 1, FMT_DEFAULT, TEXUSAGE_DEPTH ) );
        if( mDepth.empty() ) return false;

        // success
        return true;
    }

    void destroy()
    {
		mTarget.clear();
        mDepth.clear();
    }

    void render()
    {
        Renderer & r = gRenderer;

        // render to depth texture
        struct QuadVert
        {
            float x, y, z;
            float u, v;
        } vb[] = 
        {
            // x     y     z     u     v
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
        };
        r.setTexture( 0, 0 );
        r.setTexture( 1, 0 );
        r.setShaderHandles( mVs1, mPs1 );
        //r.setRenderTarget( 0, mTarget );
        //r.setRenderDepth( mDepth );
        r.clearScreen();
        r.drawQuads(
            DQ_UPDATE_DEPTH | DQ_3D_POSITION | DQ_USE_CURRENT_VS | DQ_USE_CURRENT_PS,
            &vb[0].x, &vb[0].u, 0, sizeof(QuadVert), 1 );

        // draw depth texture to screen
        //r.setRenderTarget( 0, 0 );
        //r.setRenderDepth( 0 );
        r.setTexture( 0, mDepth );
        r.setTexture( 1, mTarget );
        r.setShaderHandles( mVs2, mPs2 );
        r.clearScreen();
        r.draw2DTexturedQuad( DQ_OPAQUE | DQ_USE_CURRENT_VS | DQ_USE_CURRENT_PS );
        //*/
    }
};

class DepthTexture : public GN::app::SampleApp
{
    SceneNoPs mSceneNoPs;
    SceneWithPs mSceneWithPs;
    Scene * mScene;

public:

    DepthTexture() : mScene(0) {}

    bool onRendererCreate()
    {
        mScene = ( 0 != gRenderer.getCaps( CAPS_PSCAPS ) ) ? (Scene*)&mSceneWithPs : (Scene*)&mSceneNoPs;
        return mScene->create();
    }

    bool onRendererRestore()
    {
        return true;
    }

    void onRendererDispose()
    {
    }

    void onRendererDestroy()
    {
        if( mScene ) mScene->destroy(), mScene = 0;
    }

    void onUpdate()
    {
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
