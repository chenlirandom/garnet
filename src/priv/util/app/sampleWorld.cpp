#include "pch.h"
#include "garnet/GNutil.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::engine;
using namespace GN::util;

static GN::Logger * sLogger = GN::getLogger("GN.util");

// *****************************************************************************
// Local stuff
// *****************************************************************************

static bool sHasSemantic( const MeshVertexFormat & vf, const char * semantic )
{
    for( size_t i = 0; i < vf.numElements; ++i )
    {
        if( 0 == stringCompareI( vf.elements[i].semantic, semantic ) )
        {
            return true;
        }
    }

    return false;
}

static bool sHasPosition( const MeshVertexFormat & vf )
{
    return sHasSemantic( vf, "position" )
        || sHasSemantic( vf, "pos" );
}

static bool sHasNormal( const MeshVertexFormat & vf )
{
    return sHasSemantic( vf, "normal" );
}

static bool sHasTex0( const MeshVertexFormat & vf )
{
    return sHasSemantic( vf, "texcoord" );
}

static bool sHasTangent( const MeshVertexFormat & vf )
{
    return sHasSemantic( vf, "tangent" );
}

///
/// Determine the best model template that can show the mesh, return NULL for failure
///
static const ModelResourceDesc * sDetermineBestModelTemplate( const MeshResourceDesc & m )
{
    const MeshVertexFormat & vf = m.vtxfmt;

    // position is required
    if( !sHasPosition( vf ) )
    {
        GN_ERROR(sLogger)( "The mesh has no position, which is required by the mesh viewer." );
        return NULL;
    }

    if( !sHasNormal( vf ) )
    {
        GN_WARN(sLogger)( "The mesh has no normal." );
        return &SimpleWireframeModel::DESC;
    }

    if( !sHasTex0( vf ) )
    {
        GN_WARN(sLogger)( "The mesh has no texture coordinate." );
        return &SimpleDiffuseModel::DESC;
    }

    // Program reaches here, means that the mesh has position, norml and texcoord.

    if( sHasTangent( vf ) )
    {
        return &SimpleNormalMapModel::DESC;
    }
    else
    {
        return &SimpleDiffuseModel::DESC;
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
    uint32 size1;      ///< size tag 1
    uint32 size2;      ///< size tag 2 (file size = size1+size2+12)
    uint32 numObjects; ///< number of objects in this file
};

struct XPRObjectHeader
{
    FOURCC type;    ///< object type, could be "USER", "TX2D", "VBUF", "IBUF".
    uint32 offset;  ///< object offset in bytes. The actual offset is this value + 12.
    uint32 size;    ///< object size in bytes
    uint32 unknown; ///< I don't know what this is for.
};

// XPR texture descriptor, 0x28 bytes
struct XPRTex2DDesc
{
    // 10 dwords
    uint32 dwords[10];
};
GN_CASSERT( 0x28 == sizeof(XPRTex2DDesc) );

/// XPR vertex buffer descriptor, 0x14 bytes
struct XPRVBufDesc
{
    uint32 dwords[5];
};
GN_CASSERT( 0x14 == sizeof(XPRVBufDesc) );

/// XPR index buffer descriptor, 0x14 bytes
struct XPRIBufDesc
{
    uint32 dwords[5];
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
sLoadXprSceneFromFile( XPRScene & xpr, File & file )
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
sLoadWorldFromXPR( SampleWorldDesc & desc, File & file )
{
    // load XPR file
    XPRScene xpr;
    if( !sLoadXprSceneFromFile( xpr, file ) ) return NULL;

    GN_UNUSED_PARAM( desc );
    GN_UNIMPL_WARNING();
    return false;
}

//
//
// -----------------------------------------------------------------------------
bool
sLoadModelsFromXPR( VisualComponent & comp, GpuResourceDatabase & db, File & file )
{
    // load XPR file
    XPRScene xpr;
    if( !sLoadXprSceneFromFile( xpr, file ) ) return NULL;

    // create mesh list
    DynaArray<AutoRef<MeshResource> > meshes;
    meshes.resize( xpr.meshes.size() );
    for( size_t i = 0; i < xpr.meshes.size(); ++i )
    {
        meshes[i] = db.createResource<MeshResource>( NULL );
        if( !meshes[i] ) return false;
        if( !meshes[i]->reset( &xpr.meshes[i] ) ) return false;
    }

    GN_UNUSED_PARAM( comp );
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
sLoadWorldFromASE( SampleWorldDesc & desc, File & file )
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

#define FULL_MESH_NAME( n ) (n) // stringFormat("%s.%s",filename.cptr(),n.cptr())

    // copy meshes. create entities as well, since in ASE scene, one mesh is one node.
    for( size_t i = 0; i < ase.meshes.size(); ++i )
    {
        const AseMesh & src = ase.meshes[i];

        const StrA    & meshname = FULL_MESH_NAME(src.name);

        // copy mesh
        desc.meshes[meshname] = src;

        // create the entity
        SampleWorldDesc::EntityDesc & entityDesc = desc.entities[meshname];
        entityDesc.spatial.parent = src.parent.empty() ? "" : FULL_MESH_NAME(src.parent);
        entityDesc.spatial.position = src.pos;
        entityDesc.spatial.orientation.fromRotation( src.rotaxis, src.rotangle );
        entityDesc.spatial.bbox = src.selfbbox;
    }

    // copy mesh data
    desc.meshdata = ase.meshdata;

    // create models
    for( size_t i = 0; i < ase.subsets.size(); ++i )
    {
        const AseMeshSubset & subset = ase.subsets[i];

        const AseMesh & asemesh = ase.meshes[subset.meshid];

        // determine the model template
        const ModelResourceDesc * modelTemplate = sDetermineBestModelTemplate( asemesh );
        if( NULL == modelTemplate ) continue;

        // initialize the model descriptor based on the template
        ModelResourceDesc model = *modelTemplate;
        model.mesh = FULL_MESH_NAME(asemesh.name);
        model.subset = subset;

        // associate texture to the model
        const AseMaterial & am = ase.materials[subset.matid];
        if( model.hasTexture("ALBEDO_TEXTURE") && !am.mapdiff.bitmap.empty() )
        {
            model.textures["ALBEDO_TEXTURE"].resourceName = am.mapdiff.bitmap;
        }
        if( model.hasTexture("NORMAL_TEXTURE") && !am.mapbump.bitmap.empty() )
        {
            model.textures["NORMAL_TEXTURE"].resourceName = am.mapbump.bitmap;
        }

        // add the model to model list
        StrA modelname = stringFormat( "%s.%u", asemesh.name.cptr(), i );
        GN_ASSERT( NULL == desc.models.find( modelname ) );
        desc.models[modelname] = model;

        // add the model to appropriate entity
        GN_ASSERT( NULL != desc.entities.find( model.mesh ) );
        desc.entities[model.mesh].models.append( modelname );
    }

    // setup bounding box of the whole scene
    desc.bbox = ase.bbox;

    return true;
}

struct ModelType
{
    enum ENUM
    {
        NONE,
        WIREFRAME,
        DIFFUSE,
        NORMAL_MAP,
        NUM_EFFECT_TYPES,
    };
};

///
/// Determine the best effect that can show the mesh
///
static ModelType::ENUM sDetermineBestModel( const MeshResource & m )
{
    const MeshVertexFormat & vf = m.getDesc().vtxfmt;

    // position is required
    if( !sHasPosition( vf ) )
    {
        GN_ERROR(sLogger)( "The mesh has no position, which is required by the mesh viewer." );
        return ModelType::NONE;
    }

    if( !sHasNormal( vf ) )
    {
        GN_WARN(sLogger)( "The mesh has no normal." );
        return ModelType::WIREFRAME;
    }

    if( !sHasTex0( vf ) )
    {
        GN_WARN(sLogger)( "The mesh has no texture coordinate." );
        return ModelType::WIREFRAME;
    }

    // use normal map, if the mesh has both normal and tangent.
    if( sHasTangent( vf ) )
    {
        return ModelType::NORMAL_MAP;
    }
    else
    {
        return ModelType::DIFFUSE;
    }
}

//
//
// -----------------------------------------------------------------------------
bool
sLoadModelsFromASE( VisualComponent & comp, GpuResourceDatabase & db, File & file )
{
    GN_SCOPE_PROFILER( sLoadModelsFromASE, "Load ASE into VisualComponent" );

    // load ASE scene
    AseScene ase;
    if( !loadAseSceneFromFile( ase, file) ) return false;

    // create mesh list
    DynaArray<AutoRef<MeshResource> > meshes;
    {
        GN_SCOPE_PROFILER( sLoadModelsFromASE_GenerateMeshList, "Load ASE into VisualComponent: generating mesh list" );

        meshes.resize( ase.meshes.size() );

        for( size_t i = 0; i < ase.meshes.size(); ++i )
        {
            char meshname[1024];
            stringPrintf( meshname, 1024, "%s.mesh.%u", file.name(), i );

            meshes[i] = db.findResource<MeshResource>( meshname );
            if( meshes[i] ) continue; // use exising mesh directly.

            meshes[i] = db.createResource<MeshResource>( meshname );
            if( !meshes[i] ) return false;
            if( !meshes[i]->reset( &ase.meshes[i] ) ) return false;
        }
    }

    // initialize model templates
    SimpleWireframeModel wireframeModel( db );
    if( !wireframeModel.init() ) return false;
    SimpleDiffuseModel diffuseModel( db );
    if( !diffuseModel.init() ) return false;
    SimpleNormalMapModel normalMapModel( db );
    if( !normalMapModel.init() ) return false;

    // create model
    for( size_t i = 0; i < ase.subsets.size(); ++i )
    {
        const AseMeshSubset & subset = ase.subsets[i];

        MeshResource * mesh = meshes[subset.meshid];

        // determine the model template
        ModelType::ENUM mt = sDetermineBestModel( *mesh );
        ModelResource * modelTemplate;
        switch( mt )
        {
            case ModelType::WIREFRAME:
                modelTemplate = &wireframeModel.modelResource();
                break;

            case ModelType::DIFFUSE:
                modelTemplate = &diffuseModel.modelResource();
                break;

            case ModelType::NORMAL_MAP:
                modelTemplate = &normalMapModel.modelResource();
                break;

            default:
                modelTemplate = NULL;
                break;
        }

        // skip the mesh, if there's no appropriate template for it.
        if( !modelTemplate ) continue;

        // make a clone of the selected template
        AutoRef<ModelResource> clone = modelTemplate->makeClone( NULL );
        if( NULL == clone ) return false;

        // bind textures to effect
        {
            GN_SCOPE_PROFILER( sLoadModelsFromASE_LoadTextures, "Load ASE into VisualComponent: load textures" );

            AutoRef<EffectResource> e = clone->effectResource();

            const AseMaterial & am = ase.materials[subset.matid];

            AutoRef<TextureResource> t;

            if( e->hasTexture("ALBEDO_TEXTURE") && !am.mapdiff.bitmap.empty() )
            {
                t = TextureResource::loadFromFile( db, am.mapdiff.bitmap );
                clone->setTextureResource( "ALBEDO_TEXTURE", t );
            }
            if( e->hasTexture( "NORMAL_TEXTURE" ) && !am.mapbump.bitmap.empty() )
            {
                t = TextureResource::loadFromFile( db, am.mapbump.bitmap );
                clone->setTextureResource( "ALBEDO_TEXTURE", t );
            }
        }

        clone->setMeshResource( mesh, &subset );

        comp.addModel( clone );
    }

    // update bounding box
    Entity * e = comp.getEntity();
    SpacialComponent * sn = e ? e->getComponent<SpacialComponent>() : NULL;
    if( sn ) sn->setSelfBoundingBox( ase.bbox );

    return true;
}

// *****************************************************************************
//
// XML loader
//
// *****************************************************************************

//
// post error message
// -----------------------------------------------------------------------------
static void sPostXMLError( const XmlNode & node, const StrA & msg )
{
    GN_UNUSED_PARAM( node );
    const XmlElement * e = node.toElement();
    if( e )
    {
        GN_ERROR(sLogger)( "<%s>: %s", e->name.cptr(), msg.cptr() );
    }
    else
    {
        GN_ERROR(sLogger)( "%s", msg.cptr() );
    }
}

//
//
// -----------------------------------------------------------------------------
static bool
sParseModel( SampleWorldDesc & desc, XmlElement & root, const StrA & basedir )
{
    ModelResourceDesc md;

    if( !md.loadFromXml( root, basedir ) ) return false;

    XmlAttrib * modelName = root.findAttrib( "name" );
    if( !modelName || modelName->value.empty() )
    {
        GN_ERROR(sLogger)( "Model name attribute is missing." );
        return false;
    }

    if( NULL == desc.meshes.find( md.mesh ) )
    {
        MeshResourceDesc mesh;
        AutoRef<Blob> blob = mesh.loadFromFile( fs::resolvePath( basedir, md.mesh ) );
        if( !blob ) return false;

        desc.meshes[md.mesh] = mesh;
        desc.meshdata.append( blob );
    }

    desc.models[modelName->value] = md;

    return true;
}

//
//
// -----------------------------------------------------------------------------
static bool
sParseEntity( SampleWorldDesc & desc, XmlElement & root )
{
    GN_ASSERT( root.name == "entity" );

    XmlAttrib * entityName = root.findAttrib( "name" );
    if( NULL == entityName )
    {
        sPostXMLError( root, "Entity name attribute is missing." );
        return false;
    }

    SampleWorldDesc::EntityDesc entity;

    // parse spatial
    XmlElement * spatialNode = root.findChildElement( "spatial" );
    if( !spatialNode )
    {
        sPostXMLError( root, "<spatial> element is missing." );
        return false;
    }
    else
    {
        XmlAttrib * a = spatialNode->findAttrib( "parent" );
        if( a ) entity.spatial.parent = a->value;

        a = spatialNode->findAttrib( "position" );
        if( !a || 3 != string2FloatArray( (float*)&entity.spatial.position, 3, a->value ) )
        {
            sPostXMLError( *spatialNode, "Invalid position" );
            entity.spatial.position.set( 0, 0, 0 );
        }

        a = spatialNode->findAttrib( "orientation" );
        if( !a || 4 != string2FloatArray( (float*)&entity.spatial.orientation, 4, a->value ) )
        {
            sPostXMLError( *spatialNode, "Invalid orientation" );
            entity.spatial.orientation.set( 0, 0, 0, 1 );
        }

        a = spatialNode->findAttrib( "bbox" );
        if( !a || 6 != string2FloatArray( (float*)&entity.spatial.bbox, 6, a->value ) )
        {
            sPostXMLError( *spatialNode, "Invalid bounding box" );
            entity.spatial.bbox = Boxf( 0, 0, 0, 0, 0, 0 );
        }
    }

    // parse visual
    XmlElement * visualNode = root.findChildElement( "visual" );
    if( visualNode )
    {
        for( XmlNode * n = visualNode->firstc; n != NULL; n = n->nexts )
        {
            XmlElement * e = n->toElement();
            if( !e ) continue;

            if( "model" == e->name )
            {
                XmlAttrib * a = e->findAttrib( "ref" );
                if( !a )
                {
                    sPostXMLError( *e, "ref attribute is missing." );
                    return false;
                }

                entity.models.append( a->value );
            }
            else
            {
                sPostXMLError( *e, stringFormat( "Unknown element: <%s>", e->name.cptr() ) );
            }
        }
    }

    // done
    desc.entities[entityName->value] = entity;
    return true;
}

//
//
// -----------------------------------------------------------------------------
static bool
sLoadFromXML( SampleWorldDesc & desc, File & file )
{
    XmlDocument doc;
    XmlParseResult xpr;
    if( !doc.parse( xpr, file ) )
    {
        static Logger * sLogger = getLogger( "GN.base.xml" );
        GN_ERROR(sLogger)(
            "Fail to parse XML file (%s):\n"
            "    line   : %d\n"
            "    column : %d\n"
            "    error  : %s",
            file.name(),
            xpr.errLine,
            xpr.errColumn,
            xpr.errInfo.cptr() );
        return false;
    }
    GN_ASSERT( xpr.root );

    StrA basedir = fs::dirName( file.name() );

    XmlElement * root = xpr.root->toElement();
    if( !root || "simpleWorld" != root->name )
    {
        sPostXMLError( *root, "Root element name must be \"<simpleWorld>\"." );
        return false;
    }

    XmlAttrib * bboxAttr = root->findAttrib( "bbox" );
    if( !bboxAttr || 6 != string2FloatArray( (float*)&desc.bbox, 6, bboxAttr->value ) )
    {
        sPostXMLError( *root, "Invalid bbox attribute." );
        return false;
    }

    // parse models
    XmlElement * modelsNode = root->findChildElement( "models" );
    if( NULL == modelsNode )
    {
        sPostXMLError( *root, "Element <models> is missing." );
        return false;
    }
    for( XmlNode * n = modelsNode->firstc; n != NULL; n = n->nexts )
    {
        XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "model" == e->name )
        {
            if( !sParseModel( desc, *e, basedir ) ) return false;
        }
        else
        {
            sPostXMLError( *e, stringFormat( "Ignore unknowned element: <%s>", e->name.cptr() ) );
        }
    }

    // parse entities
    XmlElement * entitiesNode = root->findChildElement( "entities" );
    if( NULL == entitiesNode )
    {
        sPostXMLError( *root, "Element <entities> is missing." );
        return false;
    }
    for( XmlNode * n = entitiesNode->firstc; n != NULL; n = n->nexts )
    {
        XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "entity" == e->name )
        {
            if( !sParseEntity( desc, *e ) ) return false;
        }
        else
        {
            sPostXMLError( *e, stringFormat( "Ignore unknowned element: <%s>", e->name.cptr() ) );
        }
    }

    // done
    return true;
}

//
//
// -----------------------------------------------------------------------------
static bool
sSaveToXML( const SampleWorldDesc & desc, const char * filename )
{
    // check dirname
    if( NULL == filename )
    {
        GN_ERROR(sLogger)( "NULL directory name" );
        return false;
    }

    // convert to full path
    StrA fullpath = fs::resolvePath( fs::getCurrentDir(), filename );
    filename = fullpath;
    StrA dirname = fs::dirName( fullpath );
    StrA basename = fs::baseName( fullpath );

    if( !fs::isDir( dirname ) )
    {
        GN_ERROR(sLogger)( "%s is not a directory", dirname.cptr() );
        return false;
    }

    // write meshes
    int meshindex = 0;
    StringMap<char,StrA> meshNameMapping;
    for( const StringMap<char,MeshResourceDesc>::KeyValuePair * i = desc.meshes.first();
        i != NULL;
        i = desc.meshes.next( i ) )
    {
        const StrA & oldMeshName = i->key;
        const MeshResourceDesc & mesh = i->value;

        StrA newMeshName = stringFormat( "%s.%d.mesh.bin", basename.cptr(), meshindex );

        if( !mesh.saveToFile( dirname + "\\" + newMeshName ) ) return false;

        meshNameMapping[oldMeshName] = newMeshName;

        ++meshindex;
    }

    // create a new XML document
    XmlDocument xmldoc;
    XmlElement * root = xmldoc.createElement(NULL);
    root->name = "simpleWorld";

    // write models
    XmlElement * models = xmldoc.createElement( root );
    models->name = "models";
    for( const StringMap<char,gfx::ModelResourceDesc>::KeyValuePair * i = desc.models.first();
         i != NULL;
         i = desc.models.next( i ) )
    {
        const StrA & modelName  = i->key;
        ModelResourceDesc model = i->value;

        StrA * pNewMeshName = meshNameMapping.find( model.mesh );
        if( NULL != pNewMeshName )
        {
            model.mesh = *pNewMeshName;
        }

        XmlElement * modelNode = model.saveToXml( *models, dirname );
        if( !modelNode ) return false;

        XmlAttrib * a = xmldoc.createAttrib( modelNode );
        a->name = "name";
        a->value = modelName;
    }

    // rename entities
    int entityIndex = 0;
    StringMap<char,StrA> entityNameMap;
    for( const StringMap<char,SampleWorldDesc::EntityDesc>::KeyValuePair * i = desc.entities.first();
        i != NULL;
        i = desc.entities.next( i ) )
    {
        const StrA & entityName = i->key;

        entityNameMap[entityName] = stringFormat( "%d", ++entityIndex );
    }

    // write entities
    XmlElement * entities = xmldoc.createElement( root );
    entities->name = "entities";
    for( const StringMap<char,SampleWorldDesc::EntityDesc>::KeyValuePair * i = desc.entities.first();
        i != NULL;
        i = desc.entities.next( i ) )
    {
        const StrA                        & entityName = *entityNameMap.find(i->key);
        const SampleWorldDesc::EntityDesc & entityDesc = i->value;

        XmlElement * entity = xmldoc.createElement( entities );
        entity->name = "entity";

        XmlAttrib * a = xmldoc.createAttrib( entity );
        a->name  = "name";
        a->value = entityName;

        XmlElement * spatial = xmldoc.createElement( entity );
        spatial->name = "spatial";

        a = xmldoc.createAttrib( spatial );
        a->name  = "parent";
        StrA * pParentEntityName = entityNameMap.find(entityDesc.spatial.parent);
        if( NULL != pParentEntityName )
        {
            a->value = *pParentEntityName;
        }
        else if( !entityDesc.spatial.parent.empty() )
        {
            GN_WARN(sLogger)( "Entity %s has invalid parent: %s", i->key, entityDesc.spatial.parent.cptr() );
        }

        a = xmldoc.createAttrib( spatial );
        a->name  = "position";
        a->value = stringFormat( "%f,%f,%f",
            entityDesc.spatial.position.x,
            entityDesc.spatial.position.y,
            entityDesc.spatial.position.z );

        a = xmldoc.createAttrib( spatial );
        a->name  = "orientation";
        a->value = stringFormat( "%f,%f,%f,%f",
            entityDesc.spatial.orientation.v.x,
            entityDesc.spatial.orientation.v.y,
            entityDesc.spatial.orientation.v.x,
            entityDesc.spatial.orientation.w );

        a = xmldoc.createAttrib( spatial );
        a->name  = "bbox";
        a->value = stringFormat( "%f,%f,%f,%f,%f,%f",
            entityDesc.spatial.bbox.x,
            entityDesc.spatial.bbox.y,
            entityDesc.spatial.bbox.z,
            entityDesc.spatial.bbox.w,
            entityDesc.spatial.bbox.h,
            entityDesc.spatial.bbox.d );

        XmlElement * visual = xmldoc.createElement( entity );
        visual->name  = "visual";
        for( size_t i = 0; i < entityDesc.models.size(); ++i )
        {
            XmlElement * modelref = xmldoc.createElement( visual );
            modelref->name = "model";

            XmlAttrib * a = xmldoc.createAttrib( modelref );
            a->name = "ref";
            a->value = entityDesc.models[i];
        }
    }

    // write scene bounding box
    XmlAttrib * a = xmldoc.createAttrib( root->toElement() );
    a->name  = "bbox";
    a->value = stringFormat( "%f,%f,%f,%f,%f,%f",
            desc.bbox.x,
            desc.bbox.y,
            desc.bbox.z,
            desc.bbox.w,
            desc.bbox.h,
            desc.bbox.d );

    // write XML document
    AutoObjPtr<File> fp( fs::openFile( filename, "wt" ) );
    if( !fp ) return false;
    return xmldoc.writeToFile( *fp, *root, false );
}

// *****************************************************************************
// Load from garnet mesh binary
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool sLoadFromMeshBinary( SampleWorldDesc & desc, File & fp )
{
    desc.clear();

    const StrA & meshname = fp.name();

    MeshResourceDesc mesh;
    AutoRef<Blob> blob = mesh.loadFromFile( fp );
    if( !blob ) return false;

    // determine the model template
    const ModelResourceDesc * modelTemplate = sDetermineBestModelTemplate( mesh );
    if( NULL == modelTemplate ) return false;

    // initialize the model descriptor based on the template
    ModelResourceDesc model = *modelTemplate;
    model.mesh = meshname;

    // add mesh and model to scene
    desc.meshes[meshname] = mesh;
    desc.meshdata.append( blob );
    desc.models[meshname] = model;

    // create a entity for the model
    SampleWorldDesc::EntityDesc & ed = desc.entities[meshname];
    ed.spatial.position.set( 0, 0, 0 );
    ed.spatial.orientation.set( 0, 0, 0, 1 );
    mesh.calculateBoundingBox( ed.spatial.bbox );
    ed.models.append( meshname );

    // done
    desc.bbox = ed.spatial.bbox;
    return true;
}

// *****************************************************************************
// Common local functions
// *****************************************************************************

class SampleSpacialEntity : public engine::Entity
{
    engine::SpacialComponent * mComp;

public:

    SampleSpacialEntity() : mComp( new engine::SpacialComponent() )
    {
        this->setComponent( mComp );
    }

    ~SampleSpacialEntity()
    {
        safeDecref( mComp );
    }

    engine::SpacialComponent * spacial() const { return mComp; }
};

class SampleVisualEntity : public SampleSpacialEntity
{
    engine::VisualComponent * mComp;

public:

    SampleVisualEntity() : mComp( new engine::VisualComponent() )
    {
        this->setComponent( mComp );
    }

    ~SampleVisualEntity()
    {
        safeDecref( mComp );
    }

    engine::VisualComponent * visual() const { return mComp; }
};

//
//
// -----------------------------------------------------------------------------
static Entity * sCreateEntity(
    Entity                             * root,
    const SampleWorldDesc              & worldDesc,
    StringMap<char,Entity*>            & entities,
    const StrA                         & entityName,
    const SampleWorldDesc::EntityDesc  & entityDesc )
{
    // recursively populate parent entities
    SampleSpacialEntity * parent = NULL;
    if( !entityDesc.spatial.parent.empty() )
    {
        const SampleWorldDesc::EntityDesc * parentDesc = worldDesc.entities.find( entityDesc.spatial.parent );
        if( NULL == parentDesc )
        {
            GN_ERROR(sLogger)( "Entity '%s' has a invalid parent: '%s'", entityName.cptr(), entityDesc.spatial.parent.cptr() );
        }
        else
        {

            parent = (SampleSpacialEntity*)sCreateEntity( root, worldDesc, entities, entityDesc.spatial.parent, *parentDesc );
            if( NULL == parent ) return NULL;
        }
    }

    // check if the entity has been initialized;
    Entity ** ppe = entities.find( entityName );
    if( ppe ) return *ppe;

    // create a new entity instance
    SampleSpacialEntity * e = entityDesc.models.empty() ? new SampleSpacialEntity() : new SampleVisualEntity();
    if( !e )
    {
        GN_ERROR(sLogger)( "Out of memory." );
        return NULL;
    }
    entities[entityName] = e;
    GN_ASSERT( (Entity*)e == *entities.find( entityName ) );

    // attach the entity to parent node or root node
    e->spacial()->setParent( parent ? parent->spacial() : ((SampleSpacialEntity*)root)->spacial() );

    // calculate bounding sphere
    e->spacial()->setSelfBoundingBox( entityDesc.spatial.bbox );

    for( size_t i = 0; i < entityDesc.models.size(); ++i )
    {
        const StrA & modelName = entityDesc.models[i];

        const GN::gfx::ModelResourceDesc * pModelDesc = worldDesc.models.find( modelName );
        if( NULL == pModelDesc )
        {
            GN_ERROR(sLogger)(
                "Entity %s references invalid model named \"%s\".",
                entityName.cptr(),
                modelName.cptr() );
            continue;
        }

        // this variable is used to keep a reference to mesh resource,
        // to prevent it from being deleted, until the model is created.
        AutoRef<MeshResource> mesh;

        GpuResourceDatabase * gdb = engine::getGdb();
        if( !pModelDesc->mesh.empty() && gdb )
        {
            mesh = gdb->findResource<MeshResource>( pModelDesc->mesh );
            if( !mesh )
            {

                const GN::gfx::MeshResourceDesc * pMeshDesc = worldDesc.meshes.find(pModelDesc->mesh);

                if( NULL == pMeshDesc )
                {
                    GN_ERROR(sLogger)(
                        "Model \"%s\" references a mesh \"%s\" that does not belong to this scene.",
                        modelName.cptr(),
                        pModelDesc->mesh.cptr() );
                    continue; // ignore the model
                }

                // create new mesh
                mesh = gdb->createResource<MeshResource>( pModelDesc->mesh );
                if( !mesh || !mesh->reset( pMeshDesc ) ) continue;
            }
        }

        AutoRef<ModelResource> model = gdb->createResource<ModelResource>( NULL );
        if( !model->reset( pModelDesc ) ) continue;

        ((SampleVisualEntity*)e)->visual()->addModel( model );
    }

    return e;
}

//
// Check if the string is end with specific suffix
// -----------------------------------------------------------------------------
static bool sStrEndWithI( const char * string, const char * suffix )
{
    if( NULL == suffix ) return true;
    if( NULL == string ) return false;

    size_t n1 = strlen( string );
    size_t n2 = strlen( suffix );

    if( n1 < n2 ) return false;

    string = string + n1 - n2;

    return 0 == stringCompareI( string, suffix );
}

// *****************************************************************************
// SampleWorldDesc
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::util::SampleWorldDesc::clear()
{
    meshes.clear();
    meshdata.clear();
    models.clear();
    entities.clear();
}

//
//
// -----------------------------------------------------------------------------
bool GN::util::SampleWorldDesc::loadFromFile( const char * filename )
{
    GN_SCOPE_PROFILER( loadWorldFromFile, "Load simple world from file" );

    GN_INFO(sLogger)( "Load scene from file: %s", filename?filename:"<NULL>" );

    clear();

    // open file
    AutoObjPtr<File> fp( fs::openFile( filename, "rb" ) );
    if( !fp ) return false;

    // get file extension
    StrA ext = fs::extName( filename );

    // do loading
    if( sStrEndWithI( filename, ".xml" ) )
    {
        return sLoadFromXML( *this, *fp );
    }
    else if( sStrEndWithI( filename, ".ase" ) )
    {
        return sLoadWorldFromASE( *this, *fp );
    }
    else if( sStrEndWithI( filename, ".xpr" ) ||
             sStrEndWithI( filename, ".tpr" ) )
    {
        return sLoadWorldFromXPR( *this, *fp );
    }
    else if( sStrEndWithI( filename, ".mesh.bin" ) )
    {
        return sLoadFromMeshBinary( *this, *fp );
    }
    else
    {
        GN_ERROR(sLogger)( "Unknown file extension: %s", ext.cptr() );
        return false;
    }
}

///
/// write world description to file
// -----------------------------------------------------------------------------
bool GN::util::SampleWorldDesc::saveToFile( const char * filename )
{
    GN_SCOPE_PROFILER( profiler, "Save simple world to file" );

    GN_INFO(sLogger)( "Write scene to : %s", filename );

    return sSaveToXML( *this, filename );
}

// *****************************************************************************
// SampleWorld
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::util::SampleWorld::SampleWorld()
    : mRoot(NULL)
    , mShowBBox(false)
{
}

//
//
// -----------------------------------------------------------------------------
GN::util::SampleWorld::~SampleWorld()
{
    clear();
}

//
//
// -----------------------------------------------------------------------------
void GN::util::SampleWorld::clear()
{
    for( const StringMap<char,Entity*>::KeyValuePair * i = mEntities.first();
         i != NULL;
         i = mEntities.next( i ) )
    {
        delete i->value;
    }
    mEntities.clear();
    safeDelete( mRoot );
}

//
//
// -----------------------------------------------------------------------------
bool GN::util::SampleWorld::createEntites( const SampleWorldDesc & desc )
{
    // clear old data.
    clear();

    // create root spatial entity of the whole world
    mRoot = new SampleSpacialEntity();
    if( NULL == mRoot ) return NULL;

    for( const StringMap<char,SampleWorldDesc::EntityDesc>::KeyValuePair * i = desc.entities.first();
         i != NULL;
         i = desc.entities.next( i ) )
    {
        const StrA & entityName = i->key;
        const SampleWorldDesc::EntityDesc & entityDesc = i->value;

        if( NULL == sCreateEntity( mRoot, desc, mEntities, entityName, entityDesc ) )
        {
            // Failed.
            clear();
            return false;
        }
    }

    return true;
}

//
// Draw all entities in the world.
// -----------------------------------------------------------------------------
void GN::util::SampleWorld::draw( const Matrix44f & proj, const Matrix44f & view ) const
{
    engine::getStandardUniformManager()->setTransform( proj, view );

    LineRenderer * lr = engine::getLineRenderer();

    for( const StringMap<char,Entity*>::KeyValuePair * i = mEntities.first();
         i != NULL;
         i = mEntities.next( i ) )
    {
        Entity * e = i->value;

        VisualComponent * visual = e->getComponent<VisualComponent>();

        if( visual )
        {
            visual->draw();
        }

        // draw each entitie's bounding box
        SpacialComponent * spacial = e->getComponent<SpacialComponent>();
        if( mShowBBox && NULL != spacial )
        {
            const Boxf & bbox = spacial->getSelfBoundingBox();
            lr->drawBox( bbox, 0xFFFF00FF, proj * view * spacial->getLocal2Root() );
        }
    }

    // draw overall bounding box in red
    SpacialComponent * spacial = mRoot->getComponent<SpacialComponent>();
    if( mShowBBox && NULL != spacial )
    {
        const Boxf & bbox = spacial->getUberBoundingBox();
        lr->drawBox( bbox, 0xFF000000, proj * view * spacial->getLocal2Root() );
    }
}
