#ifndef __GN_UTIL_ASE_H__
#define __GN_UTIL_ASE_H__
// *****************************************************************************
/// \file
/// \brief   ASE loader
/// \author  chenli@@REDMOND (2009.1.16)
// *****************************************************************************

namespace GN { namespace util
{
    ///
    /// ASE map
    ///
    struct AseMap
    {
        StrA     name;
        StrA     class_;
        int      subno;
        float    amount;
        StrA     bitmap;
        StrA     type;
        Vector2f offset;
        Vector2f tiling;
        float    angle;
        float    blur;
        float    blur_offset;
        float    noise_amt;
        float    noise_size;
        int      noise_level;
        float    noise_phase;
        StrA     filter;
    };

    ///
    /// ASE material
    ///
    struct AseMaterial
    {
        StrA     name;
        StrA     class_;
        Vector3f ambient, diffuse, specular;
        float    shine;
        float    shinestrength;
        float    transparency;
        float    wiresize;
        StrA     shading;
        float    xp_falloff;
        float    selfillum;
        StrA     falloff;
        StrA     xp_type;
        AseMap   mapdiff;
        AseMap   mapbump;
    };

    struct AseMesh : public gfx::MeshResourceDesc
    {
        StrA      parent;    ///< parent mesh
        StrA      name;      ///< mesh name
        Matrix44f transform; ///< transformation
        Vector3f  pos;       ///< position
        Vector3f  rotaxis;   ///< rotate axis
        float     rotangle;  ///< rotation angle in radian
        Vector3f  scale;     ///< scale factors
        Boxf      selfbbox;  ///< bounding box of myself
        Boxf      treebbox;  ///< bounding box of myself and my children.
    };

    ///
    /// ase mesh subset
    ///
    struct AseMeshSubset : public gfx::MeshResourceSubset
    {
        size_t matid;  ///< index into AseScene::materials array
        size_t meshid; ///< index into AseScene::meshes array
    };

    ///
    /// ASE scene. Include multiple materials and meshes, no animation.
    ///
    struct AseScene : public NoCopy
    {
        DynaArray<AseMaterial>   materials;
        DynaArray<AseMesh>       meshes;
        DynaArray<AseMeshSubset> subsets;
        DynaArray<void*>         meshdata; ///< store all vertex and index buffers
        Boxf                     bbox;     ///< bounding box of the whole scene

        /// clear the scene
        void clear();

        /// dtor
        ~AseScene() { clear(); }
    };

    ///
    /// load ASE scene from file
    ///
    bool loadAseSceneFromFile( AseScene & scene, File & file );


}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_UTIL_ASE_H__
