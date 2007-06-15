#ifndef __GN_GFX2_D3D9SURFACE_H__
#define __GN_GFX2_D3D9SURFACE_H__
// *****************************************************************************
//! \file    gfx2/d3d9Surface.h
//! \brief   d3d9 surface interface
//! \author  chenli@@FAREAST (2007.6.15)
// *****************************************************************************

namespace GN { namespace gfx2
{
    ///
    /// D3D9 surface descriptor
    ///
    struct D3D9SurfaceDesc : public SurfaceDesc
    {
        // D3D9 specific fields
    };

    ///
    /// base D3D9 surface
    ///
    class GN_GFX2_D3D9_PUBLIC D3D9Surface : public Surface
    {
    };
}}

// *****************************************************************************
//                           End of d3d9Surface.h
// *****************************************************************************
#endif // __GN_GFX2_D3D9SURFACE_H__

