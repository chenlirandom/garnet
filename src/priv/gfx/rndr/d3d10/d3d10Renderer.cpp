#include "pch.h"
#include "d3d10Renderer.h"

#if GN_MSVC

#pragma comment(lib, "d3d9.lib") // for PIX routines.
#pragma comment(lib, "d3d10.lib")
#if GN_BUILD_DEBUG
#pragma comment(lib, "d3dx10d.lib")
#else
#pragma comment(lib, "d3dx10.lib")
#endif

#endif // GN_MSVC

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.D3D10");

// *****************************************************************************
// Global functions
// *****************************************************************************

bool gD3D10EnablePixPerf = true; // default is enabled

#if GN_BUILD_STATIC
GN::gfx::Renderer *
GNgfxCreateD3D10Renderer( const GN::gfx::RendererOptions & o )
#else
extern "C" GN_EXPORT GN::gfx::Renderer *
GNgfxCreateRenderer( const GN::gfx::RendererOptions & o )
#endif
{
    GN_GUARD;

    GN::AutoObjPtr<GN::gfx::D3D10Renderer> p( new GN::gfx::D3D10Renderer );
    if( !p->init( o ) ) return 0;
    return p.detach();

    GN_UNGUARD;
}

// *****************************************************************************
// init/quit functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10Renderer::init( const GN::gfx::RendererOptions & o )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::D3D10Renderer, ( o ) );

    // init sub-components
    if( !dispInit()        ) return failure();
    if( !capsInit()         ) return failure();
    if( !resourceInit()     ) return failure();
    if( !contextInit()      ) return failure();
    if( !drawInit()         ) return failure();

    // successful
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::quit()
{
    GN_GUARD;

    drawQuit();
    contextQuit();
    resourceQuit();
    capsQuit();
    dispQuit();

    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}
