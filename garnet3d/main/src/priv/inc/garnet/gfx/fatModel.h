#ifndef __GN_GFX_FATMODEL_H__
#define __GN_GFX_FATMODEL_H__
// *****************************************************************************
/// \file
/// \brief   A universal model class
/// \author  chen@@chenli-homepc (2011.3.22)
// *****************************************************************************

#include "garnet/GNgfx.h"

namespace GN { namespace gfx
{
    class FatVertexBuffer
    {
    public:

        enum Semantic
        {
            POSITION = 0,
            NORMAL,
            TANGENT,
            BINORMAL,
            TEXCOORD0,
            TEXCOORD1,
            TEXCOORD2,
            TEXCOORD3,
            TEXCOORD4,
            TEXCOORD5,
            TEXCOORD6,
            TEXCOORD7,
            TEXCOORD_LAST = TEXCOORD7,
            ALBEDO,
            JOINT_ID,
            JOINT_WEIGHT,
            CUSTOM0,
            CUSTOM1,
            CUSTOM2,
            CUSTOM3,
            CUSTOM4,
            CUSTOM5,
            CUSTOM6,
            CUSTOM7,
            NUM_SEMANTICS,
            INVALID = 0xFFFFFFFF,
        };

        static const uint32 MAX_TEXCOORDS = (uint32)(TEXCOORD_LAST - TEXCOORD0);

        static const uint32 POS_NORMAL_TEX = (1<<POSITION) | (1<<NORMAL) | (1<<TEXCOORD0);

        static const uint32 POS_NORMAL_TEX_SKINNING = (1<<POSITION) | (1<<NORMAL) | (1<<TEXCOORD0) | (1<<JOINT_ID) | (1<<JOINT_WEIGHT);

        static const char * const SEMANTIC_NAMES[];

        static Semantic sString2Semantic( const char * s );

        // Each element is 16 bytes;
        static const size_t ELEMENT_SIZE = 16;

        FatVertexBuffer()
            : mCount(0)
            , mLayout(0)
        {
            memset( &mElements, 0, sizeof(mElements) );
            memset( &mFormats, 0, sizeof(mFormats) );
        }

        FatVertexBuffer( const FatVertexBuffer & fvb )
            : mCount(0)
            , mLayout(0)
        {
            memset( &mElements, 0, sizeof(mElements) );
            memset( &mFormats, 0, sizeof(mFormats) );

            copyFrom( fvb );
        }

        ~FatVertexBuffer()
        {
            clear();
        }

        FatVertexBuffer & operator=( const FatVertexBuffer & rhs )
        {
            copyFrom( rhs );
            return *this;
        }

        void clear()
        {
            for( int i = 0; i < NUM_SEMANTICS; ++i )
            {
                if( mElements[i] )
                {
                    HeapMemory::dealloc( mElements[i] );
                    mElements[i] = NULL;
                }
            }
            mCount = 0;
        }

        /// If resizing fails, the function returns false. And the mesh content remains unchanged.
        bool resize( uint32 layout, uint32 count );

        uint32 getLayout() const { return mLayout; }

        uint32 getVertexCount() const { return mCount; }

        /// No matter what the format is, size of each unit in the element array is always ELEMENT_SIZE.
        void * getElementData( int semantic ) const
        {
            if( 0 <= semantic && semantic < NUM_SEMANTICS )
            {
                return mElements[semantic];
            }
            else
            {
                return NULL;
            }
        }

        /// Get format of the element.
        ColorFormat getElementFormat( int semantic ) const
        {
            if( 0 <= semantic && semantic < NUM_SEMANTICS )
            {
                return mFormats[semantic];
            }
            else
            {
                return ColorFormat::UNKNOWN;
            }
        }

        /// Set element format
        void setElementFormat( int semantic, ColorFormat format )
        {
            if( 0 <= semantic && semantic < NUM_SEMANTICS )
            {
                mFormats[semantic] = format;
            }
        }

        void * getPosition() const { return mElements[POSITION]; }
        void * getNormal() const { return mElements[NORMAL]; }
        void * getTexcoord( size_t stage ) const
        {
            size_t semantic = TEXCOORD0+stage;
            if( 0 <= semantic && semantic <= TEXCOORD_LAST )
            {
                return mElements[semantic];
            }
            else
            {
                return NULL;
            }
        }

        void GenerateMeshVertexFormat( MeshVertexFormat & mvf ) const;

        bool GenerateVertexStream(
                const MeshVertexFormat & mvf,
                size_t                   stream,
                size_t                   stride,
                void                   * buffer,
                size_t                   bufferSize ) const;

    private:

        void      * mElements[NUM_SEMANTICS];
        ColorFormat mFormats[NUM_SEMANTICS];
        uint32      mCount;
        uint32      mLayout;

        bool copyFrom( const FatVertexBuffer & other );
    };

    struct FatMeshSubset
    {
        uint32 material; //< index into FatModel.materials array.
        uint32 basevtx;
        uint32 numvtx;
        uint32 startidx;
        uint32 numidx;
    };

    struct FatMesh
    {
        /// Constant value indicating that the mesh has no skeleton.
        static const uint32 NO_SKELETON = (uint32)-1;

        FatVertexBuffer          vertices;
        DynaArray<uint32,uint32> indices;
        PrimitiveType            primitive;
        DynaArray<FatMeshSubset> subsets;
        uint32                   skeleton; ///< index into FatModel::skeletons, or NO_SKELETON if the mesh has no skeleton.
        Boxf                     bbox;
    };

    struct FatMaterial
    {
        StrA     name; //< Material name
        StrA     albedoTexture;
        StrA     normalTexture;
        Vector4f albedoColor;

        void clear()
        {
            name.clear();
            albedoTexture.clear();
            normalTexture.clear();
            albedoColor.set( 0, 0, 0, 1 );
        }
    };

    struct FatJoint
    {
        static const uint32 NO_JOINT = (uint32)-1;

        StrA               name;     //< Joint name (unique in a skeleton).

        // Hierarchy
        uint32             parent;   //< parent joint index. NO_JOINT, if the joint has no parent.
        uint32             child;    //< first child joint index. NO_JOINT, if the joint has no child.
        uint32             sibling;  //< next sibling joing index. NO_JOINT, if the joint has no next sibling.

        Matrix44f          bindPose; //< model space -> joint space transformation.
    };

    struct FatSkeleton
    {
        StrA                       name;   //< name of the skeleton.
        DynaArray<FatJoint,uint32> joints; //< Joint array.

        /// Print joint hierarchy to a string.
        void printJointHierarchy( StrA & ) const;
    };

    template<typename T>
    struct FatKeyFrame
    {
        double time;  //< Time stamp in seconds.
        T      value; //< The value at the specified time.
    };

    struct FatJointAnimation
    {
        uint32                               skeleton;  //< Index into FatModel::skeletons
        uint32                               joint;     //< Index into FatSkeleton::joints

        // Local->Parent transform = T * R * S;
        DynaArray<FatKeyFrame<Vector3f> >    positions; //< Position in parent space
        DynaArray<FatKeyFrame<Quaternionf> > rotations; //< Rotation in local space
        DynaArray<FatKeyFrame<Vector3f> >    scalings;  //< Scaling in local space.
    };

    struct FatAnimation
    {
        StrA                         name;            //< Name of the animation.
        double                       duration;        //< Duration of the animation.
        DynaArray<FatJointAnimation> jointAnimations; //< Joint animation array.
    };

    struct FatModel : public NoCopy
    {
        StrA                           name;       //< name of the model. Usually the filename which the model is loaded from.
        DynaArray<FatMesh*,uint32>     meshes;     //< Mesh array. Use FatMesh* to avoid expensive copy opertaion when the array is resized.
        DynaArray<FatMaterial,uint32>  materials;
        DynaArray<FatSkeleton,uint32>  skeletons;
        DynaArray<FatAnimation,uint32> animations;
        Boxf                           bbox;

        /// destructor
        ~FatModel()
        {
            clear();
        }

        /// clear the model
        void clear()
        {
            for( uint32 i = 0; i < meshes.size(); ++i )
            {
                delete meshes[i];
            }
            meshes.clear();
            materials.clear();
            skeletons.clear();
            animations.clear();
            bbox.clear();
        }

        // Calculate FatModel.bbox from mesh data.
        void calcBoundingBox();

        /// load fatmodel from file
        bool loadFromFile( const StrA & filename );

        /// save fatmodel to file.
        bool saveToFile( const StrA & filename ) const;
    };

    // Misc. ulitities
    //@{

    ///
    /// Print module file node hierarchy
    ///w
    void printModelFileNodeHierarchy( StrA & hierarchy, const StrA & filename );

    //@}
 }}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_FATMODEL_H__
