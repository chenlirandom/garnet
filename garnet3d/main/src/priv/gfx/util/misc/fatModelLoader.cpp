#include "pch.h"
#include "ase.h"
#include <garnet/gfx/fatModel.h>

#if GN_MSVC
#pragma warning(disable:4100) // unreferenced formal parameter
#endif
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>       // Output data structure
#include <assimp/aiPostProcess.h> // Post processing flags
#include <assimp/IOStream.h>
#include <assimp/IOSystem.h>

#ifdef HAS_FBX
# if GN_GCC
#  pragma GCC diagnostic ignored "-Wunused"
# endif
#include <fbxsdk.h>
#include <fbxfilesdk/kfbxio/kfbximporter.h>
#include <fbxfilesdk/kfbxplugins/kfbxgeometryconverter.h>
#include <fbxfilesdk/fbxfilesdk_nsuse.h>
#endif

using namespace GN;
using namespace GN::gfx;

#define ROOT_BBOX 1

static GN::Logger * sLogger = GN::getLogger("GN.gfx.FatModel");

// *****************************************************************************
// ASE loader
// *****************************************************************************

namespace ase
{

//
//
// -----------------------------------------------------------------------------
template<typename SRC_VERTEX_TYPE>
static void sCopyVertexElement( void * dst, const MeshResourceDesc & src, const MeshVertexElement & e )
{
    Vector4f * d = (Vector4f*)dst;
    const uint8 * s = (const uint8*)src.vertices[e.stream] + src.offsets[e.stream] + e.offset;
    for( size_t i = 0; i < src.numvtx; ++i )
    {
        if( IsSameType<SRC_VERTEX_TYPE, Vector3f>::value )
        {
            d->set( *(const Vector3f*)s, 0.0f );
        }
        else if( IsSameType<SRC_VERTEX_TYPE, Vector2f>::value )
        {
            d->set( *(const Vector2f*)s, 0.0f, 0.0f );
        }
        s += src.strides[e.stream];
        ++d;
    }
}

//
//
// -----------------------------------------------------------------------------
static bool
sLoadFromASE( FatModel & fatmodel, File & file, const StrA & filename )
{
    // load ASE scene
    AseScene ase;
    if( !ase.loadFromFile(file) ) return false;

    fatmodel.name = filename;

    // copy materials
    fatmodel.materials.resize( ase.materials.size() );
    for( size_t i = 0; i < ase.materials.size(); ++i )
    {
        const AseMaterial & src = ase.materials[i];

        FatMaterial & dst = fatmodel.materials[i];

        dst.clear();

        dst.name = src.name;
        dst.albedoTexture = src.mapdiff.bitmap;
        dst.normalTexture = src.mapbump.bitmap;
        dst.albedoColor.set( src.diffuse, 1.0f );
    }

    // copy meshes
    fatmodel.meshes.resize( ase.meshes.size() );
    for( size_t i = 0; i < ase.meshes.size(); ++i )
    {
        fatmodel.meshes[i] = NULL;

        const AseMesh & src = ase.meshes[i];
        AutoObjPtr<FatMesh> dst( new FatMesh );

        // determine vertex format
        const MeshVertexElement * position = NULL;
        const MeshVertexElement * normal = NULL;
        const MeshVertexElement * texcoord = NULL;
        uint32 vtxfmt = 0;
        for( uint32 e = 0; e < src.vtxfmt.numElements; ++e )
        {
            const MeshVertexElement & mve = src.vtxfmt.elements[e];
            if( 0 == stringCompare( "POSITION", mve.semantic ) )
            {
                position = &mve;
                vtxfmt |= 1 << FatVertexBuffer::POSITION;
            }
            else if( 0 == stringCompare( "NORMAL", mve.semantic ) )
            {
                normal = &mve;
                vtxfmt |= 1 << FatVertexBuffer::NORMAL;
            }
            else if( 0 == stringCompare( "TEXCOORD", mve.semantic ) )
            {
                texcoord = &mve;
                vtxfmt |= 1 << FatVertexBuffer::TEXCOORD0;
            }
        }

        // copy vertex buffer.
        if( !dst->vertices.resize( vtxfmt, src.numvtx ) )
        {
            GN_ERROR(sLogger)( "Out of memory." );
            return false;
        }
        if( position )
        {
            sCopyVertexElement<Vector3f>( dst->vertices.getPosition(), src, *position );
            dst->vertices.setElementFormat( FatVertexBuffer::POSITION, ColorFormat::FLOAT3 );
        }
        if( normal )
        {
            sCopyVertexElement<Vector3f>( dst->vertices.getNormal(), src, *normal );
            dst->vertices.setElementFormat( FatVertexBuffer::NORMAL, ColorFormat::FLOAT3 );
        }
        if( texcoord )
        {
            sCopyVertexElement<Vector2f>( dst->vertices.getTexcoord(0), src, *texcoord );
            dst->vertices.setElementFormat( FatVertexBuffer::TEXCOORD0, ColorFormat::FLOAT2 );
        }

        // copy index buffer
        if( !dst->indices.resize( src.numidx ) )
        {
            GN_ERROR(sLogger)( "Out of memory." );
            return false;
        }
        if( src.idx32 )
        {
            memcpy( dst->indices.cptr(), src.indices, src.numidx * 4 );
        }
        else
        {
            const uint16 * s = (const uint16*)src.indices;
            uint32 * d = dst->indices.cptr();
            for( size_t i = 0; i < src.numidx; ++i, ++s, ++d )
            {
                *d = *s;
            }
        }

        dst->bbox = src.selfbbox;

        fatmodel.meshes[i] = dst.detach();
    }

    // copy subsets
    for( size_t i = 0; i < ase.subsets.size(); ++i )
    {
        const AseMeshSubset & src = ase.subsets[i];

        if( fatmodel.meshes[src.meshid] )
        {
            FatMeshSubset dst;
            dst.material = src.matid;
            dst.basevtx = src.basevtx;
            dst.numvtx = src.numvtx;
            dst.startidx = src.startidx;
            dst.numidx = src.numidx;
            fatmodel.meshes[src.meshid]->subsets.append( dst );
        }
    }

    // setup bounding box of the whole scene
    fatmodel.bbox = ase.bbox;

    return true;
}

}


// *****************************************************************************
// FBX loader
// *****************************************************************************

namespace fbx
{
#ifdef HAS_FBX

class FbxSdkWrapper
{
public:

    KFbxSdkManager * manager;
    KFbxGeometryConverter * converter;

    FbxSdkWrapper() : manager(NULL), converter(NULL)
    {

    }

    ~FbxSdkWrapper()
    {
        delete converter;
        converter = NULL;

        // Delete the FBX SDK manager. All the objects that have been allocated
        // using the FBX SDK manager and that haven't been explicitly destroyed
        // are automatically destroyed at the same time.
        if (manager) manager->Destroy();
        manager = NULL;
    }

    bool init()
    {
        manager = KFbxSdkManager::Create();
        if( NULL == manager ) return false;

    	// create an IOSettings object
    	KFbxIOSettings * ios = KFbxIOSettings::Create( manager, IOSROOT );
    	manager->SetIOSettings(ios);

        // create a converter
        converter = new KFbxGeometryConverter(manager);

    	// Load plugins from the executable directory
    	KString lPath = KFbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
    	KString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
    	KString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
    	KString lExtension = "so";
#endif
    	manager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

        return true;
    }

};

//
//
// -----------------------------------------------------------------------------
static const char * sGetTextureFileName( const KFbxSurfaceMaterial * material, const char * textureType )
{
    KFbxProperty prop = material->FindProperty( textureType );
    if( !prop.IsValid() ) return NULL;

    int lLayeredTextureCount = prop.GetSrcObjectCount(KFbxLayeredTexture::ClassId);
    if(lLayeredTextureCount > 0 )
    {
        // Layered texture

        for(int j=0; j<lLayeredTextureCount; ++j)
        {
            KFbxLayeredTexture *lLayeredTexture = KFbxCast <KFbxLayeredTexture>(prop.GetSrcObject(KFbxLayeredTexture::ClassId, j));

            int lNbTextures = lLayeredTexture->GetSrcObjectCount(KFbxTexture::ClassId);

            for(int k =0; k<lNbTextures; ++k)
            {
                KFbxTexture * lTexture = KFbxCast <KFbxTexture> (lLayeredTexture->GetSrcObject(KFbxTexture::ClassId,k));
                if(lTexture)
                {
                    return (const char *)lTexture->GetRelativeFileName();
                }
            }
        }
    }
    else
    {
        // Simple texture
        int lNbTextures = prop.GetSrcObjectCount(KFbxTexture::ClassId);
        for(int j =0; j<lNbTextures; ++j)
        {

            KFbxTexture* lTexture = KFbxCast <KFbxTexture>( prop.GetSrcObject(KFbxTexture::ClassId, j) );
            if(lTexture)
            {
                return (const char *)lTexture->GetRelativeFileName();
            }
        }
    }

    return NULL;
}

//
//
// -----------------------------------------------------------------------------
template<typename T>
static inline int sGetLayerElementIndex( const KFbxLayerElementTemplate<T> * elements, int index )
{
    if( NULL == elements ) return -1;

    KFbxLayerElement::EReferenceMode refmode = elements->GetReferenceMode();

    if( KFbxLayerElement::eDIRECT == refmode )
    {
        return index;
    }
    else if( KFbxLayerElement::eINDEX_TO_DIRECT == refmode )
    {
        return elements->GetIndexArray().GetAt(index);
    }
    else
    {
        GN_ERROR(sLogger)( "Unsupport reference mode: %d", refmode );
        return -1;
    }
}

//
//
// -----------------------------------------------------------------------------
template<typename T>
static inline int sGetLayerElementIndex(
    const KFbxLayerElementTemplate<T> * elements,
    int vertexIndex,
    int polygonIndex,
    int cornerIndex )
{
    if( NULL == elements ) return -1;

    KFbxLayerElement::EMappingMode mapmode = elements->GetMappingMode();

    if( KFbxLayerElement::eBY_CONTROL_POINT == mapmode )
    {
        return sGetLayerElementIndex( elements, vertexIndex );
    }
    else if( KFbxLayerElement::eBY_POLYGON_VERTEX == mapmode )
    {
        return sGetLayerElementIndex( elements, polygonIndex * 3 + cornerIndex );
    }
    else if( KFbxLayerElement::eBY_POLYGON == mapmode )
    {
        return sGetLayerElementIndex( elements, polygonIndex );
    }
    else
    {
        GN_ERROR(sLogger)( "Invalid layer mapping mode: %d", mapmode );
        return -1;
    }
}

struct SortPolygonByMaterial
{
    KFbxLayerElementMaterial * materials;

    SortPolygonByMaterial( KFbxLayerElementMaterial * m )
        : materials(m)
    {
    }

    bool operator()( const int & a, const int & b ) const
    {
        int ma = materials->GetIndexArray().GetAt(a);
        int mb = materials->GetIndexArray().GetAt(b);
        return ma < mb;
    }
};

struct MeshVertexCache
{
    struct Skin
    {
        int boneID;
        float weight;
    };

    Vector3f                  pos;
    DynaArray<Vector3f,uint8> normal;
    DynaArray<Vector2f,uint8> tc0;
    DynaArray<Skin,uint8>     skin;

    template<typename T>
    uint8 AddAttribute( DynaArray<T,uint8> & array, const T & value )
    {
        for( uint8 i = 0; i < array.size(); ++i )
        {
            if( array[i] == value ) return i;
        }
        array.append( value );
        return array.size() - 1;
    }

    static MeshVertexFormat sGetVertexFormat()
    {
        return MeshVertexFormat::XYZ_NORM_UV();
    }
};

struct MeshVertexKey
{
    int      pos;    //< Index of the the vertex in FBX mesh control point array.
    uint8    normal; //< Index into MeshVertexCache.normal.
    uint8    tc0;    //< Index into MeshVertexCache.texcoord0.
    uint8    skin;   //< Index into MeshVertexCache.skin.

    MeshVertexKey()
        : pos(-1)
        , normal((uint8)-1)
        , tc0((uint8)-1)
        , skin((uint8)-1)
    {
    }
};

typedef HashMap<
    MeshVertexKey,
    uint32,
    HashMapUtils::HashFunc_MemoryHash<MeshVertexKey>,
    HashMapUtils::EqualFunc_MemoryCompare<MeshVertexKey> > MeshVertexHashMap;

/*
//
// -----------------------------------------------------------------------------
static void sLoadFbxSkin(
    FbxSdkWrapper & sdk,
    KFbxSkin & skin )
{
    // Load bind pose
    int numClusters = skin.GetClusterCount();
    for( int i = 0; i < numClusters; ++i )
    {

    }
}//*/

//
//
// -----------------------------------------------------------------------------
static bool
sLoadFbxVertices(
    FatVertexBuffer       & fatvb,
    KFbxNode              * fbxnode,
    const MeshVertexCache * vertices,
    const MeshVertexKey   * keys,
    uint32                  numkeys )
{
    // Compute the node's global position.
    KFbxXMatrix globalTransform = fbxnode->GetScene()->GetEvaluator()->GetNodeGlobalTransform(fbxnode);

    // Geometry offset is not inherited by the children.
    KFbxXMatrix geometryOffset;
    KFbxVector4 lT, lR, lS;
    lT = fbxnode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
    lR = fbxnode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
    lS = fbxnode->GetGeometricScaling(KFbxNode::eSOURCE_SET);
    geometryOffset.SetT(lT);
    geometryOffset.SetR(lR);
    geometryOffset.SetS(lS);
    globalTransform = globalTransform * geometryOffset;

    // FBX matrix is column major.
    double * d44 = globalTransform;
    Matrix44f m44(
        (float)d44[0], (float)d44[4], (float)d44[8],  (float)d44[12],
        (float)d44[1], (float)d44[5], (float)d44[9],  (float)d44[13],
        (float)d44[2], (float)d44[6], (float)d44[10], (float)d44[14],
        (float)d44[3], (float)d44[7], (float)d44[11], (float)d44[15] );

    // This is used to transform normal vector.
    Matrix44f itm44 = Matrix44f::sInvtrans( m44 );

    if( !fatvb.resize( FatVertexBuffer::POS_NORMAL_TEX, numkeys ) ) return false;

    fatvb.setElementFormat( FatVertexBuffer::POSITION,  ColorFormat::FLOAT3 );
    fatvb.setElementFormat( FatVertexBuffer::NORMAL,    ColorFormat::FLOAT3 );
    fatvb.setElementFormat( FatVertexBuffer::TEXCOORD0, ColorFormat::FLOAT2 );

    Vector4f * pos = (Vector4f*)fatvb.getPosition();
    Vector4f * nml = (Vector4f*)fatvb.getNormal();
    Vector4f * uv0 = (Vector4f*)fatvb.getTexcoord(0);

    Vector4f v4;

    for( size_t i = 0; i < numkeys; ++i )
    {
        const MeshVertexKey   & k = keys[i];
        const MeshVertexCache & v = vertices[k.pos];

        // translate position to global space
        v4.set( v.pos, 1.0f );
        *pos = m44 * v4;
        float divw = 1.0f / pos->w;
        pos->x *= divw;
        pos->y *= divw;
        pos->z *= divw;
        pos->w  = 1.0f;

        // translate normal to global space.
        if( k.normal < v.normal.size() )
        {
            v4.set( v.normal[k.normal], 0.0f );
            *nml = itm44 * v4;
            nml->w = 0.0f;
            nml->normalize();
        }
        else
        {
            nml->set( 0, 0, 0, 0 );
        }

        if( k.tc0 < v.tc0.size() )
        {
            uv0->set( v.tc0[k.tc0], 0, 0 );
        }
        else
        {
            uv0->set( 0, 0, 0, 0 );
        }

        ++pos;
        ++nml;
        ++uv0;
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
static void
sLoadFbxMesh(
    FatModel      & fatmodel,
    const StrA    & filename,
    FbxSdkWrapper & sdk,
    KFbxNode      * fbxnode,
    KFbxMesh      * fbxmesh )
{
    if( !fbxmesh->IsTriangleMesh() )
    {
        fbxmesh = sdk.converter->TriangulateMesh( fbxmesh );
        if( NULL == fbxmesh )
        {
            GN_ERROR(sLogger)( "Fail to triangulate fbxmesh node: %s", fbxnode->GetName() );
            return;
        }
    }

    // For now, we supports layer 0 only.
    KFbxLayer * layer0 = fbxmesh->GetLayer(0);
    if( NULL == layer0 )
    {
        GN_ERROR(sLogger)( "The fbxmesh does not have a layer: %s", fbxnode->GetName() );
        return;
    }
    if( NULL == layer0->GetNormals() )
    {
        fbxmesh->ComputeVertexNormals();
    }

    // Get basic fbxmesh properties
    int                         * fbxIndices   = fbxmesh->GetPolygonVertices();
    const KFbxVector4           * fbxPositions = fbxmesh->GetControlPoints();
    KFbxLayerElementUV          * fbxUVs       = layer0->GetUVs();
    KFbxLayerElementNormal      * fbxNormals   = layer0->GetNormals();
    KFbxLayerElementMaterial    * fbxMaterials = layer0->GetMaterials();
    //KFbxLayerElementVertexColor * fbxColors    = layer0->GetVertexColors();
    //KFbxLayerElementTangent     * fbxTangents  = layer0->GetTangents();
    //KFbxLayerElementBinormal    * fbxBinormals = layer0->GetBinormals();
    int                           numtri       = fbxmesh->GetPolygonCount();
    int                           numidx       = numtri * 3;

    // How many materials are there?
    int nummat;
    if( fbxMaterials &&
        KFbxLayerElement::eBY_POLYGON == fbxMaterials->GetMappingMode() &&
        KFbxLayerElement::eINDEX_TO_DIRECT == fbxMaterials->GetReferenceMode() )
    {
        // multiple materials
        nummat = fbxnode->GetMaterialCount();
    }
    else
    {
        if( fbxMaterials && KFbxLayerElement::eALL_SAME != fbxMaterials->GetMappingMode() )
        {
            GN_WARN(sLogger)("Unsupported FBX material layer: mapping mode=%d, reference mode=%d. It will be treated as eALL_SAME.", fbxMaterials->GetMappingMode(), fbxMaterials->GetReferenceMode() );
        }

        // one material
        nummat = 1;
    }

    // add materials to fatmodel.
    DynaArray<size_t> fatmatIndices( nummat );
    for( int i = 0; i < nummat; ++i )
    {
        const KFbxSurfaceMaterial * fbxmat = fbxnode->GetMaterial( i );
        GN_ASSERT( fbxmat );

        FatMaterial fatmat;
        fatmat.name = fbxmat->GetName();

        StrA dirname = fs::dirName( filename );
        const char * texname = sGetTextureFileName( fbxmat, KFbxSurfaceMaterial::sDiffuse );
        if( texname ) fatmat.albedoTexture = fs::resolvePath( dirname, texname );

        texname = sGetTextureFileName( fbxmat, KFbxSurfaceMaterial::sNormalMap );
        if( texname ) fatmat.normalTexture = fs::resolvePath( dirname, texname );

        // TODO: get diffuse color.
        fatmat.albedoColor.set( 1, 1, 1, 1 );

        fatmodel.materials.append( fatmat );
        fatmatIndices[i] = fatmodel.materials.size() - 1;
    }

    // sort polygon by material
    DynaArray<int> sortedPolygons;
    if( !sortedPolygons.resize( numtri ) )
    {
        GN_ERROR(sLogger)( "Fail to load FBX mesh: out of memory." );
        return;
    }
    for( size_t i = 0; i < sortedPolygons.size(); ++i )
    {
        sortedPolygons[i] = (int)i;
    }
    if( nummat > 1 )
    {
        std::sort(
            sortedPolygons.begin(),
            sortedPolygons.end(),
            SortPolygonByMaterial( fbxMaterials ) );
    }

    // Create the mesh object
    AutoObjPtr<FatMesh> fatMeshAutoPtr( new FatMesh );
    if( NULL == fatMeshAutoPtr )
    {
        GN_ERROR(sLogger)( "Fail to load FBX mesh: out of memory." );
        return;
    }
    FatMesh & fatmesh = *fatMeshAutoPtr;

    // Allocate index buffer
    if( !fatmesh.indices.resize( (uint32)numidx ) )
    {
        GN_ERROR(sLogger)( "Fail to load FBX mesh: out of memory." );
        return;
    }

    // Declare vertex cache array
    DynaArray<MeshVertexCache,uint32> vcache;
    int numpos = fbxmesh->GetControlPointsCount();
    if( !vcache.resize( (uint32)numpos ) )
    {
        GN_ERROR(sLogger)( "Fail to load FBX mesh: out of memory." );
        return;
    }
    for( int i = 0; i < numpos; ++i )
    {
        const KFbxVector4 & fbxpos = fbxPositions[i];
        vcache[i].pos.set( (float)fbxpos[0], (float)fbxpos[1], (float)fbxpos[2] );
    }

    // Declare the vertex hash table
    MeshVertexHashMap vhash( (size_t)numidx * 2 );

    // Allocate another buffer to hold the final sequance of vertex keys
    DynaArray<MeshVertexKey> vertexKeys;
    if( !vertexKeys.reserve( (uint32)numidx ) )
    {
        GN_ERROR(sLogger)( "Fail to load FBX mesh: out of memory." );
        return;
    }

    // Split the FBX fbxmesh into multiple models, one material one model.
    int uvIndex = 0;
    int normalIndex = 0;
    int lastMatID = -1;
    FatMeshSubset * lastSubset = NULL;
    for( uint32 sortedPolygonIndex = 0; sortedPolygonIndex < sortedPolygons.size(); ++sortedPolygonIndex )
    {
        int polygonIndex = sortedPolygons[sortedPolygonIndex];

        int matid = nummat > 1 ? fbxMaterials->GetIndexArray().GetAt(polygonIndex) : 0;
        GN_ASSERT( matid < nummat );
        GN_ASSERT( matid >= lastMatID );

        // create new subset for each new material
        if( matid != lastMatID )
        {
            lastMatID = matid;

            // create a new subset
            if( !fatmesh.subsets.resize( fatmesh.subsets.size() + 1 ) )
            {
                GN_ERROR(sLogger)( "Fail to load FBX mesh: out of memory." );
                return;
            }
            FatMeshSubset & subset = fatmesh.subsets.back();
            subset.material = fatmatIndices[matid];
            subset.startidx = sortedPolygonIndex*3;
            subset.numidx   = 0;
            subset.basevtx  = 0;
            // Note: final number of vertices are unknown yet.

            lastSubset = &subset;
        }
        lastSubset->numidx += 3;

        // add the polygon to the mesh
        for( int i = 0; i < 3; ++i )
        {
            int posIndex = fbxIndices[polygonIndex*3+i];

            MeshVertexCache & vc = vcache[posIndex];

            // create vetex key
            MeshVertexKey key;
            key.pos = posIndex;

            // get normal
            if( -1 != normalIndex )
            {
                normalIndex = sGetLayerElementIndex( fbxNormals, posIndex, polygonIndex, i );

                if( -1 != normalIndex )
                {
                    const KFbxVector4 & fbxnormal = fbxNormals->GetDirectArray().GetAt(normalIndex);
                    Vector3f normal( (float)fbxnormal[0], (float)fbxnormal[1], (float)fbxnormal[2] );
                    key.normal = vc.AddAttribute( vc.normal, normal );
                }
            }

            // get texcoord0
            if( -1 != uvIndex )
            {
                uvIndex = sGetLayerElementIndex( fbxUVs, posIndex, polygonIndex, i );

                if( -1 != uvIndex )
                {
                    const KFbxVector2 & fbxuv = fbxUVs->GetDirectArray().GetAt(uvIndex);
                    // BUGBUG: for some reason, U coordinates has to be inverted (1.0-v) to make the model look right in the viewer.
                    Vector2f tc0( (float)fbxuv[0], (float)(1.0-fbxuv[1]) );
                    key.tc0 = vc.AddAttribute( vc.tc0, tc0 );
                }
            }

            // If the key exists already, the pair will point to it.
            // If the key does not exisit, the pair will point to the newly inserted one.
            // Either way, pair->value should give us the correct index of the vertex.
            MeshVertexHashMap::KeyValuePair * pair;
            bool isNewVertex = vhash.insert( key, (uint32)vhash.size(), &pair );
            uint32 vertexIndex = pair->value;

            if( isNewVertex )
            {
                // If it is a new vertex, append it to the vertex key array.
                vertexKeys.append( key );

                GN_ASSERT( vertexKeys.size() == (vertexIndex + 1) );
            }

            // add the vertex index into the index buffer
            fatmesh.indices[sortedPolygonIndex*3+i] = vertexIndex;
        }
    }

    // Now both vertex and index buffers are filled up. Models' subset information are ready too.
    // We are almost there.

    // Fill up the rest of informations for each subset
    for( size_t i = 0; i < fatmesh.subsets.size(); ++i )
    {
        fatmesh.subsets[i].numvtx = (uint32)vertexKeys.size();
    }

    // Now copy vertex data to fatmesh, and translate position and normal to global space.
    if( !sLoadFbxVertices( fatmesh.vertices, fbxnode, vcache.cptr(), vertexKeys.cptr(), vertexKeys.size() ) ) return;

    // calculate the bounding box of the mesh
    const Vector4f * vertices = (const Vector4f *)fatmesh.vertices.getPosition();
    calculateBoundingBox(
        fatmesh.bbox,
        &vertices->x, sizeof(vertices[0]),
        &vertices->y, sizeof(vertices[0]),
        &vertices->z, sizeof(vertices[0]),
        fatmesh.vertices.getVertexCount() );

    GN_INFO(sLogger)( "Load FBX mesh %s: %d vertices, %d faces",
        fbxnode->GetName(),
        fatmesh.vertices.getVertexCount(),
        fatmesh.indices.size() );

    // finally, add the fatmesh to fatmodel. And we are done!
    fatmodel.meshes.append( fatMeshAutoPtr.detach() );
}

#define RECURSIVE_LOAD 0
#if RECURSIVE_LOAD
//
//
// -----------------------------------------------------------------------------
static bool
sLoadFbxNodeRecursivly(
    FatModel           & fatmodel,
    const StrA         & filename,
    FbxSdkWrapper      & sdk,
    KFbxNode           * node,
    KFbxNode           * parent )
{
    if( NULL == node ) return true;

    // Get node type
    KFbxNodeAttribute* attrib = node->GetNodeAttribute();
    KFbxNodeAttribute::EAttributeType type = attrib ? attrib->GetAttributeType() : KFbxNodeAttribute::eUNIDENTIFIED;

    if( KFbxNodeAttribute::eMESH == type )
    {
        // load mesh node
        sLoadFbxMesh( fatmodel, filename, sdk, node, (KFbxMesh*)attrib );
    }
    else if(
        // Some nodes are ignored silently.
        KFbxNodeAttribute::eNULL != type &&
        KFbxNodeAttribute::eUNIDENTIFIED != type &&
        KFbxNodeAttribute::eLIGHT != type &&
        KFbxNodeAttribute::eCAMERA != type &&
        KFbxNodeAttribute::eSKELETON != type )
    {
        GN_WARN(sLogger)( "Ignore unsupported node: type=%d, name=%s", type, node->GetName() );
    }

    // load children
    for( int i = 0; i < node->GetChildCount(); ++i )
    {
        if( !sLoadFbxNodeRecursivly( fatmodel, filename, sdk, node->GetChild( i ), node ) )
        {
            return false;
        }
    }

    // done
    return true;
}

#endif // RECURSIVE_LOAD

#endif // HAS_FBX

//
//
// -----------------------------------------------------------------------------
static bool
sLoadFromFBX( FatModel & fatmodel, File & file, const StrA & filename )
{
#ifdef HAS_FBX

    FbxSdkWrapper sdk;
    if( !sdk.init() ) return false;
    KFbxSdkManager * gSdkManager = sdk.manager;

    // TODO: setup file system.

    // detect file format
	int lFileFormat = -1;
    if (!gSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat) )
    {
        // Unrecognizable file format. Try to fall back to KFbxImporter::eFBX_BINARY
        lFileFormat = gSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
    }

    // Create the importer.
    KFbxImporter* gImporter = KFbxImporter::Create(gSdkManager,"");
    if( NULL == gImporter ) return false;
    if(!gImporter->Initialize(filename, lFileFormat))
    {
        GN_ERROR(sLogger)( gImporter->GetLastErrorString() );
        return false;
    }

    // Import the scene
    KFbxScene * gScene = KFbxScene::Create( gSdkManager, "" );
    if( NULL == gScene ) return false;
    if(!gImporter->Import(gScene))
    {
        GN_ERROR(sLogger)( gImporter->GetLastErrorString() );
        return false;
    }

    // Convert Axis System to right handed system, if needed
    KFbxAxisSystem SceneAxisSystem = gScene->GetGlobalSettings().GetAxisSystem();
    KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::RightHanded);
    if( SceneAxisSystem != OurAxisSystem )
    {
        OurAxisSystem.ConvertScene(gScene);
    }

    // Convert Unit System to 1.0, if needed
    KFbxSystemUnit SceneSystemUnit = gScene->GetGlobalSettings().GetSystemUnit();
    if( SceneSystemUnit.GetScaleFactor() != 1.0 )
    {
        KFbxSystemUnit OurSystemUnit(1.0);
        OurSystemUnit.ConvertScene(gScene);
    }

    // preallocate material array.
    int nummat = KFbxGetSrcCount<KFbxSurfaceMaterial>(gScene);
    fatmodel.materials.reserve( (size_t)nummat );

    //fatmodel.materials.resize( 1 );
    //fatmodel.materials[0].name = "=[DEFAULT]=";
    //fatmodel.materials[0].albedoTexture = "";
    //fatmodel.materials[0].normalTexture = "";
    //fatmodel.materials[0].albedoColor.set( 1, 1, 1, 1 );

#if RECURSIVE_LOAD

    if( !sLoadFbxNodeRecursivly( fatmodel, filename, sdk, gScene->GetRootNode(), NULL ) ) return false;

#else

    // Cycle through nodes in linear way.
    int lNodeCount = KFbxGetSrcCount<KFbxNode>(gScene);
    for( int lIndex=0; lIndex<lNodeCount; lIndex++ )
    {
        KFbxNode * node = KFbxGetSrc<KFbxNode>(gScene, lIndex);

        // Get node type
        KFbxNodeAttribute* attrib = node->GetNodeAttribute();
        KFbxNodeAttribute::EAttributeType type = attrib ? attrib->GetAttributeType() : KFbxNodeAttribute::eUNIDENTIFIED;

        if( KFbxNodeAttribute::eMESH == type )
        {
            // load mesh node
            sLoadFbxMesh( fatmodel, filename, sdk, node, (KFbxMesh*)attrib );
        }
        else if(
            // Some nodes are ignored silently.
            KFbxNodeAttribute::eNULL != type &&
            KFbxNodeAttribute::eUNIDENTIFIED != type &&
            KFbxNodeAttribute::eLIGHT != type &&
            KFbxNodeAttribute::eCAMERA != type &&
            KFbxNodeAttribute::eSKELETON != type )
        {
            GN_WARN(sLogger)( "Ignore unsupported node: type=%d, name=%s", type, node->GetName() );
        }
    }

#endif

    // calculate the final bounding box
    fatmodel.bbox.clear();
    for( size_t i = 0; i < fatmodel.meshes.size(); ++i )
    {
        GN_ASSERT( fatmodel.meshes[i] );
        Boxf::sGetUnion( fatmodel.bbox, fatmodel.bbox, fatmodel.meshes[i]->bbox );
    }

    return true;

#else // HAS_FBX

    fatmodel.clear();
    GN_UNUSED_PARAM( file );
    GN_ERROR(sLogger)( "Fail to load file %s: FBX is not supported.", filename.cptr() );
    return false;

#endif // HAS_FBX
}

}
// *****************************************************************************
// XML loader
// *****************************************************************************

// *****************************************************************************
// BIN loader
// *****************************************************************************

// *****************************************************************************
// FatModel
// *****************************************************************************

enum FileFormat
{
    FF_UNKNOWN = 0,
    FF_FBX,
    FF_ASE,
    FF_GARNET_XML,
    FF_GARNET_BIN,
};

//
// Check if the string is end with specific suffix
// -----------------------------------------------------------------------------
static bool sCheckFileExtension( const char * filename, const char * extension )
{
    if( NULL == extension ) return true;
    if( NULL == filename ) return false;

    size_t n1 = strlen( filename );
    size_t n2 = strlen( extension );

    if( n1 < n2 ) return false;

    filename = filename + n1 - n2;

    return 0 == stringCompareI( filename, extension );
}

//
//
// -----------------------------------------------------------------------------
static FileFormat sDetermineFileFormatByContent( File & file )
{
    // not implemented yet.
    return FF_UNKNOWN;
}

//
//
// -----------------------------------------------------------------------------
static FileFormat sDetermineFileFormatByFileName( const StrA & filename )
{
    if( sCheckFileExtension( filename, ".xml" ) )
    {
        return FF_GARNET_XML;
    }
    else if( sCheckFileExtension( filename, ".mesh.bin" ) )
    {
        return FF_GARNET_BIN;
    }
    else if( sCheckFileExtension( filename, ".ase" ) )
    {
        return FF_ASE;
    }
    else if( sCheckFileExtension( filename, ".fbx" ) )
    {
        return FF_FBX;
    }
    else
    {
        return FF_UNKNOWN;
    }
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::FatModel::loadFromFile( const StrA & filename )
{
    clear();

    // Open the file.
    AutoObjPtr<File> file( fs::openFile( filename, "rb" ) );
    if( NULL == file ) return false;

    // determine file format
    FileFormat ff = sDetermineFileFormatByContent( *file );
    if( FF_UNKNOWN == ff )
    {
        ff = sDetermineFileFormatByFileName( filename );
    }

    StrA fullFileName = fs::resolvePath( fs::getCurrentDir(), filename );

    bool noerr;
    switch( ff )
    {
        case FF_ASE:
            noerr = ase::sLoadFromASE( *this, *file, filename );
            break;

        case FF_FBX:
            noerr = fbx::sLoadFromFBX( *this, *file, filename );
            break;

        case FF_GARNET_XML:
            noerr = false;
            break;

        case FF_GARNET_BIN:
            noerr = false;
            break;

        default:
            GN_ERROR(sLogger)( "Unknown file format: %s", filename.cptr() );
            noerr = false;
            break;
    }

    if( noerr )
    {
        size_t totalVerts = 0;
        size_t totalFaces = 0;
        for( size_t i = 0; i < this->meshes.size(); ++i )
        {
            const FatMesh * m = this->meshes[i];
            if( m )
            {
                totalVerts += m->vertices.getVertexCount();
                totalFaces += m->indices.size() / 3;
            }
        }
        GN_INFO(sLogger)( "Total vertices: %d, faces: %d", totalVerts, totalFaces );
    }
    else
    {
        clear();
    }

    return noerr;
}
