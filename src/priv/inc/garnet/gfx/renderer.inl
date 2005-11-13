// *****************************************************************************
// caps functions
// *****************************************************************************

namespace GN { namespace gfx
{
    //
    inline void Renderer::CapsDesc::set( uint32_t v )
    {
        value = v;
#if GN_DEBUG
        valid = true;
#endif
    }
    //
    inline void Renderer::CapsDesc::reset()
    {
#if GN_DEBUG
        valid = false;
#endif
    }
    //
    inline void Renderer::setCaps( RendererCaps c, uint32_t v )
    {
        GN_GUARD;
        if( 0 <= c && c < NUM_CAPS )
        {
            mCaps[c].set(v);
        }
        else
        {
            GN_ERROR( "invalid caps" );
        }
        GN_UNGUARD;
    }
    //
    inline void Renderer::resetAllCaps()
    {
        GN_GUARD;
        for( size_t i = 0; i < NUM_CAPS; ++i )
        {
            mCaps[i].reset();
        }
        GN_UNGUARD;
    }

    //
    //
    // -------------------------------------------------------------------------
    inline Shader *
    Renderer::createShader( ShaderType      type,
                            ShadingLanguage lang,
                            const StrA &    code )
    {
        GN_GUARD;
        switch( type )
        {
            case VERTEX_SHADER : return createVertexShader( lang, code );
            case PIXEL_SHADER  : return createPixelShader( lang, code );
            default :
                GN_ERROR( "invalid shader type!" );
                return 0;
        }
        GN_UNGUARD;
    }
}}
