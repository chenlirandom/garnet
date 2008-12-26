#ifndef __GN_GFX_RENDERER_H__
#define __GN_GFX_RENDERER_H__
// *****************************************************************************
/// \file
/// \brief   Main renderer interface of GFX module
/// \author  chenlee (2005.9.30)
// *****************************************************************************

/// \name aliases for global renderer signals
//@{
#define gSigRendererDeviceLost     (GN::gfx::getSigRendererDeviceLost())
#define gSigRendererWindowSizeMove (GN::gfx::getSigRendererWindowSizeMove())
#define gSigRendererWindowClose    (GN::gfx::getSigRendererWindowClose())
//@}

namespace GN { namespace gfx
{
    struct Renderer;

    // ********************************************************************
    ///
    /// \name Renderer Signals
    ///
    /// - Renderer class may recreate itself when some system event happens,
    ///   like restoring from lock screen, which will trigger renderer create
    ///   and destroy signals.
    /// - Explicty (re)creation of renderer, will also trigger renderer signals.
    /// - �յ�destroy�źź�Ӧ��ɾ�����е�ͼ����Դ
    ///
    // ********************************************************************

    //@{

    ///
    /// D3D device is lost. The renderer, as well as all graphics resources, have to be recreated.
    ///
    GN_PUBLIC Signal1<void,Renderer&> & getSigRendererDeviceLost();

    ///
    /// Happens when render windows is moved or resized.
    ///
    /// The 3 parameters are:
    ///  - HandleType monior   : monitor handle that render window stays in
    ///  - UInt32 clientWidth  : width of client area of render window
    ///  - UInt32 clientHeight : height of client area of render window
    ///
    GN_PUBLIC Signal4<void, Renderer&, HandleType, UInt32, UInt32> & getSigRendererWindowSizeMove();

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
    GN_PUBLIC Signal1<void,Renderer&> & getSigRendererWindowClose();

    //@}

    ///
    /// Display mode structure
    ///
    struct DisplayMode
    {
        UInt32 width;   ///< Screen width. Zero means using current screen width. Default value is zero.
        UInt32 height;  ///< Screen height. Zero means using current screen height. Defualt value is zero.
        UInt32 depth;   ///< Color depth. Zero means using current color depth. Default value is zero.
        UInt32 refrate; ///< Referesh rate. Zero means using adapter default rate. Default value is zero.

        ///
        /// Set display mode parameters
        ///
        void set( UInt32 w, UInt32 h, UInt32 d, UInt32 r )
        {
            width = w; height = h; depth = d; refrate = r;
        }
    };

    ///
    /// Msaa type
    ///
    enum MsaaType
    {
        MSAA_NONE,      ///< No MSAA
        MSAA_LOW,       ///< low quality MSAA
        MSAA_MEDIUM,    ///< medium quality MSAA
        MSAA_HIGH,      ///< high quality MSAA
        MSAA_ULTRA,     ///< ultra quality MSAA
        NUM_MSAA_TYPES, ///< number of MSAA types
    };

    ///
    /// Define rendering API
    ///
    enum RendererAPI
    {
        API_OGL,          ///< OpenGL
        API_D3D9,         ///< D3D9
        API_D3D10,        ///< D3D10
        API_FAKE,         ///< Fake API
        NUM_RENDERER_API, ///< Number of avaliable API.
        API_AUTO,         ///< determine rendering API automatically.
    };

    ///
    /// Renderer option structure.
    ///
    /// \sa Renderer::getOptions()
    ///
    struct RendererOptions
    {
        ///
        /// Rendering API. Default value is API_AUTO.
        ///
        RendererAPI api;

        ///
        /// Display handle. No use on platform other than X Window. Default is zero.
        ///
        /// \note If zero, then default display will be used.
        ///
        HandleType displayHandle;

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
        /// Display mode for fullscreen mode. Ignored in windowed mode.
        ///
        /// \note For field equals zero, current display setting will be used.
        ///
        DisplayMode displayMode;

        ///
        /// Backbuffer width for windowed mode. Ignored in fullscreen mode.
        /// Default is zero, which means using client width of render window.
        /// If render window is also not avaiable, 640 will be used.
        ///
        UInt32 windowedWidth;

        ///
        /// Backbuffer height for windowed mode. Ignored in fullscreen mode.
        /// Default value is 0, which means using client height of render window.
        /// If render window is also not avaiable, default height 480 will be used.
        /// ȱʡΪ0.
        ///
        UInt32 windowedHeight;

        ///
        /// Use external render window or not.
        /// ȱʡΪfalse.
        ///
        bool useExternalWindow;

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
        bool reference;   ///< use reference device. ȱʡΪfalse.
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
            : api(API_AUTO)
            , displayHandle(0)
            , renderWindow(0)
            , parentWindow(0)
            , monitorHandle(0)
            , windowedWidth(0)
            , windowedHeight(0)
            , useExternalWindow(false)
            , fullscreen(false)
            , vsync(false)
            , reference(false)
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
        HandleType monitorHandle;    ///< Monitor handle.
        HandleType windowHandle;     ///< Render window handle
        UInt32     width;            ///< Back buffer width
        UInt32     height;           ///< Back buffer height
        UInt32     depth;            ///< Back buffer color depth in bits
        UInt32     refrate;          ///< Screen refresh rate

        ///
        /// equality operator
        ///
        bool operator!=( const DispDesc & rhs ) const
        {
            if( this == &rhs ) return false;
            return
                displayHandle != rhs.displayHandle ||
                monitorHandle != rhs.monitorHandle ||
                windowHandle != rhs.windowHandle ||
                width != rhs.width ||
                height != rhs.height ||
                depth != rhs.depth ||
                refrate != rhs.refrate;
        }
    };

    ///
    /// renderer caps
    ///
    struct RendererCaps
    {
        UInt32 maxTex1DSize[2];       ///< width, array
        UInt32 maxTex2DSize[3];       ///< width, height, array
        UInt32 maxTex3DSize[4];       ///< width, height, array
        UInt32 maxTextures;           ///< max number of simutaneous textures
        UInt32 maxColorRenderTargets; ///< max number of simutaneous render targets
        bool   vsProfiles[NUM_GPU_PROGRAM_PROFILES];
        bool   gsProfiles[NUM_GPU_PROGRAM_PROFILES];
        bool   psProfiles[NUM_GPU_PROGRAM_PROFILES];
    };

    ///
    /// enumerations used by renderer context structure
    ///
    enum RendererContextEnums
    {
        RC_FILL_SOLID = 0,
        RC_FILL_WIREFRAME,

        RC_CULL_NONE = 0,
        RC_CULL_FRONT,
        RC_CULL_BACK,

        RC_CMP_NEVER = 0,
        RC_CMP_LESS,
        RC_CMP_LESS_EQUAL,
        RC_CMP_EQUAL,
        RC_CMP_GREATER_EQUAL,
        RC_CMP_GREATER,
        RC_CMP_NOT_EQUAL,
        RC_CMP_ALWAYS,

        RC_STENCIL_KEEP = 0,
        RC_STENCIL_ZERO,
        RC_STENCIL_REPLACE,
        RC_STENCIL_INC_SAT,
        RC_STENCIL_DEC_SAT,
        RC_STENCIL_INVERT,
        RC_STENCIL_INC,
        RC_STENCIL_DEC,

        RC_BLEND_ZERO = 0,
        RC_BLEND_ONE,
        RC_BLEND_SRC_COLOR,
        RC_BLEND_INV_SRC_COLOR,
        RC_BLEND_SRC_ALPHA,
        RC_BLEND_INV_SRC_ALPHA,
        RC_BLEND_DEST_ALPHA,
        RC_BLEND_INV_DEST_ALPHA,
        RC_BLEND_DEST_COLOR,
        RC_BLEND_INV_DEST_COLOR,
        RC_BLEND_BLEND_FACTOR,
        RC_BLEND_INV_BLEND_FACTOR,

        RC_BLEND_OP_ADD = 0,
        RC_BLEND_OP_SUB,
        RC_BLEND_OP_REV_SUB,
        RC_BLEND_OP_MIN,
        RC_BLEND_OP_MAX,
    };

    ///
    /// definition of single vertex element
    ///
    struct VertexElement
    {
        ColorFormat format;      ///< format of the element
        UInt16      stream;      ///< vertex buffer index
        UInt16      offset;      ///< offset of the element
        char        binding[64]; ///< binding to GPU program (null terminated string)
        UInt16      bindingIndex;///< binding index. Note that
                                 ///< Combination of binding name and index must
                                 ///< be unique across the vertex format structure

        bool operator==( const VertexElement & rhs ) const
        {
            return format == rhs.format
                && stream == rhs.stream
                && offset == rhs.offset
                && bindingIndex == rhs.bindingIndex
                && 0 == strCmp( binding, rhs.binding, 64 );
            return true;
        }

        bool operator!=( const VertexElement & rhs ) const
        {
            return !operator==( rhs );
        }
    };

    ///
    /// define input vertex format
    ///
    struct VertexFormat
    {
        enum
        {
            MAX_VERTEX_ELEMENTS = 32,
        };

        UInt32        numElements;                   ///< number of elements
        VertexElement elements[MAX_VERTEX_ELEMENTS]; ///< vertex element array

        bool operator==( const VertexFormat & rhs ) const
        {
            if( numElements != rhs.numElements ) return false;
            for( UInt32 i = 0; i < numElements; ++i )
            {
                if( elements[i] != rhs.elements[i] ) return false;
            }
            return true;
        }

        bool operator!=( const VertexFormat & rhs ) const
        {
            return !operator==( rhs );
        }
    };

    ///
    /// define a render target texture
    ///
    struct RenderTargetTexture
    {
        WeakRef<Texture> texture;
        UInt32           face;
        UInt32           level;
        UInt32           slice;

        bool operator==( RenderTargetTexture & rhs ) const
        {
            return texture == rhs.texture
                && face == rhs.face
                && level == rhs.level
                && slice == rhs.slice;
        }

        bool operator!=( RenderTargetTexture & rhs ) const
        {
            return !operator==( rhs );
        }
    };

    ///
    /// renderer context
    ///
    struct RendererContext
    {
        enum
        {
            MAX_VERTEX_BUFFERS       = 32,
            MAX_TEXTURES             = 32,
            MAX_COLOR_RENDER_TARGETS = 8,
        };

        union
        {
            UInt32 bitwiseFlags[2]; // all flags in 2 dwords.

            struct
            {
                // DWORD 0
                UInt32 fillMode       : 2;
                UInt32 cullMode       : 2;
                UInt32 scissorEnabled : 1;
                UInt32 msaaEnabled    : 1;
                UInt32 depthTest      : 1;
                UInt32 depthWrite     : 1;
                UInt32 depthFunc      : 3;
                UInt32 stencilEnabled : 1;
                UInt32 stencilPassOp  : 3; ///< pass both stencil and Z
                UInt32 stencilFailOp  : 3; ///< fail stencil (no z test at all)
                UInt32 stencilZFailOp : 3; ///< pass stencil but fail Z
                UInt32 nouse_0        : 11;///< no use. must be zero

                // DWORD 1
                UInt32 blendSrc       : 4;
                UInt32 blendDst       : 4;
                UInt32 blendOp        : 3;
                UInt32 blendSrcAlpha  : 4;
                UInt32 blendDstAlpha  : 4;
                UInt32 blendOpAlpha   : 3;
                UInt32 nouse_1        : 10;///< no use. must be zero
            };
        };

        /// blend factors for RGBA
        float  blendFactors[4];

        /// 4 bits x 8 render targets.
        UInt32 colorWriteMask;

        /// viewport. (0,0,0,0) is used to represent current size of render target.
        Rect<UInt32> viewport;

        /// Scissor rect. (0,0,0,0) is used to represent current size of the render target.
        Rect<UInt32> scissorRect;

        // TODO: depth bias

        /// vertex format
        VertexFormat vtxfmt;

        /// shader
        WeakRef<GpuProgram> gpuProgram;

        // Resources
        WeakRef<VtxBuf>     vtxbufs[MAX_VERTEX_BUFFERS];     ///< vertex buffers
        UInt32              strides[MAX_VERTEX_BUFFERS];     ///< strides for each vertex buffer. Set to 0 to use default stride.
        WeakRef<IdxBuf>     idxbuf;                          ///< index buffer
        WeakRef<Texture>    textures[MAX_TEXTURES];          ///< textures
        RenderTargetTexture crts[MAX_COLOR_RENDER_TARGETS];  ///< color render targets
        RenderTargetTexture dsrt;                            ///< depth stencil render target

        // TODO: sampler

        ///
        /// reset context to default value
        void resetToDefault()
        {
            // clear all flags
            bitwiseFlags[0] = 0;
            bitwiseFlags[1] = 0;

            fillMode = RC_FILL_SOLID;
            cullMode = RC_CULL_BACK;
            scissorEnabled = false;
            msaaEnabled = true;
            depthTest = true;
            depthWrite = true;
            depthFunc = RC_CMP_LESS;
            stencilEnabled = false;
            stencilPassOp = RC_STENCIL_KEEP;
            stencilFailOp = RC_STENCIL_KEEP;
            stencilZFailOp = RC_STENCIL_KEEP;

            blendSrc = RC_BLEND_INV_SRC_ALPHA;
            blendDst = RC_BLEND_INV_SRC_ALPHA;
            blendOp  = RC_BLEND_OP_ADD;
            blendSrcAlpha = RC_BLEND_INV_SRC_ALPHA;
            blendDstAlpha = RC_BLEND_INV_SRC_ALPHA;
            blendOpAlpha  = RC_BLEND_OP_ADD;

            blendFactors[0] =
            blendFactors[1] =
            blendFactors[2] =
            blendFactors[3] = 1.0f;

            colorWriteMask = 0xFFFFFFFF;

            viewport.set( 0, 0, 0, 0 );

            scissorRect.set( 0, 0, 0, 0 );

            vtxfmt.numElements = 0;

            gpuProgram.clear();

            for( size_t i = 0; i < GN_ARRAY_COUNT(vtxbufs); ++i ) vtxbufs[i].clear();
            for( size_t i = 0; i < GN_ARRAY_COUNT(strides); ++i ) strides[i] = 0;
            idxbuf.clear();
            for( size_t i = 0; i < GN_ARRAY_COUNT(textures); ++i ) textures[i].clear();
            for( size_t i = 0; i < GN_ARRAY_COUNT(crts); ++i ) crts[i].texture.clear();
            dsrt.texture.clear();
        }
    };

    // make sure bit-wise flags occupy only 2 DWORDs.
    GN_CASSERT( GN_FIELD_OFFSET(RendererContext,blendFactors) == 2*sizeof(UInt32) );

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
    /// Options for Renderer::drawLines
    ///
    enum DrawLineOptions
    {
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
    };

    ///
    /// ��Ⱦ��ģ������ӿ���
    ///
    /// \nosubgrouping
    ///
    struct Renderer : public NoCopy
    {
        // ********************************************************************
        //
        /// \name Display Manager
        //
        // ********************************************************************

        //@{

        ///
        /// Get renderer options that are used to create this renderer.
        ///
        virtual const RendererOptions & getOptions() const = 0;

        ///
        /// Get Display Description
        ///
        virtual const DispDesc & getDispDesc() const = 0;

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

        ///
        /// Get render device caps
        ///
        virtual const RendererCaps & getCaps() const = 0;

        ///
        /// Check texture format support
        ///
        /// \param format       The texture format.
        /// \param usages       Combination of TextureUsage
        ///
        virtual bool checkTextureFormatSupport( ColorFormat format, TextureUsages usages ) const = 0;

        ///
        /// Get default texture format.
        ///
        /// \param usage        Combination of TextureUsage
        ///
        /// \return             Return COLOR_FORMAT_UNKNOWN, if the usage is not supported by current renderer.
        ///
        virtual ColorFormat getDefaultTextureFormat( TextureUsages usages ) const = 0;

        //@}

        // ********************************************************************
        //
        /// \name Resource Manager
        //
        // ********************************************************************

        //@{

        ///
        /// Compile shader into platform dependant format
        ///
        virtual CompiledGpuProgram *
        compileGpuProgram( const GpuProgramDesc & desc ) = 0;

        ///
        /// create shader
        ///
        virtual GpuProgram *
        createGpuProgram( const void * compiledGpuProgramBinary, size_t length ) = 0;

        ///
        /// create shader directly from description
        ///
        GpuProgram *
        createGpuProgram( const GpuProgramDesc & desc )
        {
            AutoRef<CompiledGpuProgram> bin( compileGpuProgram( desc ) );
            if( !bin ) return NULL;
            return createGpuProgram( bin->data(), bin->size() );
        }

        ///
        /// Create new texture
        /// See TextureDesc for detail explaination of each fields in descriptor.
        ///
        virtual Texture *
        createTexture( const TextureDesc & desc ) = 0;

        ///
        /// Create new texture, with individual creation parameters.
        ///
        Texture *
        createTexture( size_t        sx,
                       size_t        sy,
                       size_t        sz,
                       size_t        faces  = 1,
                       size_t        levels = 0, // 0 means full mipmap chain
                       ColorFormat   format = COLOR_FORMAT_UNKNOWN,
                       TextureUsages usages = TextureUsages::DEFAULT() )
        {
            TextureDesc desc =
            {
                (UInt32)sx, (UInt32)sy, (UInt32)sz,
                (UInt32)faces, (UInt32)levels,
                COLOR_FORMAT_UNKNOWN == format ? getDefaultTextureFormat( usages ) : format,
                usages,
            };
            return createTexture( desc );
        }

        ///
        /// Create 1D texture
        ///
        Texture *
        create1DTexture( size_t        sx,
                         size_t        levels = 0,
                         ColorFormat   format = COLOR_FORMAT_UNKNOWN,
                         TextureUsages usages = TextureUsages::DEFAULT() )
        {
            return createTexture( sx, 1, 1, 1, levels, format, usages );
        }

        ///
        /// Create 2D texture
        ///
        Texture *
        create2DTexture( size_t        sx,
                         size_t        sy,
                         size_t        levels = 0,
                         ColorFormat   format = COLOR_FORMAT_UNKNOWN,
                         TextureUsages usages = TextureUsages::DEFAULT() )
        {
            return createTexture( sx, sy, 1, 1, levels, format, usages );
        }

        ///
        /// Create 3D texture
        ///
        Texture *
        create3DTexture( size_t        sx,
                         size_t        sy,
                         size_t        sz,
                         size_t        levels = 0,
                         ColorFormat   format = COLOR_FORMAT_UNKNOWN,
                         TextureUsages usages = TextureUsages::DEFAULT() )
        {
            return createTexture( sx, sy, sz, 1, levels, format, usages );
        }

        ///
        /// Create CUBE texture
        ///
        Texture *
        createCubeTexture( size_t        sx,
                           size_t        levels = 0,
                           ColorFormat   format = COLOR_FORMAT_UNKNOWN,
                           TextureUsages usages = TextureUsages::DEFAULT() )
        {
            return createTexture( sx, sx, 1, 6, levels, format, usages );
        }

        ///
        /// Create new vertex buffer
        ///
        virtual VtxBuf *
        createVtxBuf( const VtxBufDesc & ) = 0;

        ///
        /// Create new index buffer
        ///
        virtual IdxBuf *
        createIdxBuf( const IdxBufDesc & desc ) = 0;

        ///
        /// Create new index buffer
        ///
        IdxBuf *
        createIdxBuf( size_t numidx, bool dynamic = false, bool readback = false )
        {
            IdxBufDesc desc;
            desc.numidx   = (UInt32)numidx;
            desc.bits32   = false;
            desc.dynamic  = dynamic;
            desc.readback = readback;
            return createIdxBuf( desc );
        }

        //@}

        // ********************************************************************
        //
        /// \name Context Manager
        //
        // ********************************************************************

        //@{

        ///
        /// Bind rendering context to rendering device.
        ///
        virtual void bindContext( const RendererContext & ) = 0;

        ///
        /// Rebind current rendering context to rendering device.
        ///
        /// Renderer usually has internal cache mechanism to avoid
        /// redunant state changing. So if you call D3D/OGL functions
        /// directly in your code that changes D3D/OGL states, this cache
        /// mechanism will be broken. One way to avoid this situation, is
        /// to store/restore D3D/OGL states by yourself. Another way is to
        /// call this function to force rebinding of current renderer
        /// context, which is much easier and less error prone.
        ///
        virtual void rebindContext() = 0;

        ///
        /// Get current render context
        ///
        virtual const RendererContext & getContext() const = 0;

        //@}

        // ********************************************************************
        //
        /// \name Drawing Manager
        //
        // ********************************************************************

        //@{

        ///
        /// ����һ֡�Ļ�ͼ����
        ///
        virtual void present() = 0;

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
                     float z = 1.0f, UInt8 s = 0,
                     BitFields flags = CLEAR_ALL ) = 0;

        ///
        /// Draw indexed primitives.
        ///
        /// \param prim
        ///     primititive type
        /// \param numprim
        ///     number of primitives
        /// \param startvtx
        ///     vertex index into vertex buffer that index "0" refers to.
        /// \param minvtxidx, numvtx
        ///     define effective range in vertex buffer, starting from startvtx.
        /// \param startidx
        ///     index into index buffer of the first index
        ///
        /// \note ������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numprim,
                                  size_t        startvtx,
                                  size_t        minvtxidx,
                                  size_t        numvtx,
                                  size_t        startidx ) = 0;

        ///
        /// Draw non-indexed primitives.
        ///
        /// \param prim
        ///     primititive type
        /// \param numprim
        ///     number of primitives
        /// \param startvtx
        ///     index into vertex buffer of the first vertex.
        ///
        /// \note ������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual void draw( PrimitiveType prim,
                           size_t        numprim,
                           size_t        startvtx ) = 0;

        ///
        /// draw on-indexed primitives with user-defined data array
        ///
        virtual void drawIndexedUp(
                             PrimitiveType  prim,
                             size_t         numprim,
                             size_t         numvtx,
                             const void *   vertexData,
                             size_t         strideInBytes,
                             const UInt16 * indexData ) = 0;

        ///
        /// draw on-indexed primitives with user-defined data array
        ///
        virtual void drawUp( PrimitiveType prim,
                             size_t        numprim,
                             const void *  vertexData,
                             size_t        strideInBytes ) = 0;

        ///
        /// Draw line segments
        ///
        /// \param options
        ///     ��Ⱦѡ���� DrawLineOptions��Set to 0 to use default options
        /// \param positions
        ///     �����������ݣ���һϵ�е�3D������ɡ�2�������ʾһ���߶Ρ�
        ///     ѡ�� DL_WINDOW_SPACE��Ӱ������ĺ��塣
        /// \param stride
        ///     stride of one vertex.
        /// \param count
        ///     Number of line segments (note: _NOT_ number of points)
        /// \param rgba
        ///     line color, in R-G-B-A format.
        /// \param model, view, proj
        ///     Transformation matrices. Ignored when using DL_WINDOW_SPACE.
        ///
        virtual void drawLines( BitFields         options,
                                const void *      positions,
                                size_t            stride,
                                size_t            count,
                                UInt32            rgba,
                                const Matrix44f & model,
                                const Matrix44f & view,
                                const Matrix44f & proj ) = 0;

        //@}

        // ********************************************************************
        //
        /// \name Misc. utilities
        //
        // ********************************************************************

        //@{

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
        virtual void enableParameterCheck( bool enable ) = 0;

        ///
        /// dump device states of the next frame.
        ///
        /// \param startBatchIndex, numBatches
        ///     Specify range of the dump. Set (0,0) to dump the whole frame.
        ///
        /// \note
        ///     Dump data format is renderer dependent.
        ///     Check document of specific renderer implementation for details.
        ///
        virtual void dumpNextFrame( size_t startBatchIndex = 0, size_t numBatches = 0 ) = 0;

        //@}

        // ********************************************************************
        //
        /// \name Renderer DLL. utilities
        //
        // ********************************************************************

        //@{

    private:

        SharedLib * mSharedLib;
        friend GN_PUBLIC Renderer * createRenderer( const RendererOptions & );
        friend GN_PUBLIC void       deleteRenderer( Renderer * );

        //@}

    };

    ///
    /// Create a new renderer.
    ///
    GN_PUBLIC Renderer * createRenderer( const RendererOptions & );

    ///
    /// Delete renderer
    ///
    GN_PUBLIC void deleteRenderer( Renderer * );
}}

#include "renderer.inl"

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_RENDERER_H__
