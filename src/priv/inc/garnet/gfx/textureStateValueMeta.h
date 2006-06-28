/*
 * This file use to define all texture stage state values
 *
 * NOTE : internelly used by render module, do NOT include it directly in your
 *        own header or source files.
 *
 * syntax : GNGFX_DEFINE_TSV(
 *               tag,
 *               d3dval,
 *               glval1,
 *               glval2 )
 *
 */

#ifndef GNGFX_DEFINE_TSV
#error "GNGFX_DEFINE_TSV not defined!"
#endif

GNGFX_DEFINE_TSV( ARG0       , D3DTOP_SELECTARG1   , GL_REPLACE         , 1 ) //!< arg0
GNGFX_DEFINE_TSV( MODULATE   , D3DTOP_MODULATE     , GL_MODULATE        , 1 ) //!< arg0 * arg1
GNGFX_DEFINE_TSV( MODULATE2  , D3DTOP_MODULATE2X   , GL_MODULATE        , 2 ) //!< arg0 * arg1 * 2
GNGFX_DEFINE_TSV( MODULATE4  , D3DTOP_MODULATE4X   , GL_MODULATE        , 4 ) //!< arg0 * arg1 * 4
GNGFX_DEFINE_TSV( ADD        , D3DTOP_ADD          , GL_ADD             , 1 ) //!< arg0 + arg1
GNGFX_DEFINE_TSV( ADDS       , D3DTOP_ADDSIGNED    , GL_ADD_SIGNED_ARB  , 1 ) //!< (arg0 + arg1)-0.5
GNGFX_DEFINE_TSV( ADDS2      , D3DTOP_ADDSIGNED2X  , GL_ADD_SIGNED_ARB  , 2 ) //!< ( (arg0 + arg1)-0.5 ) * 2;
GNGFX_DEFINE_TSV( SUBSTRACT  , D3DTOP_SUBTRACT     , GL_SUBTRACT_ARB    , 1 ) //!< arg0 - arg1
GNGFX_DEFINE_TSV( DOT3       , D3DTOP_DOTPRODUCT3  , GL_DOT3_RGBA_ARB   , 1 ) //!< arg0.r * arg1.r + arg0.g * arg1.g + arg0.b * arg1.b

#define D3DTA_TEXTURE_ALPHA     (D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE)
#define D3DTA_TEXTURE_INV_COLOR (D3DTA_TEXTURE|D3DTA_COMPLEMENT)
#define D3DTA_TEXTURE_INV_ALPHA (D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

#define D3DTA_CURRENT_ALPHA     (D3DTA_CURRENT|D3DTA_ALPHAREPLICATE)
#define D3DTA_CURRENT_INV_COLOR (D3DTA_CURRENT|D3DTA_COMPLEMENT)
#define D3DTA_CURRENT_INV_ALPHA (D3DTA_CURRENT|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

#define D3DTA_PRIMARY_ALPHA     (D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE)
#define D3DTA_PRIMARY_INV_COLOR (D3DTA_DIFFUSE|D3DTA_COMPLEMENT)
#define D3DTA_PRIMARY_INV_ALPHA (D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

//!
//! texture color
//!
GNGFX_DEFINE_TSV( TEXTURE_COLOR      , D3DTA_TEXTURE            , GL_TEXTURE           , GL_SRC_COLOR           )

//!
//! texture alpha
//!
GNGFX_DEFINE_TSV( TEXTURE_ALPHA      , D3DTA_TEXTURE_ALPHA      , GL_TEXTURE           , GL_SRC_ALPHA           )

//!
//! 1.0 - texture color
//!
GNGFX_DEFINE_TSV( TEXTURE_INV_COLOR  , D3DTA_TEXTURE_INV_COLOR  , GL_TEXTURE           , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - texture alpha
//!
GNGFX_DEFINE_TSV( TEXTURE_INV_ALPHA  , D3DTA_TEXTURE_INV_ALPHA  , GL_TEXTURE           , GL_ONE_MINUS_SRC_ALPHA )

//!
//! current color
//!
GNGFX_DEFINE_TSV( CURRENT_COLOR      , D3DTA_CURRENT            , GL_PREVIOUS_ARB      , GL_SRC_COLOR           )

//!
//! current alpha
//!
GNGFX_DEFINE_TSV( CURRENT_ALPHA      , D3DTA_CURRENT_ALPHA      , GL_PREVIOUS_ARB      , GL_SRC_ALPHA           )

//!
//! 1.0 - current color
//!
GNGFX_DEFINE_TSV( CURRENT_INV_COLOR  , D3DTA_CURRENT_INV_COLOR  , GL_PREVIOUS_ARB      , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - current alpha
//!
GNGFX_DEFINE_TSV( CURRENT_INV_ALPHA  , D3DTA_TEXTURE_INV_ALPHA  , GL_PREVIOUS_ARB      , GL_ONE_MINUS_SRC_ALPHA )

//!
//! primary color
//!
GNGFX_DEFINE_TSV( PRIMARY_COLOR      , D3DTA_DIFFUSE            , GL_PRIMARY_COLOR_ARB , GL_SRC_COLOR           )

//!
//! primary alpha
//!
GNGFX_DEFINE_TSV( PRIMARY_ALPHA      , D3DTA_PRIMARY_ALPHA      , GL_PRIMARY_COLOR_ARB , GL_SRC_ALPHA           )

//!
//! 1.0 - primary color
//!
GNGFX_DEFINE_TSV( PRIMARY_INV_COLOR  , D3DTA_PRIMARY_INV_COLOR  , GL_PRIMARY_COLOR_ARB , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - primary alpha
//!
GNGFX_DEFINE_TSV( PRIMARY_INV_ALPHA  , D3DTA_PRIMARY_INV_ALPHA  , GL_PRIMARY_COLOR_ARB , GL_ONE_MINUS_SRC_ALPHA )

#undef D3DTA_TEXTURE_ALPHA
#undef D3DTA_TEXTURE_INV_COLOR
#undef D3DTA_TEXTURE_INV_ALPHA

#undef D3DTA_CURRENT_ALPHA
#undef D3DTA_CURRENT_INV_COLOR
#undef D3DTA_CURRENT_INV_ALPHA

#undef D3DTA_PRIMARY_ALPHA
#undef D3DTA_PRIMARY_INV_COLOR
#undef D3DTA_PRIMARY_INV_ALPHA
