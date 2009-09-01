#include "pch.h"
#include "textureresource.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.gpures");

// *****************************************************************************
// Local stuff
// *****************************************************************************

// *****************************************************************************
// GN::gfx::TextureResource
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
const Guid & GN::gfx::TextureResource::guid()
{
    static const Guid TEXTURE_GUID = {0x6ad8b59d, 0xe672, 0x4b5e, { 0x8e, 0xec, 0xf7, 0xac, 0xd4, 0xf1, 0x99, 0xdd } };
    return TEXTURE_GUID;
}

//
//
// -----------------------------------------------------------------------------
GpuResourceHandle GN::gfx::TextureResource::create(
    GpuResourceDatabase & db,
    const char          * name,
    const TextureDesc   * desc )
{
    return db.createResource( TextureResource::guid(), name, desc );
}

//
//
// -----------------------------------------------------------------------------
GpuResourceHandle GN::gfx::TextureResource::loadFromFile(
    GpuResourceDatabase & db,
    const char          * filename )
{
    // convert to full (absolute) path
    StrA abspath = fs::resolvePath( fs::getCurrentDir(), filename );
    filename = abspath;

    // Reuse existing resource, if possible
    GpuResourceHandle handle = db.findResource( guid(), filename );
    if( handle ) return handle;

    // load new texture from file
    GN_INFO(sLogger)( "Load texture from file: %s", filename );

    // load image
    ImageDesc id;
    std::vector<UInt8> texels;
    if( !loadImageFromFile( id, texels, filename ) ) return 0;

    // create texture
    TextureDesc td;
    td.fromImageDesc( id );
    AutoRef<Texture> tex( db.gpu().createTexture( td ) );
    if( !tex ) return 0;

    // update texture content
    for( size_t f = 0; f < td.faces; ++f )
    for( size_t l = 0; l < td.levels; ++l )
    {
        const MipmapDesc & md = id.getMipmap( f, l );
        size_t offset = id.getMipmapOffset( f, l );
        tex->updateMipmap( f, l, 0, md.rowPitch, md.slicePitch, &texels[offset], SurfaceUpdateFlag::DEFAULT );
    }

    // create texture resource
    handle = db.createResource( TextureResource::guid(), filename, NULL );
    if( 0 == handle ) return 0;

    // attach the texture to the resource
    db.getResource( handle )->castTo<TextureResource>().setTexture( tex );

    // success
    return handle;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::TextureResource::setTexture( const AutoRef<Texture> & newTexture )
{
    if( mTexture == newTexture ) return;

    mTexture = newTexture;

    sigUnderlyingResourcePointerChanged(*this);
}

// *****************************************************************************
// GN::gfx::TextureResourceInternal
// *****************************************************************************

class TextureResourceInternal : public TextureResource
{
    //
    //
    // -----------------------------------------------------------------------------
    TextureResourceInternal( GpuResourceDatabase & db, GpuResourceHandle h )
        : TextureResource( db, h )
    {
    }

    //
    //
    // -----------------------------------------------------------------------------
    ~TextureResourceInternal()
    {
    }

    //
    //
    // -----------------------------------------------------------------------------
    static GpuResource * sCreateInstance(
        GpuResourceDatabase & db,
        GpuResourceHandle     handle,
        const void          * parameters )
    {
        TextureResource * m = new TextureResourceInternal( db, handle );

        if( NULL != parameters )
        {
            const TextureDesc * desc = (const TextureDesc*)parameters;

            AutoRef<Texture> t( db.gpu().createTexture( *desc ) );

            m->setTexture( t );
        }

        return m;
    }

    //
    //
    // -----------------------------------------------------------------------------
    static void sDeleteInstance( GpuResource * p )
    {
        delete GpuResource::castTo<TextureResourceInternal>( p );
    }

public:

    //
    //
    // -----------------------------------------------------------------------------
    static bool sRegisterFactory( GpuResourceDatabase & db )
    {
        GpuResourceFactory factory = { &sCreateInstance, &sDeleteInstance };

        if( db.hasResourceFactory( TextureResource::guid() ) ) return true;

        return db.registerResourceFactory( TextureResource::guid(), "Texture Resource", factory );
    }
};

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::registerTextureResourceFactory( GpuResourceDatabase & db )
{
    return TextureResourceInternal::sRegisterFactory( db );
}
