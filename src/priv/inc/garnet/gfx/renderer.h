#ifndef __GN_GFX_RENDERER_H__
#define __GN_GFX_RENDERER_H__
// *****************************************************************************
/// \file
/// \brief   Main renderer interface of GFX module
/// \author  chenlee (2005.9.30)
// *****************************************************************************

namespace GN { namespace gfx
{
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
    struct MsaaType
    {
        enum Enum
        {
            NONE,      ///< No MSAA
            LOW,       ///< low quality MSAA
            MEDIUM,    ///< medium quality MSAA
            HIGH,      ///< high quality MSAA
            ULTRA,     ///< ultra quality MSAA
            NUM_TYPES, ///< number of MSAA types
        };

        GN_DEFINE_ENUM_CLASS_HELPERS( MsaaType, Enum )
    };

    ///
    /// Define rendering API enumeration
    ///
    struct RendererAPI
    {
        enum Enum
        {
            OGL,      ///< OpenGL
            D3D9,     ///< D3D9
            D3D10,    ///< D3D10
            XENON,    ///< Xenon
            FAKE,     ///< Fake API
            NUM_APIs, ///< Number of avaliable APIs.
            AUTO,     ///< determine rendering API automatically.
        };

        /// convert enum to string
        const char * toString() const
        {
            static const char * TABLE[] =
            {
                "OGL",
                "D3D9",
                "D3D10",
                "XENON",
                "FAKE",
                "INVALID_RENDERING_API",
                "AUTO"
            };

            const Enum & e = *this;

            if( 0 <= e && e <= AUTO ) return TABLE[e];
            else return "INVALID_RENDERING_API";
        }

        GN_DEFINE_ENUM_CLASS_HELPERS( RendererAPI, Enum );
    };

    ///
    /// Renderer option structure.
    ///
    /// \sa Renderer::getOptions()
    ///
    struct RendererOptions
    {
        ///
        /// Rendering API. Default value is RendererAPI::AUTO.
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
        /// Backbuffer MSAA type. Default value is MsaaType::NONE
        ///
        MsaaType msaa;

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

        ///
        /// create renderer with debug facilities enabled.
        ///
        /// Default value is true for debug build; false for profile and retail build.
        ///
        bool debug;

        /// \name D3D only parameters
        //@{

        ///
        /// use reference device. ȱʡΪfalse.
        ///
        bool reference;

        //@}

        /// \name OGL only parameters
        //@{

        ///
        /// Restore display mode while render window is deactivated.
        ///
        /// ȱʡΪtrue.
        ///
        /// Note that this is a OGL only parameter. For D3D, you may use
        /// "Enable Multi-mon Debugging" option in DirectX control panel.
        ///
        bool autoRestore;

        //@}

        ///
        /// Construct default render options
        ///
        RendererOptions()
            : api(RendererAPI::AUTO)
            , displayHandle(0)
            , renderWindow(0)
            , parentWindow(0)
            , monitorHandle(0)
            , windowedWidth(0)
            , windowedHeight(0)
            , msaa(MsaaType::NONE)
            , useExternalWindow(false)
            , fullscreen(false)
            , vsync(false)
            , debug( GN_BUILD_DEBUG )
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
        UInt32 vsLanguages;           ///< renderer supported vertex program languages.
        UInt32 gsLanguages;           ///< renderer supported geometry program languages.
        UInt32 psLanguages;           ///< renderer supported pixel program languages.
    };

    ///
    /// definition of single vertex element
    ///
    struct VertexElement
    {
        ColorFormat format;       ///< format of the element
        char        binding[16];  ///< binding to GPU program (null terminated string).
                                  ///< Checking of invalidate binding is not performed here, but when the whole
                                  ///< render context is being applied to renderer.
        UInt8       bindingIndex; ///< Binding index.
                                  ///< The binding name and index combination has to be unique across whole vertex format strcture.
        UInt8       stream;       ///< vertex buffer index
        UInt16      offset;       ///< offset of the element

        ///
        /// Bind vertex element to specific GL program attribute variable.
        ///
        /// Note that name and index are shader specific (thus, API specific).
        ///
        void bindTo( const char * variableName, size_t index = 0 )
        {
            size_t len = strLen( variableName );
            if( 0 == len )
            {
                GN_ERROR(getLogger("GN.gfx.VertexElement"))( "Empty binding string is not allowed." );
                return;
            }

            if( len >= GN_ARRAY_COUNT(binding) )
            {
                GN_ERROR(getLogger("GN.gfx.VertexElement"))(
                    "Binding string (%s) is too long. Maxinum length is 16 characters including ending zero.",
                    variableName );
            }
            len = math::getmin<size_t>( GN_ARRAY_COUNT(binding), len+1 );
            memcpy( binding, variableName, len );

            if( index > 255 )
            {
                GN_ERROR(getLogger("GN.gfx.VertexElement"))(
                    "Binding index (%d) is too large. Maxinum value is 255.",
                    index );
                bindingIndex = 0;
            }
            else
            {
                bindingIndex = (UInt8)index;
            }
        }

        /// equality check
        bool operator==( const VertexElement & rhs ) const
        {
            return format == rhs.format
                && stream == rhs.stream
                && offset == rhs.offset
                && bindingIndex == rhs.bindingIndex
                && 0 == strCmp( binding, rhs.binding, sizeof(binding) );
        }

        /// equality check
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
            MAX_VERTEX_ELEMENTS = 16,
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

        bool operator<( const VertexFormat & rhs ) const
        {
            if( this == &rhs ) return false;

            const UInt32 * a = (const UInt32*)this;
            const UInt32 * b = (const UInt32*)&rhs;
            size_t         n = sizeof(*this)/4;

            for( UInt32 i = 0; i < n; ++i )
            {
                if( a[i] < b[i] ) return true;
                if( a[i] > b[i] ) return false;
            }

            return false;
        }
    };

    ///
    /// Vertex buffer binding description
    ///
    struct VertexBufferBinding
    {
        AutoRef<VtxBuf> vtxbuf; ///< vertex buffer
        UInt16          stride; ///< vertex stride. 0 means using vertex stride defined in vertex format structure.
        UInt32          offset; ///< Number of bytes from vertex buffer begining to the first element that will be used.

        /// ctor
        VertexBufferBinding()
            : stride(0)
            , offset(0)
        {
        }
    };

    ///
    /// texture binding descriptor
    ///
    struct TextureBinding
    {
        AutoRef<Texture> texture;     ///< texture pointer
        char             binding[16]; ///< binding to specific GPU program parameter

        ///
        /// clear to empty
        ///
        void clear()
        {
            texture.clear();
            binding[0] = '\0';
        }

        ///
        /// Bind to specific GPU program parameter.
        ///
        void bindTo( const char * gpuTextureParameterName )
        {
            size_t len = strLen( gpuTextureParameterName );
            if( 0 == len )
            {
                GN_ERROR(getLogger("GN.gfx.TextureBinding"))( "Empty binding string is not allowed." );
                return;
            }

            if( len >= GN_ARRAY_COUNT(binding) )
            {
                GN_ERROR(getLogger("GN.gfx.binding"))(
                    "GPU program parameter name (%s) is too long. Maxinum length is %d characters including ending zero.",
                    gpuTextureParameterName,
                    GN_ARRAY_COUNT(binding) );
            }
            len = math::getmin<size_t>( GN_ARRAY_COUNT(binding), len+1 );
            memcpy( binding, gpuTextureParameterName, len );
        }
    };

    ///
    /// define a texture sampler
    ///
    struct TextureSampler
    {
        enum SamplerEnum
        {
            FILTER_POINT  = 0,
            FILTER_LINEAR,

            ADDRESS_REPEAT = 0,
            ADDRESS_CLAMP,
            ADDRESS_CLAMP_BORDER,
            ADDRESS_MIRROR,
        };

        union
        {
            unsigned char filters; ///< all filters in one char.
            struct
            {
                unsigned char filterMin : 1; ///< Minify filter. Default is LINEAR.
                unsigned char filterMip : 1; ///< Mipmap filter. Default is LINEAR. Ignored, if the texture has no mipmap.
                unsigned char filterMag : 1; ///< Magnify filter. Default is LINEAR.
                unsigned char aniso     : 5; ///< Specify maximum degree of anisotropy filter.
                                             ///< Set to 0 for standard filtering w/o anisotropy.
                                             ///< Set to positive values to enable anisotropic filtering.
                                             ///< Maximum allowed value is hardware dependent.
            };
        };

        union
        {
            unsigned short addressModes;
            struct
            {
                unsigned short addressU : 3; ///< address mode at U direction. Default is REPEAT.
                unsigned short addressV : 3; ///< address mode at V direction. Default is REPEAT.
                unsigned short addressW : 3; ///< address mode at W direction. Default is REPEAT.
                unsigned short nouse_1  : 7; ///< no use. Must be zero.
            };
        };

        UInt8       border[4]; ///< border color in R-G-B-A. Default is (0,0,0,0)

        float       mipbias;   ///< Mip bias. Default is 0.0
        float       minlod;    ///< Min mipmap level. Default is zero
        float       maxlod;    ///< Max mipmap level. Default is negative that means no limination

        void clear()
        {
            filterMin = filterMip = filterMag = FILTER_LINEAR;
            aniso = 0;
            addressModes = 0;
            addressU = addressV = addressW = ADDRESS_REPEAT;
            border[0] = border[1] = border[2] = border[3] = 0;
            mipbias = 0.0f;
            minlod = 0.0f;
            maxlod = -1.0f;
        }

        bool operator==( const TextureSampler & rhs ) const
        {
            return filters == rhs.filters
                && addressModes == rhs.addressModes
                && *(UInt32*)border == *(UInt32*)rhs.border
                && mipbias == rhs.mipbias
                && minlod == rhs.minlod
                && maxlod == rhs.maxlod;
        }

        bool operator!=( const TextureSampler & rhs ) const
        {
            return !operator==( rhs );
        }
    };

    ///
    /// define a render target texture
    ///
    struct RenderTargetTexture
    {
        AutoRef<Texture>     texture;

        union
        {
            UInt32           subsurface;
            struct
            {
                unsigned int face  : 12;
                unsigned int level : 8;
                unsigned int slice : 12;
            };
        };

        /// ctor
        RenderTargetTexture() : subsurface(0) {}

        /// equality check
        bool operator==( const RenderTargetTexture & rhs ) const
        {
            return texture == rhs.texture && subsurface == rhs.subsurface;
        }

        /// equality check
        bool operator!=( const RenderTargetTexture & rhs ) const
        {
            return texture != rhs.texture || subsurface != rhs.subsurface;
        }
    };

    ///
    /// render targets description
    ///
    struct RenderTargetDesc
    {
        enum
        {
            /// Maxinum number of color render targets
            MAX_COLOR_RENDER_TARGETS = 8,
        };

        /// color render targets
        StackArray<RenderTargetTexture, MAX_COLOR_RENDER_TARGETS> colors;

        /// depth stencil render target
        RenderTargetTexture                                       depthstencil;

        /// clear to "render-to-back-buffer"
        void clear()
        {
            colors.clear();
            depthstencil.texture.clear();
        }

        /// return true, if the description represents the render target setup for rendering to back buffer.
        bool isRenderingToBackBuffer() const
        {
            return 0 == colors.size() && 0 == depthstencil.texture;
        }

        /// return true, if color buffers are empty and depth render target is not.
        bool isRenderingToDepthTextureOnly() const
        {
            return 0 == colors.size() && 0 != depthstencil.texture;
        }

        /// check for invalid description.
        bool valid() const
        {
            for( size_t i = 0; i < colors.size(); ++i )
            {
                if( !colors[i].texture )
                {
                    GN_ERROR(GN::getLogger("GN.gfx"))(
                        "NULL color render targets in render target array is not allowed." );
                    return false;
                }
            }

            return true;
        }

        /// equality check
        bool operator==( const RenderTargetDesc & rhs ) const
        {
            if( colors.size() != rhs.colors.size() ) return false;
            for( size_t i = 0; i < colors.size(); ++i )
            {
                if( colors[i] != rhs.colors[i] ) return false;
            }
            return depthstencil == rhs.depthstencil;
        }

        /// equality check
        bool operator!=( const RenderTargetDesc & rhs ) const
        {
            return !operator==( rhs );
        }
    };

    ///
    /// interface of GPU uniform
    ///
    struct Uniform : public RefCounter
    {
        /// get parameter size
        virtual size_t size() const = 0;

        /// get current parameter value
        virtual const void * getval() const = 0;

        /// update parameter value
        virtual void update( size_t offset, size_t length, const void * data ) = 0;

        /// update parameter value
        template<typename T>
        void update( const T & t ) { update( 0, sizeof(t), &t ); }
    };

    ///
    /// renderer context
    ///
    struct RendererContext
    {
        ///
        /// enumerations used by renderer context structure
        ///
        enum RendererContextEnum
        {
            MAX_VERTEX_BUFFERS       = 16,
            MAX_TEXTURES             = 32,

            FILL_SOLID = 0,
            FILL_WIREFRAME,
            FILL_POINT,
            NUM_FILL_MODES,

            FRONT_CCW = 0,
            FRONT_CW,
            NUM_FRONT_FACE_MODES,

            CULL_NONE = 0,
            CULL_FRONT,
            CULL_BACK,
            NUM_CULL_MODES,

            CMP_NEVER = 0,
            CMP_LESS,
            CMP_LESS_EQUAL,
            CMP_EQUAL,
            CMP_GREATER_EQUAL,
            CMP_GREATER,
            CMP_NOT_EQUAL,
            CMP_ALWAYS,
            NUM_CMP_FUNCTIONS,

            STENCIL_KEEP = 0,
            STENCIL_ZERO,
            STENCIL_REPLACE,
            STENCIL_INC_SAT,
            STENCIL_DEC_SAT,
            STENCIL_INVERT,
            STENCIL_INC,
            STENCIL_DEC,
            NUM_STENCIL_OPERATIONS,

            BLEND_ZERO = 0,
            BLEND_ONE,
            BLEND_SRC_COLOR,
            BLEND_INV_SRC_COLOR,
            BLEND_SRC_ALPHA,
            BLEND_INV_SRC_ALPHA,
            BLEND_DEST_ALPHA,
            BLEND_INV_DEST_ALPHA,
            BLEND_DEST_COLOR,
            BLEND_INV_DEST_COLOR,
            BLEND_BLEND_FACTOR,
            BLEND_INV_BLEND_FACTOR,
            NUM_BLEND_ARGUMENTS,

            BLEND_OP_ADD = 0,
            BLEND_OP_SUB,
            BLEND_OP_REV_SUB,
            BLEND_OP_MIN,
            BLEND_OP_MAX,
            NUM_BLEND_OPERATIONS,
        };

        union
        {
            UInt64 renderStates; ///< all render states in single 64bit integer.

            struct
            {
                // BYTE 0
                union
                {
                    UInt8     miscFlags0;
                    struct
                    {
                        UInt8 fillMode       : 2;
                        UInt8 cullMode       : 2;
                        UInt8 frontFace      : 1;
                        UInt8 msaaEnabled    : 1;
                        UInt8 miscNoUse      : 2; //< no use. Must be zero.
                    };
                };

                // BYTE 1
                union
                {
                    UInt8     depthFlags;
                    struct
                    {
                        UInt8 depthTest  : 1;
                        UInt8 depthWrite : 1;
                        UInt8 depthFunc  : 3;
                        UInt8 depthNoUse : 3;
                    };
                };

                // BYTE 2-3
                union
                {
                    UInt16     stencilFlags;
                    struct
                    {
                        UInt16 stencilEnabled : 1;
                        UInt16 stencilPassOp  : 3; ///< pass both stencil and Z
                        UInt16 stencilFailOp  : 3; ///< fail stencil (no z test at all)
                        UInt16 stencilZFailOp : 3; ///< pass stencil but fail Z
                        UInt16 stencilNoUse   : 6; ///< no use. must be zero
                    };
                };

                // DWORD 1
                union
                {
                    UInt32     blendFlags;
                    struct
                    {
                        UInt32 blendSrc       : 4;
                        UInt32 blendDst       : 4;
                        UInt32 blendOp        : 3;
                        UInt32 blendEnabled   : 1;
                        UInt32 blendAlphaSrc  : 4;
                        UInt32 blendAlphaDst  : 4;
                        UInt32 blendAlphaOp   : 3;
                        UInt32 nouse_1        : 9;///< no use. must be zero
                    };
                };
            };
        };

        /// blend factors for RGBA
        Vector4f blendFactors;

        /// 4 bits x 8 render targets.
        UInt32 colorWriteMask;

        /// viewport. (0,0,0,0) is used to represent current size of render target.
        Rect<UInt32> viewport;

        /// Scissor rect. (0,0,0,0) is used to represent current size of the render target.
        Rect<UInt32> scissorRect;

        // TODO: depth bias

        /// shader
        AutoRef<GpuProgram> gpuProgram;

        // Resources

        /// Uniform array, in the order described in GPU program uniform descriptor.
        DynaArray<AutoRef<Uniform> >                              uniforms; ///< uniforms

        FixedArray<AutoRef<VtxBuf>, MAX_VERTEX_BUFFERS>           vtxbufs;  ///< vertex buffers
        FixedArray<UInt16,          MAX_VERTEX_BUFFERS>           strides;  ///< stride for each vertex buffer, in bytes. Set to 0 to use default stride.
        FixedArray<UInt32,          MAX_VERTEX_BUFFERS>           offsets;  ///< offset for each vertex buffer, in bytes.
        VertexFormat                                              vtxfmt;   ///< vertex format (bindings to GPU program)

        AutoRef<IdxBuf>                                           idxbuf;   ///< index buffer

        FixedArray<TextureBinding, MAX_TEXTURES>                  textures; ///< textures
        FixedArray<TextureSampler, MAX_TEXTURES>                  samplers; ///< samplers

        RenderTargetDesc                                          rendertargets; ///< render targets

        ///
        /// ctor
        ///
        RendererContext() { clear(); }

        ///
        /// reset context to default value
        ///
        void clear()
        {
            // clear all render states first
            renderStates = 0;

            fillMode = FILL_SOLID;
            cullMode = CULL_BACK;
            frontFace = FRONT_CCW;
            msaaEnabled = true;
            depthTest = true;
            depthWrite = true;
            depthFunc = CMP_LESS;
            stencilEnabled = false;
            stencilPassOp = STENCIL_KEEP;
            stencilFailOp = STENCIL_KEEP;
            stencilZFailOp = STENCIL_KEEP;

            blendSrc = BLEND_SRC_ALPHA;
            blendDst = BLEND_INV_SRC_ALPHA;
            blendOp  = BLEND_OP_ADD;
            blendAlphaSrc = BLEND_SRC_ALPHA;
            blendAlphaDst = BLEND_INV_SRC_ALPHA;
            blendAlphaOp  = BLEND_OP_ADD;

            blendFactors.set( 0.0f, 0.0f, 0.0f, 1.0f );

            colorWriteMask = 0xFFFFFFFF;

            viewport.set( 0, 0, 0, 0 );

            scissorRect.set( 0, 0, 0, 0 );

            gpuProgram.clear();

            uniforms.clear();

            clearResources();
        }

        //
        // clear all resources binded to the context
        //
        void clearResources()
        {
            GN_CASSERT( GN_ARRAY_COUNT(vtxbufs) == GN_ARRAY_COUNT(strides) );
            GN_CASSERT( GN_ARRAY_COUNT(vtxbufs) == GN_ARRAY_COUNT(offsets) );
            for( size_t i = 0; i < GN_ARRAY_COUNT(vtxbufs); ++i )
            {
                vtxbufs[i].clear();
                strides[i] = 0;
                offsets[i] = 0;
            }
            vtxfmt.numElements = 0;

            idxbuf.clear();

            GN_CASSERT( GN_ARRAY_COUNT(textures) == GN_ARRAY_COUNT(samplers) );
            for( size_t i = 0; i < GN_ARRAY_COUNT(textures); ++i )
            {
                textures[i].clear();
                samplers[i].clear();
            }

            rendertargets.clear();
        }
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
    /// define public renderer signals
    ///
    struct RendererSignals
    {
        ///
        /// D3D device is lost. The renderer, as well as all graphics resources, have to be recreated.
        ///
        Signal0<void> deviceLost;

        ///
        /// Happens when render windows is resized or moved to another monitor.
        ///
        /// The 3 parameters are:
        ///  - HandleType monior   : monitor handle that render window stays in
        ///  - UInt32 clientWidth  : width of client area of render window
        ///  - UInt32 clientHeight : height of client area of render window
        ///
        Signal3<void, HandleType, UInt32, UInt32> rendererWindowSizeMove;

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
        Signal0<void> rendererWindowClose;
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
        /// \return             Return ColorFormat::UNKNOWN, if the usage is not supported by current renderer.
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
        /// create GPU uniform
        ///
        virtual Uniform *
        createUniform( size_t size ) = 0;

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
                       ColorFormat   format = ColorFormat::UNKNOWN,
                       TextureUsages usages = TextureUsages::DEFAULT() )
        {
            TextureDesc desc =
            {
                (UInt32)sx, (UInt32)sy, (UInt32)sz,
                (UInt32)faces, (UInt32)levels,
                ColorFormat::UNKNOWN == format ? getDefaultTextureFormat( usages ) : format,
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
                         ColorFormat   format = ColorFormat::UNKNOWN,
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
                         ColorFormat   format = ColorFormat::UNKNOWN,
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
                         ColorFormat   format = ColorFormat::UNKNOWN,
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
                           ColorFormat   format = ColorFormat::UNKNOWN,
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
        /// Create new vertex buffer
        ///
        VtxBuf *
        createVtxBuf( size_t length, bool fastCpuWrite = false )
        {
            VtxBufDesc desc;
            desc.length       = length;
            desc.fastCpuWrite = fastCpuWrite;
            return createVtxBuf( desc );
        }

        ///
        /// Create new index buffer
        ///
        virtual IdxBuf *
        createIdxBuf( const IdxBufDesc & desc ) = 0;

        ///
        /// Create new 16 bit index buffer
        ///
        IdxBuf *
        createIdxBuf16( size_t numidx, bool fastCpuWrite = false )
        {
            IdxBufDesc desc;
            desc.numidx       = (UInt32)numidx;
            desc.bits32       = false;
            desc.fastCpuWrite = fastCpuWrite;
            return createIdxBuf( desc );
        }

        ///
        /// Create new 32 bit index buffer
        ///
        IdxBuf *
        createIdxBuf32( size_t numidx, bool fastCpuWrite = false )
        {
            IdxBufDesc desc;
            desc.numidx       = (UInt32)numidx;
            desc.bits32       = true;
            desc.fastCpuWrite = fastCpuWrite;
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
        /// If binding failed, the renderer will try to restore device state to previos context.
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
        /// \param numidx
        ///     number of indices
        /// \param basevtx
        ///     vertex index into vertex buffer that index "0" refers to.
        /// \param startvtx, numvtx
        ///     define effective range in vertex buffer, starting from basevtx.
        /// \param startidx
        ///     index into index buffer of the first index
        ///
        /// \note ������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numidx,
                                  size_t        basevtx,
                                  size_t        startvtx,
                                  size_t        numvtx,
                                  size_t        startidx ) = 0;

        ///
        /// Draw non-indexed primitives.
        ///
        /// \param prim
        ///     primititive type
        /// \param numvtx
        ///     number of vertices
        /// \param startvtx
        ///     index into vertex buffer of the first vertex.
        ///
        /// \note ������ drawBegin() �� drawEnd() ֮�����
        ///
        virtual void draw( PrimitiveType prim,
                           size_t        numvtx,
                           size_t        startvtx ) = 0;

        ///
        /// draw on-indexed primitives with user-defined data array
        ///
        virtual void drawIndexedUp(
                             PrimitiveType  prim,
                             size_t         numidx,
                             size_t         numvtx,
                             const void *   vertexData,
                             size_t         strideInBytes,
                             const UInt16 * indexData ) = 0;

        ///
        /// draw on-indexed primitives with user-defined data array
        ///
        virtual void drawUp( PrimitiveType prim,
                             size_t        numvtx,
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
        /// \param numpoints
        ///     Number of points (note: _NOT_ number of line segments)
        /// \param rgba
        ///     line color, in R-G-B-A format.
        /// \param model, view, proj
        ///     Transformation matrices. Ignored when using DL_WINDOW_SPACE.
        ///
        virtual void drawLines( BitFields         options,
                                const void *      positions,
                                size_t            stride,
                                size_t            numpoints,
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
        /// get renderer signals
        ///
        virtual RendererSignals & getSignals() = 0;

        ///
        /// Process render window messages, to keep render window responding to user inputs.
        ///
        virtual void processRenderWindowMessages( bool blockWhileMinimized ) = 0;

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

        ///
        /// Attatch/Deatch user data to/from the renderer.
        ///
        /// \param id               User data ID.
        /// \param data, length     User data buffer.
        ///
        /// - Renderer won't touch user data.
        /// - Renderer will make a copy the input data buffer, and overwriting any existing data with same ID.
        /// - Settting both data and length to zero, to delete exsiting user data.
        /// - User data buffer will be deleted automatically when the renderer is deleted.
        ///
        virtual void setUserData( const Guid & id, const void * data, size_t length ) = 0;

        ///
        /// Get user data
        ///
        /// \param id               User data ID.
        /// \param length           Optional parameter to return user data length.
        /// \return                 Return user data pointer. Return NULL if ID is not found.
        ///
        virtual const void * getUserData( const Guid & id, size_t * length = NULL ) const = 0;

        ///
        /// determine if there is the user data with specific ID
        ///
        virtual bool hasUserData( const Guid & id ) const = 0;

        ///
        /// get user data length.
        ///
        inline size_t getUserDataLength( const Guid & id ) const
        {
            size_t length;
            if( NULL == getUserData( id, &length ) ) length = 0;
            return length;
        }

        ///
        /// get user data: copy it to specified user specified buffer.
        ///
        /// \param id               User data ID.
        /// \param data, length     Target buffer to store user data.
        /// \return                 return bytes copied to target buffer. Return 0 for failure.
        ///
        inline size_t getUserData( const Guid & id, void * data, size_t length ) const
        {
            size_t srcLength;
            const void * src = getUserData( id, &srcLength );
            if( NULL == src ) return 0;
            size_t copyLength = srcLength < length ? srcLength : length;
            memcpy( data, src, copyLength );
            return copyLength;
        }

        //@}
    };

    ///
    /// Create new single thread renderer.
    ///
    Renderer * createSingleThreadRenderer( const RendererOptions & );

    ///
    /// Create new renderer with a simple multithread wrapper. So
    /// the renderer will run in another thread, and communite with
    /// user through an internal command buffer.
    ///
    Renderer * createMultiThreadRenderer( const RendererOptions & );

    ///
    /// Delete renderer
    ///
    void deleteRenderer( gfx::Renderer * );
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_RENDERER_H__
