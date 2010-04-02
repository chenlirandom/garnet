#ifndef __GN_GFX_SIMPLEMODEL_H__
#define __GN_GFX_SIMPLEMODEL_H__
// *****************************************************************************
/// \file
/// \brief   commonly used model classes
/// \author  chen@@chenli-homepc (2009.9.3)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// simple wireframe model (no lighting, requires position only)
    ///
    class SimpleWireframeModel : public StdClass
    {
        GN_DECLARE_STDCLASS( SimpleWireframeModel, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        SimpleWireframeModel( GpuResourceDatabase & db ) : mDatabase(db), mModel(NULL) { Clear(); }
        virtual ~SimpleWireframeModel() { Quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool Init();
        void Quit();
    private:
        void Clear()
        {
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        /// The descriptor that is used to initialize this model
        static const ModelResourceDesc DESC;

        //@{
        const ModelResource & GetModelResource() const { GN_ASSERT(mModel); return *mModel; }
        ModelResource & GetModelResource() { GN_ASSERT(mModel); return *mModel; }
        void SetTransform( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world ); ///< Defaults are identity matrices.
        void SetColor( const Vector4f & ); ///< set wireframe color, default is (1,1,1,1)
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        GpuResourceDatabase    & mDatabase;
        AutoRef<ModelResource>   mModel;
        AutoRef<UniformResource> mMatrixPvw;
        AutoRef<UniformResource> mColor;

        // ********************************
        // private functions
        // ********************************
    private:
    };


    ///
    /// simple diffuse model:
    ///     - one texture, one color, diffuse lighting
    ///     - mesh must have position, normal and texcoord
    ///
    class SimpleDiffuseModel : public StdClass
    {
        GN_DECLARE_STDCLASS( SimpleDiffuseModel, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        SimpleDiffuseModel( GpuResourceDatabase & db ) : mDatabase(db), mModel(NULL) { Clear(); }
        virtual ~SimpleDiffuseModel() { Quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool Init();
        void Quit();
    private:
        void Clear()
        {
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        //@{

        /// The descriptor that is used to initialize this model
        static const ModelResourceDesc DESC;

        const ModelResource & GetModelResource() const { GN_ASSERT(mModel); return *mModel; }
        ModelResource & GetModelResource() { GN_ASSERT(mModel); return *mModel; }
        void SetTransform( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world ); ///< Defaults are identity matrices.
        void SetLightPos( const Vector4f & ); ///< set light position in world space, default is (0,0,0)
        void SetLightColor( const Vector4f & ); ///< set light color, default is (1,1,1,1)
        void SetAlbedoColor( const Vector4f & ); ///< set albedo color, default is (1,1,1,1)
        void SetAlbedoTexture( const AutoRef<Texture> & ); ///< set to NULL to use pure white texture. Default is NULL.
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        GpuResourceDatabase    & mDatabase;
        AutoRef<ModelResource>   mModel;
        AutoRef<Texture>         mDefaultTexture;
        AutoRef<UniformResource> mMatrixPvw;
        AutoRef<UniformResource> mMatrixWorld;
        AutoRef<UniformResource> mMatrixWorldIT;
        AutoRef<UniformResource> mLightPos;
        AutoRef<UniformResource> mLightColor;
        AutoRef<UniformResource> mAlbedoColor;
        AutoRef<TextureResource> mAlbedoTexture;

        // ********************************
        // private functions
        // ********************************
    private:
    };

    ///
    /// simple normal map effect
    ///
    class SimpleNormalMapModel : public StdClass
    {
        GN_DECLARE_STDCLASS( SimpleNormalMapModel, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        SimpleNormalMapModel( GpuResourceDatabase & db ) : mDatabase(db), mModel(NULL) { Clear(); }
        virtual ~SimpleNormalMapModel() { Quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool Init();
        void Quit();
    private:
        void Clear()
        {
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        //@{

        /// The descriptor that is used to initialize this model
        static const ModelResourceDesc DESC;

        const ModelResource & GetModelResource() const { GN_ASSERT(mModel); return *mModel; }
        ModelResource & GetModelResource() { GN_ASSERT(mModel); return *mModel; }
        void SetTransform( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world ); ///< Defaults are identity matrices.
        void SetLightPos( const Vector4f & ); ///< set light position in world space, default is (0,0,0)
        void SetLightColor( const Vector4f & ); ///< set light color, default is (1,1,1,1)
        void SetAlbedoColor( const Vector4f & ); ///< set albedo color, default is (1,1,1,1)
        void SetAlbedoTexture( const AutoRef<Texture> & ); ///< set to NULL to use pure white texture. Default is NULL.
        void SetNormalTexture( const AutoRef<Texture> & ); ///< set to NULL to use flat normal texture. Defau is NULL.
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        GpuResourceDatabase    & mDatabase;
        AutoRef<ModelResource>   mModel;
        AutoRef<Texture>         mDefaultAlbedoTexture;
        AutoRef<Texture>         mDefaultNormalTexture;
        AutoRef<UniformResource> mMatrixPvw;
        AutoRef<UniformResource> mMatrixWorld;
        AutoRef<UniformResource> mMatrixWorldIT;
        AutoRef<UniformResource> mLightPos;
        AutoRef<UniformResource> mLightColor;
        AutoRef<UniformResource> mAlbedoColor;
        AutoRef<TextureResource> mAlbedoTexture;
        AutoRef<TextureResource> mNormalTexture;

        // ********************************
        // private functions
        // ********************************
    private:
    };

}}
// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_SIMPLEMODEL_H__