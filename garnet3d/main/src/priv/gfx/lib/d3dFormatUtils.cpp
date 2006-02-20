#include "pch.h"
#include "garnet/GNd3d.h"

#if GN_MSWIN

//
//
// -----------------------------------------------------------------------------
const char * GN::gfx::d3d::d3dFormat2Str( D3DFORMAT d3dfmt )
{
    struct Item { int fmt; const char * str; };
    static const Item sTable[] =
    {
        { D3DFMT_UNKNOWN,         "D3DFMT_UNKNOWN" },
        { D3DFMT_A8R8G8B8,        "D3DFMT_A8R8G8B8" },
        { D3DFMT_X8R8G8B8,        "D3DFMT_X8R8G8B8" },
        { D3DFMT_R5G6B5,          "D3DFMT_R5G6B5" },
        { D3DFMT_X1R5G5B5,        "D3DFMT_X1R5G5B5" },
        { D3DFMT_A1R5G5B5,        "D3DFMT_A1R5G5B5" },
        { D3DFMT_A4R4G4B4,        "D3DFMT_A4R4G4B4" },
        { D3DFMT_A8,              "D3DFMT_A8" },
        { D3DFMT_X4R4G4B4,        "D3DFMT_X4R4G4B4" },
        { D3DFMT_A2B10G10R10,     "D3DFMT_A2B10G10R10" },
        { D3DFMT_A8B8G8R8,        "D3DFMT_A8B8G8R8" },
        { D3DFMT_X8B8G8R8,        "D3DFMT_X8B8G8R8" },
        { D3DFMT_G16R16,          "D3DFMT_G16R16" },
        { D3DFMT_A2R10G10B10,     "D3DFMT_A2R10G10B10" },
        { D3DFMT_A16B16G16R16,    "D3DFMT_A16B16G16R16" },
        { D3DFMT_L8,              "D3DFMT_L8" },
        { D3DFMT_A8L8,            "D3DFMT_A8L8" },
        { D3DFMT_V8U8,            "D3DFMT_V8U8" },
        { D3DFMT_L6V5U5,          "D3DFMT_L6V5U5" },
        { D3DFMT_X8L8V8U8,        "D3DFMT_X8L8V8U8" },
        { D3DFMT_Q8W8V8U8,        "D3DFMT_Q8W8V8U8" },
        { D3DFMT_V16U16,          "D3DFMT_V16U16" },
        { D3DFMT_A2W10V10U10,     "D3DFMT_A2W10V10U10" },
        { D3DFMT_UYVY,            "D3DFMT_UYVY" },
        { D3DFMT_R8G8_B8G8,       "D3DFMT_R8G8_B8G8" },
        { D3DFMT_YUY2,            "D3DFMT_YUY2" },
        { D3DFMT_G8R8_G8B8,       "D3DFMT_G8R8_G8B8" },
        { D3DFMT_DXT1,            "D3DFMT_DXT1" },
        { D3DFMT_DXT2,            "D3DFMT_DXT2" },
        { D3DFMT_DXT3,            "D3DFMT_DXT3" },
        { D3DFMT_DXT4,            "D3DFMT_DXT4" },
        { D3DFMT_DXT5,            "D3DFMT_DXT5" },
        { D3DFMT_D32,             "D3DFMT_D32" },
        { D3DFMT_D24S8,           "D3DFMT_D24S8" },
        { D3DFMT_D24X8,           "D3DFMT_D24X8" },
        { D3DFMT_D16,             "D3DFMT_D16" },
        { D3DFMT_D24FS8,          "D3DFMT_D24FS8" },
        { D3DFMT_L16,             "D3DFMT_L16" },
        { D3DFMT_VERTEXDATA,      "D3DFMT_VERTEXDATA" },
        { D3DFMT_INDEX16,         "D3DFMT_INDEX16" },
        { D3DFMT_INDEX32,         "D3DFMT_INDEX32" },
        { D3DFMT_Q16W16V16U16,    "D3DFMT_Q16W16V16U16" },
        { D3DFMT_R16F,            "D3DFMT_R16F" },
        { D3DFMT_G16R16F,         "D3DFMT_G16R16F" },
        { D3DFMT_A16B16G16R16F,   "D3DFMT_A16B16G16R16F" },
        { D3DFMT_R32F,            "D3DFMT_R32F" },
        { D3DFMT_G32R32F,         "D3DFMT_G32R32F" },
        { D3DFMT_A32B32G32R32F,   "D3DFMT_A32B32G32R32F" },
#if !GN_XENON
        { D3DFMT_R8G8B8,          "D3DFMT_R8G8B8" },
        { D3DFMT_R3G3B2,          "D3DFMT_R3G3B2" },
        { D3DFMT_A8R3G3B2,        "D3DFMT_A8R3G3B2" },
        { D3DFMT_A8P8,            "D3DFMT_A8P8" },
        { D3DFMT_P8,              "D3DFMT_P8" },
        { D3DFMT_A4L4,            "D3DFMT_A4L4" },
        { D3DFMT_D16_LOCKABLE,    "D3DFMT_D16_LOCKABLE" },
        { D3DFMT_D15S1,           "D3DFMT_D15S1" },
        { D3DFMT_D24X4S4,         "D3DFMT_D24X4S4" },
        { D3DFMT_D32F_LOCKABLE,   "D3DFMT_D32F_LOCKABLE" },
        { D3DFMT_MULTI2_ARGB8,    "D3DFMT_MULTI2_ARGB8" },
        { D3DFMT_CxV8U8,          "D3DFMT_CxV8U8" },
#endif
    };

    for( size_t i = 0; i < sizeof(sTable)/sizeof(sTable[0]); ++i )
    {
        if( d3dfmt == sTable[i].fmt ) return sTable[i].str;
    }
    return "INVALID D3DFORMAT";
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ClrFmt GN::gfx::d3d::d3dFormat2ClrFmt( D3DFORMAT d3dfmt )
{
    switch( d3dfmt )
    {
        // 32 bits
        case D3DFMT_A8R8G8B8     : return FMT_BGRA_8_8_8_8;
        case D3DFMT_X8R8G8B8     : return FMT_BGRX_8_8_8_8;
        case D3DFMT_A2B10G10R10  : return FMT_RGBA_10_10_10_2;
        case D3DFMT_G16R16       : return FMT_RG_16_16;
        //case D3DFMT_X8L8V8U8     : return FMT_UVLX_8_8_8_8;
        //case D3DFMT_Q8W8V8U8     : return FMT_UVWQ_8_8_8_8;
        case D3DFMT_V16U16       : return FMT_UV_16_16;
        //case D3DFMT_W11V11U10    : return FMT_UVW_10_11_11;
        //case D3DFMT_A2W10V10U10  : return FMT_UVWA_10_10_10_2;
        case D3DFMT_R32F         : return FMT_R_32_FLOAT;

        // 16 bits
        case D3DFMT_R5G6B5       : return FMT_BGR_5_6_5;
        case D3DFMT_X1R5G5B5     : return FMT_BGRX_5_5_5_1;
        case D3DFMT_A1R5G5B5     : return FMT_BGRA_5_5_5_1;
        case D3DFMT_A4R4G4B4     : return FMT_BGRA_4_4_4_4;
        case D3DFMT_X4R4G4B4     : return FMT_BGRX_4_4_4_4;
        //case D3DFMT_A8P8         : return FMT_PA_8_8;
        case D3DFMT_A8L8         : return FMT_LA_8_8;
        case D3DFMT_V8U8         : return FMT_UV_8_8;
        //case D3DFMT_L6V5U5       : return FMT_UVL_5_5_6;

        // 8 bits
        //case D3DFMT_R3G3B2       : return FMT_BGR_2_3_3;
        case D3DFMT_A8           : return FMT_A_8;
        //case D3DFMT_A8R3G3B2     : return FMT_BGRA_2_3_3_8;
        //case D3DFMT_P8           : return FMT_P_8;
        case D3DFMT_L8           : return FMT_L_8;
        //case D3DFMT_A4L4         : return FMT_LA_4_4;

        // compressed formats
        //case D3DFMT_UYVY         : return FMT_;
        //case D3DFMT_YUY2         : return FMT_;
        case D3DFMT_DXT1         : return FMT_DXT1;
        case D3DFMT_DXT2         : return FMT_DXT2;
        case D3DFMT_DXT4         : return FMT_DXT4;
#if !GN_XENON
        case D3DFMT_DXT3         : return FMT_DXT3;
        case D3DFMT_DXT5         : return FMT_DXT5;

        // depth formats
        case D3DFMT_D16_LOCKABLE : return FMT_D_16;
#endif
        case D3DFMT_D32          : return FMT_D_32;
        //case D3DFMT_D15S1        : return FMT_DS_15_1;
        case D3DFMT_D24S8        : return FMT_DS_24_8;
        case D3DFMT_D16          : return FMT_D_16;
        case D3DFMT_D24X8        : return FMT_DX_24_8;
        //case D3DFMT_D24X4S4      : return FMT_DXS_24_4_4;

        // failed
        default : return FMT_INVALID;
    }
}

//
//
// -----------------------------------------------------------------------------
D3DFORMAT GN::gfx::d3d::clrFmt2D3DFormat( ClrFmt clrfmt )
{
    switch( clrfmt )
    {
        // 32 bits
        case FMT_BGRA_8_8_8_8  : return D3DFMT_A8R8G8B8;
        case FMT_R_32_FLOAT    : return D3DFMT_R32F;

        // 16 bits
        case FMT_BGRA_5_5_5_1  : return D3DFMT_A1R5G5B5;
        case FMT_BGR_5_6_5     : return D3DFMT_R5G6B5;
        case FMT_LA_8_8        : return D3DFMT_A8L8;
        case FMT_UV_8_8        : return D3DFMT_V8U8;

        // 8 bits
        case FMT_L_8           : return D3DFMT_L8;
        case FMT_A_8           : return D3DFMT_A8;

        // compressed formats
        case FMT_DXT1          : return D3DFMT_DXT1;
        case FMT_DXT2          : return D3DFMT_DXT2;

        // depth formats
        case FMT_D_16          : return D3DFMT_D16;
        case FMT_DX_24_8       : return D3DFMT_D24X8;
        case FMT_DS_24_8       : return D3DFMT_D24S8;
        case FMT_D_32          : return D3DFMT_D32;

        // failed
        default : return D3DFMT_UNKNOWN;
    }
}

#endif
