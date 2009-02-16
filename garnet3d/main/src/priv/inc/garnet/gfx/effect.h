#ifndef __GN_GFX_EFFECT_H__
#define __GN_GFX_EFFECT_H__
// *****************************************************************************
/// \file
/// \brief   Graphics effect
/// \author  chenli@@REDMOND (2009.1.13)
// *****************************************************************************

namespace GN { namespace gfx
{
    struct EffectDesc
    {
        ///
        /// Shader Prerequisites
        ///
        struct ShaderPrerequisites
        {
            RendererCaps::GpuProgramProfile vsprofiles;  ///< required VS profiles.
            RendererCaps::GpuProgramProfile gsprofiles;  ///< required GS profiles.
            RendererCaps::GpuProgramProfile psprofiles;  ///< required PS profiles.
            UInt8                           numtextures; ///< minimal number of textures.

            /// check if the renderer meets all requirements.
            bool check( Renderer & r ) const;
        };

        ///
        /// Texture descriptor
        ///
        struct TextureDesc
        {
            /// sampler descriptor
            TextureSampler sampler;

            /// default ctor
            TextureDesc()
            {
                sampler.clear();
            }
        };

        ///
        /// Uniform descriptor
        ///
        struct UniformDesc
        {
            size_t           size;   ///< uniform size
            DynaArray<UInt8> defval; ///< default uniform value. Could be empty if there's no default value.

            /// default ctor
            UniformDesc() : size(0) {}
        };

        ///
        /// Shader descriptor
        ///
        struct ShaderDesc
        {
            GpuProgramDesc      gpd;           ///< GPU Program descriptor
            std::map<StrA,StrA> vertices;      ///< vertices. Key is vertex element binding, value is vertex element name.
            std::map<StrA,StrA> textures;      ///< textures. Key is texture binding, value is user-visible texture name.
            std::map<StrA,StrA> uniforms;      ///< uniforms. Key is uniform binding, value is user-visible uniform name.
            ShaderPrerequisites prerequisites; ///< prerequisites of the shader.
        };

        /// template for single render state
        template<typename T>
        struct RenderState
        {
            T    value;
            bool inherited; ///< if true, then this effect will inherit this value from current renderer context.

            /// default ctor
            RenderState() : inherited(true) {}

            /// set render state value
            RenderState & operator=( const T & rhs )
            {
                value = rhs;
                inherited = false;
                return *this;
            }

            /// set render state value
            RenderState & operator=( const RenderState<T> & rhs )
            {
                value = rhs.value;
                inherited = rhs.inherited;
                return *this;
            }
        };

        /// render state block
        struct RenderStateDesc
        {
            RenderState<UInt8> fillMode;
            RenderState<UInt8> cullMode;
            RenderState<UInt8> frontFace;
            RenderState<bool>  msaaEnabled;
            RenderState<bool>  depthTestEnabled;
            RenderState<bool>  depthWriteEnabled;
            RenderState<UInt8> depthFunc;

            // TODO: more render states.

            void clear()
            {
                fillMode.inherited = true;
                cullMode.inherited = true;
                frontFace.inherited = true;
                msaaEnabled.inherited = true;
                depthTestEnabled.inherited = true;
                depthWriteEnabled.inherited = true;
                depthFunc.inherited = true;
            }
        };

        ///
        /// Rendering pass descriptor
        ///
        struct PassDesc
        {
            StrA            shader; ///< Name of shader used in this pass. Can't be empty
            RenderStateDesc rsd;    ///< pass specific render states
        };

        ///
        /// Technique descriptor structure
        ///
        struct TechniqueDesc
        {
            int                      quality; ///< user defined rendering quality. Effect class uses
                                              ///< the technique, among available/valid techniques,
                                              ///< with the hightest quality as default active technique.
            DynaArray<PassDesc>      passes;  ///< pass list.
            RenderStateDesc          rsd;     ///< Technique specific render states

            /// default ctor
            TechniqueDesc() : quality(100) {}
        };

        std::map<StrA,TextureDesc>   textures;   ///< Texture list
        std::map<StrA,UniformDesc>   uniforms;   ///< Uniform list
        std::map<StrA,ShaderDesc>    shaders;    ///< Shader list
        std::map<StrA,TechniqueDesc> techniques; ///< Technique list. Technique name must be unique.
        RenderStateDesc              rsd;        ///< effect specific render states

        ///
        /// Make sure the effect descriptor is valid.
        ///
        bool valid() const;

        ///
        /// clear to a empty descriptor
        ///
        void clear()
        {
            textures.clear();
            uniforms.clear();
            shaders.clear();
            techniques.clear();
            rsd.clear();
        }

        ///
        /// setup the descriptor from XML
        ///
        bool loadFromXmlNode( const XmlNode & root, const StrA & basedir );

        ///
        /// write the descriptor to XML
        ///
        void saveToXmlNode( const XmlNode & root );
    };

    ///
    /// texture parameter wrapper for effect class
    ///
    class EffectTextureParameter
    {
        AutoRef<Texture> mTexture;

    public:

        /// get texture pointer
        Texture * getTexture() const { return mTexture.get(); }

        /// set texture pointer
        void setTexture( Texture * tex ) { mTexture.set( tex ); }

    protected:

        /// protected ctor
        EffectTextureParameter() {}

        /// protected copy ctor
        EffectTextureParameter( const EffectTextureParameter & t ) : mTexture(t.mTexture) {}

        /// protected dtor
        virtual ~EffectTextureParameter() {}
    };

    ///
    /// Graphics effect
    ///
    class Effect : public StdClass
    {
        GN_DECLARE_STDCLASS( Effect, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        Effect( Renderer & r );
        Effect( const Effect & ); // copy ctor
        virtual ~Effect();
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const EffectDesc & desc, const StrA & activeTechName = "" );
        void quit();
    private:
        void clear() { mActiveTech = NULL; }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        /// get descriptor
        const EffectDesc & getDesc() const { return mDesc; }

        /// get number of passes
        size_t getNumPasses() const { return mActiveTech->passes.size(); }

        /// Check if effect has a uniform with specific name.
        bool hasUniform( const StrA & name ) const { return mUniforms.end() != mUniforms.find( name ); }

        /// Get pointer to specific GPU uniform. Return dummy pointer for invalid name.
        ///
        /// Note that the reference counter of the returned parameter is not increaed by calling this function,
        /// which means you don't have to call decref() for the returned pointer after it is being used.
        Uniform * getUniform( const StrA & name ) const;

        /// Assign GPU uniform to effect
        void setUniform( const StrA & name, Uniform * );

        /// Get pointer to specific texture parameter. Return dummy pointer for invalid name.
        EffectTextureParameter * getTextureParam( const StrA & name );

        /// Apply the effect to drawable.
        bool applyToDrawable( Drawable & drawable, size_t pass ) const;

        /// copy operator (make clone)
        Effect & operator=( const Effect & rhs ) { if( this != &rhs ) { clone( rhs ); } return *this; }

        // ********************************
        // private variables
        // ********************************
    private:

        class EffectTextureParameterImpl : public EffectTextureParameter
        {
        public:
            EffectTextureParameterImpl() {}
            virtual ~EffectTextureParameterImpl() {}
        };

        struct PerShaderTextureParam
        {
            /// texture parameter wrapper, pointer to textures in Effect::mTextures
            EffectTextureParameterImpl * param;

            /// texture name (used to lookup the texture in Effect::mTextures)
            ///
            /// \note Effect::mTextures[name] == param
            ///
            StrA name;

            /// texture binding string
            StrA binding;

            /// pointer to sampler, pointer to Effec
            const TextureSampler * sampler;

            /// default ctor
            PerShaderTextureParam()
            {
            }

            /// copy ctor
            PerShaderTextureParam( const PerShaderTextureParam & p )
                : param(p.param)
                , name(p.name)
                , binding(p.binding)
                , sampler(p.sampler)
            {
            }

            /// assign operator
            PerShaderTextureParam & operator=( const PerShaderTextureParam & rhs )
            {
                param   = rhs.param;
                name    = rhs.name;
                binding = rhs.binding;
                sampler = rhs.sampler;
                return *this;
            }
        };

        typedef std::map<StrA,AutoRef<Uniform> >::iterator UniformIter;

        struct Pass
        {
            GpuProgram                     * gpuProgram; ///< Pointer to the GPU program
            DynaArray<PerShaderTextureParam> textures;   ///< Textures used in the pass.
            DynaArray<UniformIter>           uniforms;   ///< uniforms used in the pass. Note that offset of the uniform
                                                         ///< in this array is exactly same as the binding index to
                                                         ///< the GPU program in this pass.
            EffectDesc::RenderStateDesc rsd;             ///< render states
        };

        struct Technique
        {
            DynaArray<Pass> passes;
        };

        Renderer & mRenderer;
        EffectDesc mDesc;

        std::map<StrA,AutoRef<Uniform> >          mUniforms;
        std::map<StrA,EffectTextureParameterImpl> mTextures;
        std::map<StrA,AutoRef<GpuProgram> >       mGpuPrograms;
        std::map<StrA,Technique>                  mTechniques;
        Technique *                               mActiveTech;

        /// dummy parameters for invalid name
        Uniform                  * mDummyUniform;
        EffectTextureParameterImpl mDummyTexture;

        // ********************************
        // private functions
        // ********************************
    private:

        /// initialize technique
        bool initTech( Technique & tech, const StrA & name, const EffectDesc::TechniqueDesc & desc );

        /// make clone
        void clone( const Effect & );
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_EFFECT_H__
