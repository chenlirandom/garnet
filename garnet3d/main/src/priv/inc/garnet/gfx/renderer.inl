namespace GN { namespace gfx
{
    // *************************************************************************
    // resource management methods
    // *************************************************************************

    //
    //
    // -------------------------------------------------------------------------
    inline Shader *
    Renderer::createVtxShader( ShadingLanguage lang,
                               const StrA &    code,
                               const StrA &    hints )
    {
        GN_GUARD;
        return createShader( VERTEX_SHADER, lang, code, hints );
        GN_UNGUARD;
    }

    //
    //
    // -------------------------------------------------------------------------
    inline Shader *
    Renderer::createPxlShader( ShadingLanguage lang,
                               const StrA &    code,
                               const StrA &    hints )
    {
        GN_GUARD;
        return createShader( PIXEL_SHADER, lang, code, hints );
        GN_UNGUARD;
    }

    // *************************************************************************
    // context update helpers
    // *************************************************************************

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::contextUpdateBegin()
    {
        GN_ASSERT( !mHelperContextUpdateBegun );
        mHelperContextUpdateBegun = true;
        mHelperContext.clearToNull();
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::contextUpdateEnd()
    {
        GN_ASSERT( mHelperContextUpdateBegun );
        mHelperContextUpdateBegun = false;
        if( mHelperContext.flags.u32 ) setContext( mHelperContext );
    }

#define _GNGFX_CONTEXT_UPDATE_BEGIN() \
    bool immediate = !mHelperContextUpdateBegun; if( immediate ) { contextUpdateBegin(); } else void(0)

#define _GNGFX_CONTEXT_UPDATE_END() \
    if( immediate ) { contextUpdateEnd(); } else void(0)

#define _GNGFX_CONTEXT_UPDATE( contextMethod ) \
    _GNGFX_CONTEXT_UPDATE_BEGIN(); \
    mHelperContext.contextMethod; \
    _GNGFX_CONTEXT_UPDATE_END();

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setShader( ShaderType type, const Shader * shader )
    {
        _GNGFX_CONTEXT_UPDATE( setShader( type, shader ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setShaders( const Shader * const shaders[] )
    {
        _GNGFX_CONTEXT_UPDATE( setShaders( shaders ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setShaders( const Shader * vtxShader, const Shader * pxlShader )
    {
        _GNGFX_CONTEXT_UPDATE( setShaders( vtxShader, pxlShader ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setShaderHandles( ShaderDictionary::HandleType vtxShader, ShaderDictionary::HandleType pxlShader )
    {
        _GNGFX_CONTEXT_UPDATE( setShaderHandles( vtxShader, pxlShader ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setVtxShader( const Shader * s )
    {
        _GNGFX_CONTEXT_UPDATE( setVtxShader( s ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setVtxShaderHandle( ShaderDictionary::HandleType h )
    {
        _GNGFX_CONTEXT_UPDATE( setVtxShaderHandle( h ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setPxlShader( const Shader * s )
    {
        _GNGFX_CONTEXT_UPDATE( setPxlShader( s ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setPxlShaderHandle( ShaderDictionary::HandleType h )
    {
        _GNGFX_CONTEXT_UPDATE( setPxlShaderHandle( h ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setRenderStateBlock( const RenderStateBlockDesc & rsb )
    {
        _GNGFX_CONTEXT_UPDATE( setRenderStateBlock( rsb ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setRenderState( RenderState state, int32_t value )
    {
        _GNGFX_CONTEXT_UPDATE( setRenderState( state, value ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setColorBuffer( size_t index, const Texture * texture, size_t face, size_t level, size_t slice )
    {
        _GNGFX_CONTEXT_UPDATE( setColorBuffer( index, texture, face, level, slice ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setDepthBuffer( const Texture * texture, size_t face, size_t level, size_t slice )
    {
        _GNGFX_CONTEXT_UPDATE( setDepthBuffer( texture, face, level, slice ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setMsaa( MsaaType type )
    {
        _GNGFX_CONTEXT_UPDATE( setMsaa( type ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setViewport( const Rectf & viewport )
    {
        _GNGFX_CONTEXT_UPDATE( setViewport( viewport ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setViewport( float left, float top, float width, float height )
    {
        _GNGFX_CONTEXT_UPDATE( setViewport( left, top, width, height ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setWorld( const Matrix44f & value )
    {
        _GNGFX_CONTEXT_UPDATE( setWorld( value ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setView( const Matrix44f & value )
    {
        _GNGFX_CONTEXT_UPDATE( setView( value ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setProj( const Matrix44f & value )
    {
        _GNGFX_CONTEXT_UPDATE( setProj( value ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setTextureStateBlock( const TextureStateBlockDesc & value )
    {
        _GNGFX_CONTEXT_UPDATE( setTextureStateBlock( value ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setTextureState( size_t stage, TextureState state, TextureStateValue value )
    {
        _GNGFX_CONTEXT_UPDATE( setTextureState( stage, state, value ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setTexture( size_t stage, const Texture * tex )
    {
        _GNGFX_CONTEXT_UPDATE( setTexture( stage, tex ) )
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setTextureHandle( size_t stage, TextureDictionary::HandleType tex )
    {
        _GNGFX_CONTEXT_UPDATE( setTextureHandle( stage, tex ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setTextures( const Texture * const texlist[], size_t start, size_t count )
    {
        _GNGFX_CONTEXT_UPDATE( setTextures( texlist, start, count ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setTextureHandles( const TextureDictionary::HandleType texlist[], size_t start, size_t count )
    {
        _GNGFX_CONTEXT_UPDATE( setTextureHandles( texlist, start, count ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setVtxFmt( VtxFmtHandle h )
    {
        _GNGFX_CONTEXT_UPDATE( setVtxFmt( h ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setVtxBuf( size_t index, const VtxBuf * buffer, size_t stride )
    {
        _GNGFX_CONTEXT_UPDATE( setVtxBuf( index, buffer, stride ) );
    }

    //
    //
    // -------------------------------------------------------------------------
    inline void Renderer::setIdxBuf( const IdxBuf * idxBuf )
    {
        _GNGFX_CONTEXT_UPDATE( setIdxBuf( idxBuf ) );
    }

#undef _GNGFX_CONTEXT_UPDATE
#undef _GNGFX_CONTEXT_UPDATE_BEGIN
#undef _GNGFX_CONTEXT_UPDATE_END
}}
