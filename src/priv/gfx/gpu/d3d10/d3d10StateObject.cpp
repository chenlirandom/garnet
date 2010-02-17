#include "pch.h"
#include "d3d10StateObject.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpu.D3D10");

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::D3D10StateObjectManager::D3D10StateObjectManager( ID3D10Device & dev )
    : mRasterStates( dev )
    , mBlendStates( dev )
    , mDepthStates( dev )
    , mSamplerStates( dev )
{
    /* setup default rasterization states
    mRasterDirty = false;

    mRasterDesc.FillMode              = D3D10_FILL_SOLID;
    mRasterDesc.CullMode              = D3D10_CULL_NONE;
    mRasterDesc.FrontCounterClockwise = false;
    mRasterDesc.DepthBias             = 0;
    mRasterDesc.DepthBiasClamp        = 1.0f;
    mRasterDesc.SlopeScaledDepthBias  = .0f;
    mRasterDesc.DepthClipEnable       = false;
    mRasterDesc.ScissorEnable         = false;
    mRasterDesc.MultisampleEnable     = false;
    mRasterDesc.AntialiasedLineEnable = false;

    mRasterKey.u64    = 0;
    mRasterKey.u8     = 0;
    mRasterKey.bias   = 0;
    mRasterKey.slop   = 0.0f;
    mRasterKey.fill   = D3D10_FILL_SOLID;
    mRasterKey.cull   = D3D10_CULL_NONE;
    mRasterKey.msaa   = 0; // no MSAA
    mRasterKey.aaline = 0;

    // setup default blend states
    mBlendDirty = false;

    memset( &mBlendDesc, 0, sizeof(mBlendDesc) );
    mBlendDesc.SrcBlend                 = D3D10_BLEND_ONE;
    mBlendDesc.DestBlend                = D3D10_BLEND_ZERO;
    mBlendDesc.BlendOp                  = D3D10_BLEND_OP_ADD;
    mBlendDesc.SrcBlendAlpha            = D3D10_BLEND_ONE;
    mBlendDesc.DestBlendAlpha           = D3D10_BLEND_ZERO;
    mBlendDesc.BlendOpAlpha             = D3D10_BLEND_OP_ADD;
    mBlendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[1] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[2] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[3] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[4] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[5] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[6] = D3D10_COLOR_WRITE_ENABLE_ALL;
    mBlendDesc.RenderTargetWriteMask[7] = D3D10_COLOR_WRITE_ENABLE_ALL;

    mBlendKey.u64       = 0;
    mBlendKey.alphamask = 0;
    mBlendKey.blend     = 0;
    mBlendKey.blendsrc  = D3D10_BLEND_ONE;
    mBlendKey.blenddst  = D3D10_BLEND_ZERO;
    mBlendKey.blendop   = D3D10_BLEND_OP_ADD;
    mBlendKey.alphasrc  = D3D10_BLEND_ONE;
    mBlendKey.alphadst  = D3D10_BLEND_ZERO;
    mBlendKey.alphaop   = D3D10_BLEND_OP_ADD;
    mBlendKey.rtmask    = 0xFFFFFFFF;

    mBlendAaMask = 0xFFFFFFFF;

    // setup default depth stencil states
    mDepthDirty = false;

    mDepthDesc.DepthEnable = true;
    mDepthDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    mDepthDesc.DepthFunc = D3D10_COMPARISON_LESS;
    mDepthDesc.StencilEnable = false;
    mDepthDesc.StencilReadMask = 0xff;
    mDepthDesc.StencilWriteMask = 0xff;
    mDepthDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    mDepthDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
    mDepthDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    mDepthDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS; //( Note: Apr. 07 SDK document has bug about the default value of stencil op, check for newer SDK document for default valeu)
    mDepthDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    mDepthDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
    mDepthDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    mDepthDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

    mDepthKey.u64              = 0;
    mDepthKey.stencil          = mDepthDesc.StencilEnable;
    mDepthKey.zenable          = mDepthDesc.DepthEnable;
    mDepthKey.zwrite           = mDepthDesc.DepthWriteMask;
    mDepthKey.zfunc            = mDepthDesc.DepthFunc;
    mDepthKey.frontfail        = mDepthDesc.FrontFace.StencilFailOp;
    mDepthKey.frontzfail       = mDepthDesc.FrontFace.StencilDepthFailOp;
    mDepthKey.frontpass        = mDepthDesc.FrontFace.StencilPassOp;
    mDepthKey.frontfunc        = mDepthDesc.FrontFace.StencilFunc;
    mDepthKey.backfail         = mDepthDesc.BackFace.StencilFailOp;
    mDepthKey.backzfail        = mDepthDesc.BackFace.StencilDepthFailOp;
    mDepthKey.backpass         = mDepthDesc.BackFace.StencilPassOp;
    mDepthKey.backfunc         = mDepthDesc.BackFace.StencilFunc;
    mDepthKey.stencilreadmask  = mDepthDesc.StencilReadMask;
    mDepthKey.stencilwritemask = mDepthDesc.StencilWriteMask;
    */
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10StateObjectManager::setRS(
    const D3D10_RASTERIZER_DESC & desc,
    bool                          skipDirtyCheck )
{
    ID3D10RasterizerState * stateObject = mRasterStates[desc];
    if( NULL == stateObject ) return false;

    if( skipDirtyCheck || stateObject != mCurrentRS )
    {
        mRasterStates.dev().RSSetState( stateObject );

        mCurrentRS = stateObject;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10StateObjectManager::setBS(
    const D3D10_BLEND_DESC & desc,
    const Vector4f         & blendFactors,
    UInt32                   sampleMask,
    bool                     skipDirtyCheck )
{
    ID3D10BlendState * stateObject = mBlendStates[desc];
    if( NULL == stateObject ) return false;

    if( skipDirtyCheck ||
        stateObject != mCurrentBS ||
        blendFactors != mCurrentBlendFactors ||
        sampleMask != mCurrentSampleMask )
    {
        mBlendStates.dev().OMSetBlendState( stateObject, blendFactors, sampleMask );

        mCurrentBS = stateObject;
        mCurrentBlendFactors = blendFactors;
        mCurrentSampleMask = sampleMask;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10StateObjectManager::setDS(
    const D3D10_DEPTH_STENCIL_DESC & desc,
    UInt32                           stencilRef,
    bool                             skipDirtyCheck )
{
    ID3D10DepthStencilState * stateObject = mDepthStates[desc];
    if( NULL == stateObject ) return false;

    if( skipDirtyCheck ||
        stateObject != mCurrentDS ||
        stencilRef != mCurrentStencilRef )
    {
        mDepthStates.dev().OMSetDepthStencilState( stateObject, stencilRef );

        mCurrentDS = stateObject;
        mCurrentStencilRef = stencilRef;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10StateObjectManager::setVSSampler(
    const D3D10_SAMPLER_DESC & desc,
    UInt32                     stage,
    bool                       skipDirtyCheck )
{
    GN_UNIMPL();
    GN_UNUSED_PARAM( desc );
    GN_UNUSED_PARAM( stage );
    GN_UNUSED_PARAM( skipDirtyCheck );
    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10StateObjectManager::setGSSampler(
    const D3D10_SAMPLER_DESC & desc,
    UInt32                     stage,
    bool                       skipDirtyCheck )
{
    GN_UNIMPL();
    GN_UNUSED_PARAM( desc );
    GN_UNUSED_PARAM( stage );
    GN_UNUSED_PARAM( skipDirtyCheck );
    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10StateObjectManager::setPSSampler(
    const D3D10_SAMPLER_DESC & desc,
    UInt32                     stage,
    bool                       skipDirtyCheck )
{
    ID3D10SamplerState * ss = mSamplerStates[desc];
    if( NULL == ss ) return false;

    if( stage >= GN_ARRAY_COUNT(mCurrentPSSamplers) )
    {
        GN_ERROR(sLogger)( "PS sampler state is too large." );
        return false;
    }
    if( skipDirtyCheck ||
        ss != mCurrentPSSamplers[stage] )
    {
        mDepthStates.dev().PSSetSamplers( stage, 1, &ss );

        mCurrentPSSamplers[stage] = ss;
    }

    return true;
}

