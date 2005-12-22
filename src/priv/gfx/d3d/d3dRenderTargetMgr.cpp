#include "pch.h"
#include "d3dRenderer.h"
#include "d3dTexture.h"


// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DRenderer::renderTargetDeviceCreate()
{
    GN_GUARD;

    // check multiple render target support
    if( getCaps(CAPS_MAX_RENDER_TARGETS) > 4 )
    {
        GNGFX_ERROR( "Sorry, we currently do not support more then 4 simultaneous render targets." );
        return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DRenderer::renderTargetDeviceRestore()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // get default render target surface
    GN_ASSERT( 0 == mDefaultRT0 );
    GN_DX_CHECK_RV( mDevice->GetRenderTarget( 0, &mDefaultRT0 ), false );

    // restore render target size to default value
    mCurrentRTSize.set( getDispDesc().width, getDispDesc().height );
    mAutoDepthSize.set( getDispDesc().width, getDispDesc().height );

    // make sure MRT caps does not exceed maximum allowance value
    GN_ASSERT( getCaps(CAPS_MAX_RENDER_TARGETS) <= 4 );

    // (re)apply render targets
    RenderTargetTextureDesc desc;
    for( size_t i = 0; i < getCaps(CAPS_MAX_RENDER_TARGETS); ++i )
    {
        desc = mCurrentRTs[i];

        // ��mCurrentRts�޸�Ϊ��Чֵ���Ա��ƹ�SetRenderTarget()���ظ����ü�⡣
        // mCurrentDepthͬ��
        mCurrentRTs[i].tex = (const Texture*)0xdeadbeef; mCurrentRTs[i].face = NUM_TEXFACES;

        setRenderTarget( i, desc.tex, desc.face );
    }

    // (re)apply depth texture
    desc = mCurrentDepth;
    mCurrentDepth.tex = (const Texture*)0xdeadbeef; mCurrentDepth.face = NUM_TEXFACES;
    setRenderDepth( desc.tex, desc.face );

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::renderTargetDeviceDispose()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // release render target pointers
    safeRelease( mDefaultRT0 );
    safeRelease( mAutoDepth );

    GN_UNGUARD;
}

// *****************************************************************************
// interface functions
// *****************************************************************************

//
//
// ----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::setRenderTarget(
    size_t index, const Texture * tex, TexFace face )
{
    GN_GUARD_SLOW;

    if( index >= getCaps(CAPS_MAX_RENDER_TARGETS) )
    {
        GN_ERROR( "render target index (%d) is too large!", index );
        return;
    }

    RenderTargetTextureDesc & rttd = mCurrentRTs[index];

    // skip redundant call
    if( rttd.equal( tex, face ) ) return;

    // get texture surface
    AutoComPtr<IDirect3DSurface9> surf;
    Vector2<uint32_t> surfSize;
    if( tex )
    {
        // check texture's creation flag
        if( !(TEXUSAGE_RENDERTARGET & tex->getUsage() ) )
        {
            GNGFX_ERROR( "Texture must have usage of TEXUSAGE_RENDERTARGET!" );
            return;
        }

        // get surface pointer
        const D3DTexture * d3dTex = safeCast<const D3DTexture*>(tex);
        surf.attach( d3dTex->getSurface( face, 0 ) );
        if( !surf ) return;

        // get surface size
        tex->getSize<uint32_t>( &surfSize.x, &surfSize.y, NULL );
    }
    else if( 0 == index )
    {
        GN_ASSERT( mDefaultRT0 );
        surf.reset( mDefaultRT0 );
        surfSize.x = getDispDesc().width;
        surfSize.y = getDispDesc().height;
        GN_ASSERT( surf );
    }

    // update D3D render target
    GN_ASSERT( 0 != index || surf );
    GN_DX_CHECK( mDevice->SetRenderTarget( (DWORD)index, surf ) );
    rttd.tex  = tex;
    rttd.face = face;

    // handle RT size change
    if( 0 == index )
    {
        // update render target size
        mCurrentRTSize = surfSize;

        // update automatic depth surface
        if( 0 == mCurrentDepth.tex ) resizeAutoDepthBuffer( surfSize );

        // Because viewport is using relative coordinates based on render target size,
        // so here we have to re-apply the viewport.
        setRenderParameter( RPT_TRANSFORM_VIEWPORT, getRenderParameter( RPT_TRANSFORM_VIEWPORT ).valueFloats, 4 );
    }

    GN_UNGUARD_SLOW;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::setRenderDepth( const Texture * tex, TexFace face )
{
    GN_GUARD_SLOW;

    if( mCurrentDepth.equal( tex, face ) ) return;

    if( tex )
    {
        // check texture's creation flag
        if( !(TEXUSAGE_DEPTH & tex->getUsage()) )
        {
            GNGFX_ERROR( "can't set non-depth-texture as depth buffer!" );
            return;
        }

        // get surface pointer
        const D3DTexture * d3dTex = safeCast<const D3DTexture*>(tex);
        AutoComPtr<IDirect3DSurface9> surf;
        surf.attach( d3dTex->getSurface( face, 0 ) );
        if( !surf ) return;

        // change D3D depth buffer
        GN_DX_CHECK( mDevice->SetDepthStencilSurface( surf ) );
    }
    else
    {
        // fall back to automatic depth surface
        resizeAutoDepthBuffer( mCurrentRTSize );
    }

    // success
    mCurrentDepth.tex  = tex;
    mCurrentDepth.face = face;

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN_INLINE void GN::gfx::D3DRenderer::resizeAutoDepthBuffer( const Vector2<uint32_t> & sz )
{
    GN_GUARD_SLOW;

    // Check surface size
    if( sz.x > mAutoDepthSize.x || sz.y > mAutoDepthSize.y || NULL == mAutoDepth )
    {
        // release old buffer
        safeRelease( mAutoDepth );

        // create a new buffer
        AutoComPtr<IDirect3DSurface9> newSurf;
        GN_DX_CHECK_R( mDevice->CreateDepthStencilSurface(
            sz.x, sz.y,
            mPresentParameters.AutoDepthStencilFormat,
            mPresentParameters.MultiSampleType, mPresentParameters.MultiSampleQuality,
            mPresentParameters.Flags & D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL,
            &mAutoDepth, 0 ) );
    }

    // update D3D depth surface
    GN_DX_CHECK_R( mDevice->SetDepthStencilSurface( mAutoDepth ) );

    GN_UNGUARD_SLOW;
}
