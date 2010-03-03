#ifndef __GN_GN_D3D_DXGI_H__
#define __GN_GN_D3D_DXGI_H__
// *****************************************************************************
/// \file
/// \brief   DXGI utiles
/// \author  chenli@@REDMOND (2010.1.29)
// *****************************************************************************

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <d3d10_1.h>
#include <DXGI.h>

namespace GN{ /** namespace for DXGI utilities */ namespace dxgi
{
    /// Enumerates all DXGI numer formats
    enum DXGINumberFormat
    {
        // Typeless format
        DXGINF_TYPELESS,

        // Unsinged normalized format
        DXGINF_UNORM,

        // Signed normalized format
        DXGINF_SNORM,

        // Unsigned int DXGI format.
        DXGINF_UINT,

        // Signed int DXGI format.
        DXGINF_SINT,

        // Float DXGI format.
        DXGINF_FLOAT,
    };

    struct DXGI_FORMAT_DESCRIPTION
    {
        /// The format name.
        const char *     name;

        /// The block width.
        UInt32           blockWidth;

        /// The block height.
        UInt32           blockHeight;

        /// Block size in bytes
        UInt32           blockBytes;

        /// Bits of a single texel
        UInt32           texelBits;

        /// Number of color channels.
        UInt32           numChannels;

        /// The number format.
        DXGINumberFormat numfmt;

        /// Correspondance typeless format.
        DXGI_FORMAT      typelessFormat;

        /// Correspondance uint format.
        DXGI_FORMAT      uintFormat;

        /// Correspondance depth stencil buffer format
        DXGI_FORMAT      dsForamt;

        /// Is the format can be used as
    };

    /// Get the DXGI format description for a specific format.
    const DXGI_FORMAT_DESCRIPTION & getDXGIFormatDesc( DXGI_FORMAT );
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GN_D3D_DXGI_H__