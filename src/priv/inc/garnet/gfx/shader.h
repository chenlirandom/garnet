#ifndef __GN_GFX_SHADER_H__
#define __GN_GFX_SHADER_H__
// *****************************************************************************
/// \file
/// \brief   graphics shader interface
/// \author  chenli@@REDMOND (2008.12.9)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// Shading language
    ///
    enum GpuProgramLanguage
    {
        GPL_ASM,  ///< assembly
        GPL_HLSL, ///< D3D HLSL
        GPL_GLSL, ///< OpenGL Shading language
        GPL_CG,   ///< Nvidia Cg
        NUM_GPU_PROGRAM_LANGUAGES,
    };

    ///
    /// Shader profile.
    ///
    /// Shader profile defines the minimum requirements that the hardware has to support
    ///
    enum GpuProgramProfile
    {
        GPP_D3D_1_1,  ///< D3D shader model 1.1
        GPP_D3D_2_0,  ///< D3D shader model 2.0
        GPP_D3D_3_0,  ///< D3D shader model 3.0
        GPP_D3D_4_0,  ///< D3D shader model 4.0
        GPP_OGL_ARB1, ///< OpenGL ARB program
        GPP_OGL_GLSL, ///< OpenGL GLSL 1.0
        GPP_CG,       ///< Nvidia Cg
        NUM_GPU_PROGRAM_PROFILES,
    };

    ///
    /// shader code
    ///
    struct ShaderCode
    {
        GpuProgramLanguage lang;  ///< shading language. Ignored when shader code is NULL.
        const char *       code;  ///< NULL terminated shader code
        const char *       entry; ///< NULL terminated shader entry function

        /// default ctor
        ShaderCode() : code(NULL), entry(NULL) {}
    };

    ///
    /// GPU program description
    ///
    struct GpuProgramDesc
    {
        ShaderCode vs;
        ShaderCode gs;
        ShaderCode ps;
    };

    ///
    /// GPU program parameter descrption
    ///
    struct GpuProgramParameterDesc
    {
        const char * name;     ///< NULL terminated parameter name
    };

    ///
    /// Contains platform dependent program data, which can be used for fast progra saving, loading and creation
    ///
    struct CompiledGpuProgram : public RefCounter
    {
        virtual const void * data() const = 0;
        virtual size_t       size() const = 0;
    };

    ///
    /// Graphics shader interface
    ///
    struct GpuProgram : public RefCounter
    {
        ///
        /// get number of parameters
        ///
        virtual size_t getNumParameters() const = 0;

        ///
        /// get parameter array
        ///
        virtual const GpuProgramParameterDesc * getParameters() const = 0;

        ///
        /// set shader parameter
        ///
        /// \param index        Parameter index in parameter array
        /// \param value        Parameter value
        /// \param length       Length in bytes of parameter value. Set to 0 to use inherited length.
        ///
        virtual void setParameter( size_t index, const void * value, size_t length = 0 ) = 0;

        ///
        /// set shader parameter by name
        ///
        /// \param namex        Parameter name.
        /// \param value        Parameter value
        /// \param length       Length in bytes of parameter value. Set to 0 to use inherited length.
        ///
        void setParameter( const char * name, const void * value, size_t length = 0 )
        {
            const GpuProgramParameterDesc * params = getParameters();
            size_t n = getNumParameters();
            for( size_t i = 0; i < n; ++i )
            {
                if( 0 == strCmp( name, params[i].name ) )
                {
                    setParameter( i, value, length );
                    return;
                }
            }
            GN_ERROR(getLogger("GN.gfx.rndr.GpuProgram"))( "invalid parameter name: %s", name );
        }
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_SHADER_H__
