#include "pch.h"
#include "modelresource.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpures");

// *****************************************************************************
// local classes and functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
static StrA sResolveResourcePath( const StrA & basedir, const StrA & path )
{
    if( path.Size() > 1 && path[0] == '@' )
    {
        return path;
    }
    else
    {
        return fs::ResolvePath( basedir, path );
    }
}

//
//
// -----------------------------------------------------------------------------
static void
sApplyRenderStates(
    GN::gfx::GpuContext::RenderStates               & dst,
    const GN::gfx::GpuContext::RenderStates         & src,
    const EffectResourceDesc::EffectRenderStateDesc & renderstates )
{
    #define MERGE_SINGLE_RENDER_STATE( state ) dst.state = ( renderstates.state.overridden ? renderstates.state.value : src.state )

    MERGE_SINGLE_RENDER_STATE( depthTestEnabled );
    MERGE_SINGLE_RENDER_STATE( depthWriteEnabled );
    MERGE_SINGLE_RENDER_STATE( depthFunc );

    MERGE_SINGLE_RENDER_STATE( stencilEnabled );
    MERGE_SINGLE_RENDER_STATE( stencilPassOp );
    MERGE_SINGLE_RENDER_STATE( stencilFailOp );
    MERGE_SINGLE_RENDER_STATE( stencilZFailOp );

    MERGE_SINGLE_RENDER_STATE( independentAlphaBlending );
    for( size_t i = 0; i < GpuContext::MAX_COLOR_RENDER_TARGETS; ++i )
    {
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendEnabled );
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendSrc );
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendDst );
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendOp );
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendAlphaSrc );
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendAlphaDst );
        MERGE_SINGLE_RENDER_STATE( alphaBlend[i].blendAlphaOp );
    }
    MERGE_SINGLE_RENDER_STATE( blendFactors );

    MERGE_SINGLE_RENDER_STATE( fillMode );
    MERGE_SINGLE_RENDER_STATE( cullMode );
    MERGE_SINGLE_RENDER_STATE( frontFace );
    MERGE_SINGLE_RENDER_STATE( msaaEnabled );

    MERGE_SINGLE_RENDER_STATE( colorWriteMask );
    MERGE_SINGLE_RENDER_STATE( viewport );
    MERGE_SINGLE_RENDER_STATE( scissorRect );

    #undef MERGE_SINGLE_RENDER_STATE
}

//
// get value of integer attribute
// -----------------------------------------------------------------------------
template<typename T>
static bool sGetRequiredIntAttrib( T & result, const XmlElement & node, const char * attribName )
{
    const XmlAttrib * a = node.FindAttrib( attribName );
    if( !a || 0 == String2Integer<T>( result, a->value.ToRawPtr() ) )
    {
        GN_ERROR(sLogger)( "Integer attribute \"%s\" of element <%s> is either missing or invalid.",
            attribName, node.name.ToRawPtr() );
        return false;
    }
    else
    {
        return true;
    }
}

//
// get value of integer attribute
// -----------------------------------------------------------------------------
static void sBinaryEncode( StrA & result, const UInt8 * data, size_t size )
{
    static const char TABLE[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    };

    result.Clear();
    result.SetCaps( size * 2 );

    for( size_t i = 0; i < size; ++i, ++data )
    {
        UInt8 u8 = *data;

        result.Append( TABLE[u8>>4] );
        result.Append( TABLE[u8&0xF] );
    }
}

//
// get value of integer attribute
// -----------------------------------------------------------------------------
static bool sBinaryDecode( DynaArray<UInt8> & data, const StrA & s )
{
    static UInt8 TABLE[] =
    {

        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    };

    data.Clear();

    if( 0 != (s.Size() % 2) )
    {
        GN_ERROR(sLogger)( "Invalid binary string." );
        return false;
    }

    data.Resize( s.Size() / 2 );

    for( size_t i = 0; i < data.Size(); ++i )
    {
        char hi = s[i*2];
        char lo = s[i*2+1];

        UInt8 u8 = 0;

        if( '0' <= hi && hi <= '9' )
        {
            u8 = (hi-'0')<<4;
        }
        else if( 'A' <= hi && hi <= 'F' )
        {
            u8 = (hi-'A'+10)<<4;
        }
        else
        {
            GN_ERROR(sLogger)( "Invalid character in binary string: %d", hi );
            return false;
        }

        if( '0' <= lo && lo <= '9' )
        {
            u8 += (lo-'0');
        }
        else if( 'A' <= lo && lo <= 'F' )
        {
            u8 += (lo-'A'+10);
        }
        else
        {
            GN_ERROR(sLogger)( "Invalid character in binary string: %d", lo );
            return false;
        }

        data[i] = u8;
    }

    return true;
}

// *****************************************************************************
// local classes and functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResourceDesc::Clear()
{
    effect.Clear();
    textures.Clear();
    uniforms.Clear();
    mesh.Clear();
    subset.Clear();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResourceDesc::LoadFromXml( const XmlNode & root, const char * basedir )
{
    Clear();

    const XmlElement * rootElement = root.ToElement();
    if( !rootElement || rootElement->name != "model" )
    {
        GN_ERROR(sLogger)( "Root node must be a XML element named <model>." );
        return false;
    }

    bool effectFound = false;
    bool meshFound = false;
    bool subsetFound = false;
    for( const XmlNode * n = rootElement->child; n != NULL; n = n->next )
    {
        const XmlElement * e = n->ToElement();
        if( !e ) continue;

        if( "effect" == e->name )
        {
            if( effectFound )
            {
                GN_WARN(sLogger)( "Extra <effect> elements are ignored." );
            }
            else
            {
                const XmlAttrib * a = e->FindAttrib( "ref" );
                if( !a )
                {
                    GN_ERROR(sLogger)( "\"ref\" attribute of <effect> element is missing." );
                    return false;
                }
                effect = sResolveResourcePath( basedir, a->value );

                effectFound = true;
            }
        }
        else if( "mesh" == e->name )
        {
            if( meshFound )
            {
                GN_WARN(sLogger)( "Extra <mesh> elements are ignored." );
            }
            else
            {
                const XmlAttrib * a = e->FindAttrib( "ref" );
                if( !a )
                {
                    GN_ERROR(sLogger)( "\"ref\" attribute of <mesh> element is missing." );
                    return false;
                }
                mesh = sResolveResourcePath( basedir, a->value );

                meshFound = true;
            }
        }
        else if( "subset" == e->name )
        {
            if( subsetFound )
            {
                GN_WARN(sLogger)( "Redundant <subset> elements are ignored." );
            }
            else
            {
                if( !sGetRequiredIntAttrib( subset.basevtx, *e, "basevtx" ) ||
                    !sGetRequiredIntAttrib( subset.numvtx, *e, "numvtx" ) ||
                    !sGetRequiredIntAttrib( subset.startidx, *e, "startidx" ) ||
                    !sGetRequiredIntAttrib( subset.numidx, *e, "numidx" ) )
                {
                    GN_ERROR(sLogger)( "Invalid <subset> element." );
                    return false;
                }
            }
        }
        else if( "texture" == e->name )
        {
            const XmlAttrib * a = e->FindAttrib( "shaderParameter" );
            if( !a )
            {
                GN_ERROR(sLogger)( "\"shaderParameter\" attribute of <texture> element is missing." );
                return false;
            }

            ModelTextureDesc & td = textures[a->value];

            a = e->FindAttrib( "ref" );
            if( a )
            {
                td.resourceName = sResolveResourcePath( basedir, a->value );
            }
            else
            {
                GN_TODO( "read texture descriptor" );
            }
        }
        else if( "uniform" == e->name )
        {
            const XmlAttrib * a = e->FindAttrib( "shaderParameter" );
            if( !a )
            {
                GN_ERROR(sLogger)( "\"shaderParameter\" attribute of <uniform> element is missing." );
                return false;
            }

            ModelUniformDesc & ud = uniforms[a->value];

            a = e->FindAttrib( "size" );
            if( !a )
            {
                GN_ERROR(sLogger)( "\"size\" attribute of <uniform> element is missing." );
                return false;
            }
            if( 0 == String2Integer( ud.size, a->value ) )
            {
                GN_ERROR(sLogger)( "\"size\" attribute of <uniform> element is not a valid integer." );
                return false;
            }

            const XmlElement * binnode = e->FindChildElement( "initialValue" );
            if( binnode && !sBinaryDecode( ud.initialValue, binnode->text ) )
            {
                GN_ERROR(sLogger)( "Invalid uniform initial data." );
                return false;
            }
        }
        else
        {
            GN_WARN(sLogger)( "Ignore unrecognized element <%s>.", e->name.ToRawPtr() );
        }
    }

    if( !effectFound || !meshFound )
    {
        GN_ERROR(sLogger)( "<effect> and <mesh> element are required." );
        return false;
    }

    // done
    return true;
}

//
//
// -----------------------------------------------------------------------------
XmlElement * GN::gfx::ModelResourceDesc::SaveToXml( XmlNode & root, const char * basedir ) const
{
    XmlElement * rootElement = root.ToElement();
    if( !rootElement )
    {
        GN_ERROR(sLogger)( "Root node must be a XML element." );
        return NULL;
    }

    if( NULL == basedir )
    {
        GN_ERROR(sLogger)( "NULL basedir pointer." );
        return NULL;
    }

    XmlDocument & doc = rootElement->doc;

    XmlElement * modelNode = doc.CreateElement(NULL);
    modelNode->name = "model";

    // create effect node
    XmlElement * effectNode = doc.CreateElement(modelNode);
    effectNode->name = "effect";
    if( effect.Empty() )
    {
        GN_ERROR(sLogger)( "Effect name can not be empty." );
        return NULL;
    }
    else
    {
        XmlAttrib * a = doc.CreateAttrib( effectNode );
        a->name = "ref";
        a->value = fs::RelPath( effect, basedir );
    }

    // create mesh node
    XmlElement * meshNode = doc.CreateElement(modelNode);
    meshNode->name = "mesh";
    if( mesh.Empty() )
    {
        GN_ERROR(sLogger)( "Mesh name can not be empty." );
        return NULL;
    }
    else
    {
        XmlAttrib * a = doc.CreateAttrib( meshNode );
        a->name = "ref";
        a->value = fs::RelPath( mesh, basedir );
    }

    // create subset node
    XmlElement * subsetNode = doc.CreateElement(modelNode);
    subsetNode->name = "subset";
    if( !subset.IsWholeMesh() )
    {
        XmlAttrib * a = doc.CreateAttrib( subsetNode );
        a->name = "basevtx";
        a->value = StringFormat( "%u", subset.basevtx );

        a = doc.CreateAttrib( subsetNode );
        a->name = "numvtx";
        a->value = StringFormat( "%u", subset.numvtx );

        a = doc.CreateAttrib( subsetNode );
        a->name = "startidx";
        a->value = StringFormat( "%u", subset.startidx );

        a = doc.CreateAttrib( subsetNode );
        a->name = "numidx";
        a->value = StringFormat( "%u", subset.numidx );
    }

    // create texture nodes
    for( const StringMap<char,ModelTextureDesc>::KeyValuePair * i = textures.First();
         i != NULL;
         i = textures.Next( i ) )
    {
        const StrA             & texname = i->key;
        const ModelTextureDesc & texdesc = i->value;

        XmlElement * textureNode = doc.CreateElement(modelNode);
        textureNode->name = "texture";

        XmlAttrib * a = doc.CreateAttrib( textureNode );
        a->name = "shaderParameter";
        a->value = texname;
        if( texdesc.resourceName.Empty() )
        {
            //texdesc.desc.SaveToXml( basedir );
            GN_UNIMPL();
            return NULL;
        }
        else
        {
            a = doc.CreateAttrib( textureNode );
            a->name = "ref";
            a->value = fs::RelPath( texdesc.resourceName, basedir );
        }
    }

    // create uniform nodes
    for( const StringMap<char,ModelUniformDesc>::KeyValuePair * i = uniforms.First();
         i != NULL;
         i = uniforms.Next( i ) )
    {
        const StrA             & uniname = i->key;
        const ModelUniformDesc & unidesc = i->value;

        XmlElement * uniformNode = doc.CreateElement(modelNode);
        uniformNode->name = "uniform";

        XmlAttrib * a = doc.CreateAttrib( uniformNode );
        a->name = "shaderParameter";
        a->value = uniname;

        if( unidesc.resourceName.Empty() )
        {
            a = doc.CreateAttrib( uniformNode );
            a->name = "size";
            a->value = StringFormat( "%u", unidesc.size );

            if( !unidesc.initialValue.Empty() )
            {
                XmlElement * bin = doc.CreateElement( uniformNode );
                bin->name = "initialValue";
                sBinaryEncode( bin->text, unidesc.initialValue.ToRawPtr(), unidesc.initialValue.Size() );
            }
        }
        else
        {
            a = doc.CreateAttrib( uniformNode );
            a->name = "ref";
            a->value = fs::RelPath( unidesc.resourceName, basedir );
        }
    }

    // done
    modelNode->SetParent( &root );
    return modelNode;
}

// *****************************************************************************
// TextureItem
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::Impl::TextureItem::TextureItem()
    : mOwner( 0 )
{
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::Impl::TextureItem::~TextureItem()
{
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::setResource(
    Impl            & owner,
    size_t            effectParameterIndex,
    TextureResource * newTexture )
{
    if( mResource.Get() == newTexture ) return;

    // disconnect from old handle
    if( mResource ) mResource->sigTextureChanged.Disconnect( this );

    Texture * tex;
    if( newTexture )
    {
        // connect to new handle
        newTexture->sigTextureChanged.Connect( this, &TextureItem::onTextureChange );

        tex = newTexture->GetTexture();
    }
    else
    {
        tex = NULL;
    }

    // update stored handle value
    mOwner = &owner;
    mEffectParameterIndex = effectParameterIndex;
    mResource.Set( newTexture );

    updateContext( tex );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::onTextureChange( TextureResource & r )
{
    GN_ASSERT( &r == mResource );

    updateContext( r.GetTexture() );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::updateContext( Texture * tex )
{
    GN_ASSERT( mOwner );

    GN_ASSERT( mOwner->mEffect.resource );

    EffectResource * effect = mOwner->mEffect.resource;

    GN_ASSERT( mOwner->mPasses.Size() == effect->GetNumPasses() );

    const EffectResource::TextureProperties & prop = effect->GetTextureProperties( mEffectParameterIndex );

    for( size_t i = 0; i < prop.bindings.Size(); ++i )
    {
        const EffectResource::BindingLocation & location = prop.bindings[i];

        GN_ASSERT( location.pass < mOwner->mPasses.Size() );
        GN_ASSERT( location.stage < GpuContext::MAX_TEXTURES );

        TextureBinding & binding = mOwner->mPasses[location.pass].gc.textures[location.stage];

        binding.texture.Set( tex );
        binding.sampler = prop.sampler;
    }
}

// *****************************************************************************
// UniformItem
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::Impl::UniformItem::UniformItem()
    : mOwner( 0 )
{
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::Impl::UniformItem::~UniformItem()
{
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::UniformItem::setResource(
    Impl            & owner,
    size_t            effectParameterIndex,
    UniformResource * newUniform )
{
    if( mResource.Get() == newUniform ) return;

    // disconnect from old handle
    if( mResource ) mResource->sigUniformChanged.Disconnect( this );

    Uniform * uniform;
    if( newUniform )
    {
        // connect to new handle
        newUniform->sigUniformChanged.Connect( this, &UniformItem::onUniformChange );

        uniform = newUniform->GetUniform();
    }
    else
    {
        uniform = NULL;
    }

    // update stored handle value
    mOwner = &owner;
    mEffectParameterIndex = effectParameterIndex;
    mResource.Set( newUniform );

    updateContext( uniform );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::UniformItem::onUniformChange( UniformResource & r )
{
    GN_ASSERT( &r == mResource );

    updateContext( r.GetUniform() );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::UniformItem::updateContext( Uniform * uniform )
{
    GN_ASSERT( mOwner );

    GN_ASSERT( mOwner->mEffect.resource );

    EffectResource * effect = mOwner->mEffect.resource;

    GN_ASSERT( mOwner->mPasses.Size() == effect->GetNumPasses() );

    const EffectResource::UniformProperties & prop = effect->GetUniformProperties( mEffectParameterIndex );

    for( size_t i = 0; i < prop.bindings.Size(); ++i )
    {
        const EffectResource::BindingLocation & location = prop.bindings[i];

        GN_ASSERT( location.pass < mOwner->mPasses.Size() );
        GN_ASSERT( location.stage < GpuContext::MAX_TEXTURES );

        GpuContext & gc = mOwner->mPasses[location.pass].gc;

        if( location.stage >= gc.uniforms.Size() )
        {
            gc.uniforms.Resize( location.stage + 1 );
        }

        gc.uniforms[location.stage].Set( uniform );
    }
}

// *****************************************************************************
// GN::gfx::ModelResource::Impl - Initialize and shutdown
// *****************************************************************************

// *****************************************************************************
// GN::gfx::ModelResource::Impl - public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::Reset( const ModelResourceDesc * desc )
{
    Clear();

    if( desc && !fromDesc( *desc ) )
    {
        Clear();
        return false;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<ModelResource>
GN::gfx::ModelResource::Impl::MakeClone( const char * nameOfTheClone ) const
{
    AutoRef<ModelResource> clone = GetGdb().CreateResource<ModelResource>( nameOfTheClone );

    if( !clone ) return AutoRef<ModelResource>::NULLREF;

    clone->mImpl->copyFrom( *this );

    return clone;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::SetTextureResource( const char * effectParameterName, GpuResource * texture )
{
    if( texture && !GetGdb().ValidResource( TextureResource::GetGuid(), texture ) )
    {
        GN_ERROR(sLogger)( "Invalid texture resource pointer.", modelName() );
        return false;
    }

    EffectResource * effect = mEffect.resource;
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is not referencing any effect!", modelName() );
        return false;
    }

    size_t parameterIndex = effect->FindTexture( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid texture name for model %s!", effectParameterName, modelName() );
        return false;
    }

    mTextures[parameterIndex].setResource( *this, parameterIndex, GpuResource::CastTo<TextureResource>(texture) );

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<TextureResource>
GN::gfx::ModelResource::Impl::GetTextureResource( const char * effectParameterName ) const
{
    EffectResource * effect = mEffect.resource;
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is not referencing any effect!", modelName() );
        return AutoRef<TextureResource>::NULLREF;
    }

    size_t parameterIndex = effect->FindTexture( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid texture name for model %s!", effectParameterName, modelName() );
        return AutoRef<TextureResource>::NULLREF;
    }

    return mTextures[parameterIndex].getResource();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::SetUniformResource( const char * effectParameterName, GpuResource * uniform )
{
    if( uniform && !GetGdb().ValidResource( UniformResource::GetGuid(), uniform ) )
    {
        GN_ERROR(sLogger)( "Invalid uniform resource pointer.", modelName() );
        return false;
    }

    EffectResource * effect = mEffect.resource;
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is not referencing any effect!", modelName() );
        return false;
    }

    size_t parameterIndex = effect->FindUniform( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid uniform name for model %s!", effectParameterName, modelName() );
        return false;
    }

    mUniforms[parameterIndex].setResource( *this, parameterIndex, GpuResource::CastTo<UniformResource>(uniform) );

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<UniformResource>
GN::gfx::ModelResource::Impl::GetUniformResource( const char * effectParameterName ) const
{
    EffectResource * effect = mEffect.resource;
    if( NULL == effect )
    {
        AutoRef<UniformResource> & dummy = mDummyUniforms["NULL_EFFECT"];
        if( !dummy )
        {
            GN_ERROR(sLogger)( "Model %s is not referencing any effect!", modelName() );
            dummy = GetGdb().CreateResource<UniformResource>( NULL );
            AutoRef<Uniform> u( GetGdb().GetGpu().CreateUniform( sizeof(float) ) );
            dummy->SetUniform( u );
        }

        return dummy;
    }

    size_t parameterIndex = effect->FindUniform( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        AutoRef<UniformResource> & dummy = mDummyUniforms[effectParameterName?effectParameterName:"NULL_PARAMETER"];
        if( !dummy )
        {
            GN_ERROR(sLogger)( "%s is not a valid uniform name for model %s!", effectParameterName?effectParameterName:"<NULL name>", modelName() );
            dummy = GetGdb().CreateResource<UniformResource>( NULL );
            AutoRef<Uniform> u( GetGdb().GetGpu().CreateUniform( sizeof(float) ) );
            dummy->SetUniform( u );
        }
        return dummy;
    }

    return mUniforms[parameterIndex].getResource();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::SetMeshResource(
    GpuResource              * resource,
    const MeshResourceSubset * subset )
{
    // check mesh resource pointer
    if( resource && !GetGdb().ValidResource( MeshResource::GetGuid(), resource ) )
    {
        GN_ERROR(sLogger)( "invalid mesh resource pointer" );
        return false;
    }

    MeshResource * mesh = GpuResource::CastTo<MeshResource>(resource);

    // bind mesh signal with the old mesh
    if( mMesh.resource != mesh )
    {
        if( mMesh.resource ) mMesh.resource->sigMeshChanged.Disconnect( this );
        if( mesh ) mesh->sigMeshChanged.Connect( this, &Impl::onMeshChanged );
    }

    // update mesh resource pointer
    mMesh.resource.Set( mesh );

    // update mesh subset
    if( subset )
    {
        mMesh.subset = *subset;
    }
    else
    {
        mMesh.subset.Clear();
    }

    // update GPU contexts
    for( size_t i = 0; i < mPasses.Size(); ++i )
    {
        RenderPass & pass = mPasses[i];

        if( mMesh.resource )
        {
            mMesh.resource->ApplyToContext( pass.gc );
        }
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<MeshResource>
GN::gfx::ModelResource::Impl::GetMeshResource( MeshResourceSubset * subset ) const
{
    if( subset )*subset = mMesh.subset;
    return mMesh.resource;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::SetEffectResource( GpuResource * resource )
{
    if( resource && !GetGdb().ValidResource( EffectResource::GetGuid(), resource ) )
    {
        GN_ERROR(sLogger)( "Invalid effect resource pointer.", modelName() );
        return false;
    }

    EffectResource * effect = GpuResource::CastTo<EffectResource>(resource);

    // rebind changing signal
    if( effect != mEffect.resource )
    {
        if( mEffect.resource ) mEffect.resource->sigEffectChanged.Disconnect( this );
        if( effect ) effect->sigEffectChanged.Connect( this, &Impl::onEffectChanged );
    }

    // update effect resource pointer
    mEffect.resource.Set( effect );

    // initialize passes array
    size_t numpasses = effect ? effect->GetNumPasses() : 0;
    mPasses.Resize( numpasses );
    for( size_t i = 0; i < mPasses.Size(); ++i )
    {
        RenderPass & pass = mPasses[i];

        pass.gc.Clear();
        effect->ApplyToContext( i, pass.gc );

        pass.renderstates = effect->GetRenderStates( i );
    }

    // reapply mesh
    GN_VERIFY( SetMeshResource( mMesh.resource, &mMesh.subset ) );

    // reapply textures
    size_t numtextures = effect ? effect->GetNumTextures() : 0;
    mTextures.Resize( numtextures );
    for( size_t i = 0; i < numtextures; ++i )
    {
        TextureItem & ti = mTextures[i];

        AutoRef<TextureResource> texres = ti.getResource();

        if( !texres )
        {
            const EffectResource::TextureProperties & tp = mEffect.resource->GetTextureProperties( i );
            StrA texname = StringFormat( "%s.texture.%s", modelName(), tp.parameterName.ToRawPtr() );
            texres = GetGdb().FindOrCreateResource<TextureResource>( texname );
            if( !texres ) return false;
        }

        ti.setResource( *this, i, NULL );
        ti.setResource( *this, i, texres );
    }

    // reapply uniforms
    size_t numuniforms = effect ? effect->GetNumUniforms() : 0;
    mUniforms.Resize( numuniforms );
    for( size_t i = 0; i < numuniforms; ++i )
    {
        UniformItem & ui = mUniforms[i];

        AutoRef<UniformResource> unires = ui.getResource();

        if( !unires )
        {
            const EffectResource::UniformProperties & up = mEffect.resource->GetUniformProperties( i );
            StrA uniname = StringFormat( "%s.uniform.%s", modelName(), up.parameterName.ToRawPtr() );
            unires = GetGdb().FindOrCreateResource<UniformResource>( uniname );
            if( !unires ) return false;
            AutoRef<Uniform> u = unires->GetUniform();
            if( !u && up.size > 0 )
            {
                u.Attach( GetGdb().GetGpu().CreateUniform( up.size ) );
                if( !u ) return false;
            }
            unires->SetUniform( u );
        }

        ui.setResource( *this, i, NULL );
        ui.setResource( *this, i, unires );
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::Draw() const
{
    MeshResource * mesh = mMesh.resource;
    if( NULL == mesh ) return;

    const MeshResourceDesc & meshdesc = mesh->GetDesc();

    Gpu & g = GetGdb().GetGpu();

    const GpuContext & currentContext = g.GetContext();

    for( size_t i = 0; i < mPasses.Size(); ++i )
    {
        GpuContext & gc = mPasses[i].gc;

        // copy render targets from current context
        gc.colortargets = currentContext.colortargets;
        gc.depthstencil = currentContext.depthstencil;

        // apply render states
        sApplyRenderStates( gc.rs, currentContext.rs, mPasses[i].renderstates );
    }

    // determine the subset
    MeshResourceSubset subset = mMesh.subset;
    if( 0 == subset.basevtx && 0 == subset.numvtx )
    {
        subset.numvtx = meshdesc.numvtx;
    }
    if( 0 == subset.startidx && 0 == subset.numidx )
    {
        subset.numidx = meshdesc.numidx;
    }

    // draw
    for( size_t i = 0; i < mPasses.Size(); ++i )
    {
        const GpuContext & gc = mPasses[i].gc;

        g.BindContext( gc );

        // do rendering
        if( gc.idxbuf )
        {
            g.DrawIndexed(
                meshdesc.prim,
                subset.numidx,
                subset.basevtx,
                0, // startvtx,
                subset.numvtx,
                subset.startidx );
        }
        else
        {
            g.Draw(
                meshdesc.prim,
                subset.numvtx,
                subset.basevtx );
        }

    }
}

// *****************************************************************************
// GN::gfx::ModelResource::Impl - private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::fromDesc( const ModelResourceDesc & desc )
{
    GpuResourceDatabase & db = GetGdb();

    // initialize effect
    if( !desc.effect.Empty() )
    {
        AutoRef<EffectResource> effect = db.FindResource<EffectResource>( desc.effect );
        if( 0 == effect )
        {
            effect = EffectResource::LoadFromFile( db, desc.effect );
            if( 0 == effect )
            {
                GN_ERROR(sLogger)( "%s is not a valid effect resource name.", desc.effect.ToRawPtr() );
                return false;
            }
        }
        if( !SetEffectResource( effect ) ) return false;
    }

    // initialize mesh
    if( !desc.mesh.Empty() )
    {
        AutoRef<MeshResource> mesh = db.FindResource<MeshResource>( desc.mesh );
        if( 0 == mesh )
        {
            mesh = MeshResource::LoadFromFile( db, desc.mesh );
            if( 0 == mesh )
            {
                GN_ERROR(sLogger)( "%s is not a valid mesh resource name.", desc.mesh.ToRawPtr() );
                return false;
            }
        }
        if( !SetMeshResource( mesh, &desc.subset ) ) return false;
    }

    // setup textures
    GN_ASSERT( mTextures.Size() == (mEffect.resource ? mEffect.resource->GetNumTextures() : 0) );
    for( size_t i = 0; i < mTextures.Size(); ++i )
    {
        TextureItem & t = mTextures[i];

        const EffectResource::TextureProperties & tp = mEffect.resource->GetTextureProperties( i );

        const ModelResourceDesc::ModelTextureDesc * td = desc.textures.Find( tp.parameterName );

        AutoRef<TextureResource> texres;

        if( td )
        {
            if( !td->resourceName.Empty() )
            {
                texres = TextureResource::LoadFromFile( db, td->resourceName );
            }
            else
            {
                StrA texname = StringFormat( "%s.texture.%s", modelName(), tp.parameterName.ToRawPtr() );
                texres = db.FindOrCreateResource<TextureResource>( texname );
                if( texres ) texres->Reset( &td->desc );
            }
        }
        else
        {
            GN_ERROR(sLogger)(
                "Effec texture parameter '%s' in effect '%s' is not defined in model '%s'.",
                tp.parameterName.ToRawPtr(),
                mEffect.resource->Name(),
                modelName() );

            return false;
        }

        t.setResource( *this, i, NULL );
        t.setResource( *this, i, texres );
    }

    // setup uniforms
    GN_ASSERT( mUniforms.Size() == (mEffect.resource?mEffect.resource->GetNumUniforms() : 0) );
    for( size_t i = 0; i < mUniforms.Size(); ++i )
    {
        UniformItem & u = mUniforms[i];

        const EffectResource::UniformProperties & up = mEffect.resource->GetUniformProperties( i );

        const ModelResourceDesc::ModelUniformDesc * ud = desc.uniforms.Find( up.parameterName );

        AutoRef<UniformResource> unires;
        if( ud )
        {
            if( !ud->resourceName.Empty() )
            {
                unires = db.FindResource<UniformResource>( ud->resourceName );
                if( !unires )
                {
                    GN_ERROR(sLogger)( "Invalid uniform resource name '%s' in model '%s'.",
                        ud->resourceName.ToRawPtr(),
                        modelName() );
                }
            }
            else
            {
                StrA uniname = StringFormat( "%s.uniform.%s", modelName(), up.parameterName.ToRawPtr() );

                const void * initialValue = ud->initialValue.ToRawPtr();
                if( !ud->initialValue.Empty() && ud->initialValue.Size() != ud->size )
                {
                    GN_ERROR(sLogger)(
                        "Incorrect initial data size of uniform '%s in model '%s'.",
                        up.parameterName.ToRawPtr(),
                        modelName() );
                    initialValue = NULL;
                }

                unires = db.FindOrCreateResource<UniformResource>( uniname );
                if( unires ) unires->Reset( ud->size, initialValue );
            }
        }
        else
        {
            GN_ERROR(sLogger)(
                "Effec uniform parameter '%s' in effect '%s' is not defined in model '%s'.",
                up.parameterName.ToRawPtr(),
                mEffect.resource->Name(),
                modelName() );

            return false;
        }

        u.setResource( *this, i, NULL );
        u.setResource( *this, i, unires );
    }

    // success
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::Clear()
{
    if( mEffect.resource )
    {
        mEffect.resource->sigEffectChanged.Disconnect( this );
        mEffect.resource.Clear();
    }

    if( mMesh.resource )
    {
        mMesh.resource->sigMeshChanged.Disconnect( this );
        mMesh.resource.Clear();
    }

    mPasses.Clear();
    mTextures.Clear();
    mUniforms.Clear();
    mDummyUniforms.Clear();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::copyFrom( const Impl & other )
{
    Clear();

    GN_VERIFY( SetEffectResource( other.mEffect.resource ) );
    GN_ASSERT( mPasses.Size() == other.mPasses.Size() );

    GN_VERIFY( SetMeshResource( other.mMesh.resource, &other.mMesh.subset ) );

    GN_ASSERT( mTextures.Size() == other.mTextures.Size() );
    for( size_t i = 0; i < other.mTextures.Size(); ++i )
    {
        mTextures[i].setResource( *this, i, other.mTextures[i].getResource() );
    }

    GN_ASSERT( mUniforms.Size() == other.mUniforms.Size() );
    for( size_t i = 0; i < other.mUniforms.Size(); ++i )
    {
        mUniforms[i].setResource( *this, i, other.mUniforms[i].getResource() );
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::onEffectChanged( EffectResource & r )
{
    GN_ASSERT( &r == mEffect.resource );

    GN_VERIFY( SetEffectResource( &r ) );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::onMeshChanged( MeshResource & r )
{
    GN_ASSERT( &r == mMesh.resource );

    GN_VERIFY( SetMeshResource( &r, &mMesh.subset ) );
}

// *****************************************************************************
// GN::gfx::ModelResource
// *****************************************************************************

class ModelResourceInternal : public ModelResource
{
    ModelResourceInternal( GpuResourceDatabase & db )
        : ModelResource( db )
    {
    }

public:

    static GpuResource *
    sCreateInstance( GpuResourceDatabase & db )
    {
        return new ModelResourceInternal( db );
    }
};

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::registerModelResourceFactory( GpuResourceDatabase & db )
{
    if( db.HasResourceFactory( ModelResource::GetGuid() ) ) return true;

    GpuResourceFactory factory = { &ModelResourceInternal::sCreateInstance };

    return db.RegisterResourceFactory( ModelResource::GetGuid(), "Model Resource", factory );
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::ModelResource( GpuResourceDatabase & db )
    : GpuResource( db ), mImpl(NULL)
{
    mImpl = new Impl(*this);
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::~ModelResource()
{
    delete mImpl;
}

//
//
// -----------------------------------------------------------------------------
const Guid & GN::gfx::ModelResource::GetGuid()
{
    static const Guid MODEL_GUID = { 0x24a6e5eb, 0xeb76, 0x440f, { 0xaa, 0x9d, 0x6a, 0x59, 0x34, 0x2f, 0x89, 0x2e } };
    return MODEL_GUID;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<ModelResource>
GN::gfx::ModelResource::LoadFromFile(
    GpuResourceDatabase & db,
    const char          * filename )
{
    if( NULL == filename )
    {
        GN_INFO(sLogger)( "Null filename string." );
        return AutoRef<ModelResource>::NULLREF;
    }

    // Reuse existing resource, if possible
    AutoRef<ModelResource> m( db.FindResource<ModelResource>( filename ) );
    if( m ) return m;

    // convert to full (absolute) path
    StrA abspath = fs::ResolvePath( fs::GetCurrentDir(), filename );
    filename = abspath;

    // Try search for existing resource again with full path
    m = db.FindResource<ModelResource>( filename );
    if( m ) return m;

    ModelResourceDesc desc;
    if( !LoadFromXmlFile( desc, filename ) ) return AutoRef<ModelResource>::NULLREF;

    m = db.CreateResource<ModelResource>( abspath );
    if( !m || !m->Reset( &desc ) ) return AutoRef<ModelResource>::NULLREF;

    return m;
}

//
//
// -----------------------------------------------------------------------------
bool                        GN::gfx::ModelResource::Reset( const ModelResourceDesc * desc ) { return mImpl->Reset( desc ); }
AutoRef<ModelResource>      GN::gfx::ModelResource::MakeClone( const char * nameOfTheClone ) const { return mImpl->MakeClone( nameOfTheClone ); }
void                        GN::gfx::ModelResource::SetTextureResource( const char * effectParameterName, GpuResource * texture ) { mImpl->SetTextureResource( effectParameterName, texture ); }
AutoRef<TextureResource>    GN::gfx::ModelResource::GetTextureResource( const char * effectParameterName ) const { return mImpl->GetTextureResource( effectParameterName ); }
void                        GN::gfx::ModelResource::SetUniformResource( const char * effectParameterName, GpuResource * uniform ) { mImpl->SetUniformResource( effectParameterName, uniform ); }
AutoRef<UniformResource>    GN::gfx::ModelResource::GetUniformResource( const char * effectParameterName ) const { return mImpl->GetUniformResource( effectParameterName ); }
void                        GN::gfx::ModelResource::SetMeshResource( GpuResource * mesh, const MeshResourceSubset * subset ) { mImpl->SetMeshResource( mesh, subset ); }
AutoRef<MeshResource>       GN::gfx::ModelResource::GetMeshResource( MeshResourceSubset * subset ) const { return mImpl->GetMeshResource( subset ); }
void                        GN::gfx::ModelResource::SetEffectResource( GpuResource * effect ) { mImpl->SetEffectResource( effect ); }
AutoRef<EffectResource>     GN::gfx::ModelResource::GetEffectResource() const { return mImpl->GetEffectResource(); }
void                        GN::gfx::ModelResource::Draw() const { mImpl->Draw(); }