#include "pch.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpures");

//
//
// -----------------------------------------------------------------------------
static ModelResourceDesc sDiffuseModelDesc()
{
    ModelResourceDesc md;
    md.effect = "@DIFFUSE";

    md.textures["ALBEDO_TEXTURE"].resourceName = "@WHITE";

#define INIT_UNIFORM( name, type, defval ) \
    if( 1 ) { \
        md.uniforms[name].size = sizeof(type); \
        md.uniforms[name].initialValue.Resize( sizeof(type) ); \
        type default = (defval); \
        memcpy( md.uniforms[name].initialValue.ToRawPtr(), &default, sizeof(type) ); \
    } else void(0)

    INIT_UNIFORM( "MATRIX_PVW"      , Matrix44f, Matrix44f::sIdentity() );
    INIT_UNIFORM( "MATRIX_WORLD"    , Matrix44f, Matrix44f::sIdentity() );
    INIT_UNIFORM( "MATRIX_WORLD_IT" , Matrix44f, Matrix44f::sIdentity() );
    INIT_UNIFORM( "LIGHT0_POSITION" , Vector4f,  Vector4f(0,0,0,0) );
    INIT_UNIFORM( "LIGHT0_DIFFUSE"  , Vector4f,  Vector4f(1,1,1,1) );
    INIT_UNIFORM( "ALBEDO_COLOR"    , Vector4f,  Vector4f(1,1,1,1) );

    return md;
}

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

const ModelResourceDesc GN::gfx::SimpleDiffuseModel::DESC = sDiffuseModelDesc();

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::SimpleDiffuseModel::Init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::SimpleDiffuseModel, () );

    mModel = mDatabase.createResource<ModelResource>( NULL );
    if( 0 == mModel || !mModel->Reset(&DESC) ) return Failure();

    // initialize uniforms
    mMatrixPvw     = mModel->getUniformResource( "MATRIX_PVW"      );
    mMatrixWorld   = mModel->getUniformResource( "MATRIX_WORLD"    );
    mMatrixWorldIT = mModel->getUniformResource( "MATRIX_WORLD_IT" );
    mLightPos      = mModel->getUniformResource( "LIGHT0_POSITION" );
    mLightColor    = mModel->getUniformResource( "LIGHT0_DIFFUSE"  );
    mAlbedoColor   = mModel->getUniformResource( "ALBEDO_COLOR"    );

    // initialize texture
    mAlbedoTexture = mModel->getTextureResource( "ALBEDO_TEXTURE"  );

    // success
    return Success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SimpleDiffuseModel::Quit()
{
    GN_GUARD;

    mModel.Clear();
    mDefaultTexture.Clear();
    mMatrixPvw.Clear();
    mMatrixWorld.Clear();
    mMatrixWorldIT.Clear();
    mLightPos.Clear();
    mLightColor.Clear();
    mAlbedoColor.Clear();
    mAlbedoTexture.Clear();

    // standard Quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SimpleDiffuseModel::setTransformation(
    const Matrix44f & proj,
    const Matrix44f & view,
    const Matrix44f & world )
{
    Matrix44f pvw = proj * view * world;
    Matrix44f wit = Matrix44f::sInverse( Matrix44f::sTranspose( world ) );
    mMatrixPvw->getUniform()->Update( pvw );
    mMatrixWorld->getUniform()->Update( world );
    mMatrixWorldIT->getUniform()->Update( wit );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SimpleDiffuseModel::setLightPos( const Vector4f & pos )
{
    mLightPos->getUniform()->Update( pos );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SimpleDiffuseModel::setLightColor( const Vector4f & clr )
{
    mLightColor->getUniform()->Update( clr );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SimpleDiffuseModel::setAlbedoColor( const Vector4f & clr )
{
    mAlbedoColor->getUniform()->Update( clr );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SimpleDiffuseModel::setAlbedoTexture( const AutoRef<Texture> & tex )
{
    mAlbedoTexture->setTexture( tex ? tex : mDefaultTexture );
}
