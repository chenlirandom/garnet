#include "pch.h"
#include "xenonRenderer.h"
#include "xenonRenderTargetMgr.h"
#include "xenonShader.h"
#include "xenonTexture.h"
#include "xenonVertexDecl.h"
#include "xenonVtxBuf.h"
#include "xenonIdxBuf.h"

// *****************************************************************************
// local functions
// *****************************************************************************

// *****************************************************************************
// init/shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonRenderer::contextInit()
{
    GN_GUARD;

    // initialize render target manager
    //GN_ASSERT( 0 == mRTMgr );
    //mRTMgr = new XenonRenderTargetMgr( *this );
    //if( !mRTMgr->init() ) return false;

    // bind default context to device
    rebindContext();

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonRenderer::contextQuit()
{
    GN_GUARD;

    // reset context
    mContext.clear();

    // delete all vertex formats
    mVertexFormats.clear();

    //safeDelete( mRTMgr );

    GN_UNGUARD;
}

// *****************************************************************************
// from BasicRenderer
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonRenderer::bindContextImpl(
    const RendererContext & context,
    bool                    skipDirtyCheck )
{
    GN_GUARD_SLOW;

    PIXPERF_FUNCTION_EVENT();

    //
    // Parameter check
    //
    if( paramCheckEnabled() )
    {
        GN_TODO( "verify renderer context data" );
    }

    if( !bindContextRenderTargetsAndViewport( context, skipDirtyCheck ) ) return false;

    if( !bindContextRenderStates( context, skipDirtyCheck ) ) return false;

    if( !bindContextShaders( context, skipDirtyCheck ) ) return false;

    if( !bindContextResources( context, skipDirtyCheck ) ) return false;

    // success
    return true;

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
inline bool
GN::gfx::XenonRenderer::bindContextRenderTargetsAndViewport(
    const RendererContext & newContext,
    bool                    skipDirtyCheck )
{
    GN_UNUSED_PARAM( newContext );
    GN_UNUSED_PARAM( skipDirtyCheck );
    GN_UNIMPL_WARNING();
    return true;

    //bool needRebindViewport;
    //mRTMgr->bind( mContext, newContext, skipDirtyCheck, needRebindViewport );

/*

#if GN_XENON
	if( newFlags.colorBuffers )
    {
        static const RendererContext::SurfaceDesc sNullSurface = { 0, 0, 0, 0 };
        const RendererContext::SurfaceDesc *newSurf, *oldSurf;

        for( UInt32 i = 0; i < 4; ++i )
        {
            newSurf = (i >= newContext.numColorBuffers) ? &sNullSurface : &newContext.colorBuffers[i];
            oldSurf = (i >= mContext.numColorBuffers) ? &sNullSurface : &mContext.colorBuffers[i];
            if( *newSurf != *oldSurf || forceRebind )
            {
                // copy content from RT to old/current RTT
                if( *oldSurf )
                {
                    GN_TODO( "resolve to RTT" );
                }

                // setup new RT
                if( 0 == newSurf->texture )
                {
                    // restore to default RT
                    if( 0 == i )
                    {
                        GN_DX9_CHECK( mDevice->SetRenderTarget( 0, mAutoColor0 ) );
                    }
                    else
                    {
                        GN_DX9_CHECK( mDevice->SetRenderTarget( 0, 0 ) );
                    }
                }
                else
                {
                    // setup RT based on RTT
                    //const Vector3<UInt32> & sz = newSurf->texture->getMipSize( newSurf->level );
                    //setupRT( sz.x, sz.y, newSurf->texture->getDesc().format );
                }

            }
        }

        // setup default render targets
        GN_DX9_CHECK( mDevice->SetRenderTarget( 0, mAutoColor0 ) );
        GN_DX9_CHECK( mDevice->SetRenderTarget( 1, 0 ) );
        GN_DX9_CHECK( mDevice->SetRenderTarget( 2, 0 ) );
        GN_DX9_CHECK( mDevice->SetRenderTarget( 3, 0 ) );
	}

    if( newFlags.depthBuffer )
    {
        const RendererContext::SurfaceDesc *newSurf, *oldSurf;

        // bind depth buffer
        newSurf = &newContext.depthBuffer;
        oldSurf = &mContext.depthBuffer;
        if( 0 == newSurf->texture )
        {
            AutoComPtr<IDirect3DSurface9> rt0;
            GN_DX9_CHECK( mDevice->GetRenderTarget( 0, &rt0 ) );
            GN_ASSERT( rt0 );
            D3DSURFACE_DESC rt0Desc;
            GN_DX9_CHECK( rt0->GetDesc( &rt0Desc ) );
            if( mAutoDepth )
            {
                D3DSURFACE_DESC depthDesc;
                GN_DX9_CHECK( mAutoDepth->GetDesc( &depthDesc ) );
                if( depthDesc.Width < rt0Desc.Width ||
                    depthDesc.Height < rt0Desc.Height ||
                    forceRebind )
                {
                    // release old depth surface
                    GN_DX9_CHECK( mDevice->SetDepthStencilSurface( NULL ) );
                    mAutoDepth.clear();

                    // create new depth buffer
                    GN_DX9_CHECK_R( mDevice->CreateDepthStencilSurface(
                        max(depthDesc.Width, rt0Desc.Width),
                        max(depthDesc.Height, rt0Desc.Height),
                        depthDesc.Format,
                        depthDesc.MultiSampleType,
                        depthDesc.MultiSampleQuality,
                        TRUE, // discardable depth buffer
                        &mAutoDepth, 0 ) );
                    GN_DX9_CHECK( mDevice->SetDepthStencilSurface( mAutoDepth ) );
                }
            }
            else
            {
                GN_DX9_CHECK_R( mDevice->CreateDepthStencilSurface(
                    rt0Desc.Width,
                    rt0Desc.Height,
                    DEFAULT_DEPTH_FORMAT, // TODO: enumerate appropriate depth buffer format.
                    mPresentParameters.MultiSampleType,
                    mPresentParameters.MultiSampleQuality,
                    TRUE,
                    &mAutoDepth, 0 ) );
                GN_DX9_CHECK( mDevice->SetDepthStencilSurface( mAutoDepth ) );
            }
        }
        else if( *newSurf != *oldSurf || forceRebind )
        {
            GN_UNIMPL();
        }
    }

#else

#endif
*/
}

static const D3DCULL CULL_TO_D3D[]=
{
    D3DCULL_NONE, // FRONT_CCW, CULL_NONE
    D3DCULL_CCW,  // FRONT_CCW, CULL_FRONT,
    D3DCULL_CW,   // FRONT_CCW, CULL_BACK,
    D3DCULL_NONE, // FRONT_CW,  CULL_NONE,
    D3DCULL_CW,   // FRONT_CW,  CULL_FRONT,
    D3DCULL_CCW,  // FRONT_CW,  CULL_BACK,
};
GN_CASSERT( GN_ARRAY_COUNT(CULL_TO_D3D) == GN::gfx::RendererContext::NUM_FRONT_FACE_MODES * GN::gfx::RendererContext::NUM_CULL_MODES );

static const D3DBLENDOP BLEND_OP_TO_D3D[]=
{
    D3DBLENDOP_ADD,         // BLEND_OP_ADD = 0,
    D3DBLENDOP_SUBTRACT,    // BLEND_OP_SUB,
    D3DBLENDOP_REVSUBTRACT, // BLEND_OP_REV_SUB,
    D3DBLENDOP_MIN,         // BLEND_OP_MIN,
    D3DBLENDOP_MAX,         // BLEND_OP_MAX,
};
GN_CASSERT( GN_ARRAY_COUNT(BLEND_OP_TO_D3D) == GN::gfx::RendererContext::NUM_BLEND_OPERATIONS );

static const D3DBLEND BLEND_ARG_TO_D3D[]=
{
    D3DBLEND_ZERO,           // BLEND_ZERO = 0,
    D3DBLEND_ONE,            // BLEND_ONE,
    D3DBLEND_SRCCOLOR,       // BLEND_SRC_COLOR,
    D3DBLEND_INVSRCCOLOR,    // BLEND_INV_SRC_COLOR,
    D3DBLEND_SRCALPHA,       // BLEND_SRC_ALPHA,
    D3DBLEND_INVSRCALPHA,    // BLEND_INV_SRC_ALPHA,
    D3DBLEND_DESTALPHA,      // BLEND_DEST_ALPHA,
    D3DBLEND_INVDESTALPHA,   // BLEND_INV_DEST_ALPHA,
    D3DBLEND_DESTCOLOR,      // BLEND_DEST_COLOR,
    D3DBLEND_INVDESTCOLOR,   // BLEND_INV_DEST_COLOR,
    D3DBLEND_BLENDFACTOR,    // BLEND_BLEND_FACTOR,
    D3DBLEND_INVBLENDFACTOR, // BLEND_INV_BLEND_FACTOR,
};
GN_CASSERT( GN_ARRAY_COUNT(BLEND_ARG_TO_D3D) == GN::gfx::RendererContext::NUM_BLEND_ARGUMENTS );

//
//
// -----------------------------------------------------------------------------
inline bool
GN::gfx::XenonRenderer::bindContextRenderStates(
    const RendererContext & newContext,
    bool                    skipDirtyCheck )
{
    GN_UNIMPL_WARNING();

    // fill mode

    // cull and face
    if( skipDirtyCheck || newContext.frontFace != mContext.frontFace || newContext.cullMode != mContext.cullMode )
    {
        D3DCULL cullMode = CULL_TO_D3D[newContext.frontFace*RendererContext::NUM_CULL_MODES + newContext.cullMode];
        mDevice->SetRenderState( D3DRS_CULLMODE, cullMode );
    }

    // msaa

    // depth

    // stencil

    // blend
    if( skipDirtyCheck || newContext.blendFlags != mContext.blendFlags )
    {
        mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, newContext.blendEnabled );
        mDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, true );
        mDevice->SetRenderState( D3DRS_BLENDOP, BLEND_OP_TO_D3D[newContext.blendOp] );
        mDevice->SetRenderState( D3DRS_SRCBLEND, BLEND_ARG_TO_D3D[newContext.blendSrc] );
        mDevice->SetRenderState( D3DRS_DESTBLEND, BLEND_ARG_TO_D3D[newContext.blendDst] );
        mDevice->SetRenderState( D3DRS_BLENDOPALPHA, BLEND_OP_TO_D3D[newContext.blendAlphaOp] );
        mDevice->SetRenderState( D3DRS_SRCBLENDALPHA, BLEND_ARG_TO_D3D[newContext.blendAlphaSrc] );
        mDevice->SetRenderState( D3DRS_DESTBLENDALPHA, BLEND_ARG_TO_D3D[newContext.blendAlphaDst] );
    }
    if( skipDirtyCheck || newContext.blendFactors != mContext.blendFactors )
    {
        DWORD bgra32 = GN_BGRA32(
            newContext.blendFactors.r * 255.0f,
            newContext.blendFactors.g * 255.0f,
            newContext.blendFactors.b * 255.0f,
            newContext.blendFactors.a * 255.0f );
        mDevice->SetRenderState( D3DRS_BLENDFACTOR, bgra32 );
    }

    // scissor

    return true;
}

//
//
// -----------------------------------------------------------------------------
inline bool
GN::gfx::XenonRenderer::bindContextShaders(
    const RendererContext & newContext,
    bool                    skipDirtyCheck )
{
    if( newContext.gpuProgram )
    {
        const XenonBasicGpuProgram * prog = (const XenonBasicGpuProgram *)newContext.gpuProgram.get();

        if( skipDirtyCheck || mContext.gpuProgram != newContext.gpuProgram )
        {
            prog->apply();
        }

        prog->applyUniforms( (const Uniform * const *)newContext.uniforms.cptr(), newContext.uniforms.size(), skipDirtyCheck );
        prog->applyTextures( newContext.textures.cptr(), newContext.textures.MAX_SIZE, skipDirtyCheck );
    }
    else
    {
        if( skipDirtyCheck || mContext.gpuProgram )
        {
            // re-apply shader pointer to NULL, only when dirty check is disabled,
            // or last GPU program is not NULL.
            mDevice->SetVertexShader( NULL );
            mDevice->SetPixelShader( NULL );
        }
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
inline bool
GN::gfx::XenonRenderer::bindContextResources(
    const RendererContext & newContext,
    bool                    skipDirtyCheck )
{
    //
    // bind vertex format
    //
    if( skipDirtyCheck || newContext.vtxfmt != mContext.vtxfmt )
    {
        AutoComPtr<IDirect3DVertexDeclaration9> & decl = mVertexFormats[newContext.vtxfmt];
        if( !decl )
        {
            decl.attach( createXenonVertexDecl( *mDevice, newContext.vtxfmt ) );
            if( !decl ) return false;
        }

        // apply to D3D device
        GN_DX9_CHECK( mDevice->SetVertexDeclaration( decl ) );
    }

    ///
    /// bind vertex buffers
    ///
    for( UINT i = 0; i < RendererContext::MAX_VERTEX_BUFFERS; ++i )
    {
        const VertexBufferBinding & vbb = newContext.vtxbufs[i];

        if( skipDirtyCheck || vbb != mContext.vtxbufs[i] )
        {
            GN_DX9_CHECK( mDevice->SetStreamSource(
                i,
                vbb.vtxbuf ? safeCastPtr<const XenonVtxBuf>(vbb.vtxbuf.get())->getD3DBuffer() : NULL,
                vbb.offset,
                vbb.stride ) );
        }
    }

    //
    // bind index buffer
    //
    if( skipDirtyCheck || newContext.idxbuf != mContext.idxbuf )
    {
        GN_DX9_CHECK( mDevice->SetIndices( newContext.idxbuf
            ? safeCastPtr<const XenonIdxBuf>(newContext.idxbuf.get())->getD3DBuffer()
            : NULL ) );
    }

    return true;
}
