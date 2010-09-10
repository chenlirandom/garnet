#ifndef __GN_GFX_D3D10RENDERTARGETMGR_H__
#define __GN_GFX_D3D10RENDERTARGETMGR_H__
// *****************************************************************************
/// \file
/// \brief   D3D10 render target manager
/// \author  chen@@CHENLI-HOMEPC (2007.1.11)
// *****************************************************************************

namespace GN { namespace gfx
{
    class D3D10Gpu;

    ///
    /// D3D10 render target manager
    ///
    class D3D10RTMgr : public StdClass
    {
        GN_DECLARE_STDCLASS( D3D10RTMgr, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        D3D10RTMgr( D3D10Gpu & r ) : mGpu(r) { clear(); }
        virtual ~D3D10RTMgr() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init();
        void quit();
    private:
        void clear()
        {
            mAutoColor0 = 0;
            mAutoDepthTexture = 0;
            mAutoDepth = 0;
            memset(mColors, 0, sizeof(mColors) );
            mDepth = 0;
            mNumColors = 0;
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        ///
        /// bind render target to device
        ///
        bool bind(
            const RenderTargetDesc & oldrt,
            const RenderTargetDesc & newrt,
            bool                     skipDirtyCheck,
            bool                   & renderTargetSizeChanged );

        ///
        /// get render target count
        ///
        uint32 getRenderTargetCount() const { return (uint32)mNumColors; }

        ///
        /// get render target view
        ///
        ID3D10RenderTargetView * getRenderTargetView( size_t index ) const
        {
            GN_ASSERT( index < mNumColors );
            return mColors[index];
        }

        ///
        /// get depth stencil view
        ///
        ID3D10DepthStencilView * getDepthStencilView() const { return mDepth; }

        ///
        /// get render target size in pixels
        ///
        const Vector2<uint32> & getRenderTargetSize() const { return mRenderTargetSize; }

        // ********************************
        // private variables
        // ********************************
    private:

        D3D10Gpu & mGpu;

        ID3D10RenderTargetView * mAutoColor0;
        ID3D10Texture2D *        mAutoDepthTexture;
        ID3D10DepthStencilView * mAutoDepth;

        ID3D10RenderTargetView * mColors[GpuContext::MAX_COLOR_RENDER_TARGETS];
        ID3D10DepthStencilView * mDepth;
        size_t                   mNumColors;

        Vector2<uint32>          mRenderTargetSize;

        // ********************************
        // private functions
        // ********************************
    private:

    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_D3D10RENDERTARGETMGR_H__
