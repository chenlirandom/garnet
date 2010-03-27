#include "pch.h"
#include "uniformresource.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.gpures");

// *****************************************************************************
// Local stuff
// *****************************************************************************

struct UniformDesc
{
    size_t       length;
    const void * initialData;
};

// *****************************************************************************
// GN::gfx::UniformResource
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
const Guid & GN::gfx::UniformResource::GetGuid()
{
    static const Guid UNIFORM_GUID = { 0xd140dbea, 0x9ce3, 0x4bb9, { 0x9c, 0xa7, 0xb8, 0x14, 0xf3, 0x83, 0xab, 0x95 } };
    return UNIFORM_GUID;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::UniformResource::Reset(
    size_t                length,
    const void          * initialData )
{
    AutoRef<Uniform> u( GetGdb().GetGpu().CreateUniform( length ) );
    if( !u ) return false;

    if( initialData ) u->Update( 0, length, initialData );

    SetUniform( u );

    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::UniformResource::SetUniform( const AutoRef<Uniform> & newUniform )
{
    if( mUniform == newUniform ) return;

    mUniform = newUniform;

    sigUniformChanged(*this);
}

// *****************************************************************************
// GN::gfx::UniformResourceInternal
// *****************************************************************************

class UniformResourceInternal : public UniformResource
{
    //
    //
    // -----------------------------------------------------------------------------
    UniformResourceInternal( GpuResourceDatabase & db )
        : UniformResource( db )
    {
    }

    //
    //
    // -----------------------------------------------------------------------------
    ~UniformResourceInternal()
    {
    }

    //
    //
    // -----------------------------------------------------------------------------
    static GpuResource * sCreateInstance( GpuResourceDatabase & db )
    {
        return new UniformResourceInternal( db );
    }

public:

    //
    //
    // -----------------------------------------------------------------------------
    static bool sRegisterFactory( GpuResourceDatabase & db )
    {
        GpuResourceFactory factory = { &sCreateInstance };

        if( db.HasResourceFactory( UniformResource::GetGuid() ) ) return true;

        return db.RegisterResourceFactory( UniformResource::GetGuid(), "Uniform Resource", factory );
    }
};

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::registerUniformResourceFactory( GpuResourceDatabase & db )
{
    return UniformResourceInternal::sRegisterFactory( db );
}
