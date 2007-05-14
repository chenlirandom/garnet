#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::engine;

static GN::Logger * sLogger = GN::getLogger("GN.engine.Texture");

// *****************************************************************************
// local functions
// *****************************************************************************

class TextureLoader : public GraphicsResourceLoader
{
    const StrA mFileName;

public:

    TextureLoader( const StrA & filename ) : mFileName(filename) {}

    virtual bool load( const GraphicsResourceDesc &, void * & outbuf, size_t & outbytes, int )
    {
        // open file
        AutoObjPtr<File> fp( core::openFile( mFileName, "rb" ) );
        if( 0 == fp ) return false;

        // create temporary buffer
        size_t bytes = fp->size();
        AutoTypePtr<UInt8> buf( new UInt8[bytes] );

        // read file
        size_t readen;
        if( !fp->read( buf, bytes, &readen ) || readen != bytes ) return false;

        // success
        outbuf = buf.detach();
        outbytes = bytes;
        return true;
    }

    virtual bool decompress( const GraphicsResourceDesc &, void * & outbuf, size_t & outbytes, const void * inbuf, size_t inbytes, int )
    {
        MemFile<const UInt8> file( (const UInt8*)inbuf, inbytes );

        ImageReader ir;
        ImageDesc   id;

        if( !ir.reset( file ) ) return false;

        if( !ir.readHeader( id ) ) return false;

        size_t mipcount = id.numFaces*id.numLevels;

        // create temporary buffer
        size_t bytes = id.getTotalBytes() + sizeof(ImageDesc) + sizeof(MipmapDesc) * mipcount;
        AutoTypePtr<UInt8> buf( new UInt8[bytes] );

        // fill header of output buffer
        ImageDesc  * outdesc = (ImageDesc*)buf.get();
        MipmapDesc * outmips = (MipmapDesc*)(outdesc+1);
        UInt8      * data    = (UInt8*)(outmips+mipcount);
        memcpy( outdesc, &id, sizeof(ImageDesc) );
        outdesc->mipmaps = outmips;

        // read image data
        if( !ir.readImage( data ) ) return false;

        // success
        outbuf = buf.detach();
        outbytes = bytes;
        return true;
    }

    virtual bool copy( GraphicsResource & res, const void * inbuf, size_t inbytes, int )
    {
        Texture * tex = res.texture;
        GN_ASSERT( tex );

        const ImageDesc  * indesc = (ImageDesc*)inbuf;
        size_t             offset = sizeof(ImageDesc) + sizeof(MipmapDesc) * indesc->numFaces * indesc->numLevels;
        const UInt8      * data   = (const UInt8*)inbuf + offset;

        if( offset + indesc->getTotalBytes() > inbytes )
        {
            GN_ERROR(sLogger)( "input buffer is incomplete!" );
            return false;
        }

        GN_ASSERT( indesc->numFaces == tex->getDesc().faces );
        GN_ASSERT( indesc->numLevels <= tex->getDesc().levels );

        for( size_t face = 0; face < indesc->numFaces; ++face )
        {
            for( size_t level = 0; level < indesc->numLevels; ++level )
            {
                const MipmapDesc & md = indesc->getMipmap( face, level );

                const UInt8 * src = data + indesc->getLevelOffset( face, level );

                TexLockedResult tlr;
                if( !tex->lock( tlr, face, level, NULL, LOCK_DISCARD ) ) return false;

                if( tlr.sliceBytes == md.slicePitch )
                {
                    memcpy( tlr.data, src, tlr.sliceBytes * md.depth );
                }
                else
                {
                    GN_UNIMPL_WARNING();
                }

                tex->unlock();
            }
        }

        // success
        return true;
    }

    virtual void freebuf( void * inbuf, size_t )
    {
        safeDeleteArray( inbuf );
    }
};

//
//
// -----------------------------------------------------------------------------
static bool imageDesc2TextureDesc( TextureDesc & td, const ImageDesc & id )
{
    // get image size
    UInt32 w = id.mipmaps[0].width;
    UInt32 h = id.mipmaps[0].height;
    UInt32 d = id.mipmaps[0].depth;

    // determine texture dimension, based on image demension
    if( 1 == id.numFaces )
    {
        td.dim = 1 == d ? TEXDIM_2D : TEXDIM_3D;
    }
    else if( 6 == id.numFaces && w == h && 1 == d )
    {
        td.dim = TEXDIM_CUBE;
    }
    else if( 1 == d )
    {
        GN_ASSERT( id.numFaces > 1 );
        td.dim = TEXDIM_STACK;
    }
    else
    {
        GN_ERROR(sLogger)( "Can't determine texture dimension for image: face(%d), width(%d), height(%d), depth:%d)." );
        return false;
    }

    td.width     = w;
    td.height    = h;
    td.depth     = d;
    td.faces     = id.numFaces;
    td.levels    = id.numLevels;
    td.format    = id.format;
    td.usage.u32 = 0;

    return td.validate();
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::engine::EntityTypeId GN::engine::getTextureEntityType( EntityManager & em )
{
    static EntityTypeId type = em.createEntityType( "texture" );
    return type;
}

//
//
// -----------------------------------------------------------------------------
GN::engine::Entity * GN::engine::loadTextureEntity(
    EntityManager & em, RenderEngine & re, const StrA & filename )
{
    GN_TODO( "convert filename to absolute/full path" );

    // check if the texture is already loaded
    Entity * e = em.getEntityByName( filename, true );
    if( e ) return e;

    // open file
    AutoObjPtr<File> fp( core::openFile( filename, "rb" ) );
    if( 0 == fp ) return false;

    // read image header
    ImageReader ir;
    ImageDesc   id;
    if( !ir.reset( *fp ) ) return false;
    if( !ir.readHeader( id ) ) return false;

    // close file
    fp.clear();

    // initialize resource descriptor
    GraphicsResourceDesc grd;
    grd.type = GRT_TEXTURE;
    if( !imageDesc2TextureDesc( grd.td, id ) ) return 0;

    // create the resource
    GraphicsResource * res = re.allocResource( grd );
    if( 0 == res ) return 0;

    // do load
    AutoObjPtr<TextureLoader> loader( new TextureLoader(filename) );
    re.updateResource( res, 0, loader );

    // success
    return em.createEntity( getTextureEntityType(em), filename, res );
}

//
//
// -----------------------------------------------------------------------------
GN::engine::Entity * GN::engine::createTextureEntity(
    EntityManager & em,
    RenderEngine & re,
    const StrA & name,
    const gfx::TextureDesc & desc )
{
    // check if the texture is already loaded
    Entity * e = em.getEntityByName( name, true );
    if( e )
    {
        GN_ERROR(sLogger)( "entity named '%s' does exist already!", name.cptr() );
        return 0;
    }

    // create the resource (note that content of the texture is leaving undefined)
    GraphicsResourceDesc grd;
    grd.type = GRT_TEXTURE;
    grd.td = desc;
    GraphicsResource * res = re.allocResource( grd );
    if( 0 == res ) return 0;
    
    // success
    return em.createEntity( getTextureEntityType(em), name, res );
}

//
//
// -----------------------------------------------------------------------------
void GN::engine::deleteTextureEntity( Entity * )
{
}

//
//
// -----------------------------------------------------------------------------
void GN::engine::deleteAllTextureEntitys( EntityManager & )
{
}
