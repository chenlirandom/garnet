#ifndef __GN_GFX_EFFECT_H__
#define __GN_GFX_EFFECT_H__
// *****************************************************************************
//! \file    gfx/effect.h
//! \brief   Graphics effect class (not implemented)
//! \author  chenlee (2006.1.7)
// *****************************************************************************

namespace GN { namespace gfx {
    //!
    //! Graphics effect descriptor
    //!
    struct EffectDesc
    {
        //!
        //! Contidional expression for renderer caps check
        //!
        class CondExp
        {
            enum TokenType
            {
                OPCODE, // opcode
                VALUEI, // integer value
                VALUES, // string value
            };

            enum OpCode
            {
                CMP_LT,
                CMP_LE,
                CMP_EQ,
                CMP_NE,
                CMP_GE,
                CMP_GT,

                ALU_ADD,
                ALU_DEC,
                ALU_NEG,

                BIT_AND,
                BIT_OR,
                BIT_XOR,
                BIT_NOT,

                REL_AND,
                REL_OR,
                REL_NOT,

                CHECK_RENDERER_CAPS,
                CHECK_SHADER_PROFILE,

                NUM_OPCODES,
            };

            struct OpCodeDesc
            {
                OpCode    op;
                TokenType dst;
                TokenType src0;
                TokenType src1;
                int       numArgs;
            };

            struct Token
            {
                TokenType type;
                union
                {
                    int32_t opcode;
                    int32_t valueI;
                };
                StrA  valueS;
            };

            // TODO: use custom allocator to optimize runtime memory allocation performance.

            AutoArray<Token> mTokens;

            //!
            //! Token descriptor table
            //!
            static const OpCodeDesc msOpCodeTable[NUM_OPCODES];

            //!
            //! Evaluate single opcode
            //!
            static bool sCalc( Token & result, int32_t op, const Token * s0, const Token * s1 );

            //!
            //! Evaluate expression of [p,e)
            //!
            static bool sDoEval( Token & result, const Token * & p, const Token * e );

        public:

            //!
            //! Default ctor
            //!
            CondExp() {}

            //!
            //! Construct from integer
            //!
            CondExp( int32_t i )
            {
                mTokens.resize(1);
                mTokens[0].type = VALUEI;
                mTokens[0].valueI = i;
            }

            //!
            //! Construct from string
            //!
            CondExp( const StrA & s )
            {
                mTokens.resize(1);
                mTokens[0].type = VALUES;
                mTokens[0].valueI = s;
            }

            //!
            //! Construct expression from specific operation.
            //!
            CondExp( OpCode op, const CondExp & c1, const CondExp & c2 );

            //!
            //! Copy constructor
            //!
            CondExp( const CondExp & c )
            {
                mTokens.resize( c.mTokens.size() );
                memcpy( mTokens, c.mTokens, sizeof(Token)*mTokens.size() );
            }

            //!
            //! Evaluate the expression.
            //!
            //! - Return true for:
            //!   - empty expression
            //!   - zero integer
            //!   - empty string
            //! - Return false for:
            //!   - invalid expression
            //!   - non-zero integer
            //!   - non-empty string
            //!
            bool evaluate() const;

            //!
            //! Construct expression from string. Setup a empty expression, if string is invalid.
            //!
            void fromStr( const char * s, size_t strLen = 0 );

            //!
            //! convert to string
            //!
            void toStr( StrA & s ) const;

            //!
            //! convert to string
            //!
            StrA toStr() const { StrA s; toStr(s); return s; }

            //!
            //! Copy operator
            //!
            CondExp & operator=( const CondExp & rhs )
            {
                mTokens.resize( rhs.mTokens.size() );
                memcpy( mTokens, rhs.mTokens, sizeof(Token)*mTokens.size() );
                return *this;
            }

            //@{
            //! \name CondExp constructors

            //!
            //! make new CondExp from gfxcaps
            //!
            static CondExp sGfxCaps( RendererCaps c )
            {
                CondExp exp;
                exp.mTokens.resize(2);
                exp.mTokens[0].type = OPCODE;
                exp.mTokens[0].opcode = CHECK_RENDERER_CAPS;
                exp.mTokens[1].type = VALUEI;
                exp.mTokens[1].valueI = c;
                return exp;
            }

            static CondExp sBitAnd( const CondExp & a0, const CondExp & a1 )
            {
                return CondExp( BIT_AND, a0, a1 );
            }

            static CondExp sBitOr( const CondExp & a0, const CondExp & a1 )
            {
                return CondExp( BIT_OR, a0, a1 );
            }

            static CondExp sBitXor( const CondExp & a0, const CondExp & a1 )
            {
                return CondExp( BIT_XOR, a0, a1 );
            }

#define GN_CONDEXP_OPERATOR( x, y ) \
            CondExp operator x ( const CondExp & rhs ) const \
            { \
                return CondExp( y, *this, rhs ); \
            }

            GN_CONDEXP_OPERATOR( <  , CMP_LT  );
            GN_CONDEXP_OPERATOR( <= , CMP_LE  );
            GN_CONDEXP_OPERATOR( == , CMP_EQ  );
            GN_CONDEXP_OPERATOR( != , CMP_NE  );
            GN_CONDEXP_OPERATOR( >= , CMP_GE  );
            GN_CONDEXP_OPERATOR( >  , CMP_GT  );
            GN_CONDEXP_OPERATOR( && , REL_AND );
            GN_CONDEXP_OPERATOR( || , REL_OR  );

#undef GN_CONDEXP_OPERATOR

            //@}
        };

        //!
        //! Texture descriptor
        //!
        struct TextureDesc
        {
            // TODO: filters, wraps ...
        };

        //!
        //! Uniform descriptor
        //!
        struct UniformDesc
        {
            bool hasDefaultValue;      //!< Indicates that the uniform has a default value.
            UniformValue defaultValue; //!< Default value of the uniform. Ignored if hasDefaultValue is false.

            UniformDesc() : hasDefaultValue(false) {}
        };

        //!
        //! Shader descriptor
        //!
        struct ShaderDesc
        {
            ShaderType type; //!< Shader type.
            ShadingLanguage lang; //!< Shading language. Ignored if code is empty.
            StrA code; //!< Shader code. Empty means fixed functional pipeline.
            StrA hints; //!< Shader hints.
            std::map<uint32_t,StrA> textures; //!< textures used by the shader. Key is texture stage, value is texture name.
            std::map<StrA,StrA>     uniforms; //!< uniforms used by the shader. Key is uniform binding, value is uniform name.
            CondExp conditions; //!< required renderer caps of this shader.
        };

        //!
        //! Rendering pass descriptor
        //!
        struct PassDesc
        {
            RenderStateBlockDesc rsb; //!< Pass specific render state block
            StrA shaders[NUM_SHADER_TYPES]; //!< shader names of the pass. Can't be empty

            //!
            //! Ctor
            //!
            PassDesc() : rsb(RenderStateBlockDesc::EMPTY) {}
        };

        //!
        //! Technique descriptor structure
        //!
        struct TechniqueDesc
        {
            StrA                  name; //!< technique name
            std::vector<PassDesc> passes; //!< pass list.
            RenderStateBlockDesc  rsb; //!< Technique specific render states

            //!
            //! Ctor
            //!
            TechniqueDesc() : rsb(RenderStateBlockDesc::EMPTY) {}
        };

        std::map<StrA,TextureDesc> textures;   //!< Texture list
        std::map<StrA,UniformDesc> uniforms;   //!< Uniform list
        std::map<StrA,ShaderDesc>  shaders;    //!< Shader list
        std::vector<TechniqueDesc> techniques; //!< Technique list. Technique name must be unique.
        RenderStateBlockDesc       rsb;        //!< Effect global render states

        //!
        //! Ctor
        //!
        EffectDesc() : rsb(RenderStateBlockDesc::DEFAULT) {}

        //!
        //! Make sure the effect descriptor is valid.
        //!
        bool valid() const;

        //!
        //! clear to a empty descriptor
        //!
        void clear()
        {
            textures.clear();
            uniforms.clear();
            shaders.clear();
            techniques.clear();
            rsb.resetToDefault();
        }

        //!
        //! setup the descriptor from XML string
        //!
        bool fromXml( const char * str, size_t size = 0 );

        //!
        //! setup the descriptor from XML string
        //!
        bool fromXml( File & );

        //!
        //! write the descriptor to file
        //!
        void toXml( File & );

        //!
        //! Find uniform by name. Return NULL if not found.
        //!
        const UniformDesc * findUniform( const StrA & name ) const
        {
            std::map<StrA,UniformDesc>::const_iterator i = uniforms.find( name );
            return ( uniforms.end() != i ) ? &i->second : NULL;
        }

        //!
        //! Get uniform by name.
        //! \param name
        //!     MUST be a valid uniform name.
        //!
        const UniformDesc & getUniform( const StrA & name ) const
        {
            std::map<StrA,UniformDesc>::const_iterator i = uniforms.find( name );
            GN_ASSERT( uniforms.end() != i );
            return i->second;
        }

        //!
        //! Find texture by name. Return NULL if not found.
        //!
        const TextureDesc * findTexture( const StrA & name ) const
        {
            std::map<StrA,TextureDesc>::const_iterator i = textures.find( name );
            return ( textures.end() != i ) ? &i->second : NULL;
        }

        //!
        //! Get texture by name.
        //! \param name
        //!     MUST be a valid texture name.
        //!
        const TextureDesc & getTexture( const StrA & name ) const
        {
            std::map<StrA,TextureDesc>::const_iterator i = textures.find( name );
            GN_ASSERT( textures.end() != i );
            return i->second;
        }

        //!
        //! Find shader by name. Return NULL if not found.
        //!
        const ShaderDesc * findShader( const StrA & name ) const
        {
            std::map<StrA,ShaderDesc>::const_iterator i = shaders.find( name );
            return ( shaders.end() != i ) ? &i->second : NULL;
        }

        //!
        //! Get shader by name.
        //! \param name
        //!     MUST be a valid shader name.
        //!
        const ShaderDesc & getShader( const StrA & name ) const
        {
            std::map<StrA,ShaderDesc>::const_iterator i = shaders.find( name );
            GN_ASSERT( shaders.end() != i );
            return i->second;
        }
    };

    typedef uint32_t EffectItemID; //!< effect item (such as technique, uniform and texture) ID.

    //!
    //! Effect class
    //!
    class Effect : public StdClass
    {
        GN_DECLARE_STDCLASS( Effect, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        Effect()          { clear(); }
        virtual ~Effect() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const EffectDesc & ); //!< initialize from descriptor
        bool init( const Effect & ); //!< Make effect clone
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() { mPassBegun = false; }
        //@}

        // ********************************
        //! \name rendering functions
        //!
        //! Standard call sequence:
        //! <pre>
        //!     set_common_uniforms_and_textures();
        //!     size_t numPasses = myEffect->getNumPasses() )
        //!     for( size_t i = 0; i < numPasses; ++i )
        //!     {
        //!         myEffect->passBegin( i );
        //!         for_each_mesh
        //!         {
        //!             set_mesh_specific_uniforms_textures();
        //!             set_mesh_vertex_and_index_buffers();
        //!             myEffect->commitChanges();
        //!             draw_the_mesh();
        //!         }
        //!         myEffect->passEnd();
        //!     }
        //!     myEffect->drawEnd();
        //! </pre>
        // ********************************
    public:

        //@{

        //!
        //! Get rendering pass count. This value may change when active technique changes.
        //!
        size_t getNumPasses() const;

        //!
        //! apply render state of specific pass.
        //!
        void passBegin( size_t ) const;

        //!
        //! end the rendering pass.
        //! Must be called between drawBegin() and drawEnd(), and after passBegin().
        //!
        void passEnd() const { GN_ASSERT(mPassBegun); mPassBegun = false; }

        //!
        //! Commit modified uniforms and textures to renderer.
        //! Must be called between passBegin() and passEnd().
        //!
        void commitChanges() const;

        //@}

        // ********************************
        //! \name technique management
        // ********************************
    public:

        //@{

        //!
        //! get technique ID
        //!
        EffectItemID getTechniqueID( const StrA & name ) const;

        //!
        //! set active technique. 0 means the default one.
        //!
        void setActiveTechnique( EffectItemID ) const;

        //!
        //! Set active technique.
        //! \param name Technique name. Empty string means the default one.
        //!
        void setActiveTechniqueByName( const StrA & name ) const;

        //@}

        // ********************************
        //! \name uniform management
        // ********************************
    public:

        //@{
        EffectItemID getUniformID( const StrA & ) const;
        void setUniform( EffectItemID, const UniformValue & ) const;
        void setUniformByName( const StrA &, const UniformValue & ) const;
        //@}

        // ********************************
        //! \name texture management
        //!
        //! Note that effect class won't hold reference of the textures.
        // ********************************
    public:

        //@{
        EffectItemID getTextureID( const StrA & ) const;
        void setTexture( EffectItemID, const Texture * ) const;
        void setTextureByName( const StrA &, const Texture * ) const;
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        struct TextureData
        {
            StrA            name;  // texture name
            const Texture * value;
        };

        struct TextureRefData
        {
            EffectItemID id; // texture ID that is referenced.
            uint32_t     stage;
        };

        struct ShaderRefData
        {
            EffectItemID id;    // shader ID that is referenced
            size_t       index; // index to ShaderData::uniforms: which uniform are we referencing to.
        };

        struct UniformData
        {
            StrA                        name;
            UniformValue                value;
            std::vector<ShaderRefData>  shaders; // shaders that are referencing this uniform.
        };

        struct UniformRefData
        {
            EffectItemID id; //!< uniform ID that is being referenced.
            union
            {
                uint32_t shaderUniformHandle; //!< shader-specific uniform handle. Effective only when ffp is false.
                int32_t  ffpType; //!< FFP uniform type. Effective only when ffp is true.
            };                
            bool ffp;     //!< is this binding to fixed functional pipeline? (determined by binding name)
        };

        struct ShaderData
        {
            StrA                        name;
            AutoRef<Shader>             value;
            std::vector<TextureRefData> textures;      // texture referencing list.
            std::vector<UniformRefData> uniforms;      // uniform referencing list.
            std::vector<size_t>         dirtyUniforms; // dirty uniform list. Each item is a index into ShaderData::uniforms
        };

        struct PassData
        {
            EffectItemID shaders[NUM_SHADER_TYPES]; //!< shader IDs of this pass.
            RenderStateBlockDesc rsb; //!< render state block
        };

        struct TechniqueData
        {
            StrA                  name;
            std::vector<PassData> passes;
        };

        template<typename T>
        struct NamedItemManager
        {
            std::map<StrA,EffectItemID>   names;
            HandleManager<T,EffectItemID> items;

            bool add( const StrA & name, const T & item )
            {
                GN_ASSERT( names.end() == names.find(name) );
                names[name] = items.add( item );
                GN_ASSERT( items.validHandle( names[name] ) );
                return true;
            }
            void clear() { names.clear(); items.clear(); }
            bool empty() const { GN_ASSERT(names.size() == items.size()); return items.empty(); }
            size_t size() const { GN_ASSERT(names.size() == items.size()); return items.size(); }
            EffectItemID find( const StrA & name ) const
            {
                if( 0 == name ) return 0;
                std::map<StrA,EffectItemID>::const_iterator i = names.find(name);
                if( names.end() == i ) return 0;
                GN_ASSERT( items.validHandle(i->second)  );
                return i->second;
            }
        };

        EffectDesc mDesc;

        NamedItemManager<TextureData>   mTextures;
        NamedItemManager<UniformData>   mUniforms;
        NamedItemManager<ShaderData>    mShaders;
        NamedItemManager<TechniqueData> mTechniques;
        
        mutable uint32_t mActiveTechnique;
        mutable bool     mPassBegun;
        mutable size_t   mActivePass;

        // ********************************
        // private functions
        // ********************************
    private:

        bool createEffect(); // called by init()
        bool createShader( ShaderData &, const StrA &, const EffectDesc::ShaderDesc & );
        bool createTechnique( TechniqueData &, const EffectDesc::TechniqueDesc & );
        static void sSetFfpUniform( int32_t, const UniformData & );
    };
}}

#if GN_ENABLE_INLINE
#include "effect.inl"
#endif

// *****************************************************************************
//                           End of effect.h
// *****************************************************************************
#endif // __GN_GFX_EFFECT_H__
