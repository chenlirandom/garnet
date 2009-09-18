#include "pch.h"
#include "garnet/GNutil.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::util;

static GN::Logger * sLogger = GN::getLogger("GN.util");

// *****************************************************************************
// Local stuff
// *****************************************************************************

static bool sHasSemantic( const VertexFormat & vf, const char * binding, size_t index )
{
    for( size_t i = 0; i < vf.numElements; ++i )
    {
        if( 0 == strCmpI( vf.elements[i].binding, binding ) &&
            index == vf.elements[i].bindingIndex )
        {
            return true;
        }
    }

    return false;
}

static bool sHasPosition( const VertexFormat & vf )
{
    return sHasSemantic( vf, "position", 0 )
        || sHasSemantic( vf, "pos", 0 );
}

static bool sHasNormal( const VertexFormat & vf )
{
    return sHasSemantic( vf, "normal", 0 );
}

static bool sHasTex0( const VertexFormat & vf )
{
    return sHasSemantic( vf, "texcoord", 0 );
}

static bool sHasTangent( const VertexFormat & vf )
{
    return sHasSemantic( vf, "tangent", 0 );
}

///
/// Determine the best effect that can show the mesh, return NULL for failure
///
static const char * sDetermineBestEffect( const MeshResourceDesc & m )
{
    const VertexFormat & vf = m.vtxfmt;

    // position is required
    if( !sHasPosition( vf ) )
    {
        GN_ERROR(sLogger)( "The mesh has no position, which is required by the mesh viewer." );
        return NULL;
    }

    if( !sHasNormal( vf ) )
    {
        GN_WARN(sLogger)( "The mesh has no normal." );
        return "media::/effect/wireframe.effect.xml";
    }

    if( !sHasTex0( vf ) )
    {
        GN_WARN(sLogger)( "The mesh has no texture coordinate." );
        return "media::/effect/diffuse.effect.xml";
    }

    // use normal map, if the mesh has both normal and tangent.
    if( sHasTangent( vf ) )
    {
        return "media::/effect/normalmap.effect.xml";
    }
    else
    {
        return "media::/effect/diffuse.effect.xml";
    }
}

// *****************************************************************************
// XPR loader
//
// NOTE: XPR2 format is 3D model format used by various games from Tecomo,
//       like DOA.
//
// The file is structured like this:
//
//     <XPRFileHeader> (16 bytes)
//     <ObjectTable>   (16 bytes * number_of_objects)
//     <Unknown>       (12 bytes)
//     <Data>          (data)
//
// *****************************************************************************

struct XPRFileHeader
{
    FOURCC tag;        ///< must be XPR2
    UInt32 size1;      ///< size tag 1
    UInt32 size2;      ///< size tag 2 (file size = size1+size2+12)
    UInt32 numObjects; ///< number of objects in this file
};

struct XPRObjectHeader
{
    FOURCC type;    ///< object type, could be "USER", "TX2D", "VBUF", "IBUF".
    UInt32 offset;  ///< object offset in bytes. The actual offset is this value + 12.
    UInt32 size;    ///< object size in bytes
    UInt32 unknown; ///< I don't know what this is for.
};

// XPR texture descriptor, 0x28 bytes
struct XPRTex2DDesc
{
    // 10 dwords
    UInt32 dwords[10];
};
GN_CASSERT( 0x28 == sizeof(XPRTex2DDesc) );

/// XPR vertex buffer descriptor, 0x14 bytes
struct XPRVBufDesc
{
    UInt32 dwords[5];
};
GN_CASSERT( 0x14 == sizeof(XPRVBufDesc) );

/// XPR index buffer descriptor, 0x14 bytes
struct XPRIBufDesc
{
    UInt32 dwords[5];
};
GN_CASSERT( 0x14 == sizeof(XPRIBufDesc) );

struct XPRScene
{
    DynaArray<gfx::MeshResourceDesc> meshes;
    DynaArray<XPRTex2DDesc*>         texDescs;
    DynaArray<XPRVBufDesc*>          vbDescs;
    DynaArray<XPRIBufDesc*>          ibDescs;
    DynaArray<void*>                 sceneData; ///< store all texture and mesh data
    Boxf                             bbox;      ///< bounding box
};

static bool
loadXprSceneFromFile( XPRScene & xpr, File & file )
{
    size_t readen;

    // read file header
    XPRFileHeader header;
    if( !file.read( &header, sizeof(header), &readen ) || sizeof(header) != readen )
    {
        GN_ERROR(sLogger)( "Fail to read file header." );
        return false;
    }

    // swap header to little endian
    header.size1 = swap8in32( header.size1 );
    header.size2 = swap8in32( header.size2 );
    header.numObjects = swap8in32( header.numObjects );

    // read scene data
    size_t dataSize = header.size1 + header.size2 + 12 - sizeof(header);
    xpr.sceneData.resize( dataSize );
    if( !file.read( xpr.sceneData.cptr(), dataSize, &readen ) || dataSize != readen )
    {
        GN_ERROR(sLogger)( "Fail to read XPR data." );
        return false;
    }

    // iterate all objects
    XPRObjectHeader * objects = (XPRObjectHeader *)xpr.sceneData.cptr();
    for( size_t i = 0; i < header.numObjects; ++i )
    {
        XPRObjectHeader & o = objects[i];

        // do endian swap
        o.offset = swap8in32( o.offset );
        o.size   = swap8in32( o.size );
        o.unknown = swap8in32( o.unknown );

        size_t offset = o.offset - sizeof(header) + 12;
        void * desc   = &xpr.sceneData[offset];

        switch( o.type.u32 )
        {
            case GN_MAKE_FOURCC( 'V','B','U','F' ):
            {
                XPRVBufDesc * vbdesc = (XPRVBufDesc*)desc;
                if( sizeof(XPRVBufDesc) != o.size )
                {
                    GN_ERROR(sLogger)( "object size is invalid." );
                    return false;
                }
                swap8in32( vbdesc->dwords, vbdesc->dwords, sizeof(*vbdesc)/4 );
                xpr.vbDescs.append( vbdesc );
                break;
            }

            case GN_MAKE_FOURCC( 'I','B','U','F' ):
            {
                XPRIBufDesc * ibdesc = (XPRIBufDesc*)desc;
                if( sizeof(XPRIBufDesc) != o.size )
                {
                    GN_ERROR(sLogger)( "object size is invalid." );
                    return false;
                }
                swap8in32( ibdesc->dwords, ibdesc->dwords, sizeof(*ibdesc)/4 );
                xpr.ibDescs.append( ibdesc );
                break;
            }

            case GN_MAKE_FOURCC( 'T','X','2','D' ):
            {
                XPRTex2DDesc * texdesc = (XPRTex2DDesc*)desc;
                if( sizeof(XPRTex2DDesc) == o.size )
                {
                    GN_ERROR(sLogger)( "object size is invalid." );
                    return false;
                }
                swap8in32( texdesc->dwords, texdesc->dwords, sizeof(*texdesc)/4 );
                xpr.texDescs.append( texdesc );
                break;
            }

            case GN_MAKE_FOURCC( 'U','S','E','R' ):
                // user data block is silently ignored.
                break;

            default:
                GN_WARN(sLogger)( "unknown object type." );
                return false;
        }
    }

    // success
    return true;
}

//
//
// -----------------------------------------------------------------------------
static bool
sLoadModelsFromXPR( SimpleWorldDesc & desc, File & file )
{
    // load XPR file
    XPRScene xpr;
    if( !loadXprSceneFromFile( xpr, file ) ) return NULL;

    GN_UNUSED_PARAM( desc );
    GN_UNIMPL_WARNING();
    return false;
}

// *****************************************************************************
//
// ASE loader
//
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
static bool
sLoadModelsFromASE( SimpleWorldDesc & desc, File & file )
{
    // load ASE scene
    AseScene ase;
    if( !loadAseSceneFromFile( ase, file) ) return false;

    StrA filename = file.name();
    if( filename.empty() )
    {
        GN_WARN(sLogger)( "Can not get filename" );
        return false;
    }
    filename = fs::resolvePath( fs::getCurrentDir(), filename );

#define FULL_MESH_NAME( n ) strFormat("%s.%s",filename.cptr(),n.cptr())

    // copy mesh to simple world descriptor
    for( size_t i = 0; i < ase.meshes.size(); ++i )
    {
        const AseMesh    & src = ase.meshes[i];

        MeshResourceDesc & dst = desc.meshes[FULL_MESH_NAME(src.name)];

        dst = src;

        for( size_t i = 0; i < GpuContext::MAX_VERTEX_BUFFERS; ++i )
        {
            size_t vbsize = src.getVtxBufSize( i );
            if( vbsize > 0 )
            {
                desc.meshdata.resize( desc.meshdata.size() + 1 );
                desc.meshdata.back().size = vbsize;
                desc.meshdata.back().data = heapAlloc( vbsize );
                if( !desc.meshdata.back().data )
                {
                    GN_ERROR(sLogger)( "Out of memory." );
                    return false;
                }
                memcpy( desc.meshdata.back().data, src.vertices[i], vbsize );
                dst.vertices[i] = desc.meshdata.back().data;
            }
            else
            {
                dst.vertices[i] = 0;
            }
        }

        size_t ibsize = src.getIdxBufSize();
        if( ibsize > 0 )
        {
            desc.meshdata.resize( desc.meshdata.size() + 1 );
            desc.meshdata.back().size = ibsize;
            desc.meshdata.back().data = heapAlloc( ibsize );
            if( !desc.meshdata.back().data )
            {
                GN_ERROR(sLogger)( "Out of memory." );
                return false;
            }
            memcpy( desc.meshdata.back().data, src.indices, ibsize );
            dst.indices = desc.meshdata.back().data;
        }
        else
        {
            dst.indices = 0;
        }
    }

    // create models
    for( size_t i = 0; i < ase.subsets.size(); ++i )
    {
        const AseMeshSubset & subset = ase.subsets[i];

        const AseMesh & asemesh = ase.meshes[subset.meshid];

        ModelResourceDesc model;

        // determine the effect
        model.effectResourceName = sDetermineBestEffect( asemesh );
        if( model.effectResourceName.empty() ) continue;

        // setup uniforms
        model.uniforms["MATRIX_PVW"].size = sizeof(Matrix44f);
        model.uniforms["MATRIX_WORLD"].size = sizeof(Matrix44f);
        model.uniforms["MATRIX_WORLD_IT"].size = sizeof(Matrix44f);
        model.uniforms["LIGHT0_POSITION"].size = sizeof(Vector4f);
        model.uniforms["LIGHT0_DIFFUSE"].size = sizeof(Vector4f);
        model.uniforms["ALBEDO_COLOR"].size = sizeof(Vector4f);
        model.uniforms["ALBEDO_COLOR"].initialValue.resize(sizeof(Vector4f));
        Vector4f WHITE(1,1,1,1);
        memcpy( model.uniforms["ALBEDO_COLOR"].initialValue.cptr(), &WHITE, sizeof(WHITE) );

        // bind color and texture to effect
        const AseMaterial & am = ase.materials[subset.matid];
        if( !am.mapdiff.bitmap.empty() )
        {
            model.textures["ALBEDO_TEXTURE"].resourceName = am.mapdiff.bitmap;
        }
        if( !am.mapbump.bitmap.empty() )
        {
            model.textures["NORMAL_TEXTURE"].resourceName = am.mapbump.bitmap;
        }

        // TODO: setup uniforms

        // attach mesh
        model.meshResourceName = FULL_MESH_NAME(asemesh.name);

        // setup subset
        model.subset = subset;

        // add model to model list
        desc.models.append( model );
    }

    // add add models into single visual node
    desc.visuals.resize( 1 );
    desc.visuals[0].models.resize( desc.models.size() );
    for( size_t i = 0; i < desc.models.size(); ++i )
    {
        desc.visuals[0].models[i] = i;
    }

    // create an entity
    SimpleWorldDesc::EntityDesc & entity = desc.entities[filename];
    entity.spatial.position.set( 0, 0, 0 );
    entity.spatial.orientation.set( 0, 0, 0, 1 );
    entity.spatial.bbox = ase.bbox;
    entity.visual = 0;

    // setup bounding box of the whole scene
    desc.bbox = ase.bbox;

    return true;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::util::SimpleWorldDesc::clear()
{
    for( size_t i = 0; i < meshdata.size(); ++i )
    {
        safeHeapFree( meshdata[i].data );
    }
    meshdata.clear();

    meshes.clear();
    models.clear();
    visuals.clear();
    entities.clear();
}

//
//
// -----------------------------------------------------------------------------
bool GN::util::SimpleWorldDesc::loadFromFile( const char * filename )
{
    GN_SCOPE_PROFILER( loadWorldFromFile, "Load simple world from file" );

    clear();

    // open file
    AutoObjPtr<File> fp( fs::openFile( filename, "rb" ) );
    if( !fp ) return false;

    // get file extension
    StrA ext = fs::extName( filename );

    // do loading
    if( 0 == strCmpI( ".ase", ext.cptr() ) )
    {
        return sLoadModelsFromASE( *this, *fp );
    }
    else if( 0 == strCmpI( ".xpr", ext.cptr() ) ||
             0 == strCmpI( ".tpr", ext.cptr() ))
    {
        return sLoadModelsFromXPR( *this, *fp );
    }
    else
    {
        GN_ERROR(sLogger)( "Unknown file extension: %s", ext.cptr() );
        return false;
    }
}

//
//
// -----------------------------------------------------------------------------
Entity * GN::util::SimpleWorldDesc::populateTheWorld( World & world ) const
{
    // create root spatial entity of the whole world
    Entity * root = world.createSpatialEntity( NULL );
    if( NULL == root ) return NULL;

    for( std::map<StrA,EntityDesc>::const_iterator i = entities.begin();
         i != entities.end();
         ++i )
    {
        const StrA & entityName = i->first;
        const EntityDesc & entityDesc = i->second;

        Entity * e = world.createVisualEntity( entityName );
        if( !e ) continue;

        // attach the entity to root entity
        e->getNode<SpatialNode>()->setParent( root->getNode<SpatialNode>() );

        // calculate bounding sphere
        const Boxf & bbox = entityDesc.spatial.bbox;
        Spheref bs;
        calculateBoundingSphereFromBoundingBox( bs, bbox );
        e->getNode<SpatialNode>()->setBoundingSphere( bs );

        if( entityDesc.visual != (size_t)-1 )
        {
            const VisualDesc & visual = visuals[entityDesc.visual];

            for( size_t i = 0; i < visual.models.size(); ++i )
            {
                const ModelResourceDesc & modelDesc = models[visual.models[i]];

                // this variable is used to keep a reference to mesh resource,
                // to prevent it from being deleted, until the model is created.
                AutoRef<MeshResource> mesh;

                if( !modelDesc.meshResourceName.empty() )
                {
                    mesh = world.gdb().findResource<MeshResource>( modelDesc.meshResourceName );
                    if( !mesh )
                    {
                        std::map<StrA,gfx::MeshResourceDesc>::const_iterator meshIter = meshes.find(modelDesc.meshResourceName);

                        if( meshes.end() == meshIter )
                        {
                            GN_ERROR(sLogger)(
                                "Model %d references a mesh '%s' that does not belong to this scene.",
                                i,
                                modelDesc.meshResourceName.cptr() );
                            continue; // ignore the model
                        }

                        const MeshResourceDesc & meshDesc = meshIter->second;

                        // create new mesh
                        mesh = world.gdb().createResource<MeshResource>( modelDesc.meshResourceName );
                        if( !mesh || !mesh->reset( &meshDesc ) ) continue;
                    }
                }

                AutoRef<ModelResource> model = world.gdb().createResource<ModelResource>( NULL );
                if( !model->reset( &modelDesc ) ) continue;

                e->getNode<VisualNode>()->addModel( model );
            }
        }
    }

    return root;
}
