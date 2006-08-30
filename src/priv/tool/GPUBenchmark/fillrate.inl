using namespace GN;
using namespace GN::gfx;

//!
//! Fillrate benchmarking application
//!
class TestFillrate : public BasicTestCase
{
    ManyManyQuads mGeometry;
    BasicEffect * mEffect;
    AutoRef<Texture> mTextures[16];
    RendererContext mContext;
    StrA mFillrateStr;

public:

    UInt mInitTexCount;
    bool mInitDoubleDepth;
    bool mInitMaxBandwidth;

    AverageValue<float> mFillrate;

public:

    TestFillrate( app::SampleApp & app, const StrA & name,
                  UInt texCount,
                  bool doubleDepth,
                  bool maxBandwidth )
        : BasicTestCase(app,name)
        , mEffect( 0 )
        , mInitTexCount(texCount)
        , mInitDoubleDepth(doubleDepth)
        , mInitMaxBandwidth(maxBandwidth)
    {}

    bool create()
    {
        Renderer & r = gRenderer;
        
        // create geometry
        if( !mGeometry.create() ) return false;

        // create effect
        if( mInitTexCount )
            mEffect = new TexturedEffect( mInitTexCount );
        else
            mEffect = new SolidEffect;
        if( !mEffect || !mEffect->create() ) return false;

        // create texture
        for( UInt i = 0; i < mInitTexCount; ++i )
        {
            mTextures[i].attach( r.create2DTexture( 2, 2, 1, FMT_D3DCOLOR, 0, true ) );
            if( !mTextures[i] ) return false;
            TexLockedResult tlr;
            mTextures[i]->lock( tlr, 0, 0, 0, LOCK_DISCARD );
            memset( tlr.data, 0xFF, tlr.sliceBytes );
            mTextures[i]->unlock();
        }

        // initialize the context
        mContext.clearToNull();
        mContext.setVS( mEffect->vs );
        if( mInitDoubleDepth )
        {
            mContext.setRenderState( RS_COLOR0_WRITE, 0 );
            mContext.setPS( 0 );
        }
        else
        {
            mContext.setRenderState( RS_COLOR0_WRITE, 0xF );
            mContext.setPS( mEffect->ps );
        }
        for( UInt i = 0; i < mInitTexCount; ++i ) mContext.setTexture( i, mTextures[i] );
        if( mInitMaxBandwidth )
        {
            //mContext->
        }
        mContext.setVtxFmt( mGeometry.vtxfmt );
        mContext.setVtxBuf( 0, mGeometry.vtxbuf, sizeof(ManyManyQuads::Vertex) );
        mContext.setIdxBuf( mGeometry.idxbuf );

        // success
        return true;
    }

    void destroy()
    {
        mGeometry.destroy();
        safeDelete( mEffect );
        for( UInt i = 0; i < 16; ++i ) mTextures[i].clear();
    }

    void onkey( input::KeyEvent key )
    {
        if( !key.status.down )
        {
            switch( key.code )
            {
                case input::KEY_XB360_RIGHT_SHOULDER :
                case input::KEY_NUMPAD_ADD:
                    mGeometry.DRAW_COUNT += 1;
                    break;

                case input::KEY_XB360_LEFT_SHOULDER:
                case input::KEY_NUMPAD_SUBTRACT:
                    if( mGeometry.DRAW_COUNT > 0 ) mGeometry.DRAW_COUNT -= 1;
                    break;

                default : ; // do nothing
            }
        }
    }

    void onmove( input::Axis, int ) {}

    void update()
    {
        const DispDesc & dd = gRenderer.getDispDesc();
        float pixfr = dd.width * dd.height / 1000000000.0f * mGeometry.QUAD_COUNT * mGeometry.DRAW_COUNT * getApp().getFps();
        float texfr = pixfr * mInitTexCount;
        float bandwidth = pixfr * dd.depth / 8;
        mFillrateStr.format(
            "%s\n"
            "quads = %d x %d\n"
            "pixel fillrate = %f GB/sec\n"
            "texel fillrate = %f GB/sec\n"
            "EDRAM bandwidth = %f GB/sec",
            getName().cptr(),
            mGeometry.DRAW_COUNT, mGeometry.QUAD_COUNT,
            pixfr,
            texfr,
            bandwidth );
        mFillrate = pixfr;
    }

    void render()
    {
        Renderer & r = gRenderer;

        r.setContext( mContext );
        mGeometry.draw();

        static const Vector4f RED(1,0,0,1);
        r.drawDebugText( mFillrateStr.cptr(), 0, 100, RED );
    }

    StrA printResult()
    {
        return strFormat( "fillrate(%f)", mFillrate.getAverageValue() );
    }
};
