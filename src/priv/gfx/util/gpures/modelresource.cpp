#include "pch.h"
#include "modelresource.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.gpures");

// *****************************************************************************
// local classes and functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
template<typename T>
static inline const T *
sFindNamedPtr( const std::map<StrA,T> & container, const StrA & name )
{
    typename std::map<StrA,T>::const_iterator iter = container.find( name );
    return ( container.end() == iter ) ? NULL : &iter->second;
}

//
//
// -----------------------------------------------------------------------------
static void
sApplyRenderState(
    GN::gfx::GpuContext::RenderStates               & dst,
    const GN::gfx::GpuContext::RenderStates         & src,
    const EffectResourceDesc::EffectRenderStateDesc & rsdesc )
{
    #define MERGE_SINGLE_RENDER_STATE( state ) dst.state = ( rsdesc.state.overridden ? rsdesc.state.value : src.state )

    MERGE_SINGLE_RENDER_STATE( depthTestEnabled );
    MERGE_SINGLE_RENDER_STATE( depthWriteEnabled );
    MERGE_SINGLE_RENDER_STATE( depthFunc );

    MERGE_SINGLE_RENDER_STATE( stencilEnabled );
    MERGE_SINGLE_RENDER_STATE( stencilPassOp );
    MERGE_SINGLE_RENDER_STATE( stencilFailOp );
    MERGE_SINGLE_RENDER_STATE( stencilZFailOp );

    MERGE_SINGLE_RENDER_STATE( blendEnabled );
    MERGE_SINGLE_RENDER_STATE( blendSrc );
    MERGE_SINGLE_RENDER_STATE( blendDst );
    MERGE_SINGLE_RENDER_STATE( blendOp );
    MERGE_SINGLE_RENDER_STATE( blendAlphaSrc );
    MERGE_SINGLE_RENDER_STATE( blendAlphaDst );
    MERGE_SINGLE_RENDER_STATE( blendAlphaOp );

    MERGE_SINGLE_RENDER_STATE( fillMode );
    MERGE_SINGLE_RENDER_STATE( cullMode );
    MERGE_SINGLE_RENDER_STATE( frontFace );
    MERGE_SINGLE_RENDER_STATE( msaaEnabled );

    MERGE_SINGLE_RENDER_STATE( blendFactors );
    MERGE_SINGLE_RENDER_STATE( colorWriteMask );
    MERGE_SINGLE_RENDER_STATE( viewport );
    MERGE_SINGLE_RENDER_STATE( scissorRect );

    #undef MERGE_SINGLE_RENDER_STATE
}

// *****************************************************************************
// local classes and functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResourceDesc::clear()
{
    effectResourceName.clear();
    effectResourceDesc.clear();
    textures.clear();
    uniforms.clear();
    meshResourceName.clear();
    meshResourceDesc.clear();
    subset.clear();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResourceDesc::loadFromXmlNode( const XmlNode & root, const char * basedir )
{
    GN_UNUSED_PARAM( root );
    GN_UNUSED_PARAM( basedir );
    GN_UNIMPL_WARNING();

    clear();

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResourceDesc::saveToXmlNode( const XmlNode & root )
{
    GN_UNUSED_PARAM( root );
    GN_UNIMPL_WARNING();
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
    if( mResource.get() == newTexture ) return;

    // disconnect from old handle
    if( mResource ) mResource->sigTextureChanged.disconnect( this );

    Texture * tex;
    if( newTexture )
    {
        // connect to new handle
        newTexture->sigTextureChanged.connect( this, &TextureItem::onTextureChange );

        tex = newTexture->getTexture();
    }
    else
    {
        tex = NULL;
    }

    // update stored handle value
    mOwner = &owner;
    mEffectParameterIndex = effectParameterIndex;
    mResource.set( newTexture );

    updateContext( tex );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::onTextureChange( TextureResource & r )
{
    GN_ASSERT( &r == mResource );

    updateContext( mResource->getTexture() );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::updateContext( Texture * tex )
{
    GN_ASSERT( mOwner );

    GN_ASSERT( mOwner->mEffect.resource );

    EffectResource * effect = mOwner->mEffect.resource;

    GN_ASSERT( mOwner->mPasses.size() == effect->getNumPasses() );

    const EffectResource::TextureProperties & prop = effect->getTextureProperties( mEffectParameterIndex );

    for( size_t i = 0; i < prop.bindings.size(); ++i )
    {
        const EffectResource::BindingLocation & location = prop.bindings[i];

        GN_ASSERT( location.pass < mOwner->mPasses.size() );
        GN_ASSERT( location.stage < GpuContext::MAX_TEXTURES );

        TextureBinding & binding = mOwner->mPasses[location.pass].gc.textures[location.stage];

        binding.texture.set( tex );
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
    if( mResource.get() == newUniform ) return;

    // disconnect from old handle
    if( mResource ) mResource->sigUniformChanged.disconnect( this );

    Uniform * uniform;
    if( newUniform )
    {
        // connect to new handle
        newUniform->sigUniformChanged.connect( this, &UniformItem::onUniformChange );

        uniform = newUniform->getUniform();
    }
    else
    {
        uniform = NULL;
    }

    // update stored handle value
    mOwner = &owner;
    mEffectParameterIndex = effectParameterIndex;
    mResource.set( newUniform );

    updateContext( uniform );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::UniformItem::onUniformChange( UniformResource & r )
{
    GN_ASSERT( &r == mResource );

    updateContext( mResource->getUniform() );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::UniformItem::updateContext( Uniform * uniform )
{
    GN_ASSERT( mOwner );

    GN_ASSERT( mOwner->mEffect.resource );

    EffectResource * effect = mOwner->mEffect.resource;

    GN_ASSERT( mOwner->mPasses.size() == effect->getNumPasses() );

    const EffectResource::UniformProperties & prop = effect->getUniformProperties( mEffectParameterIndex );

    for( size_t i = 0; i < prop.bindings.size(); ++i )
    {
        const EffectResource::BindingLocation & location = prop.bindings[i];

        GN_ASSERT( location.pass < mOwner->mPasses.size() );
        GN_ASSERT( location.stage < GpuContext::MAX_TEXTURES );

        GpuContext & gc = mOwner->mPasses[location.pass].gc;

        if( location.stage >= gc.uniforms.size() )
        {
            gc.uniforms.resize( location.stage + 1 );
        }

        gc.uniforms[location.stage].set( uniform );
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
bool GN::gfx::ModelResource::Impl::reset( const ModelResourceDesc * desc )
{
    clear();

    if( desc && !init( *desc ) )
    {
        clear();
        return false;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<ModelResource>
GN::gfx::ModelResource::Impl::makeClone( const char * nameOfTheClone ) const
{
    AutoRef<ModelResource> clone = database().createResource<ModelResource>( nameOfTheClone );

    if( !clone ) return AutoRef<ModelResource>::NULLREF;

    clone->mImpl->copyFrom( *this );

    return clone;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::setEffect( GpuResource * resource )
{
    if( resource && !database().validResource( EffectResource::guid(), resource ) )
    {
        GN_ERROR(sLogger)( "Invalid effect resource pointer.", modelName() );
        return false;
    }

    EffectResource * effect = GpuResource::castTo<EffectResource>(resource);

    // rebind changing signal
    if( effect != mEffect.resource )
    {
        if( mEffect.resource ) mEffect.resource->sigEffectChanged.disconnect( this );
        if( effect ) effect->sigEffectChanged.connect( this, &Impl::onEffectChanged );
    }

    // update effect resource pointer
    mEffect.resource.set( effect );

    // initialize passes array
    size_t numpasses = effect ? effect->getNumPasses() : 0;
    mPasses.resize( numpasses );
    for( size_t i = 0; i < mPasses.size(); ++i )
    {
        RenderPass & pass = mPasses[i];

        pass.gc.clear();
        effect->applyToContext( i, pass.gc );

        pass.rsdesc = effect->getRenderState( i );
    }

    // reapply mesh
    GN_VERIFY( setMesh( mMesh.resource, &mMesh.subset ) );

    // reapply textures
    size_t numtextures = effect ? effect->getNumTextures() : 0;
    mTextures.resize( numtextures );
    for( size_t i = 0; i < numtextures; ++i )
    {
        TextureItem & t = mTextures[i];

        AutoRef<TextureResource> texres = t.getResource();

        t.setResource( *this, i, NULL );
        t.setResource( *this, i, texres );
    }

    // reapply uniforms
    size_t numuniforms = effect ? effect->getNumUniforms() : 0;
    mUniforms.resize( numuniforms );
    for( size_t i = 0; i < numuniforms; ++i )
    {
        UniformItem & u = mUniforms[i];

        AutoRef<UniformResource> unires = u.getResource();

        u.setResource( *this, i, NULL );
        u.setResource( *this, i, unires );
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::setTexture( const char * effectParameterName, GpuResource * texture )
{
    if( texture && !database().validResource( TextureResource::guid(), texture ) )
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

    size_t parameterIndex = effect->findTexture( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid texture name for model %s!", effectParameterName, modelName() );
        return false;
    }

    mTextures[parameterIndex].setResource( *this, parameterIndex, GpuResource::castTo<TextureResource>(texture) );

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<TextureResource>
GN::gfx::ModelResource::Impl::getTexture( const char * effectParameterName ) const
{
    EffectResource * effect = mEffect.resource;
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is not referencing any effect!", modelName() );
        return AutoRef<TextureResource>::NULLREF;
    }

    size_t parameterIndex = effect->findTexture( effectParameterName );
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
bool GN::gfx::ModelResource::Impl::setUniform( const char * effectParameterName, GpuResource * uniform )
{
    if( uniform && !database().validResource( UniformResource::guid(), uniform ) )
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

    size_t parameterIndex = effect->findUniform( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid uniform name for model %s!", effectParameterName, modelName() );
        return false;
    }

    mUniforms[parameterIndex].setResource( *this, parameterIndex, GpuResource::castTo<UniformResource>(uniform) );

    return true;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<UniformResource>
GN::gfx::ModelResource::Impl::getUniform( const char * effectParameterName ) const
{
    EffectResource * effect = mEffect.resource;
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is not referencing any effect!", modelName() );
        return AutoRef<UniformResource>::NULLREF;
    }

    size_t parameterIndex = effect->findUniform( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid uniform name for model %s!", effectParameterName, modelName() );
        return AutoRef<UniformResource>::NULLREF;
    }

    return mUniforms[parameterIndex].getResource();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::setMesh(
    GpuResource              * resource,
    const MeshResourceSubset * subset )
{
    // check mesh resource pointer
    if( resource && !database().validResource( MeshResource::guid(), resource ) )
    {
        GN_ERROR(sLogger)( "invalid mesh resource pointer" );
        return false;
    }

    MeshResource * mesh = GpuResource::castTo<MeshResource>(resource);

    // bind mesh signal with the old mesh
    if( mMesh.resource != mesh )
    {
        if( mMesh.resource ) mMesh.resource->sigMeshChanged.disconnect( this );
        if( mesh ) mesh->sigMeshChanged.connect( this, &Impl::onMeshChanged );
    }

    // update mesh resource pointer
    mMesh.resource.set( mesh );

    // update mesh subset
    if( subset )
    {
        mMesh.subset = *subset;
    }
    else
    {
        mMesh.subset.clear();
    }

    // update GPU contexts
    for( size_t i = 0; i < mPasses.size(); ++i )
    {
        RenderPass & pass = mPasses[i];

        if( mMesh.resource )
        {
            mMesh.resource->applyToContext( pass.gc );
        }
        else
        {
            GN_TODO( "clear context VB/IB/VF" );
        }
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::draw() const
{
    MeshResource * mesh = mMesh.resource;
    if( NULL == mesh ) return;

    const MeshResourceDesc & meshdesc = mesh->getDesc();

    Gpu & g = database().gpu();

    const GpuContext & currentContext = g.getContext();

    for( size_t i = 0; i < mPasses.size(); ++i )
    {
        GpuContext & gc = mPasses[i].gc;

        // copy render targets from current context
        gc.colortargets = currentContext.colortargets;
        gc.depthstencil = currentContext.depthstencil;

        // TODO: copy render states from current context
        sApplyRenderState( gc.rs, currentContext.rs, mPasses[i].rsdesc );
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
    for( size_t i = 0; i < mPasses.size(); ++i )
    {
        const GpuContext & gc = mPasses[i].gc;

        g.bindContext( gc );

        // do rendering
        if( gc.idxbuf )
        {
            g.drawIndexed(
                meshdesc.prim,
                subset.numidx,
                subset.basevtx,
                0, // startvtx,
                subset.numvtx,
                subset.startidx );
        }
        else
        {
            g.draw(
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
bool GN::gfx::ModelResource::Impl::init( const ModelResourceDesc & desc )
{
    GpuResourceDatabase & db = database();

    // initialize effect
    AutoRef<EffectResource> effect;
    if( !desc.effectResourceName.empty() )
    {
        effect = db.findResource<EffectResource>( desc.effectResourceName );
        if( 0 == effect )
        {
            GN_ERROR(sLogger)( "%s is not a valid effect resource name.", desc.effectResourceName.cptr() );
        }
    }
    else
    {
        effect = db.findOrCreateResource<EffectResource>( strFormat("%s.effect", modelName()) );
        if( effect ) effect->reset( &desc.effectResourceDesc );
    }
    if( !effect || !setEffect( effect ) ) return false;

    // initialize mesh
    AutoRef<MeshResource> mesh;
    if( !desc.meshResourceName.empty() )
    {
        mesh = db.findResource<MeshResource>( desc.meshResourceName );
        if( 0 == mesh )
        {
            GN_ERROR(sLogger)( "%s is not a valid mesh name.", desc.meshResourceName.cptr() );
        }
    }
    else
    {
        mesh = db.findOrCreateResource<MeshResource>( strFormat("%s.model", modelName()) );
        if( mesh ) mesh->reset( &desc.meshResourceDesc );
    }
    if( !mesh || !setMesh( mesh, &desc.subset ) ) return false;

    // setup textures
    GN_ASSERT( mTextures.size() == (mEffect.resource?mEffect.resource->getNumTextures() : 0) );
    for( size_t i = 0; i < mTextures.size(); ++i )
    {
        TextureItem & t = mTextures[i];

        const EffectResource::TextureProperties & tp = mEffect.resource->getTextureProperties( i );

        const ModelResourceDesc::ModelTextureDesc * td = sFindNamedPtr( desc.textures, tp.parameterName );

        AutoRef<TextureResource> texres;

        if( td )
        {
            if( !td->resourceName.empty() )
            {
                texres = TextureResource::loadFromFile( db, td->resourceName );
            }
            else
            {
                StrA texname = strFormat( "%s.texture.%s", modelName(), tp.parameterName.cptr() );
                texres = db.findOrCreateResource<TextureResource>( texname );
                if( texres ) texres->reset( &td->desc );
            }
        }
        else
        {
            GN_ERROR(sLogger)(
                "Effec texture parameter '%s' in effect '%s' is not defined in model '%s'.",
                tp.parameterName.cptr(),
                effect->name(),
                modelName() );

            return false;
        }

        t.setResource( *this, i, NULL );
        t.setResource( *this, i, texres );
    }

    // setup uniforms
    GN_ASSERT( mUniforms.size() == (mEffect.resource?mEffect.resource->getNumUniforms() : 0) );
    for( size_t i = 0; i < mUniforms.size(); ++i )
    {
        UniformItem & u = mUniforms[i];

        const EffectResource::UniformProperties & up = mEffect.resource->getUniformProperties( i );

        const ModelResourceDesc::ModelUniformDesc * ud = sFindNamedPtr( desc.uniforms, up.parameterName );

        AutoRef<UniformResource> unires;
        if( ud )
        {
            if( !ud->resourceName.empty() )
            {
                unires = db.findResource<UniformResource>( ud->resourceName );
                if( !unires )
                {
                    GN_ERROR(sLogger)( "Invalid uniform resource name '%s' in model '%s'.",
                        ud->resourceName.cptr(),
                        modelName() );
                }
            }
            else
            {
                StrA uniname = strFormat( "%s.uniform.%s", modelName(), up.parameterName.cptr() );

                const void * initialValue = ud->initialValue.cptr();
                if( !ud->initialValue.empty() && ud->initialValue.size() != ud->size )
                {
                    GN_ERROR(sLogger)(
                        "Incorrect initial data size of uniform '%s in model '%s'.",
                        up.parameterName.cptr(),
                        modelName() );
                    initialValue = NULL;
                }

                unires = db.findOrCreateResource<UniformResource>( uniname );
                if( unires ) unires->reset( ud->size, initialValue );
            }
        }
        else
        {
            GN_ERROR(sLogger)(
                "Effec uniform parameter '%s' in effect '%s' is not defined in model '%s'.",
                up.parameterName.cptr(),
                effect->name(),
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
void GN::gfx::ModelResource::Impl::clear()
{
    if( mEffect.resource )
    {
        mEffect.resource->sigEffectChanged.disconnect( this );
        mEffect.resource.clear();
    }

    if( mMesh.resource )
    {
        mMesh.resource->sigMeshChanged.disconnect( this );
        mMesh.resource.clear();
    }

    mPasses.clear();
    mTextures.clear();
    mUniforms.clear();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::copyFrom( const Impl & other )
{
    clear();

    GN_VERIFY( setEffect( other.mEffect.resource ) );
    GN_ASSERT( mPasses.size() == other.mPasses.size() );

    GN_VERIFY( setMesh( other.mMesh.resource, &other.mMesh.subset ) );

    GN_ASSERT( mTextures.size() == other.mTextures.size() );
    for( size_t i = 0; i < other.mTextures.size(); ++i )
    {
        mTextures[i].setResource( *this, i, other.mTextures[i].getResource() );
    }

    GN_ASSERT( mUniforms.size() == other.mUniforms.size() );
    for( size_t i = 0; i < other.mUniforms.size(); ++i )
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

    GN_VERIFY( setEffect( &r ) );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::onMeshChanged( MeshResource & r )
{
    GN_ASSERT( &r == mMesh.resource );

    GN_VERIFY( setMesh( &r, &mMesh.subset ) );
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
    if( db.hasResourceFactory( ModelResource::guid() ) ) return true;

    GpuResourceFactory factory = { &ModelResourceInternal::sCreateInstance };

    return db.registerResourceFactory( ModelResource::guid(), "Model Resource", factory );
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
const Guid & GN::gfx::ModelResource::guid()
{
    static const Guid MODEL_GUID = { 0x24a6e5eb, 0xeb76, 0x440f, { 0xaa, 0x9d, 0x6a, 0x59, 0x34, 0x2f, 0x89, 0x2e } };
    return MODEL_GUID;
}

//
//
// -----------------------------------------------------------------------------
AutoRef<ModelResource>
GN::gfx::ModelResource::loadFromFile(
    GpuResourceDatabase & db,
    const char          * filename )
{
    StrA abspath = fs::resolvePath( fs::getCurrentDir(), filename );
    filename = abspath;

    AutoRef<ModelResource> m( db.findResource<ModelResource>( filename ) );
    if( m ) return m;

    ModelResourceDesc desc;
    desc.clear();
    //if( !loadFromXmlFile( desc, filename ) ) return 0;

    m = db.createResource<ModelResource>( abspath );
    if( m ) m->reset( &desc );

    return m;
}

//
//
// -----------------------------------------------------------------------------
bool                        GN::gfx::ModelResource::reset( const ModelResourceDesc * desc ) { return mImpl->reset( desc ); }
AutoRef<ModelResource>      GN::gfx::ModelResource::makeClone( const char * nameOfTheClone ) const { return mImpl->makeClone( nameOfTheClone ); }
void                        GN::gfx::ModelResource::setTexture( const char * effectParameterName, GpuResource * texture ) { mImpl->setTexture( effectParameterName, texture ); }
AutoRef<TextureResource>    GN::gfx::ModelResource::getTexture( const char * effectParameterName ) const { return mImpl->getTexture( effectParameterName ); }
void                        GN::gfx::ModelResource::setUniform( const char * effectParameterName, GpuResource * uniform ) { mImpl->setUniform( effectParameterName, uniform ); }
AutoRef<UniformResource>    GN::gfx::ModelResource::getUniform( const char * effectParameterName ) const { return mImpl->getUniform( effectParameterName ); }
void                        GN::gfx::ModelResource::setMesh( GpuResource * mesh, const MeshResourceSubset * subset ) { mImpl->setMesh( mesh, subset ); }
//GpuResourceHandle GN::gfx::ModelResource::getMesh( MeshResourceSubset * subset ) const;
void                        GN::gfx::ModelResource::draw() const { mImpl->draw(); }
