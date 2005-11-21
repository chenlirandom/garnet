#ifndef __GN_GFX_RENDERER_H__
#define __GN_GFX_RENDERER_H__
// *****************************************************************************
//! \file    renderer.h
//! \brief   Main renderer interface of GFX module
//! \author  chenlee (2005.9.30)
// *****************************************************************************

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
        //! and startup your application through an debugger to make that
        //! option effective.
        //! ȱʡΪtrue.
        //!
        bool autoRestore;

        //@}

        //!
        //! Construct a default device settings
        //!
        RendererOptions()
            : displayHandle(0)
            , useExternalWindow(false)
            , parentWindow(0)
            , monitorHandle(0)
            , autoBackbufferResizing(true)
            , windowedWidth(0)
            , windowedHeight(0)
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
        VSCAPS_OGL_ARB1  = 1<<4, //!< OpenGL ARB vertex program 1.0
        VSCAPS_OGL_GLSL  = 1<<5, //!< OpenGL shading language

        //!
        //! alias for all D3D vertex shader caps
        //!
        VSCAPS_D3D_ALL = VSCAPS_D3D_1_1 |
                         VSCAPS_D3D_2_0 |
                         VSCAPS_D3D_2_X |
                         VSCAPS_D3D_3_0
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
                         PSCAPS_D3D_3_0
    };

    //!
    //! Render parameter
    //!
    enum RenderParameter
    {
        RP_PER_STAGE_CONSTANT,  //!< Per-texture-stage constant (4 floats). Default is (1,1,1,1)
                                //! \note
                                //!     If current device does not support per-stage constant,
                                //!     this parameter will be ignored.
        RP_LIGHT0_POSITION,     //!< Light 0 position
        RP_LIGHT0_DIFFUSE,      //!< Light 0 diffuse color
        RP_MATERIAL_DIFFUSE,    //!< Material diffuse color
        RP_TRANSFORM_WORLD,     //!< World transformation
        RP_TRANSFORM_VIEW,      //!< Camera transformation
        RP_TRANSFORM_PROJ,      //!< Projection transformation
        RP_TRANSFORM_VIEWPORT,  //!< 4 floats: left, top, width, height; ranging from 0.0 to 1.0.
        NUM_RENDER_PARAMETERS,  //!< Number of avaiable 
        RP_INVALID              //!< Indicates invalid state.
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
    //! gfxģ������ӿ���
    //!
    //! \nosubgrouping
    //!
    struct Renderer : public NoCopy
    {
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
        //! Triggered after rendering device is restored to normal stage.
        //!
        //! \note
        //! - Resources that may lost their contents after device reset are:
        //!   - Textures, and
        //!   - Vertex buffers that are NOT type of BUFUSAGE_READBACK.
        //!   - Note that only contents are lost, not resources themselves.
        //! - Index buffers, shaders and rsblocks survive device reset.
        //!
        Signal0<bool> sigDeviceRestore;

        //!
        //! Triggered right before invalidating of rendering device.
        //!
        //! \note
        //! - After receiving this signal, no rendering function should be called,
        //!   before you receive sigDeviceRestore.
        //! - This signal will also be triggered, before rendering device is
        //!   destroyed.
        //!
        Signal0<void> sigDeviceDispose;

        //!
        //! Change device configuration.
        //!
        //! \param ro
        //!     new device settings
        //! \param forceDeviceRecreation
        //!     force a full device recreation
        //! \note
        //!     This function may trigger sigDeviceRestore.
        //!
        virtual bool changeOptions( const RendererOptions & ro,
                                    bool forceDeviceRecreation = false ) = 0;

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
        //! request a instance of vertex shader
        //!
        virtual Shader *
        createVertexShader( ShadingLanguage lang, const StrA & code ) = 0;

        //!
        //! request a instance of pixel shader
        //!
        virtual Shader *
        createPixelShader( ShadingLanguage lang, const StrA & code ) = 0;

        //!
        //! request a instance of shader
        //!
        //! \param type        shader type
        //! \param lang        shading language
        //! \param code        shader code string
        //! \return            instance of shader
        //!
        //! \note              This is inline function implemented in renderer.inl
        //!
        Shader *
        createShader( ShaderType      type,
                      ShadingLanguage lang,
                      const StrA &    code );

        //!
        //! Bind programmable shader to rendering device. Set to NULL to use
        //! fixed pipeline.
        //!
        virtual void
        bindShaders( const Shader * vtxShader, const Shader * pxlShader ) = 0;

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
        virtual uint32_t
        createRenderStateBlock( const RenderStateBlockDesc & ) = 0;

        //!
        //! Bind render state block to rendering device
        //!
        virtual void bindRenderStateBlock( uint32_t ) = 0;

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
        { RenderStateBlockDesc result; getCurrentRenderStateBlock(result); return result; }

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
        //! Update individual texture state.
        //!
        //! \return
        //!     Return the render state block handler that represents current render state.
        //!     Return 0, if failed.
        //!
        //! \note
        //!     - This function is purely for coding convenience.
        //!       Please use render state block at performance critical section.
        //!     - Also, Texture states are only used for fixed function pipeline.
        //!
        virtual uint32_t setTextureState( uint32_t stage, TextureState state, TextureStateValue value ) = 0;

        //@}

        // ********************************************************************
        //
        //! \name Texture Manager
        //
        // ********************************************************************

        //@{

        //!
        //! Create new texture.
        //!
        //! \param textype     texture type
        //! \param sx, sy, sz  texture size
        //! \param levels      how many mipmap levels?
        //!                    "0" means generate full mipmap levels down to 1x1
        //! \param format      texture format, FMT_DEFAULT means
        //!                    using default/appropriating format of current
        //!                    rendering hardware.
        //! \param usages      texture usages, one or combination of TexUsage
        //! \note
        //!    - sy/sz will be ignored for 1D/Cube texture,
        //!    - sz will be ignored for 2D texture.
        //!
        virtual Texture *
        createTexture( TexType textype,
                       uint32_t sx, uint32_t sy, uint32_t sz,
                       uint32_t levels = 0,
                       ClrFmt format = FMT_DEFAULT,
                       uint32_t usages = USAGE_NORMAL ) = 0;

        //!
        //! Load texture from file
        //!
        virtual Texture *
        createTextureFromFile( File & file ) = 0;

        //!
        //! bind textures ( from stage[start] to stage[start+numtex-1] )
        //!
        //! \param texlist texture list
        //! \param start   start stage
        //! \param numtex  number of textures
        //!
        virtual void
        bindTextures( const Texture * const texlist[],
                      uint32_t start, uint32_t numtex ) = 0;

        //@}

        // ********************************************************************
        //
        //! \name Renderable Buffer Manager
        //
        // ********************************************************************

        //@{

        //!
        //! Create new vertex buffer
        //!
        //! \param numVtx
        //!     Max number of vertices this vertex buffer can hold.
        //! \param stride
        //!     Bytes of each vertex.
        //! \param usage
        //!     buffer usage. Can be only USAGE_STATIC/USAGE_NORMAL or USAGE_DYNAMIC
        //! \param sysCopy
        //!     has system copy or not
        //!
        virtual VtxBuf *
        createVtxBuf( size_t        numVtx,
                      size_t        stride,
                      ResourceUsage usage = USAGE_NORMAL,
                      bool          sysCopy = true ) = 0;

        //!
        //! Create new index buffer
        //!
        //! \param numIdx
        //!     number of indices
        //! \param usage
        //!     buffer usage. Can be only USAGE_STATIC/USAGE_NORMAL or USAGE_DYNAMIC
        //! \param sysCopy
        //!     has system copy or not
        //!
        //! \note
        //!     ÿ�������̶�ռ��16bit
        //!
        virtual IdxBuf *
        createIdxBuf( size_t        numIdx,
                      ResourceUsage usage = USAGE_NORMAL,
                      bool          sysCopy = true ) = 0;

        //!
        //! Bind a serias vertex buffers to rendering device.
        //!
        //! \param buffers  Buffer list.
        //! \param start    Stream index of the 1st buffer in buffer list.
        //! \param count    Stream count in buffer list.
        //!
        virtual void
        bindVtxBufs( const VtxBuf * const buffers[],
                     uint32_t start, uint32_t count ) = 0;

        //!
        //! Bind index buffer to rendering device
        //!
        virtual void bindIdxBuf( const IdxBuf * ) = 0;

        //@}

        // ********************************************************************
        //
        //! \name Render Parameter Manager
        //
        // ********************************************************************

        //@{

        // TODO: comment these functions.

        virtual void setParameter( RenderParameter, uint32_t ) = 0;
        virtual void setParameter( RenderParameter, float ) = 0;
        virtual void setParameter( RenderParameter, const double & ) = 0;
        virtual void setParameter( RenderParameter, const Vector4f & ) = 0;
        virtual void setParameter( RenderParameter, const Matrix44f & ) = 0;
        virtual void setParameter( RenderParameter, const Vector4f * ) = 0;
        virtual void setParameter( RenderParameter, const Matrix44f * ) = 0;
        virtual void pushParameter( RenderParameter ) = 0;
        virtual void popParameter( RenderParameter ) = 0;

        virtual Matrix44f &
        computePerspectiveMatrix( Matrix44f & result,
                                  float fovy,
                                  float ratio,
                                  float znear,
                                  float zfar ) const = 0;
        virtual Matrix44f &
        computeOrthoMatrix( Matrix44f & result,
                            float left,
                            float bottom,
                            float width,
                            float height,
                            float znear,
                            float zfar ) const = 0;

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
        //! set render target texture
        //!
        //! \param index
        //!     render target index, starting from 0
        //! \param texture
        //!     target texture, must be created with flag TEXUSAGE_RENDERTARGET. Set
        //!     this parameter to NULL will reset to default target (back buffer
        //!     for RT0 and null for others.
        //! \param face
        //!     Ignored if target_texture is not cubemap.
        //!
        virtual void setRenderTarget( size_t index,
                                      const Texture * texture,
                                      TexFace face = TEXFACE_PX ) = 0;

        //!
        //! set render target texture
        //!
        //! \param texture
        //!     Target texture, must be created with flag TEXUSAGE_DEPTH. Set this
        //!     parameter to NULL will reset to default depth buffer.
        //! \param face
        //!     Ignored if target_texture is not cubemap.
        //!
        virtual void setRenderDepth( const Texture * texture,
                                     TexFace face = TEXFACE_PX ) = 0;

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
                     uint32_t flags = C_BUFFER | Z_BUFFER ) = 0;

        //!
        //! Draw indexed primitives.
        //!
        //! \param prim
        //!     primititive type
        //! \param numPrim
        //!     number of primitives
        //! \param baseVtx
        //!     vertex index into vertex buffer that index "0" will be refering to.
        //! \param minVtxIdx, numVtx
        //!     define effective range in vertex buffer, starting from baseVtx.
        //! \param startIdx
        //!     index into index buffer of the first index
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numPrim,
                                  size_t        baseVtx,
                                  size_t        minVtxIdx,
                                  size_t        numVtx,
                                  size_t        startIdx ) = 0;

        //!
        //! Draw non-indexed primitives.
        //!
        //! \param prim
        //!     primititive type
        //! \param numPrim
        //!     number of primitives
        //! \param startVtx
        //!     index into vertex buffer of the first vertex.
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void draw( PrimitiveType prim,
                           size_t        numPrim,
                           size_t        startVtx ) = 0;

        //!
        //! ����Ļ��ָ����λ�û���2D�ַ���.
        //!
        //! - ��Ļ���Ͻ�Ϊ����ԭ�㣬X������Y�����¡�
        //!   �����Ƶ��ַ���������λ���Ե�һ���ַ������Ͻ�Ϊ׼��
        //! - �������Դ�����Ӣ�ĵĻ���ַ������������ٶȽ�������ҪΪ���Զ��á�
        //!   ���ֵĸ߶ȹ̶�Ϊ16�����أ����Ϊ8(English)/16(Chinese)�����ء�
        //! - ������ drawBegin() �� drawEnd() ֮�����
        //!
        //! \param text  �����ƶ��ַ���
        //! \param x, y  ��һ���ַ����Ͻǵ���Ļ����
        //! \param color ������ɫ
        //!
        virtual void
        drawTextA( const char * text, int x, int y,
                   const Vector4f & color = Vector4f(1,1,1,1) ) = 0;

        //!
        //!  ����unicode����
        //!
        //! \sa draw_texta()
        //!
        //! \note ������ drawBegin() �� drawEnd() ֮�����
        //!
        virtual void
        drawTextW( const wchar_t * text, int x, int y,
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
    };

    //!
    //! Function prototype to create instance of renderer.
    //!
    typedef Renderer * (*CreateRendererFunc)( const RendererOptions & );

#if GN_STATIC
    //!
    //! Create instance of D3D renderer.
    //!
    Renderer * createD3DRenderer( const RendererOptions & );

    //!
    //! Create instance of OGL renderer.
    //!
    Renderer * createOGLRenderer( const RendererOptions & );
#endif
}}

#include "renderer.inl"

// *****************************************************************************
//                           End of renderer.h
// *****************************************************************************
#endif // __GN_GFX_RENDERER_H__
