#include "pch.h"
#include "coreResourceDict.h"

// *****************************************************************************
// local functions
// *****************************************************************************

using namespace GN;
using namespace GN::gfx;

//
// Create a 1x1 pure blue texture.
// -----------------------------------------------------------------------------
static bool sCreateNullTexture( Texture * & result, const StrA & name )
{
    GN_GUARD;

    // check for global renderer
    if( 0 == gRendererPtr )
    {
        GN_ERROR( "Null texture '%s' creation failed: renderer is not ready.", name.cstr() );
        return false;
    }

    AutoRef<Texture> tex( gRenderer.create1DTexture( 1 ) );
    uint32_t * texData = (uint32_t*)tex->lock1D( 0, 0, 0, GN::gfx::LOCK_WO );
    if( 0 == texData ) return false;
    //           A R G B
    *texData = 0xFF0000FF;
    tex->unlock();

    // success
    result = tex.detach();
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static bool sCreateTexture( Texture * & result, const StrA & name )
{
    GN_GUARD;

    // check for global renderer
    if( 0 == gRendererPtr )
    {
        GN_ERROR( "Texture '%s' creation failed: renderer is not ready." );
        return false;
    }

    // get resouce path
    StrA path = GN::core::searchResource( name );
    if( path.empty() )
    {
        GN_ERROR( "Texture '%s' creation failed: path not found.", name.cstr() );
        return false;
    }

    GN_INFO( "Load texture '%s' from file '%s'.", name.cstr(), path.cstr() ); 

    // open file
    AnsiFile fp;
    if( !fp.open( path::resolve(path), "rb" ) )
    {
        GN_ERROR( "Texture '%s' creation failed: can't open file '%s'.", name.cstr(), path.cstr() );
        return false;
    }

    // create texture instance
    result = gRenderer.createTextureFromFile( fp );
    return NULL != result;

    GN_UNGUARD;
}

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::core::CoreTextureDict::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::core::CoreTextureDict, () );

    // create the dictionary instance
    mDict = new gfx::TextureDictionary;

    // register functors
    mDict->setCreator( &sCreateTexture );
    mDict->setDeletor( &safeDecref<Texture> );
    mDict->setNullor( &sCreateNullTexture );

    // connect to renderer signals
    GN::gfx::Renderer::sSigDeviceDispose.connect( mDict, &gfx::TextureDictionary::dispose );

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::core::CoreTextureDict::quit()
{
    GN_GUARD;

    if( mDict )
    {
        GN::gfx::Renderer::sSigDeviceDispose.disconnect( mDict );
        delete mDict;
    }

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}
