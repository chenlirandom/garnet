#ifndef __GN_GFXD3D10_D3D10RENDERER_H__
#define __GN_GFXD3D10_D3D10RENDERER_H__
// *****************************************************************************
/// \file
/// \brief   D3D10 renderer class
/// \author  chenlee (2005.10.2)
// *****************************************************************************

#include "../common/basicRendererMsw.h"

#pragma warning(disable:4100)

namespace GN { namespace gfx
{
    // Forward declarations
    class D3D10Resource;
    class D3D10RTMgr;
    class D3D10StateObjectManager;

    ///
    /// D3D10 vertex format structure
    ///
    class D3D10VertexLayout
    {
    public:

        AutoComPtr<ID3D10InputLayout> il; ///< D3D10 input layout
        UInt32                        defaultStrides[RendererContext::MAX_VERTEX_BUFFERS];

    public:

        /// initialize the layout
        bool init( ID3D10Device & dev, const GN::gfx::VertexFormat & format );

        /// less operator
        bool operator<( const D3D10VertexLayout & rhs ) const
        {
            if( il < rhs.il ) return true;
            if( rhs.il < il ) return false;
            return memcmp( defaultStrides, rhs.defaultStrides, sizeof(defaultStrides) ) < 0;
        }
    };

    ///
    /// D3D10 renderer class
    ///
    class D3D10Renderer : public BasicRendererMsw
    {
        GN_DECLARE_STDCLASS(D3D10Renderer, BasicRendererMsw);

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public :
        D3D10Renderer()          { clear(); }
        virtual ~D3D10Renderer() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const RendererOptions & );
        void quit();
    private:
        void clear()
        {
            dispClear();
            capsClear();
            resourceClear();
            contextClear();
            drawClear();
        }
        //@}

    // ************************************************************************
    //
    /// \name                     Display Manager
    //
    // ************************************************************************

        //@{

    public :

        virtual void * getD3DDevice() const { return mDevice; }
        virtual void * getOGLRC() const { return 0; }

    public :

        ID3D10Device   & getDeviceRefInlined() const { GN_ASSERT(mDevice); return *mDevice; }
        IDXGISwapChain & getSwapChainRef() const { GN_ASSERT(mSwapChain); return *mSwapChain; }

    private :

        bool dispInit();
        void dispQuit();
        void dispClear()
        {
            mAdapter = 0;
            mDevice = 0;
            mSwapChain = 0;
        }

    private :

        IDXGIAdapter   * mAdapter;
        ID3D10Device   * mDevice;
        IDXGISwapChain * mSwapChain;

        //@}

    // ************************************************************************
    //
    /// \name                 Capability Manager
    //
    // ************************************************************************

        //@{

    public :

        virtual const RendererCaps & getCaps() const { return mCaps; }
        virtual bool checkTextureFormatSupport( ColorFormat format, TextureUsage usage ) const;
        virtual ColorFormat getDefaultTextureFormat( TextureUsage usage ) const;

    private :

        bool capsInit();
        void capsQuit() {}
        void capsClear() {}

    private :

        RendererCaps mCaps;

        //@}

    // ************************************************************************
    //
    /// \name                     Resource Manager
    //
    // ************************************************************************

        //@{

    public :

        virtual CompiledGpuProgram * compileGpuProgram( const GpuProgramDesc & desc );
        virtual GpuProgram         * createGpuProgram( const void * compiledGpuProgramBinary, size_t length );
        virtual Uniform            * createUniform( size_t size );
        virtual Texture            * createTexture( const TextureDesc & desc );
        virtual VtxBuf             * createVtxBuf( const VtxBufDesc & desc );
        virtual IdxBuf             * createIdxBuf( const IdxBufDesc & desc );

    public :

        ///
        /// Insert resource into resource list. Can be only called by
        /// constructor of D3D9Resource.
        ///
        void insertResource( D3D10Resource * p )
        {
            mResourceList.push_back(p);
        }

        ///
        /// Remove resource from resource list. Can be only called by
        /// destructor of D3D9Resource.
        ///
        void removeResource( D3D10Resource * p )
        {
            mResourceList.remove(p);
        }

    private:

        bool resourceInit();
        void resourceQuit();
        void resourceClear() {}

    private :

        std::list<D3D10Resource*> mResourceList;

        //@}


    // ********************************************************************
    //
    /// \name Context manager
    //
    // ********************************************************************

        //@{

    public:

        virtual bool bindContextImpl( const RendererContext & context, bool skipDirtyCheck );

    private :

        bool contextInit();
        void contextQuit();
        void contextClear() { mContext.clear(); mSOMgr = 0; mRTMgr = 0; }

        inline bool bindContextRenderTarget( const RendererContext & newContext, bool skipDirtyCheck );
        inline bool bindContextShader( const RendererContext & newContext, bool skipDirtyCheck );
        inline bool bindContextState( const RendererContext & newContext, bool skipDirtyCheck );
        inline bool bindContextResource( const RendererContext & newContext, bool skipDirtyCheck );

    private:

        std::map<VertexFormat,D3D10VertexLayout> mVertexLayouts;
        AutoComPtr<ID3D10SamplerState>           mDefaultSampler;
        D3D10StateObjectManager * mSOMgr;
        D3D10RTMgr              * mRTMgr;

        //@}

    // ************************************************************************
    //
    /// \name                     Drawing Manager
    //
    // ************************************************************************

        //@{

    public: // from Renderer

        virtual void present();
        virtual void clearScreen( const Vector4f & c, float z, UInt8 s, BitFields flags );
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numidx,
                                  size_t        basevtx,
                                  size_t        startvtx,
                                  size_t        numvtx,
                                  size_t        startidx );
        virtual void draw( PrimitiveType prim,
                           size_t        numvtx,
                           size_t        startvtx );
        virtual void drawIndexedUp(
                             PrimitiveType  prim,
                             size_t         numidx,
                             size_t         numvtx,
                             const void *   vertexData,
                             size_t         strideInBytes,
                             const UInt16 * indexData );
        virtual void drawUp( PrimitiveType prim,
                             size_t        numvtx,
                             const void *  vertexData,
                             size_t        strideInBytes );
        virtual void drawLines( BitFields         options,
                                const void *      positions,
                                size_t            stride,
                                size_t            numpoints,
                                UInt32            rgba,
                                const Matrix44f & model,
                                const Matrix44f & view,
                                const Matrix44f & proj );

    private:

        bool drawInit();
        void drawQuit();
        void drawClear() { mFrameCounter = 0; mDrawCounter = 0; mUserVB = mUserIB = 0; }

    private:

        size_t mFrameCounter;
        size_t mDrawCounter;

        ID3D10Buffer * mUserVB;
        ID3D10Buffer * mUserIB;

        //@}

        // ********************************************************************
        //
        /// \name Misc. utilities
        //
        // ********************************************************************

        //@{

    public:

        virtual void getBackBufferContent( BackBufferContent & );

        virtual void dumpNextFrame( size_t startBatchIndex, size_t numBatches )
        {
            GN_UNUSED_PARAM( startBatchIndex );
            GN_UNUSED_PARAM( numBatches );
            GN_TODO( "D3D10 frame dump is not implemented." );
        }

        //@}
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFXD3D10_D3D10RENDERER_H__
