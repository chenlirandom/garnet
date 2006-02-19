#include "pch.h"
#include "d3dShader.h"
#include "d3dRenderer.h"
#include "garnet/GNd3d.h"

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DPxlShaderHlsl::init( const StrA & code, const StrA & entry )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( D3DPxlShaderHlsl, () );

    mCode = code;
    mEntry = entry;

    if( !deviceCreate() || !deviceRestore() )
    {
        quit(); return selfOK();
    }

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DPxlShaderHlsl::quit()
{
    GN_GUARD;

    deviceDispose();
    deviceDestroy();

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// from D3DResource
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DPxlShaderHlsl::deviceCreate()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    GN_ASSERT( !mConstTable && !mD3DShader );

    mD3DShader = d3d::compilePS( getRenderer().getDevice(), mCode.cstr(), mCode.size(), 0, mEntry.cstr(), 0 );
    if( 0 == mD3DShader ) return false;

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DPxlShaderHlsl::deviceDestroy()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    safeRelease( mD3DShader );
    safeRelease( mConstTable );

    GN_UNGUARD;
}


// *****************************************************************************
// from D3DBasicShader
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DPxlShaderHlsl::apply() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mD3DShader );

    LPDIRECT3DDEVICE9 dev = getRenderer().getDevice();

    GN_DX_CHECK( dev->SetPixelShader( mD3DShader ) );

    // apply ALL uniforms to D3D device
    uint32_t handle = getFirstUniform();
    while( handle )
    {
        applyUniform( dev, mConstTable, getUniform( handle ) );
        handle = getNextUniform( handle );
    }
    clearDirtySet();

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DPxlShaderHlsl::applyDirtyUniforms() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mD3DShader );

    LPDIRECT3DDEVICE9 dev = getRenderer().getDevice();

    const std::set<uint32_t> dirtySet = getDirtyUniforms();
    std::set<uint32_t>::const_iterator i, e = dirtySet.end();
    for( i = dirtySet.begin(); i != e; ++i )
    {
        applyUniform( dev, mConstTable, getUniform( *i ) );
    }
    clearDirtySet();

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// from Shader
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DPxlShaderHlsl::queryDeviceUniform( const char * name, HandleType & userData ) const
{
    GN_GUARD;

    GN_ASSERT( !strEmpty(name) );

    GN_ASSERT( mConstTable );

    D3DXHANDLE h = mConstTable->GetConstantByName( NULL, name );
    if( 0 == h )
    {
        GN_ERROR( "%s is not a valid shader uniform!", name );
        return false;
    }

    // success
    userData = (HandleType)h;
    return true;

    GN_UNGUARD;
}
