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
        SimpleWireframeModel( GpuResourceDatabase & db ) : mDatabase(db), mModel(NULL) { clear(); }
        virtual ~SimpleWireframeModel() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init();
        void quit();
    private:
        void clear()
        {
            mModel = NULL;
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        //@{
        const ModelResource & modelResource() const { GN_ASSERT(mModel); return *mModel; }
        ModelResource & modelResource() { GN_ASSERT(mModel); return *mModel; }
        void setTransformation( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world ); ///< Defaults are identity matrices.
        void setColor( const Vector4f & ); ///< set wireframe color, default is (1,1,1,1)
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        GpuResourceDatabase & mDatabase;
        ModelResource       * mModel;
        UniformResource     * mMatrixPvw;
        UniformResource     * mColor;

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
        SimpleDiffuseModel( GpuResourceDatabase & db ) : mDatabase(db), mModel(NULL) { clear(); }
        virtual ~SimpleDiffuseModel() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init();
        void quit();
    private:
        void clear()
        {
            mModel = NULL;
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        //@{
        const ModelResource & modelResource() const { GN_ASSERT(mModel); return *mModel; }
        ModelResource & modelResource() { GN_ASSERT(mModel); return *mModel; }
        void setTransformation( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world ); ///< Defaults are identity matrices.
        void setLightPos( const Vector4f & ); ///< set light position in world space, default is (0,0,0)
        void setLightColor( const Vector4f & ); ///< set light color, default is (1,1,1,1)
        void setAlbedoColor( const Vector4f & ); ///< set albedo color, default is (1,1,1,1)
        void setAlbedoTexture( const AutoRef<Texture> & ); ///< set to NULL to use pure white texture. Default is NULL.
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        GpuResourceDatabase & mDatabase;
        ModelResource       * mModel;
        AutoRef<Texture>      mDefaultTexture;
        UniformResource     * mMatrixPvw;
        UniformResource     * mMatrixWorld;
        UniformResource     * mMatrixWorldIT;
        UniformResource     * mLightPos;
        UniformResource     * mLightColor;
        UniformResource     * mAlbedoColor;
        TextureResource     * mAlbedoTexture;

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
        SimpleNormalMapModel( GpuResourceDatabase & db ) : mDatabase(db), mModel(NULL) { clear(); }
        virtual ~SimpleNormalMapModel() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init();
        void quit();
    private:
        void clear()
        {
            mModel = NULL;
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        //@{
        const ModelResource & modelResource() const { GN_ASSERT(mModel); return *mModel; }
        ModelResource & modelResource() { GN_ASSERT(mModel); return *mModel; }
        void setTransformation( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world ); ///< Defaults are identity matrices.
        void setLightPos( const Vector4f & ); ///< set light position in world space, default is (0,0,0)
        void setLightColor( const Vector4f & ); ///< set light color, default is (1,1,1,1)
        void setAlbedoColor( const Vector4f & ); ///< set albedo color, default is (1,1,1,1)
        void setAlbedoTexture( const AutoRef<Texture> & ); ///< set to NULL to use pure white texture. Default is NULL.
        void setNormalTexture( const AutoRef<Texture> & ); ///< set to NULL to use flat normal texture. Defau is NULL.
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        GpuResourceDatabase   mDatabase;
        ModelResource       * mModel;
        AutoRef<Texture>      mDefaultAlbedoTexture;
        AutoRef<Texture>      mDefaultNormalTexture;
        UniformResource     * mMatrixPvw;
        UniformResource     * mMatrixWorld;
        UniformResource     * mMatrixWorldIT;
        UniformResource     * mLightPos;
        UniformResource     * mLightColor;
        UniformResource     * mAlbedoColor;
        TextureResource     * mAlbedoTexture;
        TextureResource     * mNormalTexture;

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
