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
    ::GN::Signal0<bool> GN::gfx::Renderer::sSigDeviceCreate; \
    ::GN::Signal0<bool> GN::gfx::Renderer::sSigDeviceRestore; \
    ::GN::Signal0<void> GN::gfx::Renderer::sSigDeviceDispose; \
    ::GN::Signal0<void> GN::gfx::Renderer::sSigDeviceDestroy; \
    ::GN::SharedLib     GN::gfx::Renderer::msSharedLib;

namespace GN { namespace gfx
{
    //!
    //! Msaa type
    //!
    enum MsaaType
    {
        MSAA_NONE,   //!< No MSAA
        MSAA_LOW,    //!< low quality MSAA
        MSAA_MEDIUM, //!< medium quality MSAA
        MSAA_HIGH,   //!< high quality MSAA
        MSAA_ULTRA,  //!< ultra quality MSAA
    };

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
    //! Vertex shader capability flags
    //!
    enum VSCaps
    {
        VSCAPS_D3D_1_1   = 1<<0, //!< DirectX vs.1.1
        VSCAPS_D3D_2_0   = 1<<1, //!< DirectX vs.2.0
        VSCAPS_D3D_2_X   = 1<<2, //!< DirectX vs.2.x
        VSCAPS_D3D_3_0   = 1<<3, //!< DirectX vs.3.0
        VSCAPS_D3D_XVS   = 1<<4, //!< D3D xvs 3.0 (Xenon only)
        VSCAPS_OGL_ARB1  = 1<<5, //!< OpenGL ARB vertex program 1.0
        VSCAPS_OGL_GLSL  = 1<<6, //!< OpenGL shading language

        //!
        //! alias for all D3D vertex shader caps
        //!
        VSCAPS_D3D_ALL = VSCAPS_D3D_1_1 |
                         VSCAPS_D3D_2_0 |
                         VSCAPS_D3D_2_X |
                         VSCAPS_D3D_3_0 |
                         VSCAPS_D3D_XVS
    };

    //!
    //! Pixel shader capability flags
    //!
    enum PSCaps
    {
        PSCAPS_D3D_1_1   = 1<<0, //!< DirectX ps.1.1
        PSCAPS_D3D_1_2   = 1<<1, //!< DirectX ps.1.2
        PSCAPS_D3D_1_3   = 1<<2, //!< DirectX ps.1.3
        PSCAPS_D3D_1_4   = 1<<3, //!< DirectX ps.1.4
        PSCAPS_D3D_2_0   = 1<<4, //!< DirectX ps.2.0
        PSCAPS_D3D_2_X   = 1<<5, //!< DirectX ps.2.x
        PSCAPS_D3D_3_0   = 1<<6, //!< DirectX ps.3.0
        PSCAPS_D3D_XVS   = 1<<7, //!< D3D XPS 3.0 (Xenon only)
        PSCAPS_OGL_ARB1  = 1<<8, //!< OpenGL ARB pixel program 1.0
        PSCAPS_OGL_GLSL  = 1<<9, //!< OpenGL shading language

        //!
        //! alias for all D3D pixel shader caps
        //!
        PSCAPS_D3D_ALL = PSCAPS_D3D_1_1 |
                         PSCAPS_D3D_1_2 |
                         PSCAPS_D3D_1_3 |
                         PSCAPS_D3D_1_4 |
                         PSCAPS_D3D_2_0 |
                         PSCAPS_D3D_2_X |
                         PSCAPS_D3D_3_0 |
                         PSCAPS_D3D_XVS
    };

    typedef uint32_t RsbHandle; //!< Render state block handle

    typedef uint32_t VtxFmtHandle; //!< Vertex format handle

    enum
    {
        MAX_RENDER_TARGETS = 4 //!< We support 4 render targets at most.
    };

    //!
    //! ������־
    //!
    enum ClearFlag
    {
        C_BUFFER       = 1,     //!< clear color buffer
        Z_BUFFER       = 2,     //!< clear z buffer
        S_BUFFER       = 4,     //!< clear stencil buffer
        ALL_BUFFERS    = 7      //!< clear all buffers
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
        //! position in window (post-transformed) space:
        //! (0,0) for left-up corner, (width,height) for right-bottom corner.
        //!
        //! By default, quad positios are in screen space. That is:
        //! (0,0) for left-up of the screen, and (1,1) for right-bottom of the screen)
        //!
        //! \note This option is meaningful only when DQ_USE_CURRENT_VS is _NOT_ set.
        //!
        DQ_WINDOW_SPACE = 1<<3,

        //!
        //! Use 3-D position. Default is 2-D position
        //!
        DQ_3D_POSITION = 1<<4,

        //!
        //! Disable blending. Default is enabled.
        //!
        DQ_OPAQUE = 1<<5,

        //!
        //! Enable depth write. Default is disabled.
        //!
        DQ_UPDATE_DEPTH = 1<<6,

        //!
        //! ���� DQ_USE_CURRENT_XX �ļ���
        //!
        DQ_USE_CURRENT = DQ_USE_CURRENT_RS | DQ_USE_CURRENT_VS | DQ_USE_CURRENT_PS
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
        //! position in window (post-transformed) space:
        //! (0,0) for left-up corner, (width,height) for right-bottom corner.
        //!
        //! By default, line positions are in object space.
        //!
        //! \note This option is meaningful only when DL_USE_CURRENT_VS is _NOT_ set.
        //!
        DL_WINDOW_SPACE = 1<<3,

        //!
        //! Using line strip. By default input points are treated as line list.
        //!
        DL_LINE_STRIP = 1<<4,

        //!
        //! ���� DL_USE_CURRENT_XX �ļ���
        //!
        DL_USE_CURRENT = DL_USE_CURRENT_RS | DL_USE_CURRENT_VS | DL_USE_CURRENT_PS
    };

    //!
    //! Rendering context. Completely define how rendering would be donw
    //!
    struct RenderingContext
    {
        //!
        //! Context flag structure
        //!
        union FieldFlags
        {
            unsigned int u32; //!< all flags as uint32
            struct
            {
                // byte 0
                unsigned int shaders            : 2; //!< shaders
                unsigned int rsb                : 1; //!< render state block
                unsigned int colorBuffers       : 3; //!< number of color buffers
                unsigned int depthBuffer        : 1; //!< depth buffer
                unsigned int viewport           : 1; //!< viewport
                // byte 1
                unsigned int world              : 1; //!< world transformation
                unsigned int view               : 1; //!< view transformation
                unsigned int proj               : 1; //!< projection transformation
                unsigned int light0Pos          : 1; //!< light 0 position
                unsigned int light0Diffuse      : 1; //!< light 0 diffuse
                unsigned int materialDiffuse    : 1; //!< material diffues color
                unsigned int materialSpecular   : 1; //!< material specular color
                unsigned int textureStates      : 1; //!< texture states
                // byte 2,3
                unsigned int                    : 16;
            };
        };

        //!
        //! render target binding descriptor
        //!
        struct RenderTargetDesc
        {
            AutoRef<Texture> texture; //!< render target
            size_t           level;   //!< mipmap level
            size_t           face;    //!< cubemap face
        };

        FieldFlags            flags; //!< field flags
        AutoRef<Shader>       shaders[NUM_SHADER_TYPES]; //!< shaders
        RsbHandle             rsb; //!< render state block handle
        RenderTargetDesc      colorBuffers[MAX_RENDER_TARGETS]; //!< color buffers
        size_t                numColorBuffers;
        RenderTargetDesc      depthBuffer; //!< depth buffers
        Rectf                 viewport; //!< viewport
        Matrix44f             world, //!< world transformation
                              view, //!< view transformation
                              proj; //!< projection transformation
        Vector4f              light0Pos, //!< light0 position
                              light0Diffuse, //!< light0 diffuse color
                              materialDiffuse, //!< diffuse material color
                              materialSpecular; //!< specular material color
        TextureStateBlockDesc textureStates; //!< texture stage states

        //!
        //! Clear to empty context, all fields are leave unused.
        //!
        void clear()
        {
            GN_CASSERT( 4 == sizeof(FieldFlags) );
            flags.u32 = 0;
            for( int i = 0; i < NUM_SHADER_TYPES; ++i ) shaders[i].clear();
            for( int i = 0; i < MAX_RENDER_TARGETS; ++i ) colorBuffers[i].texture.clear();
        }

        //!
        //! Reset to default context.
        //!
        void reset()
        {
            flags.u32 = 0xFFFFFFFF; // set all flags to true.
            for( int i = 0; i < NUM_SHADER_TYPES; ++i ) shaders[i].clear();
            rsb = 0;
            for( int i = 0; i < MAX_RENDER_TARGETS; ++i ) colorBuffers[i].texture.clear();
            numColorBuffers = 0;
            depthBuffer.texture.clear();
            viewport.set( 0.0f, 0.0f, 1.0f, 1.0f );
            world.identity();
            view.identity();
            proj.identity();
            light0Pos.set( 0.0f, 0.0f, 0.0f, 1.0f );
            light0Diffuse.set( 1.0f, 1.0f, 1.0f, 1.0f );
            materialDiffuse.set( 1.0f, 1.0f, 1.0f, 1.0f );
            materialSpecular.set( 0.2f, 0.2f, 0.2f, 1.0f );
            textureStates.reset( TextureStateBlockDesc::RESET_TO_DEFAULT );
        }

    };

    //!
    //! Vertex and pixel data. Define input data of renderer.
    //!
    struct VtxPxlData
    {
        //!
        //! flag structure
        //!
        union FieldFlags
        {
            unsigned int u32; //!< all flags as uint32
            struct
            {
                // byte 0
                unsigned int textures : 1; //!< textures
                unsigned int vtxFmt   : 1; //!< vertex format
                unsigned int vtxBufs  : 1; //!< vertex buffers
                unsigned int idxBuf   : 1; //!< index buffer
                unsigned int          : 4;
                // byte 1-3
                unsigned int          : 24;
            };
        };

        //!
        //! Vertex buffer binding descriptor
        //!
        struct VtxBufDesc
        {
            AutoRef<VtxBuf> buffer; //!< buffer pointer
            size_t          stride; //!< buffer stride
        };

        FieldFlags       flags; //!< flags
        AutoRef<Texture> textures[MAX_TEXTURE_STAGES]; //!< texture list
        size_t           numTextures; //!< texture count
        VtxFmtHandle     vtxFmt; //!< vertex format handle
        VtxBufDesc       vtxBufs[MAX_VERTEX_STREAMS]; //!< vertex buffers. Note that vertex buffer count is determined by current @vtxFmt.
        AutoRef<IdxBuf>  idxBuf; //!< index buffer

        //!
        //! clear to empty
        //!
        void clear()
        {
            GN_CASSERT( 4 == sizeof(FieldFlags) );
            for( int i = 0; i < MAX_TEXTURE_STAGES; ++i ) textures[i].clear();
            numTextures = 0;
            vtxFmt = 0;
            for( int i = 0; i < MAX_VERTEX_STREAMS; ++i ) vtxBufs[i].buffer.clear();
            idxBuf.clear();
        }
    };

    //!
    //! Define rendering API
    //!
    enum RendererAPI
    {
        API_OGL,  //!< OpenGL
        API_D3D,  //!< Direct3D
        API_FAKE, //!< Fake API
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
        //!                           +--------------+
        //!                           |              |
        //!                          \|/             |
        //!                           '              |
        //!   (start)-->create----->restore------->dispose------>destroy-->(end)
        //!               .                                          |
        //!              /|\                                         |
        //!               |                                          |
        //!               +------------------------------------------+
        //!   </pre>
        //! - create�źź���ض������һ��restore�ź�.
        //! - �յ�create����restore�ź�˵����Ⱦ��ready to use��
        //! - ��Щ�źŵı�׼ʹ�÷������£�
        //!   - �յ�create�źź�, ��������ͼ����Դ��
        //!   - �յ�restore�źź����ͼ����Դ�����ݣ���Ӵ��̶�ȡ��ͼ��ģ�͡�
        //!   - ����dispose�ź�
        //!   - �յ�destroy�źź�ɾ�����е�ͼ����Դ
        // ********************************************************************

        //@{

        //!
        //! Triggered right after render device is created.
        //!
        static GN_PUBLIC Signal0<bool> sSigDeviceCreate;

        //!
        //! Triggered after rendering device is created or restored from
        //! disposed state.
        //!
        static GN_PUBLIC Signal0<bool> sSigDeviceRestore;

        //!
        //! Triggered right before invalidating of rendering device.
        //!
        //! - Only lockable resources (such as texture and vertex buffer) that have
        //!   neither system-copy no content loader will lost their contents after
        //!   device dispose.
        //! - Note that only contents are lost, not resources themselves.
        //! - Non-lockable resources (such as shaders and render-state-blocks )
        //!   will survive device dispose.
        //! - After receiving this signal, no rendering function should be called,
        //!   until you receive sSigDeviceRestore.
        //!
        static GN_PUBLIC Signal0<void> sSigDeviceDispose;

        //!
        //! Triggered right before render device is deleted.
        //!
        //! \note
        //! - You must release all graphics resources (such as textures, shaders...),
        //!   after received this signal.
        //!
        static GN_PUBLIC Signal0<void> sSigDeviceDestroy;

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
        //!     This function may trigger sSigDeviceRestore and/or sSigDeviceDispose.
        //!
        virtual bool changeOptions( RendererOptions ro, bool forceDeviceRecreation = false ) = 0;

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
        virtual uint32_t getCaps( RendererCaps c ) const { GN_ASSERT( 0 <= c && c < NUM_RENDERER_CAPS ); return mCaps[c]; }

        //!
        //! Does specific shading language supported by hardware?
        //!
        virtual bool supportShader( ShaderType, ShadingLanguage ) = 0;

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
        virtual Shader *
        createShader( ShaderType type, ShadingLanguage lang, const StrA & code, const StrA & entry = "main" ) = 0;

        //!
        //! Create vetex shader. Parameter 'entry' will be ignored for low-level shading language.
        //!
        Shader *
        createVtxShader( ShadingLanguage lang, const StrA & code, const StrA & entry = "main" );

        //!
        //! Create pixel shader. Parameter 'entry' will be ignored for low-level shading language.
        //!
        Shader *
        createPxlShader( ShadingLanguage lang, const StrA & code, const StrA & entry = "main" );

        //!
        //! request a render state block object with specific render state block structure.
        //! Return 0, if failed.
        //!
        virtual RsbHandle
        createRenderStateBlock( const RenderStateBlockDesc & ) = 0;

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
                       const TextureLoader & loader = TextureLoader() )
        {
            TextureDesc desc = { type, (uint32_t)sx, (uint32_t)sy, (uint32_t)sz, (uint32_t)faces, (uint32_t)levels, format, usage };
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
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_1D, sx, 0, 0, 1, levels, format, usage, loader );
        }

        //!
        //! Create 2D texture
        //!
        Texture *
        create2DTexture( size_t   sx, size_t sy,
                         size_t   levels = 0,
                         ClrFmt   format = FMT_DEFAULT,
                         BitField usage = 0,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_2D, sx, sy, 0, 1, levels, format, usage, loader );
        }

        //!
        //! Create 3D texture
        //!
        Texture *
        create3DTexture( size_t   sx, size_t sy, size_t sz,
                         size_t   levels = 0,
                         ClrFmt   format = FMT_DEFAULT,
                         BitField usage = 0,
                         const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_3D, sx, sy, sz, 1, levels, format, usage, loader );
        }

        //!
        //! Create CUBE texture
        //!
        Texture *
        createCubeTexture( size_t   sx,
                           size_t   levels = 0,
                           ClrFmt   format = FMT_DEFAULT,
                           BitField usage = 0,
                           const TextureLoader & loader = TextureLoader() )
        {
            return createTexture( TEXTYPE_CUBE, sx, 0, 0, 6, levels, format, usage, loader );
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

        //!
        //! Set rendering context
        //!
        virtual void setContext( const RenderingContext & ) = 0;

        //!
        //! Set rendering data
        //!
        virtual void setVtxPxlData( const VtxPxlData & ) = 0;

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
                     BitField flags = C_BUFFER | Z_BUFFER ) = 0;

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
        //!   ���ֵĸ߶ȹ̶�Ϊ16�����أ�����Ϊ8(English)/16(Chinese)�����ء�
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

#if GN_DEBUG
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
        Renderer() {}

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

        static GN_PUBLIC SharedLib msSharedLib;
        friend Renderer * createRenderer( const RendererOptions &, RendererAPI );
        friend void deleteRenderer();

        //@}
    };

    //!
    //! (Re)Create a renderer.
    //!
    //! This function will release old renderer, then create a new one with new settings.
    //!
    Renderer * createRenderer( const RendererOptions &, RendererAPI = API_AUTO );

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
