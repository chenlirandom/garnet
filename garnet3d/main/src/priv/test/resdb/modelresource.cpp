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
    , mHandle( 0 )
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
void GN::gfx::ModelResource::Impl::TextureItem::setHandle(
    Impl            & owner,
    size_t            effectParameterIndex,
    GpuResourceHandle newTextureHandle )
{
    if( mHandle == newTextureHandle ) return;

    GpuResourceDatabase & db = owner.mOwner.database();

    // disconnect from old handle
    if( mHandle )
    {
        GpuResource * r = db.getResource( mHandle );
        r->sigUnderlyingResourcePointerChanged.disconnect( this );
    }

    Texture * tex;
    if( newTextureHandle )
    {
        // connect to new handle
        GpuResource * r = db.getResource( newTextureHandle );
        r->sigUnderlyingResourcePointerChanged.connect( this, &TextureItem::onTextureChange );

        tex = ((TextureResource*)r)->getTexture();
    }
    else
    {
        tex = NULL;
    }

    // update stored handle value
    mOwner = &owner;
    mEffectParameterIndex = effectParameterIndex;
    mHandle = newTextureHandle;

    updateContext( tex );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::onTextureChange( GpuResource & r )
{
    GN_ASSERT( r.handle() == mHandle );

    Texture * tex = ((TextureResource&)r).getTexture();

    updateContext( tex );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::TextureItem::updateContext( Texture * tex )
{
    GN_ASSERT( mOwner );

    EffectResource * effect = (EffectResource*)mOwner->mOwner.database().getResource( mOwner->mEffect.handle );

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
// GN::gfx::ModelResource::Impl - Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ModelResource::Impl::init( const ModelResourceDesc & desc )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( ModelResource::Impl, () );

    GpuResourceDatabase & db = database();

    // initialize effect handle
    mEffect.handle = 0;
    if( !desc.effectResourceName.empty() )
    {
        mEffect.handle = db.findResource( EffectResource::guid(), desc.effectResourceName );
        if( 0 == mEffect.handle )
        {
            GN_ERROR(sLogger)( "%s is not a valid effect name.", desc.effectResourceName.cptr() );
        }
    }
    else
    {
        mEffect.handle = EffectResource::create( db, strFormat("%s.effect", modelName()), desc.effectResourceDesc );
    }

    // fallback to dummy effect
    if( 0 == mEffect.handle )
    {
        mEffect.handle = db.findResource( EffectResource::guid(), "dummy" );
        if( 0 == mEffect.handle ) GN_ERROR(sLogger)( "No dummy effect defined in GPU resource database." );
    }
    if( 0 == mEffect.handle )
    {
        GN_ERROR(sLogger)( "Fail to initialize effect for model '%s'.", modelName() );
        return failure();
    }

    // initialize mesh
    if( !desc.meshResourceName.empty() )
    {
        mMesh.handle = db.findResource( MeshResource::guid(), desc.meshResourceName );
        if( 0 == mMesh.handle )
        {
            GN_ERROR(sLogger)( "%s is not a valid mesh name.", desc.meshResourceName.cptr() );
        }
    }
    else
    {
        mMesh.handle = MeshResource::create( db, strFormat("%s.model", modelName()), desc.meshResourceDesc );
    }
    if( 0 == mMesh.handle )
    {
        mMesh.handle = db.findResource( MeshResource::guid(), "dummy" );
        if( 0 == mMesh.handle ) GN_ERROR(sLogger)( "No dummy mesh defined in GPU resource database." );
    }
    if( 0 == mMesh.handle )
    {
        return failure();
    }
    GpuResource * mesh = db.getResource( mMesh.handle );
    mesh->sigUnderlyingResourcePointerChanged.connect( this, &Impl::onMeshChanged );

    // attach to effect changing event
    GpuResource * effect = db.getResource( mEffect.handle );
    effect->sigUnderlyingResourcePointerChanged.connect( this, &Impl::onEffectChanged );

    // store the descriptor (used in onEffectChanged())
    mDesc = desc;

    // trigger a effect changing event to initialize everthing else.
    onEffectChanged( *effect );

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::quit()
{
    GN_GUARD;

    GpuResourceDatabase & db = database();

    if( mMesh.handle )
    {
        GpuResource * mesh = db.getResource( mMesh.handle );
        if( mesh )
        {
            mesh->sigUnderlyingResourcePointerChanged.disconnect( this );
        }
    }

    if( mEffect.handle )
    {
        GpuResource * effect = db.getResource( mEffect.handle );
        if( effect )
        {
            effect->sigUnderlyingResourcePointerChanged.disconnect( this );
        }
    }

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::clear()
{
    mEffect.handle = 0;
    mMesh.handle = 0;
    mPasses.clear();
    mTextures.clear();
    mUniforms.clear();
}

// *****************************************************************************
// GN::gfx::ModelResource::Impl - public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::setTexture( const char * effectParameterName, GpuResourceHandle handle )
{
    EffectResource * effect = GpuResource::castTo<EffectResource>( database().getResource( mEffect.handle ) );
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is referencing a invalid effect handle!", modelName() );
        return;
    }

    size_t parameterIndex = effect->findTexture( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid texture name for model %s!", effectParameterName, modelName() );
        return;
    }

    mTextures[parameterIndex].setHandle( *this, parameterIndex, handle );
}

//
//
// -----------------------------------------------------------------------------
GpuResourceHandle
GN::gfx::ModelResource::Impl::getTexture( const char * effectParameterName ) const
{
    EffectResource * effect = GpuResource::castTo<EffectResource>( database().getResource( mEffect.handle ) );
    if( NULL == effect )
    {
        GN_ERROR(sLogger)( "Model %s is referencing a invalid effect handle!", modelName() );
        return 0;
    }

    size_t parameterIndex = effect->findTexture( effectParameterName );
    if( EffectResource::PARAMETER_NOT_FOUND == parameterIndex )
    {
        GN_ERROR(sLogger)( "%s is not a valid texture name for model %s!", effectParameterName, modelName() );
        return 0;
    }

    return mTextures[parameterIndex].getHandle();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::setUniform( const char * effectParameterName, GpuResourceHandle handle )
{
    GN_UNIMPL_WARNING();
    GN_UNUSED_PARAM( effectParameterName );
    GN_UNUSED_PARAM( handle );
}

//
//
// -----------------------------------------------------------------------------
GpuResourceHandle
GN::gfx::ModelResource::Impl::getUniform( const char * effectParameterName ) const
{
    GN_UNIMPL_WARNING();
    GN_UNUSED_PARAM( effectParameterName );
    return 0;
}
//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::draw() const
{
    MeshResource * mesh = GpuResource::castTo<MeshResource>( database().getResource( mMesh.handle ) );
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
                mDesc.subset.numidx,
                mDesc.subset.basevtx,
                0, // startvtx,
                mDesc.subset.numvtx,
                mDesc.subset.startidx );
        }
        else
        {
            g.draw( meshdesc.prim, mDesc.subset.numvtx, mDesc.subset.basevtx );
        }

    }
}

// *****************************************************************************
// GN::gfx::ModelResource::Impl - private methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::onEffectChanged( GpuResource & r )
{
    EffectResource & effect = r.castTo<EffectResource>();

    // initialize passes array
    mPasses.resize( effect.getNumPasses() );
    for( size_t i = 0; i < mPasses.size(); ++i )
    {
        RenderPass & pass = mPasses[i];

        pass.gc.clear();
        effect.applyToContext( i, pass.gc );

        pass.rsdesc = effect.getRenderState( i );
    }

    // reapply mesh
    if( mMesh.handle )
    {
        GpuResource * mesh = database().getResource( mMesh.handle );
        if( mesh ) onMeshChanged( *mesh );
    }

    // reapply textures
    mTextures.resize( effect.getNumTextures() );
    for( size_t i = 0; i < effect.getNumTextures(); ++i )
    {
        TextureItem & t = mTextures[i];

        const EffectResource::TextureProperties & tp = effect.getTextureProperties( i );

        const ModelResourceDesc::ModelTextureDesc * td = sFindNamedPtr( mDesc.textures, tp.parameterName );

        GpuResourceHandle texhandle;
        if( td )
        {
            if( td->resourceName )
            {
                texhandle = TextureResource::loadFromFile( database(), td->resourceName );
            }
            else
            {
                StrA texname = strFormat( "%s.texture.%s", modelName(), tp.parameterName.cptr() );
                texhandle = TextureResource::create( database(), texname, &td->desc );
            }
        }
        else
        {
            GN_WARN(sLogger)(
                "Effec texture parameter '%s' in effect '%s' is not defined in model '%s'.",
                tp.parameterName.cptr(),
                database().getResourceName( effect.handle() ),
                modelName() );

            texhandle = 0;
        }

        t.setHandle( *this, i, 0 );
        t.setHandle( *this, i, texhandle );
    }

    GN_TODO( "initialize uniform array" );
    mUniforms.resize( 0 );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ModelResource::Impl::onMeshChanged( GpuResource & r )
{
    MeshResource & mesh = r.castTo<MeshResource>( r );

    const MeshResourceDesc & meshdesc = mesh.getDesc();

    if( ( mDesc.subset.startidx + mDesc.subset.numidx ) > meshdesc.numidx ||
        ( mDesc.subset.basevtx + mDesc.subset.numvtx ) > meshdesc.numvtx )
    {
        GN_ERROR(sLogger)( "Mesh subset is out of range." );
    }

    for( size_t i = 0; i < mPasses.size(); ++i )
    {
        RenderPass & pass = mPasses[i];

        mesh.applyToContext( pass.gc );
    }
}

// *****************************************************************************
// GN::gfx::ModelResource
// *****************************************************************************

class ModelResourceInternal : public ModelResource
{
    ModelResourceInternal( GpuResourceDatabase & db, GpuResourceHandle handle )
        : ModelResource( db, handle )
    {
    }

    bool init( const void * parameters )
    {
        if( NULL == parameters )
        {
            GN_ERROR(sLogger)( "Null parameter pointer." );
            return false;
        }
        return mImpl->init( *(const ModelResourceDesc*)parameters );
    }

    static GpuResource *
    createInstance( GpuResourceDatabase & db,
                    GpuResourceHandle     handle,
                    const void          * parameters )
    {
        AutoObjPtr<ModelResourceInternal> m( new ModelResourceInternal( db, handle ) );
        if( !m->init( parameters ) ) return NULL;
        return m.detach();
    }

    static void deleteInstance( GpuResource * p )
    {
        delete GpuResource::castTo<ModelResourceInternal>( p );
    }

public:

    static bool checkAndRegisterFactory( GpuResourceDatabase & db )
    {
        if( db.hasResourceFactory( guid() ) ) return true;

        GpuResourceFactory factory = { &createInstance, &deleteInstance };

        return db.registerResourceFactory( guid(), "Model Resource", factory );
    }
};

//
//
// -----------------------------------------------------------------------------
GN::gfx::ModelResource::ModelResource( GpuResourceDatabase & db, GpuResourceHandle h )
    : GpuResource( db, h ), mImpl(NULL)
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
GpuResourceHandle GN::gfx::ModelResource::create(
    GpuResourceDatabase     & db,
    const char              * name,
    const ModelResourceDesc & desc )
{
    if( !ModelResourceInternal::checkAndRegisterFactory( db ) ) return NULL;

    return db.createResource( ModelResource::guid(), name, &desc );
}

//
//
// -----------------------------------------------------------------------------
GpuResourceHandle GN::gfx::ModelResource::loadFromFile(
    GpuResourceDatabase & db,
    const char          * filename )
{
    if( !ModelResourceInternal::checkAndRegisterFactory( db ) ) return NULL;

    StrA abspath = fs::resolvePath( fs::getCurrentDir(), filename );
    filename = abspath;

    GpuResourceHandle handle = db.findResource( guid(), filename );
    if( handle ) return handle;

    ModelResourceDesc desc;
    desc.clear();
    //if( !loadFromXmlFile( desc, filename ) ) return 0;

    return db.createResource( ModelResource::guid(), abspath, &desc );
}

//
//
// -----------------------------------------------------------------------------
void              GN::gfx::ModelResource::setTexture( const char * effectParameterName, GpuResourceHandle handle ) { return mImpl->setTexture( effectParameterName, handle ); }
GpuResourceHandle GN::gfx::ModelResource::getTexture( const char * effectParameterName ) const { return mImpl->getTexture( effectParameterName ); }
void              GN::gfx::ModelResource::setUniform( const char * effectParameterName, GpuResourceHandle handle ) { return mImpl->setUniform( effectParameterName, handle ); }
GpuResourceHandle GN::gfx::ModelResource::getUniform( const char * effectParameterName ) const { return mImpl->getUniform( effectParameterName ); }
//void              GN::gfx::ModelResource::setRenderTarget( const char * effectParameterName, GpuResourceHandle handle, size_t face, size_t level, size_t slice );
//GpuResourceHandle GN::gfx::ModelResource::getRenderTarget( const char * effectParameterName, size_t * face, size_t * level, size_t * slice ) const;
//void              GN::gfx::ModelResource::setMesh( GpuResourceHandle mesh, const MeshResourceSubset * subset );
//GpuResourceHandle GN::gfx::ModelResource::getMesh( MeshResourceSubset * subset ) const;
void              GN::gfx::ModelResource::draw() const { mImpl->draw(); }
