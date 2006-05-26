#ifndef __GN_GFX_RENDERER_H__
#define __GN_GFX_RENDERER_H__
// *****************************************************************************
//! \file    renderer.h
//! \brief   Main renderer interface of GFX module
//! \author  chenlee (2005.9.30)
// *****************************************************************************

//!
//! Global renderer instance
//!
#define gRenderer (::GN::gfx::Renderer::sGetInstance())

//!
//! Pointer of global renderer instance
//!
#define gRendererPtr (::GN::gfx::Renderer::sGetInstancePtr())

//!
//! Implement static renderer data members
//!
#define GN_IMPLEMENT_RENDERER_STATIC_MEMBERS() \
    GN_PUBLIC ::GN::Signal0<bool> GN::gfx::Renderer::sSigCreate; \
    GN_PUBLIC ::GN::Signal0<bool> GN::gfx::Renderer::sSigRestore; \
    GN_PUBLIC ::GN::Signal0<void> GN::gfx::Renderer::sSigDispose; \
    GN_PUBLIC ::GN::Signal0<void> GN::gfx::Renderer::sSigDestroy; \
    GN_PUBLIC ::GN::Signal0<void> GN::gfx::Renderer::sSigWindowClosing;

namespace GN { namespace gfx
{
    //!
    //! Renderer option structure.
    //!
    //! \sa Renderer::getOptions()
    //!
    struct RendererOptions
    {
        //!
        //! Display handle. No use on platform other than X Window. Default is zero.
        //!
        //! \note If zero, then default display will be used.
        //!
        HandleType displayHandle;

        //!
        //! Use external render window or not.
        //! ȱʡΪfalse.
        //!
        bool useExternalWindow;

        union
        {
            //!
            //! Handle of external render window.
            //! ȱʡΪ0.
            //!
            //! \note Effective only if useExternalWindow is true.
            //!
            HandleType renderWindow;

            //!
            //! Handle of parent window. Can be NULL.
            //! ȱʡΪ0.
            //!
            //! \note Effective only if useExternalWindow is false.
            //!
            void * parentWindow;
        };

        //!
        //! Monitor handle, effective only if useExternalWindow is false.
        //!
        //! - Should be HMONITOR on MS Window or pointer to Screen structure on X Windows.
        //! - 0 means using the monitor where parent window stays in. If monitorHandle
        //!   and parent window are both zero, primary monitor will be used.
        //! - ȱʡΪ0.
        //!
        HandleType monitorHandle;

        //!
        //! �Ƿ������Ⱦ���ڵĴ�С�����Զ�����Ⱦ�豸����Ӧ�ĵ���
        //! ȱʡΪtrue.
        //!
        bool autoBackbufferResizing;

        //!
        //! Display mode for fullscreen mode. Ignored in windowed mode.
        //!
        //! \note For field equals zero, current display setting will be used.
        //!
        DisplayMode displayMode;

        //!
        //! Backbuffer width for windowed mode. Ignored in fullscreen mode.
        //! Zero means using client width of render window. If render window
        //! is also not avaiable, default width 640 will be used.
        //! ȱʡΪ0.
        //!
        uint32_t windowedWidth;

        //!
        //! Backbuffer height for windowed mode. Ignored in fullscreen mode.
        //! Zero means using client height of render window. If render window
        //! is also not avaiable, default height 480 will be used.
        //! ȱʡΪ0.
        //!
        uint32_t windowedHeight;

        //!
        //! Msaa type
        //!
        MsaaType msaa;

        //!
        //! fullscreen or windowed mode.
        //! ȱʡΪfalse.
        //!
        bool fullscreen;

        //!
        //! �Ƿ�ͬ��ˢ��. ȱʡΪfalse.
        //!
        bool vsync;

        //! \name D3D only parameters
        //@{
        bool software;    //!< use software device. ȱʡΪfalse.
        bool reference;   //!< use reference device. ȱʡΪfalse.
        bool pure;        //!< use pure device. Default is false.
        bool multithread; //!< use multi-thread safe deivce. Default is false.
        //@}

        //! \name OGL only parameters
        //@{

        //!
        //! Restore display mode while render window is deactivated.
        //!
        //! Note that this is a OGL only parameter. For D3D, you may use
        //! "Enable Multi-mon Debugging" option in DirectX control panel,
        //! and startup your application through a debugger to make that
        //! option effective.
        //! ȱʡΪtrue.
        //!
        bool autoRestore;

        //@}

        //!
        //! Construct default render options
        //!
        RendererOptions()
            : displayHandle(0)
            , useExternalWindow(false)
            , parentWindow(0)
            , monitorHandle(0)
            , autoBackbufferResizing(true)
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

    //!
    //! Display descriptor.
    //!
    //! \sa RendererOptions, Renderer::getDispDesc()
    //!
    struct DispDesc
    {
        HandleType displayHandle;    //!< Display handle. For X Window only.
        HandleType windowHandle;     //!< Render window handle
        HandleType monitorHandle;    //!< Monitor handle.
        uint32_t width;              //!< Back buffer width
        uint32_t height;             //!< Back buffer height
        uint32_t depth;              //!< Back buffer depth
        uint32_t refrate;            //!< Screen refresh rate

        //!
        //! equality operator
        //!
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

    //! \def GNGFX_CAPS
    //! Define renderer caps

    //!
    //! renderer caps
    //!
    enum RendererCaps
    {
        #define GNGFX_CAPS(X) CAPS_##X,
        #include "rendererCapsMeta.h"
        #undef GNGFX_CAPS
        NUM_RENDERER_CAPS,
        CAPS_INVALID
    };

    //!
    //! ������־
    //!
    enum ClearFlag
    {
        CLEAR_C      = 1,     //!< clear color buffer
        CLEAR_Z      = 2,     //!< clear z buffer
        CLEAR_S      = 4,     //!< clear stencil buffer
        CLEAR_ALL    = 7      //!< clear all buffers
    };

    //!
    //! Options for Renderer::drawQuads
    //!
    enum DrawQuadOptions
    {
        //!
        //! ʹ�õ�ǰ����Ⱦ״̬��
        //!
        //! By default, Renderer::drawQuads() will use a special render state block that
        //! suites for transparent quads:
        //!   - enable blending
        //!   - enable depth testing
        //!   - disable depth writing
        //!
        DQ_USE_CURRENT_RS = 1<<0,

        //!
        //! ʹ�õ�ǰ��Vertex Shader��
        //!
        //! - ȱʡ����£�Renderer::drawQuads() ��ʹ��һ�����õ�vertex shader
        //! - �Զ����vertex shaderӦ����һ��2D�ռ������һ��2D��ͼ���ꡣ
        //!
        DQ_USE_CURRENT_VS = 1<<1,

        //!
        //! ʹ�õ�ǰ��Pixel Shader��
        //!
        //! ȱʡ����£�Renderer::drawQuads() ��ʹ��һ�����õ�Pixel Shader��ֱ��ֱ�����
        //! ��0����ͼ����ɫ��
        //!
        DQ_USE_CURRENT_PS = 1<<2,

        //!
        //! ʹ�õ�ǰ��Texture states.
        //!
        //! Effective only when using fixed function pipeline.
        //!
        DQ_USE_CURRENT_TS = 1<<3,

        //!
        //! position in window (post-transformed) space:
        //! (0,0) for left-up corner, (width,height) for right-bottom corner.
        //!
        //! By default, quad positios are in screen space. That is:
        //! (0,0) for left-up of the screen, and (1,1) for right-bottom of the screen)
        //!
        //! \note This option is meaningful only when DQ_USE_CURRENT_VS is _NOT_ set.
        //!
        DQ_WINDOW_SPACE = 1<<4,

        //!
        //! Use 3-D position. Default is 2-D position
        //!
        DQ_3D_POSITION = 1<<5,

        //!
        //! Disable blending. Default is enabled.
        //!
        DQ_OPAQUE = 1<<6,

        //!
        //! Enable depth write. Default is disabled.
        //!
        DQ_UPDATE_DEPTH = 1<<7,

        //!
        //! Enable depth test. Default is disabled.
        //!
        DQ_DEPTH_ENABLE = 1<<8,

        //!
        //! ���� DQ_USE_CURRENT_XX �ļ���
        //!
        DQ_USE_CURRENT = DQ_USE_CURRENT_RS | DQ_USE_CURRENT_VS | DQ_USE_CURRENT_PS | DQ_USE_CURRENT_TS
    };

    //!
    //! Options for Renderer::drawLines
    //!
    enum DrawLineOptions
    {
        //!
        //! ʹ�õ�ǰ����Ⱦ״̬��
        //!
        //! By default, Renderer::drawLines() will use a special render state block that
        //! suites for colored lines
        //!
        DL_USE_CURRENT_RS = 1<<0,

        //!
        //! ʹ�õ�ǰ��Vertex Shader��
        //!
        //! - ȱʡ����£�Renderer::drawLines() ��ʹ��һ�����õ�vertex shader
        //! - �Զ����vertex shaderӦ����һ��3D�ռ�����
        //!
        DL_USE_CURRENT_VS = 1<<1,

        //!
        //! ʹ�õ�ǰ��Pixel Shader��
        //!
        //! ȱʡ����£�Renderer::drawLines() ��ʹ��һ�����õ�Pixel Shader
        //!
        DL_USE_CURRENT_PS = 1<<2,

        //!
        //! ʹ�õ�ǰ��Texture states.
        //!
        //! Effective only when using fixed function pipeline.
        //!
        DL_USE_CURRENT_TS = 1<<3,

        //!
        //! position in window (post-transformed) space:
        //! (0,0) for left-up corner, (width,height) for right-bottom corner.
        //!
        //! By default, line positions are in object space.
        //!
        //! \note This option is meaningful only when DL_USE_CURRENT_VS is _NOT_ set.
        //!
        DL_WINDOW_SPACE = 1<<4,

        //!
        //! Using line strip. By default input points are treated as line list.
        //!
        DL_LINE_STRIP = 1<<5,

        //!
        //! ���� DL_USE_CURRENT_XX �ļ���
        //!
        DL_USE_CURRENT = DL_USE_CURRENT_RS | DL_USE_CURRENT_VS | DL_USE_CURRENT_PS | DL_USE_CURRENT_TS
    };

    //!
    //! Define rendering API
    //!
    enum RendererAPI
    {
        API_OGL,   //!< OpenGL
        API_D3D9,  //!< D3D9
        API_D3D10, //!< D3D10
        API_FAKE,  //!< Fake API
        NUM_RENDERER_API, //!< Number of avaliable API.
        API_AUTO, //!< determine rendering API automatically.
    };

    //!
    //! gfxģ������ӿ���
    //!
    //! \nosubgrouping
    //!
    struct Renderer : public Singleton<Renderer>, public NoCopy
    {
        // ********************************************************************
        //!
        //! \name Renderer Signals
        //!
        //! - �źſ��Ա���δ�������һ�����ϸ������µ�˳��
        //!   <pre>
        //!                         +---------+
        //!                        \|/        |
        //!                         '         |
        //!   (start)-->create-->restore-->dispose-->destroy-->(end)
        //!               .                             |
        //!              /|\                            |
        //!               +-----------------------------+
        //!   </pre>
        //! - create������, �ض�����һ��restore
        //! - �յ�create/restore�ź�˵����Ⱦ��ready to use��
        //! - �յ�destroy�źź���Ⱦ����context�ᱻ����Ϊȱʡֵ��
        //! - ��Щ�źŵı�׼ʹ�÷������£�
        //!   - �յ�create�źź�, ��������ͼ����Դ��
        //!   - �յ�restore�źź����ͼ����Դ�����ݣ���Ӵ��̶�ȡ��ͼ��ģ�͡�
        //!     - Ӧ���ⴴ���µ���Դ����Ϊ����ź��ڳ������������п��ܱ���δ�����
        //!     - �ڴ˴�������ԴӦ���յ�dispose�źź��ͷ�
        //!   - �յ�dispose�źź�Ӧ�ͷ���restore�ź��д�������Դ��
        //!   - �յ�destroy�źź�ɾ�����е�ͼ����Դ
        //!
        // ********************************************************************

        //@{

        //!
        //! D3D/OGL device creation signal
        //!
        static GN_PUBLIC Signal0<bool> sSigCreate;

        //!
        //! Triggered after renderer is created or restored from last dispose successfully, and ready to use.
        //!
        //! (Re)load content of graphics resources.
        //! - Only lockable resources (texture, vertex/index buffer) that have
        //!   neither system-copy nor content loader need content reloading.
        //! - No need to reload shaders, render state blocks and VtxFmtHandle.
        //!
        static GN_PUBLIC Signal0<bool> sSigRestore;

        //!
        //! D3D/OGL device dispose signal
        //!
        static GN_PUBLIC Signal0<void> sSigDispose;

        //!
        //! D3D/OGL device destroy signal
        //!
        static GN_PUBLIC Signal0<void> sSigDestroy;

        //!
        //! ���û���ͼ�ر���Ⱦ����ʱ���������������ڵĹرհ�ť���߰�ALT-F4��
        //!
        //! This signal is useful when you want application to quit when user click close button or
        //! press ALT-F4, while using internal render window.
        //! \par
        //! Note that if you igore this sigal, _NOTHING_ will happen. Internal render window will
        //! _NOT_ be closed. You can only close the internal render window by delete the renderer.
        //! \par
        //! This signall will be triggered as well, when using external render window, to make the renderer
        //! behavior consistent. But normally, you should have handled external window messages somewhere
        //! else. If that's the case, then you can safely ignore this signal.
        //!
        static GN_PUBLIC Signal0<void> sSigWindowClosing;

        //@}

        // ********************************************************************
        //
        //! \name Device Manager
        //
        // ********************************************************************

        //@{

    private:

        RendererOptions mOptions;

    protected:

        //!
        //! Update private renderer option variable.
        //!
        void setOptions( const RendererOptions & ro ) { mOptions = ro; }

    public:

        //!
        //! Change renderer options.
        //!
        //! \param ro
        //!     new renderer options
        //! \param forceDeviceRecreation
        //!     force a full device recreation
        //! \note
        //!     - You must call this function at least once, to make renderer usable.
        //!     - This function may trigger sSigRestore and/or sSigDispose.
        //!
        virtual bool changeOptions( const RendererOptions & ro, bool forceDeviceRecreation = false ) = 0;

        //!
        //! Get renderer options
        //!
        const RendererOptions & getOptions() const { return mOptions; }

        //@}

        // ********************************************************************
        //
        //! \name Display Manager
        //! \note
        //!     We provide two functions to get API specific rendering context:
        //!     - Sometime, you want to run some API specific codes,
        //!       for debug, test or any other purpose.
        //!     - You may use these functions to detect the current API
        //
        // ********************************************************************

        //@{

    private:

        DispDesc mDispDesc;

    protected:

        //!
        //! Update private diplay decriptor
        //!
        void setDispDesc( const DispDesc & desc )
        {
            mDispDesc = desc;
        }

    public:

        //!
        //! ��õ�ǰ����Ⱦ���ھ��
        //!
        const DispDesc & getDispDesc() const { return mDispDesc; }

        //!
        //! For D3D, return pointer to current D3D device; for OGL, return NULL.
        //!
        virtual void * getD3DDevice() const = 0;

        //!
        //! For OGL, return current rendering context; for D3D, return NULL.
        //!
        virtual void * getOGLRC() const = 0;

        //@}

        // ********************************************************************
        //
        //! \name Capabilities Manager
        //
        // ********************************************************************

        //@{

    protected:

        uint32_t mCaps[NUM_RENDERER_CAPS];

    public:

        //!
        //! Get render device caps
        //!
        uint32_t getCaps( int32_t c ) const { GN_ASSERT( 0 <= c && c < NUM_RENDERER_CAPS ); return mCaps[c]; }

        //!
        //! Check renderer support to specific shader profile. Profile tag could be:
        //!     vs_1_1 vs_2_0 vs_2_x vs_3_0 xvs
        //!     ps_1_1 ps_1_2 ps_1_3 ps_1_4 ps_2_0 ps_2_x ps_3_0 xps
        //!     arbvp1, arbfp1,
        //!     glslvs, glslps,
        //!
        //! \note Profile tag is case sensitive.
        //!
        virtual bool supportShader( const StrA & ) = 0;

        //!
        //! Test compability of specific texture format
        //!
        virtual bool supportTextureFormat( TexType type, BitField usage, ClrFmt format ) const = 0;

        //@}

        // ********************************************************************
        //
        //! \name Resource Manager
        //
        // ********************************************************************

        //@{

        //!
        //! Create shader. Parameter 'entry' will be ignored for low-level shading language.
        //!
        //! \param type
        //!     Shader type
        //! \param lang
        //!     Shading language
        //! \param code
        //!     Shader code
        //! \param hints
        //!     Shader compilation hints. Hints string must be in format that can be imported
        //!     into a registry object. See Registry::importFromStr() for details.
        //!     \par
        //!     For D3D shader, several hints are supported:
        //!     - "entry": specify entry function name, default is "main"
        //!     - "target": specify HLSL compile target, default is empty, means using the highest possible target.
        //!     - "sm30": favor shader model 3.0 or not, default is yes.
        //!       - In DirectX, VS 3.0 and PS 3.0 can _ONLY_ be used with each other. So sometimes, you may want
        //!         your HLSL shader to be compiled into VS/PS 2.x. So it can be used with other non-SM3 shaders.
        //!       - This hints has higher priority then "target". So if you set "target" to "vs_3_0", while setting
        //!         this hint to "true". The shader will be compiled to "vs_2_a".
        //!
        virtual Shader *
        createShader( ShaderType type, ShadingLanguage lang, const StrA & code, const StrA & hints = "" ) = 0;

        //!
        //! Create vetex shader. Parameter 'entry' will be ignored for low-level shading language.
        //!
        Shader *
        createVtxShader( ShadingLanguage lang, const StrA & code, const StrA & hints = "" );

        //!
        //! Create pixel shader. Parameter 'entry' will be ignored for low-level shading language.
        //!
        Shader *
        createPxlShader( ShadingLanguage lang, const StrA & code, const StrA & hints = "" );

        //!
        //! Create new texture
        //! See TextureDesc for detail explaination of each fields in descriptor.
        //!
        virtual Texture *
        createTexture( const TextureDesc & desc, const TextureLoader & loader = TextureLoader() ) = 0;

        //!
        //! Load texture from file
        //!
        virtual Texture *
        createTextureFromFile( File & file ) = 0;

        //!
        //! Create new texture, with individual creation parameters.
        //!
        Texture *
        createTexture( TexType  type,
                       size_t   sx, size_t sy, size_t sz,
                       size_t   faces = 0,
                       size_t   levels = 0,
                       ClrFmt   format = FMT_DEFAULT,
                       BitField usage = 0,
                       bool     tiled = false,
                       const TextureLoader & loader = TextureLoader() )
        {
            TextureDesc desc = { type, (uint32_t)sx, (uint32_t)sy, (uint32_t)sz, (uint32_t)faces, (uint32_t)levels, format, usage, tiled };
            return createTexture( desc, loader );
        }

        //!
        //! Create 1D texture
        //!
        Texture *
        create1DTexture( size_t   sx,
                         size_t   levels = 0,
                         ClrFmt   format = FMT_DEFAULT,
                         BitField usage = 0,
                         bool     tiled = false,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_1D, sx, 0, 0, 1, levels, format, usage, tiled, loader );
        }

        //!
        //! Create 2D texture
        //!
        Texture *
        create2DTexture( size_t   sx, size_t sy,
                         size_t   levels = 0,
                         ClrFmt   format = FMT_DEFAULT,
                         BitField usage = 0,
                         bool     tiled = false,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_2D, sx, sy, 0, 1, levels, format, usage, tiled, loader );
        }

        //!
        //! Create 3D texture
        //!
        Texture *
        create3DTexture( size_t   sx, size_t sy, size_t sz,
                         size_t   levels = 0,
                         ClrFmt   format = FMT_DEFAULT,
                         BitField usage = 0,
                         bool     tiled = false,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_3D, sx, sy, sz, 1, levels, format, usage, tiled, loader );
        }

        //!
        //! Create CUBE texture
        //!
        Texture *
        createCubeTexture( size_t   sx,
                           size_t   levels = 0,
                           ClrFmt   format = FMT_DEFAULT,
                           BitField usage = 0,
                           bool     tiled = false,
                           const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_CUBE, sx, 0, 0, 6, levels, format, usage, tiled, loader );
        }

        //!
        //! Create vertex format handle.
        //!
        virtual VtxFmtHandle createVtxFmt( const VtxFmtDesc & ) = 0;

        //!
        //! Create new vertex buffer
        //!
        //! \param bytes
        //!     Size of vertex buffer in bytes.
        //! \param dynamic
        //!     Dynamic or static vertex buffer.
        //! \param sysCopy
        //!     has system copy or not
        //! \param loader
        //!     Optional content loader.
        //!
        virtual VtxBuf *
        createVtxBuf( size_t bytes,
                      bool   dynamic = false,
                      bool   sysCopy = true,
                      const  VtxBufLoader & loader = VtxBufLoader() ) = 0;

        //!
        //! Create new index buffer
        //!
        //! \param numIdx
        //!     number of indices
        //! \param dynamic
        //!     Dynamic or static vertex buffer.
        //! \param sysCopy
        //!     has system copy or not
        //! \param loader
        //!     Optional content loader.
        //!
        //! \note
        //!     ÿ�������̶�ռ��16bit
        //!
        virtual IdxBuf *
        createIdxBuf( size_t numIdx,
                      bool   dynamic = false,
                      bool   sysCopy = true,
                      const  IdxBufLoader & loader = IdxBufLoader() ) = 0;

        //@}

        // ********************************************************************
        //
        //! \name Context Manager
        //
        // ********************************************************************

        //@{

    public:

        //!
        //! Set rendering context.
        //!
        //! When you want to change many renderer states, this method is recommended
        //! over single context update helper functions, for performance reason.
        //!
        virtual void setContext( const RendererContext & ) = 0;

        //!
        //! Rebind current rendering context to rendering device.
        //!
        //! \par
        //!     This function will "reset" some of states of low-level rendering device,
        //!     based on input field flags, to sync-up with current context stored in
        //!     in renderer.
        //! \par
        //!     Call this function to restore state of low-level rendering device, when
        //!     state is modified (by calling OpenGL functions or IDirect3DDevice methods,
        //!     for example), and you want to restore deivce to its previous states.
        //!
        virtual void rebindContext( RendererContext::FieldFlags ) = 0;

        //!
        //! Get current render state block descriptor
        //!
        virtual const RenderStateBlockDesc & getCurrentRenderStateBlock() const = 0;

        // ********************************************************************
        //!
        //! \name Helper functions to update rendering context.
        //!
        //! - See corresponding methods in RendererContext for usage of each method.
        //! - Recommended call sequence is:
        //! <pre>
        //!     contextUpdateBegin();
        //!     ... // call context update methods here.
        //!     contextUpdateEnd();
        //! </pre>
        //! - Call update method outside of contextUpdateBegin() and contextUpdateEnd() is allowed,
        //!   but not recommented for performance reason.
        //!
        // ********************************************************************

        //@{

    private:

        RendererContext      mHelperContext;
        AutoInit<bool,false> mHelperContextUpdateBegun;

    public:

        //!
        //! start context update
        //!
        inline void contextUpdateBegin();

        //!
        //! end context update, flush modified context to renderer.
        //!
        inline void contextUpdateEnd();

        inline void setShader( ShaderType type, const Shader * shader );
        inline void setShaders( const Shader * const shaders[] );
        inline void setShaders( const Shader * vtxShader, const Shader * pxlShader );
        inline void setVtxShader( const Shader * s );
        inline void setPxlShader( const Shader * s );
        inline void setRenderStateBlock( const RenderStateBlockDesc & );
        inline void setRenderState( RenderState state, int32_t value );
        inline void setColorBuffer( size_t index, const Texture * texture, size_t face = 0, size_t level = 0, size_t slice = 0 );
        inline void setDepthBuffer( const Texture * texture, size_t face = 0, size_t level = 0, size_t slice = 0 );
        inline void setMsaa( MsaaType );
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
        //! \name Drawing Manager
        //
        // ********************************************************************

        //@{

    protected:

        AutoInit<size_t,0> mNumPrims; //!< Number of primitives per frame.
        AutoInit<size_t,0> mNumDraws; //!< Number of draws per frame.

    public :

        //!
        //! ��ʼ��ͼ����.
        //!
        //! ���еĻ�ͼ������������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual bool drawBegin() = 0;

        //!
        //! ������ͼ����. Similar as EndScene() followed by Present()
        //!
        //! call drawEnd() <b>if and only if</b> drawBegin() returns true.
        //!
        virtual void drawEnd() = 0;

        //!
        //! �������Ĺ���������OpenGL�е�glFinish()
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void drawFinish() = 0;

        //!
        //! ��������
        //!
        //! \param flags ������־, see ClearFlag
        //! \param c     ����ɫ
        //! \param z     ���ֵ
        //! \param s     ģ��ֵ
        //!
        //! \note Can be called outside of drawBegin()/drawEnd() scope.
        //!
        virtual void
        clearScreen( const Vector4f & c = Vector4f(0,0,0,1),
                     float z = 1.0f, uint32_t s = 0,
                     BitField flags = CLEAR_ALL ) = 0;

        //!
        //! Draw indexed primitives.
        //!
        //! \param prim
        //!     primititive type
        //! \param numPrims
        //!     number of primitives
        //! \param startVtx
        //!     vertex index into vertex buffer that index "0" will be refering to.
        //! \param minVtxIdx, numVtx
        //!     define effective range in vertex buffer, starting from startVtx.
        //! \param startIdx
        //!     index into index buffer of the first index
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numPrims,
                                  size_t        startVtx,
                                  size_t        minVtxIdx,
                                  size_t        numVtx,
                                  size_t        startIdx ) = 0;

        //!
        //! Draw non-indexed primitives.
        //!
        //! \param prim
        //!     primititive type
        //! \param numPrims
        //!     number of primitives
        //! \param startVtx
        //!     index into vertex buffer of the first vertex.
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void draw( PrimitiveType prim,
                           size_t        numPrims,
                           size_t        startVtx ) = 0;

        //!
        //! draw on-indexed primitives with user-defined data array
        //!
        virtual void drawIndexedUp(
                             PrimitiveType    prim,
                             size_t           numPrims,
                             size_t           numVertices,
                             const void *     vertexData,
                             size_t           strideInBytes,
                             const uint16_t * indexData ) = 0;

        //!
        //! draw on-indexed primitives with user-defined data array
        //!
        virtual void drawUp( PrimitiveType prim,
                             size_t        numPrims,
                             const void *  vertexData,
                             size_t        strideInBytes ) = 0;

        //!
        //! Draw quads
        //!
        //! \param options
        //!     ��Ⱦѡ���� DrawQuadOptions��Set to 0 to use default options
        //! \param positions, posStride
        //!     �����������ݣ���һϵ�еĶ�����ɡ�4�������ʾһ�����Ρ�
        //!     ѡ�� DQ_WINDOW_SPACE �� DQ_3D_POSITION ��Ӱ������ĺ��塣
        //!     Note "posStride" is stride of one vertex.
        //! \param texcoords, texStride
        //!     ��ͼ�������飬��һϵ�е�2D������ɡ�4�������ʾһ�����Ρ�
        //!     Note "texStride" is stride of one vertex.
        //!     Specify texcoords to NULL, if you want non-textured quad.
        //!     texStride is be ignored in this case.
        //! \param colors, clrStride
        //!     ������ɫ���飬��һϵ�е�BGRA32��ɫֵ��ɡ�4�������ʾһ�����Ρ�
        //!     Note "clrStride" is stride of one vertex.
        //!     Set colors to NULL, to specify pure white for all vertices.
        //! \param count
        //!     Number of quads.
        //!
        virtual void drawQuads(
            BitField options,
            const void * positions, size_t posStride,
            const void * texcoords, size_t texStride,
            const void * colors, size_t clrStride,
            size_t count ) = 0;

        //!
        //! Draw quads, with same stride for positions, texcoords and colors
        //!
        void drawQuads(
            BitField options,
            const void * positions, const void * texcoords, const void * colors, size_t stride,
            size_t count )
        {
            drawQuads( options, positions, stride, texcoords, stride, colors, stride, count );
        }

        //!
        //! Draw single 2D textured quad.
        //!
        //! \note This function may not very effecient.
        //!
        void draw2DTexturedQuad(
            BitField options,
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

            Vector2f pos[4];
            Vector2f tex[4];

            pos[0].set( x1, y1 );
            pos[1].set( x2, y1 );
            pos[2].set( x2, y2 );
            pos[3].set( x1, y2 );

            tex[0].set( u1, v1 );
            tex[1].set( u2, v1 );
            tex[2].set( u2, v2 );
            tex[3].set( u1, v2 );

            drawQuads( options&(~DQ_3D_POSITION), pos, sizeof(Vector2f), tex, sizeof(Vector2f), 0, 0, 1 );
        }

        //!
        //! Draw single 2D solid quad.
        //!
        //! \note This function may not very effecient.
        //!
        void draw2DSolidQuad(
            BitField options,
            double left = 0.0, double top = 0.0, double right = 1.0, double bottom = 1.0,
            uint32_t color = 0xFFFFFFFF )
        {
            float x1 = (float)left;
            float y1 = (float)top;
            float x2 = (float)right;
            float y2 = (float)bottom;

            Vector2f pos[4];
            pos[0].set( x1, y1 );
            pos[1].set( x2, y1 );
            pos[2].set( x2, y2 );
            pos[3].set( x1, y2 );

            uint32_t colors[] = { color, color, color, color };

            drawQuads( options&(~DQ_3D_POSITION), pos, sizeof(Vector2f), 0, 0, colors, sizeof(uint32_t), 1 );
        }

        //!
        //! Draw line segments
        //!
        //! \param options
        //!     ��Ⱦѡ���� DrawLineOptions��Set to 0 to use default options
        //! \param positions
        //!     �����������ݣ���һϵ�еĶ�����ɡ�2�������ʾһ�����Ρ�
        //!     ѡ�� DQ_WINDOW_SPACE �� DQ_3D_POSITION ��Ӱ������ĺ��塣
        //! \param stride
        //!     stride of one vertex.
        //! \param count
        //!     Number of line segments (note: _NOT_ number of points)
        //! \param color
        //!     line color. in FMT_BGRA32 format.
        //! \param model, view, proj
        //!     Transformation matrices. Ignored when using DL_WINDOW_SPACE.
        //!
        virtual void drawLines(
            BitField options,
            const void * positions,
            size_t stride,
            size_t count,
            uint32_t color,
            const Matrix44f & model,
            const Matrix44f & view,
            const Matrix44f & proj ) = 0;

        //!
        //! ����Ļ��ָ����λ�û���2D�ַ���.
        //!
        //! - �������Դ�����Ӣ�ĵĻ���ַ������������ٶȽ�������ҪΪ���Զ��á�
        //!   ���ֵĸ߶ȹ̶�Ϊ16�����أ����Ϊ8(English)/16(Chinese)�����ء�
        //! - ������ drawBegin() �� drawEnd() ֮�����
        //!
        //! \param text  �����ƶ��ַ���
        //! \param x, y  ��һ���ַ����ϽǵĴ�������
        //!              - ��Ļ���Ͻ�Ϊ(0,0)�����½�Ϊ(width,height)
        //!              - �����Ƶ��ַ���������λ���Ե�һ���ַ������Ͻ�Ϊ׼��
        //! \param color ������ɫ
        //!
        virtual void
        drawDebugTextA( const char * text, int x, int y,
                        const Vector4f & color = Vector4f(1,1,1,1) ) = 0;

        //!
        //!  ����unicode����
        //!
        //! \sa drawDebugTextA()
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void
        drawDebugTextW( const wchar_t * text, int x, int y,
                        const Vector4f & color = Vector4f(1,1,1,1) ) = 0;

        //!
        //! ������һ�� drawEnd() �����������Ƶ�ԭ��ĸ���
        //!
        size_t getNumPrimitives() const { return mNumPrims; }

        //!
        //! ������һ�� drawEnd() ������ draw() / drawindexed() �Ĵ���
        //!
        size_t getNumDraws() const { return mNumDraws; }

        //!
        //! ������һ�� drawEnd() ������ƽ��ÿ�� draw()/drawIndexed() ��ԭ����
        //!
        size_t getNumPrimsPerDraw() const
        {
            return 0 == mNumDraws ? 0 : mNumPrims / mNumDraws;
        }

        //@}

        // ********************************************************************
        //
        //! \name Misc. utilities
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

        //!
        //! This function is provided because different API has different ways
        //! to compose projection matrix.
        //!
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

        //!
        //! This function is provided because different API has different ways
        //! to compose projection matrix.
        //!
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

        //!
        //! Enable/Disable parameter check for performance critical functions.
        //!
        //! Enabled by default for debug build; disabled by default for release build.
        //!
        void enableParameterCheck( bool enable ) { mEnableParameterCheck = enable; }

        //!
        //! Get parameter check flag.
        //!
        bool isParameterCheckEnabled() const { return mEnableParameterCheck; }

        //!
        //! Dump current renderer state to string. For debug purpose only.
        //!
        virtual void dumpCurrentState( StrA & ) const = 0;

        //@}

        // ********************************************************************
        //
        //! \name Ctor / dtor
        //
        // ********************************************************************

        //@{

    protected:

        //!
        //! ctor
        //!
        Renderer() : mSharedLib(0) {}

        //!
        //! Dtor
        //!
        virtual ~Renderer() {}

        //@}

        // ********************************************************************
        //
        //! \name Instance Manager
        //
        // ********************************************************************

        //@{

    private:

        SharedLib * mSharedLib;
        friend Renderer * createRenderer( RendererAPI );
        friend void deleteRenderer();

        //@}
    };

    //!
    //! (Re)Create a renderer.
    //!
    //! This function will release old renderer, then create a new one with new settings.
    //!
    Renderer * createRenderer( RendererAPI = API_AUTO );

    //!
    //! Delete renderer
    //!
    void deleteRenderer();
}}

#include "renderer.inl"

// *****************************************************************************
//                           End of renderer.h
// *****************************************************************************
#endif // __GN_GFX_RENDERER_H__
