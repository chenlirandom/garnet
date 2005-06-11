/*
 * This file use to define all texture stage state values
 *
 * NOTE : internelly used by render module, do NOT include it directly in your
 *        own header or source files.
 *
 * syntax : GN_DEFINE_TSV(
 *               tag,
 *               d3dval,
 *               glval1,
 *               glval2 )
 *
 */

#ifndef GN_DEFINE_TSV
#error "GN_DEFINE_TSV not defined!"
#endif

GN_DEFINE_TSV( DISABLE    , D3DTOP_DISABLE      , GL_REPLACE         , 1 ) //!< disable
GN_DEFINE_TSV( ARG0       , D3DTOP_SELECTARG1   , GL_REPLACE         , 1 ) //!< arg0
GN_DEFINE_TSV( MODULATE   , D3DTOP_MODULATE     , GL_MODULATE        , 1 ) //!< arg0 * arg1
GN_DEFINE_TSV( MODULATE2  , D3DTOP_MODULATE2X   , GL_MODULATE        , 2 ) //!< arg0 * arg1 * 2
GN_DEFINE_TSV( MODULATE4  , D3DTOP_MODULATE4X   , GL_MODULATE        , 4 ) //!< arg0 * arg1 * 4
GN_DEFINE_TSV( ADD        , D3DTOP_ADD          , GL_ADD             , 1 ) //!< arg0 + arg1
GN_DEFINE_TSV( ADDS       , D3DTOP_ADDSIGNED    , GL_ADD_SIGNED_ARB  , 1 ) //!< (arg0 + arg1)-0.5
GN_DEFINE_TSV( ADDS2      , D3DTOP_ADDSIGNED2X  , GL_ADD_SIGNED_ARB  , 2 ) //!< ( (arg0 + arg1)-0.5 ) * 2;
GN_DEFINE_TSV( SUBSTRACT  , D3DTOP_SUBTRACT     , GL_SUBTRACT_ARB    , 1 ) //!< arg0 - arg1
GN_DEFINE_TSV( DOT3       , D3DTOP_DOTPRODUCT3  , GL_DOT3_RGBA_ARB   , 1 ) //!< arg0.r * arg1.r + arg0.g * arg1.g + arg0.b * arg1.b

# define D3DTA_TEXTURE_ALPHA     (D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE)
# define D3DTA_TEXTURE_INV_COLOR (D3DTA_TEXTURE|D3DTA_COMPLEMENT)
# define D3DTA_TEXTURE_INV_ALPHA (D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

# define D3DTA_CURRENT_ALPHA     (D3DTA_CURRENT|D3DTA_ALPHAREPLICATE)
# define D3DTA_CURRENT_INV_COLOR (D3DTA_CURRENT|D3DTA_COMPLEMENT)
# define D3DTA_CURRENT_INV_ALPHA (D3DTA_CURRENT|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

# define D3DTA_PRIMARY_ALPHA     (D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE)
# define D3DTA_PRIMARY_INV_COLOR (D3DTA_DIFFUSE|D3DTA_COMPLEMENT)
# define D3DTA_PRIMARY_INV_ALPHA (D3DTA_DIFFUSE|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

# define D3DTA_CONSTANT_ALPHA     (D3DTA_CONSTANT|D3DTA_ALPHAREPLICATE)
# define D3DTA_CONSTANT_INV_COLOR (D3DTA_CONSTANT|D3DTA_COMPLEMENT)
# define D3DTA_CONSTANT_INV_ALPHA (D3DTA_CONSTANT|D3DTA_ALPHAREPLICATE|D3DTA_COMPLEMENT)

//!
//! texture color
//!
GN_DEFINE_TSV( TEXTURE_COLOR      , D3DTA_TEXTURE            , GL_TEXTURE           , GL_SRC_COLOR           )

//!
//! texture alpha
//!
GN_DEFINE_TSV( TEXTURE_ALPHA      , D3DTA_TEXTURE_ALPHA      , GL_TEXTURE           , GL_SRC_ALPHA           )

//!
//! 1.0 - texture color
//!
GN_DEFINE_TSV( TEXTURE_INV_COLOR  , D3DTA_TEXTURE_INV_COLOR  , GL_TEXTURE           , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - texture alpha
//!
GN_DEFINE_TSV( TEXTURE_INV_ALPHA  , D3DTA_TEXTURE_INV_ALPHA  , GL_TEXTURE           , GL_ONE_MINUS_SRC_ALPHA )

//!
//! current color
//!
GN_DEFINE_TSV( CURRENT_COLOR      , D3DTA_CURRENT            , GL_PREVIOUS_ARB      , GL_SRC_COLOR           )

//!
//! current alpha
//!
GN_DEFINE_TSV( CURRENT_ALPHA      , D3DTA_CURRENT_ALPHA      , GL_PREVIOUS_ARB      , GL_SRC_ALPHA           )

//!
//! 1.0 - current color
//!
GN_DEFINE_TSV( CURRENT_INV_COLOR  , D3DTA_CURRENT_INV_COLOR  , GL_PREVIOUS_ARB      , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - current alpha
//!
GN_DEFINE_TSV( CURRENT_INV_ALPHA  , D3DTA_TEXTURE_INV_ALPHA  , GL_PREVIOUS_ARB      , GL_ONE_MINUS_SRC_ALPHA )

//!
//! primary color
//!
GN_DEFINE_TSV( PRIMARY_COLOR      , D3DTA_DIFFUSE            , GL_PRIMARY_COLOR_ARB , GL_SRC_COLOR           )

//!
//! primary alpha
//!
GN_DEFINE_TSV( PRIMARY_ALPHA      , D3DTA_PRIMARY_ALPHA      , GL_PRIMARY_COLOR_ARB , GL_SRC_ALPHA           )

//!
//! 1.0 - primary color
//!
GN_DEFINE_TSV( PRIMARY_INV_COLOR  , D3DTA_PRIMARY_INV_COLOR  , GL_PRIMARY_COLOR_ARB , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - primary alpha
//!
GN_DEFINE_TSV( PRIMARY_INV_ALPHA  , D3DTA_PRIMARY_INV_ALPHA  , GL_PRIMARY_COLOR_ARB , GL_ONE_MINUS_SRC_ALPHA )

//!
//! constant color
//!
GN_DEFINE_TSV( CONSTANT_COLOR     , D3DTA_CONSTANT           , GL_CONSTANT_ARB      , GL_SRC_COLOR           )

//!
//! constant alpha
//!
GN_DEFINE_TSV( CONSTANT_ALPHA     , D3DTA_CONSTANT_ALPHA     , GL_CONSTANT_ARB      , GL_SRC_ALPHA           )

//!
//! 1.0 - constant color
//!
GN_DEFINE_TSV( CONSTANT_INV_COLOR , D3DTA_CONSTANT_INV_COLOR , GL_CONSTANT_ARB      , GL_ONE_MINUS_SRC_COLOR )

//!
//! 1.0 - constant alpha
//!
GN_DEFINE_TSV( CONSTANT_INV_ALPHA , D3DTA_CONSTANT_INV_ALPHA , GL_CONSTANT_ARB      , GL_ONE_MINUS_SRC_ALPHA )


# undef D3DTA_TEXTURE_ALPHA
# undef D3DTA_TEXTURE_INV_COLOR
# undef D3DTA_TEXTURE_INV_ALPHA

# undef D3DTA_CURRENT_ALPHA
# undef D3DTA_CURRENT_INV_COLOR
# undef D3DTA_CURRENT_INV_ALPHA

# undef D3DTA_PRIMARY_ALPHA
# undef D3DTA_PRIMARY_INV_COLOR
# undef D3DTA_PRIMARY_INV_ALPHA

# undef D3DTA_CONSTANT_ALPHA
# undef D3DTA_CONSTANT_INV_COLOR
# undef D3DTA_CONSTANT_INV_ALPHA
