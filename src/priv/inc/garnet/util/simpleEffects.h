#ifndef __GN_UTIL_SIMPLEEFFECTS_H__
#define __GN_UTIL_SIMPLEEFFECTS_H__
// *****************************************************************************
/// \file
/// \brief   simple (commonly used) effects
/// \author  chenli@@REDMOND (2009.1.20)
// *****************************************************************************

namespace GN { namespace util
{
    ///
    /// simple diffuse effect
    ///
    class SimpleDiffuseEffect : public StdClass
    {
        GN_DECLARE_STDCLASS( SimpleDiffuseEffect, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        SimpleDiffuseEffect() : mEffect(NULL) { clear(); }
        virtual ~SimpleDiffuseEffect() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( gfx::Renderer & );
        void quit();
    private:
        void clear()
        {
            mDefaultTexture = NULL;
            mEffect = NULL;
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        //@{
        gfx::Effect * getEffect() const { return mEffect; }
        void setTransformation( const Matrix44f & proj, const Matrix44f & view, const Matrix44f & world );
        void setLightPos( const Vector4f & ); ///< set light position in world space
        void setLightColor( const Vector4f & ); ///< set light color
        void setAlbedoColor( const Vector4f & ); ///< set albedo color
        void setAlbedoTexture( gfx::Texture * ); ///< set to NULL to use pure white texture.
        void setMesh( const gfx::Mesh & mesh, const gfx::MeshSubset * subset = NULL );
        void draw();
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        gfx::Texture            * mDefaultTexture;
        gfx::Effect             * mEffect;
        AutoRef<gfx::Uniform>   * mMatrixPvw;
        AutoRef<gfx::Uniform>   * mMatrixWorld;
        AutoRef<gfx::Uniform>   * mMatrixWorldIT;
        AutoRef<gfx::Uniform>   * mLightPos;
        AutoRef<gfx::Uniform>   * mLightColor;
        AutoRef<gfx::Uniform>   * mAlbedoColor;
        AutoRef<gfx::Texture>   * mAlbedoTexture;
        gfx::Drawable             mDrawable;

        // ********************************
        // private functions
        // ********************************
    private:
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_UTIL_SIMPLEEFFECTS_H__