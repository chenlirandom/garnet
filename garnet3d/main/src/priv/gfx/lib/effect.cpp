#include "pch.h"
#include "garnet/gfx/effect.h"
#if !GN_ENABLE_INLINE
#include "garnet/gfx/effect.inl"
#endif

// *****************************************************************************
// local functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
template<class T>
static inline bool sExist( const std::map<GN::StrA,T> & theMap, const GN::StrA & theName )
{
    return theMap.end() != theMap.find( theName );
}

// *****************************************************************************
// Efect descriptor methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::effect::EffectDesc::valid() const
{
    GN_GUARD;

    // check shaders
    for( std::map<StrA,ShaderDesc>::const_iterator i = shaders.begin(); i != shaders.end(); ++i )
    {
        const StrA & shaderName = i->first;
        const ShaderDesc & shader = i->second;

        if( shader.type < 0 || shader.type >= NUM_SHADER_TYPES )
        {
            GN_ERROR( "Shader(%s)�����ͷǷ�: %d��", shaderName.cstr(), shader.type );
            return false;
        }

        if( !shader.code.empty() )
        {
            if( shader.lang < 0 || shader.lang >= NUM_SHADING_LANGUAGES )
            {
                GN_ERROR( "Shader(%s)���������ͷǷ�: %d��", shaderName.cstr(), shader.lang );
                return false;
            }
        }

        // check texture referencing list
        for( size_t i = 0; i < shader.textures.size(); ++i )
        {
            const TextureRefDesc & tr = shader.textures[i];

            if( !sExist( textures, tr.name ) )
            {
                GN_ERROR( "Shader(%s)�к�����Ч����ͼ���ã�%s��", shaderName.cstr(), tr.name.cstr() );
                return false;
            }

            if( tr.stage >= MAX_TEXTURE_STAGES )
            {
                GN_ERROR( "Shader(%s)����ͼ����(%s)��stage������������(%d): %d.",
                    shaderName.cstr(), tr.name.cstr(), MAX_TEXTURE_STAGES, tr.stage );
                return false;
            }
        }

        // check uniform referencing list
        for( size_t i = 0; i < shader.uniforms.size(); ++i )
        {
            const UniformRefDesc & ur = shader.uniforms[i];

            if( !sExist( uniforms, ur.name ) )
            {
                GN_ERROR( "Shader(%s)�к�����Ч�ı������ã�%s��", shaderName.cstr(), ur.name.cstr() );
                return false;
            }
        }
    }

    // at least one technique
    if( techniques.empty() )
    {
        GN_ERROR( "technique list can't be empty!" );
        return false;
    }

    // check techniques
    for( std::map<StrA,TechniqueDesc>::const_iterator i = techniques.begin(); i != techniques.end(); ++i )
    {
        const StrA & techName = i->first;
        const TechniqueDesc & tech = i->second;

        for( size_t i = 0; i < tech.passes.size(); ++i )
        {
            const PassDesc & pass = tech.passes[i];

            if( !pass.rsb.valid() )
            {
                GN_ERROR( "Render state block of technique('%s')::pass(%d) is invalid.!", techName.cstr(), i );
                return false;
            }

            for( size_t i = 0; i < NUM_SHADER_TYPES; ++i )
            {
                const StrA & shaderName = pass.shaders[i];

                if( !sExist( shaders, shaderName ) )
                {
                    GN_ERROR( "Shader '%s' not found, referenced by technique '%s'.", shaderName.cstr(), techName.cstr() );
                    return false;
                }

                const ShaderDesc & shader = getShader( shaderName );

                if( (ShaderType)i != shader.type )
                {
                    GN_ERROR( "Shader type(%s) of shader '%s'  is not consistent with excepting type(%s) of technique '%s'.",
                        shaderType2Str(shader.type),
                        shaderName.cstr(),
                        shaderType2Str((ShaderType)i),
                        techName.cstr() );
                    return false;
                }
            }
        }

        if( !tech.rsb.valid() )
        {
            GN_ERROR( "Render state block of technique '%s' is invalid.!", techName.cstr() );
            return false;
        }
    }

    // check global render state block
    if( !rsb.valid() )
    {
        GN_ERROR( "Global render state block is invalid.!" );
        return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::effect::Effect::init( Renderer & r, const EffectDesc & d )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( Effect, () );

    if( !d.valid() )
    {
        GN_ERROR( "effect descriptor is invalid!" );
        quit(); return selfOK();
    }

    mRenderer = &r;
    mDesc = d;

    if( !createEffect() ) { quit(); return selfOK(); }

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::effect::Effect::init( const Effect & e )
{
    GN_GUARD;

    GN_ASSERT( e.mRenderer && e.mDesc.valid() );
    return init( *e.mRenderer, e.mDesc );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::effect::Effect::quit()
{
    GN_GUARD;

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::effect::Effect::draw( const GeometryData * geometryDataArray, size_t count ) const
{
    GN_GUARD_SLOW;

    // make sure effect is initialized.
    GN_ASSERT( ok() );

    if( 0 == geometryDataArray || count > 0 )
    {
        GN_ERROR( "invalid geometry data array (null data pointer)!" );
        return;
    }

    size_t numPasses = drawBegin();
    for( size_t i = 0; i < numPasses; ++i )
    {
        passBegin( i );

        // render the geometries
        for( size_t i = 0; i < count; ++i )
        {
            const GeometryData & g = geometryDataArray[i];

            for( std::map<StrA,UniformValue>::const_iterator i = g.uniforms.begin(); i != g.uniforms.end(); ++i )
            {
                setUniformByName( i->first, i->second );
            }
            for( std::map<StrA,uint32_t>::const_iterator i = g.textures.begin(); i != g.textures.end(); ++i )
            {
                setTextureByName( i->first, i->second );
            }

            commitChanges();

            mRenderer->bindVtxBinding( g.vtxBinding );
            mRenderer->bindVtxBufs( g.vtxBufs[0].addr(), 0, g.numVtxBufs );
            mRenderer->bindIdxBuf( g.idxBuf.get() );

            if( g.idxBuf.empty() )
            {
                mRenderer->draw( g.prim, g.numPrim, g.startVtx );
            }
            else
            {
                mRenderer->drawIndexed( g.prim, g.numPrim, g.startVtx, g.minVtxIdx, g.numVtx, g.startIdx );
            }
        }

        passEnd();
    }
    drawEnd();

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::effect::Effect::createEffect()
{
    GN_GUARD;

    GN_ASSERT( mRenderer && mDesc.valid() && mUniforms.empty() && mTextures.empty() );

    // create texture list
    for( std::map<StrA,TextureDesc>::const_iterator i = mDesc.textures.begin();
         i != mDesc.textures.end(); ++i )
    {
        TextureData td;
        td.name = i->first;
        mTextures.add( i->first, td );

        // set default texture value
        if( !i->second.defaultValue.empty() )
        {
            uint32_t id = gTexDict.getResourceHandle( i->second.defaultValue );
            if( 0 == id )
            {
                GN_WARN( "Default texture value '%s' of texture '%s' is not a valid texture resource.",
                    i->second.defaultValue.cstr(), td.name.cstr() );
            }
            else
            {
                setTextureByName( td.name, id );
            }
        }
    }

    // create uniform list
    for( std::map<StrA,UniformDesc>::const_iterator i = mDesc.uniforms.begin();
         i != mDesc.uniforms.end(); ++i )
    {
        UniformData ud;
        ud.name = i->first;
        mUniforms.add( i->first, ud );
    }

    // create shader list
    for( std::map<StrA,ShaderDesc>::const_iterator i = mDesc.shaders.begin();
         i != mDesc.shaders.end(); ++i )
    {
        const ShaderDesc & s = i->second;

        ShaderData sd;

        sd.name = i->first;

        // build texture referencing list
        sd.textures.resize( s.textures.size() );
        for( size_t i = 0; i < s.textures.size(); ++i )
        {
            const TextureRefDesc & tr = s.textures[i];
            TextureRefData & td = sd.textures[i];
            td.stage = tr.stage;
            td.handle = mTextures.find( tr.name );
            if( 0 == td.handle )
            {
                GN_ERROR( "Texture named '%s' not found (shader name: '%s')", tr.name.cstr(), sd.name.cstr() );
                return 0;
            }
        }

        // build uniform refernecing list
        sd.uniforms.resize( s.uniforms.size() );
        for( size_t i = 0; i < s.uniforms.size(); ++i )
        {
            const UniformRefDesc & ur = s.uniforms[i];
            UniformRefData & ud = sd.uniforms[i];
            ud.ffp = sCheckFfpParameterType( ur.binding, ud.ffpParameterType );
            ud.handle = mUniforms.find( ur.name );
            if( 0 == ud.handle )
            {
                GN_ERROR( "Uniform named '%s' not found (shader name: '%s')", ur.name.cstr(), sd.name.cstr() );
                return 0;
            }
        }

        mShaders.add( sd.name, sd );
    }

    // build shader referencing list for all uniforms
    for( uint32_t hUniform = mUniforms.items.first(); hUniform != 0; hUniform = mUniforms.items.next(hUniform) )
    {
        UniformData & ud = mUniforms.items[hUniform];

        for( uint32_t hShader = mShaders.items.first(); hShader != 0; hShader = mShaders.items.next(hShader) )
        {
            const ShaderData & sd = mShaders.items[hShader];
            const ShaderDesc & s = mDesc.getShader(sd.name);
            for( size_t i = 0; i < s.uniforms.size(); ++i )
            {
                if( s.uniforms[i].name == ud.name )
                {
                    ShaderRefData urd = { hShader, i };
                    ud.shaders.push_back( urd );
                }
            }
        }

        // set default uniform value.
        const UniformDesc & u = mDesc.getUniform(ud.name);
        if( u.hasDefaultValue ) setUniform( hUniform, u.defaultValue );
    }

    // create technique list
    GN_ASSERT( !mDesc.techniques.empty() );
    for( std::map<StrA,TechniqueDesc>::const_iterator iTech = mDesc.techniques.begin();
         iTech != mDesc.techniques.end(); ++iTech )
    {
        const TechniqueDesc & t = iTech->second;
        TechniqueData td;
        td.name = iTech->first;
        td.ready = false;

        // create pass list
        td.passes.resize( t.passes.size() );
        for( size_t iPass = 0; iPass < t.passes.size(); ++iPass )
        {
            const PassDesc & p = t.passes[iPass];
            PassData & pd = td.passes[iPass];

            pd.rsb = mRenderer->createRenderStateBlock( mDesc.rsb + t.rsb + p.rsb );
            if( 0 == pd.rsb )
            {
                GN_ERROR( "Fail to create render state block for pass(%d) of technique named '%s'.",
                    i, iTech->first.cstr() );
                return false;
            }
            for( size_t iShader = 0; iShader < NUM_SHADER_TYPES; ++iShader )
            {
                const StrA & s = p.shaders[iShader];
                pd.shaders[iShader] = mShaders.find( s );
                if( 0 == pd.shaders[iShader] )
                {
                    GN_ERROR( "Fail to find shader named '%s' for pass(%d) of technique named '%s'.",
                        s.cstr(), iShader, iTech->first.cstr() );
                    return false;
                }
            }
        }

        // add technique data to technique list
        mTechniques.add( iTech->first, td );
    }
    mActiveTechnique = mTechniques.items.first();

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::effect::Effect::initTechnique( uint32_t handle ) const
{
    GN_GUARD;

    GN_ASSERT( mTechniques.items.validHandle( handle ) );

    TechniqueData & td = mTechniques.items[handle];

    for( size_t i = 0; i < td.passes.size(); ++i )
    {
        PassData & pd = td.passes[i];

        for( size_t i = 0; i < NUM_SHADER_TYPES; ++i )
        {
            GN_ASSERT( mShaders.items.validHandle( pd.shaders[i] ) );
            ShaderData & sd = mShaders.items[pd.shaders[i]];
            const ShaderDesc & s = mDesc.getShader(sd.name);

            // create shaders instance
            if( !sd.value.empty() && !s.code.empty() )
            {
                sd.value.attach( mRenderer->createShader( s.type, s.lang, s.code, s.entry ) );
                if( sd.value.empty() )
                {
                    GN_ERROR( "Fail to create shader '%s' for technique '%s'.", sd.name.cstr(), td.name.cstr() );
                    return false;
                }
            }

            // realize uniform handles.
            for( size_t i = 0; i < sd.uniforms.size(); ++i )
            {
                UniformRefData & ur = sd.uniforms[i];
                if( !ur.ffp )
                {
                    if( sd.value.empty() )
                    {
                        GN_ERROR( "FFP shader '%s' can't have non-FFP uniform.", sd.name.cstr() );
                        return false;
                    }
                    const UniformRefDesc & u = s.uniforms[i];
                    GN_ASSERT( u.name == mUniforms.items[ur.handle].name );
                    ur.shaderUniformHandle = sd.value->getUniformHandle( u.binding );
                    if( 0 == ur.shaderUniformHandle )
                    {
                        GN_ERROR( "'%s' is not valid uniform binding to shader '%s'", u.binding.cstr(), sd.name.cstr() );
                        return false;
                    }
                }
            }
        }
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::effect::Effect::sCheckFfpParameterType(
    const StrA & name, FfpParameterType & type )
{
    GN_GUARD;

    static const char * sTable[] =
    {
        "FFP_TRANSFORM_WORLD",
        "FFP_TRANSFORM_VIEW",
        "FFP_TRANSFORM_PROJ",
        "FFP_VIEWPORT",
        "FFP_LIGHT0_POS",
        "FFP_LIGHT0_DIFFUSE",
        "FFP_MATERIAL_DIFFUSE",
        "FFP_MATERIAL_SPECULAR",
    };

    for( size_t i = 0; i < sizeof(sTable)/sizeof(sTable[0]); ++i )
    {
        if( sTable[i] == name )
        {
            type = (FfpParameterType)i;
            return true;
        }
    }

    return false;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::effect::Effect::sSetFfpParameter(
    Renderer &, FfpParameterType, const UniformData & )
{
    GN_GUARD;

    GN_UNIMPL_WARNING();

    GN_UNGUARD;
}
