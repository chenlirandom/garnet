#include "pch.h"
#include "d3d10Renderer.h"
#include "d3d10RenderTargetMgr.h"
#include "d3d10Texture.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.D3D10");

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10RTMgr::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::D3D10RTMgr, () );

    ID3D10Device & dev = mRenderer.getDeviceRefInlined();

    // create default rener target view
    AutoComPtr<ID3D10Texture2D> backBuffer;
    GN_DX10_CHECK_RV( mRenderer.getSwapChainRef().GetBuffer( 0, __uuidof( ID3D10Texture2D ), (void**)&backBuffer ), failure() );
    GN_DX10_CHECK_RV( dev.CreateRenderTargetView( backBuffer, NULL, &mAutoColor0 ), failure() );
    GN_ASSERT( mAutoColor0 );

    // create depth texture
    const DispDesc & dd = mRenderer.getDispDesc();
    D3D10_TEXTURE2D_DESC td;
    td.Width              = dd.width;
    td.Height             = dd.height;
    td.MipLevels          = 1;
    td.ArraySize          = 1;
    td.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.SampleDesc.Count   = 1;
    td.SampleDesc.Quality = 0;
    td.Usage              = D3D10_USAGE_DEFAULT;
    td.BindFlags          = D3D10_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags     = 0;
    td.MiscFlags          = 0;
    GN_DX10_CHECK_RV( dev.CreateTexture2D( &td, NULL, &mAutoDepthTexture ), failure() );

    // create depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
    dsvd.Format             = td.Format;
    dsvd.ViewDimension      = D3D10_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;
    GN_DX10_CHECK_RV( dev.CreateDepthStencilView( mAutoDepthTexture, &dsvd, &mAutoDepth ), failure() );

    // bind these views to device.
    dev.OMSetRenderTargets( 1, &mAutoColor0, mAutoDepth );

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10RTMgr::quit()
{
    GN_GUARD;

    safeRelease( mAutoColor0 );
    safeRelease( mAutoDepthTexture );
    safeRelease( mAutoDepth );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10RTMgr::bind(
    const RenderTargetDesc & oldrt,
    const RenderTargetDesc & newrt,
    bool                     skipDirtyCheck,
    bool                   & renderTargetSizeChanged )
{
    renderTargetSizeChanged = false;

    // make new render target description is valid.
    GN_ASSERT( oldrt.valid() );
    if( !newrt.valid() ) return false;

    // check for redundancy
    if( !skipDirtyCheck && oldrt == newrt )
    {
        return true;
    }

    // build RTV array
    mNumColors = newrt.colors.size();
    for( size_t i = 0; i < mNumColors; ++i )
    {
        const RenderTargetTexture & rtt = newrt.colors[i];

        D3D10Texture * tex = (D3D10Texture*)rtt.texture.get();

        GN_ASSERT( tex );

        mColors[i] = tex->getRTView( rtt.face, rtt.level, rtt.slice );

        if( NULL == mColors[i] )
        {
            return false;
        }
    }
    // fill remained items in RTV array with NULLs
    for( size_t i = mNumColors; i < RenderTargetDesc::MAX_COLOR_RENDER_TARGETS; ++i )
    {
        mColors[i] = NULL;
    }
    // handle render-to-back-buffer case
    if( 0 == mNumColors )
    {
        mColors[0] = mAutoColor0;
        mNumColors = 1;
    }

    // Get RSV pointer
    D3D10Texture * dstex = (D3D10Texture*)newrt.depthstencil.texture.get();
    if( dstex )
    {
        mDepth = dstex->getDSView(
            newrt.depthstencil.face,
            newrt.depthstencil.level,
            newrt.depthstencil.slice );
        if( NULL == mDepth )
        {
            return false;
        }
    }
    else
    {
        GN_ASSERT( mAutoDepth );
        mDepth = mAutoDepth;
    }

    // bind to D3D device
    mRenderer.getDeviceRefInlined().OMSetRenderTargets(
        (UINT)RenderTargetDesc::MAX_COLOR_RENDER_TARGETS,
        mColors,
        mDepth );

    // update mRenderTargetSize, according to render target 0 size
    Vector2<UInt32> newRtSize;
    if( newrt.colors.size() > 0 )
    {
        newrt.colors[0].texture->getMipSize( newrt.colors[0].level, &newRtSize.x, &newRtSize.y );
    }
    else
    {
        const DispDesc & dd = mRenderer.getDispDesc();
        newRtSize.x = dd.width;
        newRtSize.y = dd.height;
    }

    // need to rebind viewport when render target size is changed.
    renderTargetSizeChanged = ( newRtSize != mRenderTargetSize );

    // store new render target size
    mRenderTargetSize = newRtSize;

    // done
    return true;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------

