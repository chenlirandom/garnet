#ifndef __GN_GFX_EFFECTRESOURCE_H__
#define __GN_GFX_EFFECTRESOURCE_H__
// *****************************************************************************
/// \file
/// \brief   Effect resource implementation class
/// \author  chenli@@REDMOND (2009.8.27)
// *****************************************************************************

#include "gpures.h"

namespace GN { namespace gfx
{
    ///
    /// Effect resource implementation class
    ///
    class EffectResource::Impl : public StdClass, public SlotBase
    {
        GN_DECLARE_STDCLASS( Impl, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        Impl( EffectResource & owner ) : mOwner(owner) { clear(); }
        virtual ~Impl() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const EffectResourceDesc & desc );
        void quit();
    private:
        void clear() {}
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        size_t                        getNumPasses() const { return mPasses.size(); }

        size_t                        getNumTextures() const { return mTextures.size(); }
        size_t                        findTexture( const char * name ) const;
        const TextureProperties     & getTextureProperties( size_t i ) const { return mTextures[i]; }

        size_t                        getNumUniforms() const { return mUniforms.size(); }
        size_t                        findUniform( const char * name ) const;
        const UniformProperties     & getUniformProperties( size_t i ) const { return mUniforms[i]; }

        const EffectResourceDesc::EffectRenderStateDesc &
                                      getRenderState( size_t pass ) const { GN_ASSERT( pass < mPasses.size() ); return mPasses[pass].rsdesc; }

        void                          applyToContext( size_t pass, GpuContext & gc ) const;

        // ********************************
        // private types
        // ********************************
    private:

        struct GpuProgramItem
        {
            StrA                name;
            AutoRef<GpuProgram> prog;
        };

        struct RenderPass
        {
            /// Index of GPU program used in this pass
            size_t gpuProgramIndex;

            /// render states
            EffectResourceDesc::EffectRenderStateDesc rsdesc;
        };

        // ********************************
        // private variables
        // ********************************
    private:

        EffectResource                & mOwner;
        DynaArray<GpuProgramItem>       mPrograms;
        DynaArray<RenderPass>           mPasses;
        DynaArray<TextureProperties>    mTextures;
        DynaArray<UniformProperties>    mUniforms;

        // ********************************
        // private functions
        // ********************************
    private:

        GpuResourceDatabase & database() const { return mOwner.database(); }
        const char *          effectName() const { return mOwner.database().getResourceName( mOwner.handle() ); }

        bool initGpuPrograms( const EffectResourceDesc & effectDesc );

        bool initTechniques( const EffectResourceDesc & effectDesc );

        bool initTech(
            const EffectResourceDesc                      & effectDesc,
            const StrA                                    & techName,
            const EffectResourceDesc::EffectTechniqueDesc & techDesc );

        bool initTextures( const EffectResourceDesc & effectDesc );

        bool initUniforms( const EffectResourceDesc & effectDesc );

        size_t findGpuProgram( const StrA & shaderName ) const;
    };

    ///
    /// register effect resource factory to database
    ///
    bool registerEffectResourceFactory( GpuResourceDatabase & );

}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_EFFECTRESOURCE_H__
