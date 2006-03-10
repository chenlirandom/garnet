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
        MSAA_NONE,
        MSAA_LOW_QUALITY,
        MSAA_MEDIUM_QUALITY,
        MSAA_HIGH_QULITY,
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
        bool software;   //!< using software device. ȱʡΪfalse.
        bool reference;  //!< using reference device. ȱʡΪfalse.
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
        NUM_CAPS,
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

    typedef uint32_t VtxBindingHandle; //!< Vertex binding handle

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
    //! Rendering parmaqeter structure. Completly define how rendering would be done.
    //!
    struct RenderingParameters
    {
        //!
        //! template of one parameter
        //!
        template<typename T>
        struct Parameter
        {
            T value;   //!< parameter value
            bool used; //!< parameter is being used or not.

            //!
            //! default constructor.
            //!
            Parameter() : used(false) {}
        };

        //!
        //! render target descriptor
        //!
        struct RenderTargetDesc
        {
            AutoRef<Texture> texture; //!< render target 
            uint32_t         level;   //!< mipmap level
            TexFace          face;    //!< cubemap face
        };

        Parameter<AutoRef<Shader> > vtxShader; //!< vertex shader
        Parameter<AutoRef<Shader> > pxlShader; //!< pixle shader

        Parameter<RsbHandle> renderStateBlock; //!< render states

        Parameter<AutoRef<Texture> > textures[MAX_TEXTURE_STAGES]; //!< texture list

        Parameter<RenderTargetDesc> renderTargets[MAX_RENDER_TARGETS]; //!< render target list
        Parameter<RenderTargetDesc> renderDepth; //!< depth texture

        //!
        //! \name Fixed pipeline parameters
        //!
        //@{
        Parameter<Matrix44f>
            TransformWorld, //!< world transformation
            TransformView, //!< view transformation
            TransformProj; //!< projection transformation
        Parameter<Rectf>
            Viewport; //!< Viewport
        Parameter<Vector4f>
            Light0Pos, //!< light0 position
            Light0Diffuse, //!< light0 diffuse color
            MaterialDiffuse, //!< diffuse material color
            MaterialSpecular; //!< specular material color
        Parameter<TextureStateBlockDesc>
            TextureStates; //!< texture stage states
        //@}
    };

    //!
    //! Define geomety data
    //!
    //! \todo Customizable vertex buffer stride.
    //!
    struct RenderingGeometry
    {
        VtxBindingHandle vtxBinding; //!< vertex binding ID.
        AutoRef<VtxBuf>  vtxBufs[MAX_VERTEX_STREAMS]; //!< vertex buffer list
        size_t           numVtxBufs; //!< vertex buffer count
        AutoRef<IdxBuf>  idxBuf; //!< index buffer

        PrimitiveType prim;      //!< primitive type
        size_t        numPrims;  //!< primitive count
        size_t        startVtx;  //!< base vertex index
        size_t        minVtxIdx; //!< ignored if index buffer is NULL.
        size_t        numVtx;    //!< ignored if index buffer is NULL.
        size_t        startIdx;  //!< ignored if index buffer is NULL.
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
        //!     new rendeer options
        //! \param forceDeviceRecreation
        //!     force a full device recreation
        //! \note
        //!     This function may trigger sSigDeviceRestore and/or sSigDeviceDispose.
        //!
        virtual bool changeOptions( RendererOptions ro, bool forceDeviceRecreation = false ) = 0;

        //!
        //! Get current renderer options
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

        //!
        //! caps descriptor
        //!
        class CapsDesc
        {
            uint32_t value; //!< caps value
#if GN_DEBUG
            bool     valid; //!< caps is initialized or not.
#endif

        public:
            CapsDesc()
#if GN_DEBUG
                : valid(false)
#endif
            {}

            //!
            //! get caps value
            //!
            uint32_t get() const
            {
                GN_ASSERT( valid );
                return value;
            }

            //!
            //! set caps value
            //!
            void set( uint32_t value );

            //!
            //! reset caps value (to invalid state)
            //!
            void reset();
        };

    private:

        CapsDesc mCaps[NUM_CAPS];

    protected:

        //!
        //! update caps. called by child class.
        //!
        void setCaps( RendererCaps, uint32_t );

        //!
        //! reset(clear) all caps. called by child class.
        //!
        void resetAllCaps();

    public:

        //!
        //! Get render device caps
        //!
        uint32_t getCaps( RendererCaps c ) const
        {
            GN_ASSERT( 0 <= c && c < NUM_CAPS );
            return mCaps[c].get();
        }

        //@}

        // ********************************************************************
        //
        //! \name Shader Manager
        //
        // ********************************************************************

        //@{

        //!
        //! Does specific shading language supported by hardware?
        //!
        virtual bool supportShader( ShaderType, ShadingLanguage ) = 0;

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
        //! Bind a shaders to rendering device. Set NULL to use fixed pipeline.
        //!
        virtual void bindShader( ShaderType type, const Shader * shader ) = 0;

        //!
        //! Bind a list of shaders to rendering device. The list must have
        //! NUM_SHADER_TYPES elements.
        //!
        virtual void bindShaders( const Shader * const shaders[] ) = 0;

        //!
        //! Bind programmable shaders to rendering device. Set to NULL to use
        //! fixed pipeline.
        //!
        void bindShaders( const Shader * vtxShader, const Shader * pxlShader );

        //!
        //! Bind programmable shader handles to rendering device. Set to 0 to use
        //! fixed pipeline.
        //!
        void bindShaderHandles( ShaderDictionary::HandleType vtxShader, ShaderDictionary::HandleType pxlShader );

        //!
        //! Bind programmable vertex shader to rendering device. Set to NULL to use
        //! fixed pipeline.
        //!
        void bindVtxShader( const Shader * s ) { bindShader( VERTEX_SHADER, s ); }

        //!
        //! Bind shader by handle.
        //!
        void bindVtxShaderHandle( ShaderDictionary::HandleType h ) { bindShader( VERTEX_SHADER, gShaderDict.getResource(h) ); }

        //!
        //! Bind programmable pixel to rendering device. Set to NULL to use
        //! fixed pipeline.
        //!
        void bindPxlShader( const Shader * s ) { bindShader( PIXEL_SHADER, s ); }

        //!
        //! Bind shader by handle.
        //!
        void bindPxlShaderHandle( ShaderDictionary::HandleType h ) { bindShader( PIXEL_SHADER, gShaderDict.getResource(h) ); }

        //@}

        // ********************************************************************
        //
        //! \name Render State Block Manager
        //
        // ********************************************************************

        //@{

        //!
        //! request a render state block object with specific render state block structure.
        //! Return 0, if failed.
        //!
        virtual RsbHandle
        createRenderStateBlock( const RenderStateBlockDesc & ) = 0;

        //!
        //! Bind render state block to rendering device
        //!
        virtual void bindRenderStateBlock( RsbHandle ) = 0;

        //!
        //! Get current render state.
        //!
        //! \note Better not use this functio in performance critical code.
        //!
        virtual void getCurrentRenderStateBlock( RenderStateBlockDesc & ) const = 0;

        //!
        //! Another style of getting current render state.
        //!
        //! \note This is even slower because of extra data copy.
        //!
        RenderStateBlockDesc getCurrentRenderStateBlock() const
        {
            RenderStateBlockDesc result;
            getCurrentRenderStateBlock(result);
            return result;
        }

        //!
        //! Update individual render state.
        //!
        //! \return
        //!     Return the render state block handler that represents current render state.
        //!     Return 0, if failed.
        //!
        //! \note
        //!     This function is purely for coding convenience.
        //!     Please use render state block at performance critical section.
        //!
        virtual uint32_t setRenderState( RenderState state, RenderStateValue value ) = 0;

        //!
        //! Update a bunch of render states.
        //!
        //! \param statePairs
        //!     Render state and values like this : ( state1, value1, state2, value2, .... )
        //! \param count
        //!     Number of render state and value pairs.
        //!
        //! \return
        //!     Return the render state block handler that represents current render state.
        //!     Return 0, if failed.
        //!
        //! \note
        //!     This function is faster then multiple calls to setRenderState(), but slower
        //!     then using render state block handle.
        //!
        virtual uint32_t setRenderStates( const int * statePairs, size_t count ) = 0;

        //@}

        // ********************************************************************
        //
        //! \name Texture Manager
        //
        // ********************************************************************

        //@{

    private:

        AutoRef<const Texture> mCurrentTextures[MAX_TEXTURE_STAGES]; // current texture list
        mutable AutoInit<size_t,0> mDirtyTextureStages;

    protected:

        //!
        //! Get current texture list
        //!
        const Texture * const * getCurrentTextures() const { return mCurrentTextures[0].addr(); }

        //!
        //! Clear current textures
        //!
        void clearCurrentTextures()
        {
            for( size_t i = 0; i < MAX_TEXTURE_STAGES; ++i ) mCurrentTextures[i].clear();
        }

        //!
        //! Get dirty texture stage
        //!
        size_t getDirtyTextureStages() const { return mDirtyTextureStages; }

        //!
        //! Clear dirty texture stage
        //!
        void clearDirtyTextureStages() const { mDirtyTextureStages = 0; }

        //!
        //! Set all stages dirty
        //!
        void setAllTextureStagesDirty() { mDirtyTextureStages = MAX_TEXTURE_STAGES; }

    public:

        //!
        //! Test compability of specific texture format
        //!
        virtual bool supportTextureFormat( TexType type, BitField usage, ClrFmt format ) const = 0;

        //!
        //! Create new texture
        //! See TextureDesc for detail explaination of each fields in descriptor.
        //!
        virtual Texture *
        createTexture( const TextureDesc & desc, const TextureLoader & loader = TextureLoader() ) = 0;

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
        //! Load texture from file
        //!
        virtual Texture *
        createTextureFromFile( File & file ) = 0;

        //!
        //! bind one texture
        //!
        inline void bindTexture( size_t stage, const Texture * tex )
        {
            GN_GUARD_SLOW;
            GN_ASSERT( stage < MAX_TEXTURE_STAGES );
            if( mCurrentTextures[stage] != tex )
            {
                mCurrentTextures[stage].reset( tex );
                ++stage;
                if( stage > mDirtyTextureStages ) mDirtyTextureStages = stage;
            }
            GN_UNGUARD_SLOW;
        }

        //!
        //! bind one texture handle
        //!
        void bindTextureHandle( size_t stage, TextureDictionary::HandleType tex ) { return bindTexture( stage, gTexDict.getResource(tex) ); }

        //!
        //! bind textures ( from stage[start] to stage[start+numtex-1] )
        //!
        //! \param texlist texture list
        //! \param start   start stage
        //! \param count   number of textures
        //!
        void bindTextures( const Texture * const texlist[], size_t start, size_t count )
        {
            GN_GUARD_SLOW;
            GN_ASSERT( (start + count) <= MAX_TEXTURE_STAGES );
            const Texture * const * tex = texlist;
            for( uint32_t i = 0; i < count; ++i, ++start, ++tex )
            {
                if( mCurrentTextures[start] != *tex )
                {
                    mCurrentTextures[start].reset( *tex );
                }
            }
            if( start > mDirtyTextureStages ) mDirtyTextureStages = start;
            GN_UNGUARD_SLOW;
        }

        //!
        //! bind texture handles
        //!
        void bindTextureHandles( const TextureDictionary::HandleType texlist[], size_t start, size_t count )
        {
            GN_GUARD_SLOW;
            for( size_t i = 0; i < count; ++i, ++start )
                bindTextureHandle( start, texlist[i] );
            GN_UNGUARD_SLOW;
        }

        //@}

        // ********************************************************************
        //
        //! \name Renderable Buffer Manager
        //
        // ********************************************************************

        //@{

        //!
        //! Create vertex bindings.
        //!
        virtual VtxBindingHandle createVtxBinding( const VtxFmtDesc & ) = 0;

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

        //!
        //! Bind vertex bindings
        //!
        virtual void bindVtxBinding( VtxBindingHandle ) = 0;

        //!
        //! Bind a serias vertex buffers to rendering device.
        //!
        //! \param buffers  Buffer list.
        //! \param start    Stream index of the 1st buffer in buffer list.
        //! \param count    Stream count in buffer list.
        //!
        virtual void
        bindVtxBufs( const VtxBuf * const buffers[], size_t start, size_t count ) = 0;

        //!
        //! Bind a vertex buffers to rendering device, with user-specified stride.
        //!
        //! \note
        //! - By default, vertex buffer stride will be determined by current vertex binding.
        //!   Use this function only when your want to use custom stride for your vertex buffer.
        //! - To bind a single vertex buffer with default stride, please use
        //!   bindVtxBufs( &yourBuffer, yourIndex, 1 ) instead.
        //!
        virtual void
        bindVtxBuf( size_t index, const VtxBuf * buffer, size_t stride ) = 0;

        //!
        //! Bind index buffer to rendering device
        //!
        virtual void bindIdxBuf( const IdxBuf * ) = 0;

        //@}

        // ********************************************************************
        //
        //! \name Fixed Function Pipeline Manager.
        //
        // ********************************************************************

        //@{

    protected:

        //!
        //! Fixed-size stack container that do not perform any runtime
        //! memory allocation/deallocation.
        //!
        template< class T, size_t MAX_DEPTH = 256 >
        class FixedStack
        {
            T      mTop;              //!< top element
            T      mStack[MAX_DEPTH]; //!< element stack
            size_t mDepth;            //!< current depth

        public :

            //!
            //! default constructor
            //!
            FixedStack() : mDepth(0) {}

            //!
            //! get current depth
            //!
            size_t depth() const { return mDepth; }

            //!
            //! push the top element into stack
            //!
            void push()
            {
                GN_ASSERT( mDepth < MAX_DEPTH );
                mStack[mDepth] = mTop;
                ++mDepth;
            }

            //!
            //! pop out the stack to top element
            //!
            T & pop()
            {
                GN_ASSERT( mDepth > 0 );
                mTop = mStack[--mDepth];
                return mTop;
            }

            //!
            //! Get top element
            //!
            const T & top() const { return mTop; }

            //!
            //! Get top element
            //!
            T & top() { return mTop; }
        };

        //!
        //! Fixed function pipeline dirty flags
        //!
        union FfpDirtyFlags
        {
            uint16_t u32; //!< dirty flags as unsigned 32-bit integer
            int16_t  i32; //!< dirty flags as signed 32-bit integer
            struct
            {
                int TransformWorld   : 1; //!< world matrix dirty flag
                int TransformView    : 1; //!< view matrix dirty flag
                int TransformProj    : 1; //!< projection matrix dirty flag
                int Viewport         : 1; //!< viewport dirty flag
                int Light0Pos        : 1; //!< light 0 position dirty flag
                int Light0Diffuse    : 1; //!< light 0 color dirty flag
                int MaterialDiffuse  : 1; //!< material diffuse dirty flag
                int MaterialSpecular : 1; //!< material specular dirty flag
                int TextureStates    : 1; //!< texture states dirty flag
            };
        };

        FfpDirtyFlags mFfpDirtyFlags; //!< Fixed function pipeline dirty flags.

        FixedStack<Matrix44f>
            mTransformWorld, //!< World transformation
            mTransformView,  //!< Camera transformation
            mTransformProj;  //!< Projection transformation

        FixedStack<Rectf>
            mViewport; //!< Resolution-independent viewport: (0,0,1,1) means whole screen or render target.

        FixedStack<Vector4f>
            mLight0Pos,        //!< Light0 position
            mLight0Diffuse,    //!< Light0 diffuse color
            mMaterialDiffuse,  //!< Material diffuse color
            mMaterialSpecular; //!< Material specular color

        FixedStack<TextureStateBlockDesc>
            mTextureStates; //!< Texture stage states

        //!
        //! re-apply all fixed function pipeline states
        //!
        void reapplyAllFfpStates()
        {
            setTransformWorld( getTransformWorld() );
            setTransformView( getTransformView() );
            setTransformProj( getTransformProj() );
            setViewport( getViewport() );
            setLight0Pos( getLight0Pos() );
            setLight0Diffuse( getLight0Diffuse() );
            setMaterialDiffuse( getMaterialDiffuse() );
            setMaterialSpecular( getMaterialSpecular() );
            setTextureStates( getTextureStates() );
        }

    private:

        //!
        //! construct default value
        //!
        void ffpCtor()
        {
            Vector4f v;

            GN_CASSERT( 4 == sizeof(FfpDirtyFlags) );
            mFfpDirtyFlags.u32 = 0;

            setTransformWorld( Matrix44f::IDENTITY );
            setTransformView( Matrix44f::IDENTITY );
            setTransformProj( Matrix44f::IDENTITY );
            setViewport( 0, 0, 1, 1 );
            setLight0Pos( v.set(0,0,0,1) );
            setLight0Diffuse( v.set(1,1,1,1) );
            setMaterialDiffuse( v.set(1,1,1,1) );
            setMaterialSpecular( v.set(0,0,0,0) );
            setTextureStates( TextureStateBlockDesc::DEFAULT );
        }

    public:

#define GN_RENDERER_FFP_METHODS( name, type ) \
    void set##name(const type & newValue ) { m##name.top() = newValue; mFfpDirtyFlags.name = true; } \
    const type & get##name() const { return m##name.top(); } \
    void push##name() { m##name.push(); } \
    void pop##name() { m##name.pop(); mFfpDirtyFlags.name = true; }

        GN_RENDERER_FFP_METHODS( TransformWorld, Matrix44f );
        GN_RENDERER_FFP_METHODS( TransformView, Matrix44f );
        GN_RENDERER_FFP_METHODS( TransformProj, Matrix44f );
        GN_RENDERER_FFP_METHODS( Viewport, Rectf );
        GN_RENDERER_FFP_METHODS( Light0Pos, Vector4f );
        GN_RENDERER_FFP_METHODS( Light0Diffuse, Vector4f );
        GN_RENDERER_FFP_METHODS( MaterialDiffuse, Vector4f );
        GN_RENDERER_FFP_METHODS( MaterialSpecular, Vector4f );
        GN_RENDERER_FFP_METHODS( TextureStates, TextureStateBlockDesc );

#undef GN_RENDERER_FFP_METHODS

        //!
        //! Set viewport with 4 individual parameters
        //!
        void setViewport( float left, float top, float width, float height )
        {
            Rectf rc( left, top, width, height );
            setViewport( rc );
        }

        //!
        //! Set single texture state
        //!
        void setTextureState( size_t stage, TextureState state, TextureStateValue value )
        {
            GN_ASSERT( stage < MAX_TEXTURE_STAGES );
            GN_ASSERT( 0 <= state && state < NUM_TEXTURE_STATES );
            GN_ASSERT( 0 <= value && value < NUM_TEXTURE_STATE_VALUES );
            TextureStateBlockDesc & desc = mTextureStates.top();
            desc.ts[stage][state] = value;
            mFfpDirtyFlags.TextureStates = true;
        }

        //!
        //! This function is provided because different API has different ways
        //! to compose projection matrix.
        //!
        virtual Matrix44f &
        composePerspectiveMatrix( Matrix44f & result,
                                  float fovy,
                                  float ratio,
                                  float znear,
                                  float zfar ) const = 0;
        //!
        //! This function is provided because different API has different ways
        //! to compose projection matrix.
        //!
        virtual Matrix44f &
        composeOrthoMatrix( Matrix44f & result,
                            float left,
                            float bottom,
                            float width,
                            float height,
                            float znear,
                            float zfar ) const = 0;

        //@}

        // ********************************************************************
        //
        //! \name Render Target Manager
        //
        // ********************************************************************

        //@{

    public:

        //!
        //! set render target texture
        //!
        //! \param index
        //!     render target index, starting from 0
        //! \param texture
        //!     target texture, must be created with flag TEXUSAGE_RENDER_TARGET. Set
        //!     this parameter to NULL will reset to default target (back buffer
        //!     for RT0 and null for others.
        //! \param level
        //!     Mipmap level.
        //! \param face
        //!     Ignored if target_texture is not cubemap.
        //!
        virtual void setRenderTarget( size_t index,
                                      const Texture * texture,
                                      size_t level = 0,
                                      TexFace face = TEXFACE_PX ) = 0;

        //!
        //! set render target texture
        //!
        //! \param texture
        //!     Target texture, must be created with flag TEXUSAGE_DEPTH. Set this
        //!     parameter to NULL will reset to default depth buffer.
        //! \param level
        //!     Mipmap level.
        //! \param face
        //!     Ignored if target_texture is not cubemap.
        //!
        virtual void setRenderDepth( const Texture * texture,
                                     size_t level = 0,
                                     TexFace face = TEXFACE_PX ) = 0;

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
        //! draw geometry
        //!
        virtual void drawGeometry( const RenderingParameters &, const RenderingGeometry *, size_t ) = 0;

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

#if GN_DEBUG
        AutoInit<bool,true> mEnableParameterCheck;
#else
        AutoInit<bool,false> mEnableParameterCheck;
#endif

    public:

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
        Renderer()
        {
            ffpCtor();
        }

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
        friend Renderer * createRenderer( RendererAPI, const RendererOptions & );
        friend void deleteRenderer();

        //@}
    };

    //!
    //! (Re)Create a renderer.
    //!
    //! This function will release old renderer, then create a new one with new settings.
    //!
    //! \note This function is implemented in core module.
    //!
    Renderer * createRenderer( RendererAPI, const RendererOptions & );

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
