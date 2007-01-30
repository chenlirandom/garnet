#ifndef __GN_GFX_RENDERER_H__
#define __GN_GFX_RENDERER_H__
// *****************************************************************************
/// \file    renderer.h
/// \brief   Main renderer interface of GFX module
/// \author  chenlee (2005.9.30)
// *****************************************************************************

///
/// Global renderer instance
///
#define gRenderer (::GN::gfx::Renderer::sGetInstance())

///
/// Pointer of global renderer instance
///
#define gRendererPtr (::GN::gfx::Renderer::sGetInstancePtr())

///
/// Implement static renderer data members
///
#define GN_IMPLEMENT_RENDERER_STATIC_MEMBERS() \
    GN_PUBLIC ::GN::Signal0<bool> GN::gfx::Renderer::sSigCreate; \
    GN_PUBLIC ::GN::Signal0<bool> GN::gfx::Renderer::sSigRestore; \
    GN_PUBLIC ::GN::Signal0<void> GN::gfx::Renderer::sSigDispose; \
    GN_PUBLIC ::GN::Signal0<void> GN::gfx::Renderer::sSigDestroy; \
    GN_PUBLIC ::GN::Signal0<void> GN::gfx::Renderer::sSigWindowClosing;

namespace GN { namespace gfx
{
    ///
    /// Renderer option structure.
    ///
    /// \sa Renderer::getOptions()
    ///
    struct RendererOptions
    {
        ///
        /// Display handle. No use on platform other than X Window. Default is zero.
        ///
        /// \note If zero, then default display will be used.
        ///
        HandleType displayHandle;

        ///
        /// Use external render window or not.
        /// ȱʡΪfalse.
        ///
        bool useExternalWindow;

        /// \name these fields are valid only when 'useExternalWindow' is true.
        ///@{

        ///
        /// Handle of external render window.
        /// ȱʡΪ0.
        ///
        /// \note Effective only if useExternalWindow is true.
        ///
        HandleType renderWindow;

        //@}

        /// \name these fields are valid only when 'useExternalWindow' is false.
        ///@{

        ///
        /// Handle of parent window. Default is zero, means a top-level window.
        ///
        HandleType parentWindow;

        ///
        /// Monitor handle.
        ///
        /// - Should be HMONITOR on MS Window or pointer to Screen structure on X Windows.
        /// - 0 means using the monitor where parent and/or render window stays in.
        ///   If monitorHandle and parent window are both zero, primary monitor will be used.
        /// - ȱʡΪ0.
        ///
        HandleType monitorHandle;

        //@}

        ///
        /// �Ƿ������Ⱦ���ڵĴ�С�����Զ�����Ⱦ�豸����Ӧ�ĵ���
        /// ȱʡΪtrue.
        ///
        bool trackWindowSizeMove;

        ///
        /// Display mode for fullscreen mode. Ignored in windowed mode.
        ///
        /// \note For field equals zero, current display setting will be used.
        ///
        DisplayMode displayMode;

        ///
        /// Backbuffer width for windowed mode. Ignored in fullscreen mode.
        /// Zero means using client width of render window. If render window
        /// is also not avaiable, default width 640 will be used.
        /// ȱʡΪ0.
        ///
        UInt32 windowedWidth;

        ///
        /// Backbuffer height for windowed mode. Ignored in fullscreen mode.
        /// Zero means using client height of render window. If render window
        /// is also not avaiable, default height 480 will be used.
        /// ȱʡΪ0.
        ///
        UInt32 windowedHeight;

        ///
        /// Msaa type
        ///
        MsaaType msaa;

        ///
        /// fullscreen or windowed mode.
        /// ȱʡΪfalse.
        ///
        bool fullscreen;

        ///
        /// �Ƿ�ͬ��ˢ��. ȱʡΪfalse.
        ///
        bool vsync;

        /// \name D3D only parameters
        //@{
        bool software;    ///< use software device. ȱʡΪfalse.
        bool reference;   ///< use reference device. ȱʡΪfalse.
        bool pure;        ///< use pure device. Default is false.
        bool multithread; ///< use multi-thread safe deivce. Default is false.
        //@}

        /// \name OGL only parameters
        //@{

        ///
        /// Restore display mode while render window is deactivated.
        ///
        /// Note that this is a OGL only parameter. For D3D, you may use
        /// "Enable Multi-mon Debugging" option in DirectX control panel,
        /// and startup your application through a debugger to make that
        /// option effective.
        /// ȱʡΪtrue.
        ///
        bool autoRestore;

        //@}

        ///
        /// Construct default render options
        ///
        RendererOptions()
            : displayHandle(0)
            , useExternalWindow(false)
            , renderWindow(0)
            , parentWindow(0)
            , monitorHandle(0)
            , trackWindowSizeMove(true)
            , windowedWidth(0)
            , windowedHeight(0)
            , msaa(MSAA_NONE)
            , fullscreen(false)
            , vsync(false)
            , software(false)
            , reference(false)
            , pure(false)
            , autoRestore(true)
        {
            displayMode.set(0,0,0,0);
        }
    };

    ///
    /// Display descriptor.
    ///
    /// \sa RendererOptions, Renderer::getDispDesc()
    ///
    struct DispDesc
    {
        HandleType displayHandle;    ///< Display handle. For X Window only.
        HandleType windowHandle;     ///< Render window handle
        HandleType monitorHandle;    ///< Monitor handle.
        UInt32 width;              ///< Back buffer width
        UInt32 height;             ///< Back buffer height
        UInt32 depth;              ///< Back buffer depth
        UInt32 refrate;            ///< Screen refresh rate

        ///
        /// equality operator
        ///
        bool operator!=( const DispDesc & rhs ) const
        {
            if( this == &rhs ) return false;
            return
                displayHandle != rhs.displayHandle ||
                windowHandle != rhs.windowHandle ||
                monitorHandle != rhs.monitorHandle ||
                width != rhs.width ||
                height != rhs.height ||
                depth != rhs.depth ||
                refrate != rhs.refrate;
        }
    };

    /// \def GNGFX_CAPS
    /// Define renderer caps

    ///
    /// renderer caps
    ///
    enum RendererCaps
    {
        #define GNGFX_CAPS(X) CAPS_##X,
        #include "rendererCapsMeta.h"
        #undef GNGFX_CAPS
        NUM_RENDERER_CAPS,
        CAPS_INVALID
    };

    ///
    /// ������־
    ///
    enum ClearFlag
    {
        CLEAR_C      = 1,     ///< clear color buffer
        CLEAR_Z      = 2,     ///< clear z buffer
        CLEAR_S      = 4,     ///< clear stencil buffer
        CLEAR_ALL    = 7      ///< clear all buffers
    };

    ///
    /// Options for Renderer::drawQuads
    ///
    enum DrawQuadOptions
    {
        ///
        /// ʹ�õ�ǰ����Ⱦ״̬��
        ///
        /// By default, Renderer::drawQuads() will use a special render state block that
        /// suites for transparent quads:
        ///   - enable blending
        ///   - enable depth testing
        ///   - disable depth writing
        ///
        DQ_USE_CURRENT_RS = 1<<0,

        ///
        /// ʹ�õ�ǰ��Vertex Shader��
        ///
        /// - ȱʡ����£�Renderer::drawQuads() ��ʹ��һ�����õ�vertex shader
        /// - �Զ����vertex shaderӦ����һ��2D�ռ������һ��2D��ͼ���ꡣ
        ///
        DQ_USE_CURRENT_VS = 1<<1,

        ///
        /// ʹ�õ�ǰ��Pixel Shader��
        ///
        /// ȱʡ����£�Renderer::drawQuads() ��ʹ��һ�����õ�Pixel Shader��ֱ��ֱ�����
        /// ��0����ͼ����ɫ��
        ///
        DQ_USE_CURRENT_PS = 1<<2,

        ///
        /// ʹ�õ�ǰ��Texture states.
        ///
        /// Effective only when using fixed function pipeline.
        ///
        DQ_USE_CURRENT_TS = 1<<3,

        ///
        /// position in window (post-transformed) space:
        /// (0,0) for left-up corner, (width,height) for right-bottom corner.
        ///
        /// By default, quad positios are in screen space. That is:
        /// (0,0) for left-up of the screen, and (1,1) for right-bottom of the screen)
        ///
        /// \note This option is meaningful only when DQ_USE_CURRENT_VS is _NOT_ set.
        ///
        DQ_WINDOW_SPACE = 1<<4,

        ///
        /// Disable blending. Default is enabled.
        ///
        DQ_OPAQUE = 1<<6,

        ///
        /// Enable depth write. Default is disabled.
        /// Note thtat this option will implicitly enable DQ_DEPTH_ENABLE.
        ///
        DQ_UPDATE_DEPTH = 1<<7,

        ///
        /// Enable depth test. Default is disabled.
        ///
        DQ_DEPTH_ENABLE = 1<<8,

        ///
        /// ���� DQ_USE_CURRENT_XX �ļ���
        ///
        DQ_USE_CURRENT = DQ_USE_CURRENT_RS | DQ_USE_CURRENT_VS | DQ_USE_CURRENT_PS | DQ_USE_CURRENT_TS
    };

    ///
    /// Options for Renderer::drawLines
    ///
    enum DrawLineOptions
    {
        ///
        /// ʹ�õ�ǰ����Ⱦ״̬��
        ///
        /// By default, Renderer::drawLines() will use a special render state block that
        /// suites for colored lines
        ///
        DL_USE_CURRENT_RS = 1<<0,

        ///
        /// ʹ�õ�ǰ��Vertex Shader��
        ///
        /// - ȱʡ����£�Renderer::drawLines() ��ʹ��һ�����õ�vertex shader
        /// - �Զ����vertex shaderӦ����һ��3D�ռ�����
        ///
        DL_USE_CURRENT_VS = 1<<1,

        ///
        /// ʹ�õ�ǰ��Pixel Shader��
        ///
        /// ȱʡ����£�Renderer::drawLines() ��ʹ��һ�����õ�Pixel Shader
        ///
        DL_USE_CURRENT_PS = 1<<2,

        ///
        /// ʹ�õ�ǰ��Texture states.
        ///
        /// Effective only when using fixed function pipeline.
        ///
        DL_USE_CURRENT_TS = 1<<3,

        ///
        /// position in window (post-transformed) space:
        /// (0,0) for left-up corner, (width,height) for right-bottom corner.
        ///
        /// By default, line positions are in object space.
        ///
        /// \note This option is meaningful only when DL_USE_CURRENT_VS is _NOT_ set.
        ///
        DL_WINDOW_SPACE = 1<<4,

        ///
        /// Using line strip. By default input points are treated as line list.
        ///
        DL_LINE_STRIP = 1<<5,

        ///
        /// ���� DL_USE_CURRENT_XX �ļ���
        ///
        DL_USE_CURRENT = DL_USE_CURRENT_RS | DL_USE_CURRENT_VS | DL_USE_CURRENT_PS | DL_USE_CURRENT_TS
    };

    ///
    /// Define rendering API
    ///
    enum RendererAPI
    {
        API_OGL,   ///< OpenGL
        API_D3D9,  ///< D3D9
        API_D3D10, ///< D3D10
        API_FAKE,  ///< Fake API
        NUM_RENDERER_API, ///< Number of avaliable API.
        API_AUTO, ///< determine rendering API automatically.
    };

    ///
    /// gfxģ������ӿ���
    ///
    /// \nosubgrouping
    ///
    struct Renderer : public Singleton<Renderer>, public NoCopy
    {
        // ********************************************************************
        ///
        /// \name Renderer Signals
        ///
        /// - �źſ��Ա���δ�������һ�����ϸ������µ�˳��
        ///   <pre>
        ///                         +---------+
        ///                        \|/        |
        ///                         '         |
        ///   (start)-->create-->restore-->dispose-->destroy-->(end)
        ///               .                             |
        ///              /|\                            |
        ///               +-----------------------------+
        ///   </pre>
        /// - create������, �ض�������ŷ���һ��restore
        /// - �յ�create/restore�ź�˵����Ⱦ��ready to use��
        /// - �յ�destroy�źź���Ⱦ����context�ᱻ����Ϊȱʡֵ��
        /// - ��Щ�źŵı�׼ʹ�÷������£�
        ///   - �յ�create�źź�, ��������ͼ����Դ��
        ///   - �յ�restore�źź����ͼ����Դ�����ݣ���Ӵ��̶�ȡ��ͼ��ģ�͡�
        ///     - Ӧ���������ڴ˴����µ���Դ����Ϊ����ź��ڳ������������п���
        ///       ����δ�����
        ///     - ����������ﴴ��ͼ����Դ������Щ��ԴӦ���յ�dispose�źź��ͷ�
        ///   - �յ�dispose�źź�Ӧ�ͷ���restore�ź��д�������Դ��
        ///   - �յ�destroy�źź�ɾ�����е�ͼ����Դ
        ///
        // ********************************************************************

        //@{

        ///
        /// D3D/OGL device creation signal
        ///
        static GN_PUBLIC Signal0<bool> sSigCreate;

        ///
        /// Triggered after renderer is created or restored successfully from
        /// last dispose, and ready to use.
        ///
        /// (Re)load content of graphics resources.
        /// - Only lockable resources (texture, vertex/index buffer) that have
        ///   neither system-copy nor content loader need content reloading.
        /// - No need to reload shaders, render state blocks and VtxFmtHandle.
        ///
        static GN_PUBLIC Signal0<bool> sSigRestore;

        ///
        /// D3D/OGL device dispose signal
        ///
        static GN_PUBLIC Signal0<void> sSigDispose;

        ///
        /// D3D/OGL device destroy signal
        ///
        static GN_PUBLIC Signal0<void> sSigDestroy;

        ///
        /// ���û���ͼ�ر���Ⱦ����ʱ���������������ڵĹرհ�ť���߰�ALT-F4��
        ///
        /// This signal is useful when you want your application to quit when
        /// user click close button or press ALT-F4, while using internal
        /// render window.
        /// \par
        /// Note that if you igore this sigal, _NOTHING_ will happen. Internal
        /// render window will _NOT_ be closed. You can only close the internal
        /// render window by delete the renderer.
        /// \par
        /// When using external render window, this signall will be triggered
        /// as well, to make renderer behavior consistent. But normally, you
        /// should have external window messages handled already somewhere else
        /// in your code. So you may safely ignore this signal.
        ///
        static GN_PUBLIC Signal0<void> sSigWindowClosing;

        //@}

        // ********************************************************************
        //
        /// \name Device Manager
        //
        // ********************************************************************

        //@{

    private:

        RendererOptions mOptions;

    protected:

        ///
        /// Update private renderer option variable.
        ///
        void setOptions( const RendererOptions & ro ) { mOptions = ro; }

    public:

        ///
        /// Change renderer options.
        ///
        /// \param ro
        ///     new renderer options
        /// \param forceDeviceRecreation
        ///     force a full device recreation
        /// \note
        ///     - You must call this function at least once, to make renderer usable.
        ///     - This function may trigger sSigRestore and/or sSigDispose.
        ///
        virtual bool changeOptions( const RendererOptions & ro, bool forceDeviceRecreation = false ) = 0;

        ///
        /// Get renderer options
        ///
        const RendererOptions & getOptions() const { return mOptions; }

        //@}

        // ********************************************************************
        //
        /// \name Display Manager
        /// \note
        ///     We provide two functions to get API specific rendering context:
        ///     - Sometime, you want to run some API specific codes,
        ///       for debug, test or any other purpose.
        ///     - You may use these functions to detect the current API
        //
        // ********************************************************************

        //@{

    private:

        DispDesc mDispDesc;

    protected:

        ///
        /// Update private diplay decriptor
        ///
        void setDispDesc( const DispDesc & desc )
        {
            mDispDesc = desc;
        }

    public:

        ///
        /// ��õ�ǰ����Ⱦ���ھ��
        ///
        const DispDesc & getDispDesc() const { return mDispDesc; }

        ///
        /// For D3D, return pointer to current D3D device; for OGL, return NULL.
        ///
        virtual void * getD3DDevice() const = 0;

        ///
        /// For OGL, return current rendering context; for D3D, return NULL.
        ///
        virtual void * getOGLRC() const = 0;

        //@}

        // ********************************************************************
        //
        /// \name Capabilities Manager
        //
        // ********************************************************************

        //@{

    protected:

        UInt32 mCaps[NUM_RENDERER_CAPS];

    public:

        ///
        /// Get render device caps
        ///
        UInt32 getCaps( SInt32 c ) const { GN_ASSERT( 0 <= c && c < NUM_RENDERER_CAPS ); return mCaps[c]; }

        ///
        /// Check renderer support to specific shader profile. Profile tag could be:
        ///     vs_1_1 vs_2_0 vs_2_x vs_3_0 xvs
        ///     ps_1_1 ps_1_2 ps_1_3 ps_1_4 ps_2_0 ps_2_x ps_3_0 xps
        ///     arbvp1, arbfp1,
        ///     glslvs, glslps,
        ///     cgvs, cgps
        ///
        /// \note Profile tag is case sensitive.
        ///
        virtual bool supportShader( const StrA & ) = 0;

        ///
        /// Test compability of specific texture format
        ///
        virtual bool supportTextureFormat( TexType type, BitFields usage, ClrFmt format ) const = 0;

        //@}

        // ********************************************************************
        //
        /// \name Resource Manager
        //
        // ********************************************************************

        //@{

        ///
        /// Create shader. Parameter 'entry' will be ignored for low-level shading language.
        ///
        /// \param type
        ///     Shader type
        /// \param lang
        ///     Shading language
        /// \param code
        ///     Shader code
        /// \param hints
        ///     Shader compilation hints. Hints string must be in format that can be imported
        ///     into a registry object. See Registry::importFromStr() for details.
        ///     \par
        ///     For D3D shader, several hints are supported:
        ///     - "entry": specify entry function name, default is "main"
        ///     - "target": specify HLSL compile target, default is empty, means using the highest possible target.
        ///     - "sm30": favor shader model 3.0 or not, default is yes.
        ///       - In DirectX, VS 3.0 and PS 3.0 can _ONLY_ be used with each other. So sometimes, you may want
        ///         your HLSL shader to be compiled into VS/PS 2.x. So it can be used with other non-SM3 shaders.
        ///       - This hints has higher priority then "target". So if you set "target" to "vs_3_0", while setting
        ///         this hint to "true". The shader will be compiled to "vs_2_a".
        ///
        virtual Shader *
        createShader( ShaderType type, ShadingLanguage lang, const StrA & code, const StrA & hints = "" ) = 0;

        ///
        /// Create vetex shader.
        ///
        Shader *
        createVS( ShadingLanguage lang, const StrA & code, const StrA & hints = "" );

        ///
        /// Create pixel shader.
        ///
        Shader *
        createPS( ShadingLanguage lang, const StrA & code, const StrA & hints = "" );

        ///
        /// Create new texture
        /// See TextureDesc for detail explaination of each fields in descriptor.
        ///
        virtual Texture *
        createTexture( const TextureDesc & desc, const TextureLoader & loader = TextureLoader() ) = 0;

        ///
        /// Load texture from file
        ///
        virtual Texture *
        createTextureFromFile( File & file ) = 0;

        ///
        /// Create new texture, with individual creation parameters.
        ///
        Texture *
        createTexture( TexType   type,
                       size_t    sx, size_t sy, size_t sz,
                       size_t    faces = 0,
                       size_t    levels = 0,
                       ClrFmt    format = FMT_DEFAULT,
                       BitFields usage = 0,
                       bool      tiled = false,
                       const TextureLoader & loader = TextureLoader() )
        {
            TextureDesc desc = { type, (UInt32)sx, (UInt32)sy, (UInt32)sz, (UInt32)faces, (UInt32)levels, format, usage, tiled };
            return createTexture( desc, loader );
        }

        ///
        /// Create 1D texture
        ///
        Texture *
        create1DTexture( size_t    sx,
                         size_t    levels = 0,
                         ClrFmt    format = FMT_DEFAULT,
                         BitFields usage = 0,
                         bool      tiled = false,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_1D, sx, 0, 0, 1, levels, format, usage, tiled, loader );
        }

        ///
        /// Create 2D texture
        ///
        Texture *
        create2DTexture( size_t    sx, size_t sy,
                         size_t    levels = 0,
                         ClrFmt    format = FMT_DEFAULT,
                         BitFields usage = 0,
                         bool      tiled = false,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_2D, sx, sy, 0, 1, levels, format, usage, tiled, loader );
        }

        ///
        /// Create 3D texture
        ///
        Texture *
        create3DTexture( size_t    sx, size_t sy, size_t sz,
                         size_t    levels = 0,
                         ClrFmt    format = FMT_DEFAULT,
                         BitFields usage = 0,
                         bool      tiled = false,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_3D, sx, sy, sz, 1, levels, format, usage, tiled, loader );
        }

        ///
        /// Create CUBE texture
        ///
        Texture *
        createCubeTexture( size_t    sx,
                           size_t    levels = 0,
                           ClrFmt    format = FMT_DEFAULT,
                           BitFields usage = 0,
                           bool      tiled = false,
                           const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_CUBE, sx, 0, 0, 6, levels, format, usage, tiled, loader );
        }

        ///
        /// Create vertex format handle.
        ///
        virtual VtxFmtHandle createVtxFmt( const VtxFmtDesc & ) = 0;

        ///
        /// Create new vertex buffer
        ///
        /// \param bytes
        ///     Size of vertex buffer in bytes.
        /// \param dynamic
        ///     Dynamic or static vertex buffer.
        /// \param sysCopy
        ///     has system copy or not
        /// \param loader
        ///     Optional content loader.
        ///
        virtual VtxBuf *
        createVtxBuf( size_t bytes,
                      bool   dynamic = false,
                      bool   sysCopy = true,
                      const  VtxBufLoader & loader = VtxBufLoader() ) = 0;

        ///
        /// Create new index buffer
        ///
        /// \param numIdx
        ///     number of indices
        /// \param dynamic
        ///     Dynamic or static vertex buffer.
        /// \param sysCopy
        ///     has system copy or not
        /// \param loader
        ///     Optional content loader.
        ///
        /// \note
        ///     ÿ�������̶�ռ��16bit
        ///
        virtual IdxBuf *
        createIdxBuf( size_t numIdx,
                      bool   dynamic = false,
                      bool   sysCopy = true,
                      const  IdxBufLoader & loader = IdxBufLoader() ) = 0;

        //@}

        // ********************************************************************
        //
        /// \name Context Manager
        //
        // ********************************************************************

        //@{

    public:

        ///
        /// Set rendering context.
        ///
        /// This function, with pre-initialized renderer context structure, is
        /// always prefered over context update helper functions below, for
        /// better performance.
        ///
        virtual void setContext( const RendererContext & ) = 0;

        ///
        /// Rebind current rendering context to rendering device.
        ///
        /// \par
        ///     This function will rebind current rendering context to renderer.
        /// \par
        ///     Renderer class have internal cache mechanism to avoid
        ///     redunant state changing. But if you call D3D/OGL functions
        ///     directly in your code that changes D3D/OGL states, this cache
        ///     mechanism will be broken. One way to avoid this situation, is
        ///     to store/restore D3D/OGL states by yourself. Another way is to
        ///     call this function to force rebinding of current renderer
        ///     context, which is much easier and less error prone.
        ///
        virtual void rebindContext( RendererContext::FieldFlags ) = 0;

        ///
        /// Get current render state block descriptor
        ///
        virtual const RenderStateBlockDesc & getCurrentRenderStateBlock() const = 0;

        // ********************************************************************
        ///
        /// \name Helper functions to update rendering context.
        ///
        /// - See corresponding methods in RendererContext for detail usage.
        /// - Recommended call sequence is:
        /// <pre>
        ///     contextUpdateBegin();
        ///     ... // call context update methods here.
        ///     contextUpdateEnd();
        /// </pre>
        /// - Calling update method outside of contextUpdateBegin() and
        ///   contextUpdateEnd() will take effect immediatly, but is not
        ///   recommented for performance reason.
        ///
        // ********************************************************************

        //@{

    private:

        RendererContext      mHelperContext;
        AutoInit<bool,false> mHelperContextUpdateBegun;

    public:

        ///
        /// start context update
        ///
        inline void contextUpdateBegin();

        ///
        /// end context update, flush modified context to renderer.
        ///
        inline void contextUpdateEnd();

        inline void setShader( ShaderType type, const Shader * shader );
        inline void setShaders( const Shader * vs, const Shader * ps, const Shader * gs );
        inline void setVS( const Shader * s );
        inline void setPS( const Shader * s );
        inline void setGS( const Shader * s );
        inline void setRenderStateBlock( const RenderStateBlockDesc & );
        inline void setRenderState( RenderState state, SInt32 value );
        inline void setRenderTargets( const RenderTargetDesc & );
        inline void setDrawToBackBuf();
        inline void setDrawToTexture( UInt32 count, const Texture * rt0, const Texture * rt1=0, const Texture * rt2=0, const Texture * rt3=0, const Texture * z=0, MsaaType aa=MSAA_NONE );
        inline void setViewport( const Rectf & );
        inline void setViewport( float left, float top, float width, float height );

        inline void setWorld( const Matrix44f & );
        inline void setView( const Matrix44f & );
        inline void setProj( const Matrix44f & );
        inline void setTextureStateBlock( const TextureStateBlockDesc & );
        inline void setTextureState( size_t stage, TextureState state, TextureStateValue value );

        inline void setTexture( size_t stage, const Texture * tex );
        inline void setTextures( const Texture * const texlist[], size_t start, size_t count );
        inline void setVtxFmt( VtxFmtHandle );
        inline void setVtxBuf( size_t index, const VtxBuf * buffer, size_t stride );
        inline void setIdxBuf( const IdxBuf * );

        //@}

        // ********************************************************************
        //
        /// \name Drawing Manager
        //
        // ********************************************************************

        //@{

    protected:

        AutoInit<size_t,0> mNumPrims;   ///< Number of primitives per frame.
        AutoInit<size_t,0> mNumBatches; ///< Number of draws per frame.

    public :

        ///
        /// ��ʼ��ͼ����.
        ///
        /// ���еĻ�ͼ������������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual bool drawBegin() = 0;

        ///
        /// ������ͼ����. Similar as EndScene() followed by Present()
        ///
        /// call drawEnd() <b>if and only if</b> drawBegin() returns true.
        ///
        virtual void drawEnd() = 0;

        ///
        /// ��������
        ///
        /// \param flags ������־, see ClearFlag
        /// \param c     ����ɫ
        /// \param z     ���ֵ
        /// \param s     ģ��ֵ
        ///
        /// \note Must be called btween drawBegin() and drawEnd().
        ///
        virtual void
        clearScreen( const Vector4f & c = Vector4f(0,0,0,1),
                     float z = 1.0f, UInt32 s = 0,
                     BitFields flags = CLEAR_ALL ) = 0;

        ///
        /// Draw indexed primitives.
        ///
        /// \param prim
        ///     primititive type
        /// \param numPrims
        ///     number of primitives
        /// \param startVtx
        ///     vertex index into vertex buffer that index "0" will be refering to.
        /// \param minVtxIdx, numVtx
        ///     define effective range in vertex buffer, starting from startVtx.
        /// \param startIdx
        ///     index into index buffer of the first index
        ///
        /// \note ������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numPrims,
                                  size_t        startVtx,
                                  size_t        minVtxIdx,
                                  size_t        numVtx,
                                  size_t        startIdx ) = 0;

        ///
        /// Draw non-indexed primitives.
        ///
        /// \param prim
        ///     primititive type
        /// \param numPrims
        ///     number of primitives
        /// \param startVtx
        ///     index into vertex buffer of the first vertex.
        ///
        /// \note ������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual void draw( PrimitiveType prim,
                           size_t        numPrims,
                           size_t        startVtx ) = 0;

        ///
        /// draw on-indexed primitives with user-defined data array
        ///
        virtual void drawIndexedUp(
                             PrimitiveType    prim,
                             size_t           numPrims,
                             size_t           numVertices,
                             const void *     vertexData,
                             size_t           strideInBytes,
                             const UInt16 * indexData ) = 0;

        ///
        /// draw on-indexed primitives with user-defined data array
        ///
        virtual void drawUp( PrimitiveType prim,
                             size_t        numPrims,
                             const void *  vertexData,
                             size_t        strideInBytes ) = 0;

        ///
        /// Draw quads
        ///
        /// \param options
        ///     ��Ⱦѡ���� DrawQuadOptions��Set to 0 to use default options
        /// \param positions, posStride
        ///     �����������ݣ���һϵ�е�3D������ɡ�4�������ʾһ�����Ρ�
        ///     ѡ�� DQ_WINDOW_SPACE ��Ӱ������ĺ��塣
        ///     Note "posStride" is stride of one vertex.
        /// \param texcoords, texStride
        ///     ��ͼ�������飬��һϵ�е�2D������ɡ�4�������ʾһ�����Ρ�
        ///     Note "texStride" is stride of one vertex.
        ///     Specify texcoords to NULL, if you want non-textured quad.
        ///     texStride is be ignored in this case.
        /// \param colors, clrStride
        ///     ������ɫ���飬��һϵ�е�BGRA32��ɫֵ��ɡ�4�������ʾһ�����Ρ�
        ///     Note "clrStride" is stride of one vertex.
        ///     Set colors to NULL, to specify pure white for all vertices.
        /// \param count
        ///     Number of quads.
        ///
        virtual void drawQuads(
            BitFields options,
            const void * positions, size_t posStride,
            const void * texcoords, size_t texStride,
            const void * colors, size_t clrStride,
            size_t count ) = 0;

        ///
        /// Draw quads, with same stride for positions, texcoords and colors
        ///
        void drawQuads(
            BitFields options,
            const void * positions, const void * texcoords, const void * colors, size_t stride,
            size_t count )
        {
            drawQuads( options, positions, stride, texcoords, stride, colors, stride, count );
        }

        ///
        /// Draw single 2D textured quad.
        ///
        void draw2DTexturedQuad(
            BitFields options,
            double z = 0.0,
            double left = 0.0, double top = 0.0, double right = 1.0, double bottom = 1.0,
            double leftU = 0.0, double topV = 0.0, double rightU = 1.0, double bottomV = 1.0 )
        {
            float x1 = (float)left;
            float y1 = (float)top;
            float x2 = (float)right;
            float y2 = (float)bottom;
            float u1 = (float)leftU;
            float v1 = (float)topV;
            float u2 = (float)rightU;
            float v2 = (float)bottomV;
            float zz = (float)z;

            Vector3f pos[4];
            Vector3f tex[4];

            pos[0].set( x1, y1, zz );
            pos[1].set( x2, y1, zz );
            pos[2].set( x2, y2, zz );
            pos[3].set( x1, y2, zz );

            tex[0].set( u1, v1, zz );
            tex[1].set( u2, v1, zz );
            tex[2].set( u2, v2, zz );
            tex[3].set( u1, v2, zz );

            drawQuads( options, pos, sizeof(Vector3f), tex, sizeof(Vector3f), 0, 0, 1 );
        }

        ///
        /// Draw single 2D solid quad.
        ///
        void draw2DSolidQuad(
            BitFields options,
            double z,
            double left = 0.0, double top = 0.0, double right = 1.0, double bottom = 1.0,
            UInt32 color = 0xFFFFFFFF )
        {
            float x1 = (float)left;
            float y1 = (float)top;
            float x2 = (float)right;
            float y2 = (float)bottom;
            float zz = (float)z;

            Vector3f pos[4];
            pos[0].set( x1, y1, zz );
            pos[1].set( x2, y1, zz );
            pos[2].set( x2, y2, zz );
            pos[3].set( x1, y2, zz );

            UInt32 colors[] = { color, color, color, color };

            drawQuads( options, pos, sizeof(Vector3f), 0, 0, colors, sizeof(UInt32), 1 );
        }

        ///
        /// Draw line segments
        ///
        /// \param options
        ///     ��Ⱦѡ���� DrawLineOptions��Set to 0 to use default options
        /// \param positions
        ///     �����������ݣ���һϵ�еĶ�����ɡ�2�������ʾһ�����Ρ�
        ///     ѡ�� DQ_WINDOW_SPACE �� DQ_3D_POSITION ��Ӱ������ĺ��塣
        /// \param stride
        ///     stride of one vertex.
        /// \param count
        ///     Number of line segments (note: _NOT_ number of points)
        /// \param color
        ///     line color. in FMT_BGRA32 format.
        /// \param model, view, proj
        ///     Transformation matrices. Ignored when using DL_WINDOW_SPACE.
        ///
        virtual void drawLines(
            BitFields options,
            const void * positions,
            size_t stride,
            size_t count,
            UInt32 color,
            const Matrix44f & model,
            const Matrix44f & view,
            const Matrix44f & proj ) = 0;

        ///
        /// ����Ļ��ָ����λ�û���2D�ַ���.
        ///
        /// - Ŀǰֻ����Ӣ���ַ����������ٶȽ�������ҪΪ���Զ��á�
        ///   ���ֵĸ߶ȹ̶�Ϊ14�����أ����Ϊ8�����ء�
        /// - ������ drawBegin() �� drawEnd() ֮�����
        ///
        /// \param text  �����ƶ��ַ���
        /// \param x, y  ��һ���ַ����ϽǵĴ�������
        ///              - ��Ļ���Ͻ�Ϊ(0,0)�����½�Ϊ(width,height)
        ///              - �����Ƶ��ַ���������λ���Ե�һ���ַ������Ͻ�Ϊ׼��
        /// \param color ������ɫ
        ///
        virtual void
        drawDebugText( const char * text, int x, int y,
                       const Vector4f & color = Vector4f(1,1,1,1) ) = 0;

        ///
        /// ������һ�� drawEnd() �����������Ƶ�ԭ��ĸ���
        ///
        size_t getNumPrimitives() const { return mNumPrims; }

        ///
        /// ������һ�� drawEnd() ������ draw() / drawindexed() �Ĵ���
        ///
        size_t getNumBatches() const { return mNumBatches; }

        ///
        /// ������һ�� drawEnd() ������ƽ��ÿ�� draw()/drawIndexed() ��ԭ����
        ///
        size_t getNumPrimsPerBatch() const
        {
            return 0 == mNumBatches ? 0 : mNumPrims / mNumBatches;
        }

        //@}

        // ********************************************************************
        //
        /// \name Misc. utilities
        //
        // ********************************************************************

        //@{

    private:

#if GN_DEBUG_BUILD
        AutoInit<bool,true> mEnableParameterCheck;
#else
        AutoInit<bool,false> mEnableParameterCheck;
#endif

    public:

        ///
        /// This function is provided because different API has different ways
        /// to compose projection matrix.
        ///
        Matrix44f &
        composePerspectiveMatrixLh( Matrix44f & result,
                                    float fovy,
                                    float ratio,
                                    float znear,
                                    float zfar ) const
        {
            return getD3DDevice()
                ? result.perspectiveD3DLh( fovy, ratio, znear, zfar )
                : result.perspectiveOGLLh( fovy, ratio, znear, zfar );
        }

        ///
        /// This function is provided because different API has different ways
        /// to compose projection matrix.
        ///
        Matrix44f &
        composePerspectiveMatrixRh( Matrix44f & result,
                                    float fovy,
                                    float ratio,
                                    float znear,
                                    float zfar ) const
        {
            return getD3DDevice()
                ? result.perspectiveD3DRh( fovy, ratio, znear, zfar )
                : result.perspectiveOGLRh( fovy, ratio, znear, zfar );
        }

        ///
        /// This function is provided because different API has different ways
        /// to compose projection matrix.
        ///
        Matrix44f &
        composePerspectiveMatrix( Matrix44f & result,
                                  float fovy,
                                  float ratio,
                                  float znear,
                                  float zfar ) const
        {
            return getD3DDevice()
                ? result.perspectiveD3D( fovy, ratio, znear, zfar )
                : result.perspectiveOGL( fovy, ratio, znear, zfar );
        }

        ///
        /// This function is provided because different API has different ways
        /// to compose projection matrix.
        ///
        Matrix44f &
        composeOrthoMatrix( Matrix44f & result,
                            float left,
                            float bottom,
                            float width,
                            float height,
                            float znear,
                            float zfar ) const
        {
            return getD3DDevice()
                ? result.orthoD3D( left, left+width, bottom, bottom+height, znear, zfar )
                : result.orthoOGL( left, left+width, bottom, bottom+height, znear, zfar );
        }

        ///
        /// Enable/Disable parameter check for performance critical functions.
        ///
        /// Enabled by default for debug build; disabled by default for release build.
        ///
        void enableParameterCheck( bool enable ) { mEnableParameterCheck = enable; }

        ///
        /// Get parameter check flag.
        ///
        bool parameterCheckEnabled() const { return mEnableParameterCheck; }

        ///
        /// Dump current renderer state to string. For debug purpose only.
        ///
        virtual void dumpCurrentState( StrA & ) const = 0;

        //@}

        // ********************************************************************
        //
        /// \name Ctor / dtor
        //
        // ********************************************************************

        //@{

    protected:

        ///
        /// ctor
        ///
        Renderer() : mSharedLib(0) {}

        ///
        /// Dtor
        ///
        virtual ~Renderer() {}

        //@}

        // ********************************************************************
        //
        /// \name Instance Manager
        //
        // ********************************************************************

        //@{

    private:

        SharedLib * mSharedLib;
        friend Renderer * createRenderer( RendererAPI );
        friend void deleteRenderer();

        //@}
    };

    ///
    /// (Re)Create a renderer.
    ///
    /// This function will release old renderer, then create a new one with new settings.
    ///
    Renderer * createRenderer( RendererAPI = API_AUTO );

    ///
    /// Delete renderer
    ///
    void deleteRenderer();
}}

#include "renderer.inl"

// *****************************************************************************
//                           End of renderer.h
// *****************************************************************************
#endif // __GN_GFX_RENDERER_H__
