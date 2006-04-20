#include "pch.h"
#include "d3d9Font.h"
#include "d3d9Renderer.h"

#if !GN_XENON

//
//! convert Vector4f to D3COLOR
// ------------------------------------------------------------------------
static GN_INLINE D3DCOLOR sRgba2D3DCOLOR( const GN::Vector4f & c )
{
    D3DCOLOR dc = D3DCOLOR_COLORVALUE(c.r, c.g, c.b, c.a);
    return dc;
}

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9Font::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::D3D9Font, () );

    if( !createFont() || !deviceRestore() ) { quit(); return selfOK(); }

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9Font::quit()
{
    GN_GUARD;

    deviceDispose();

    safeRelease( mFont );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// Public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9Font::drawTextW( const wchar_t * text, int x, int y, const Vector4f & color )
{
    GN_GUARD_SLOW;

    // skip empty string
    if( strEmpty(text) ) return;

    int r;
    RECT rc;
    D3DCOLOR cl = sRgba2D3DCOLOR(color);

    // calculate drawing rect
    rc.left = 0;
    rc.top  = 0;
    r = mFont->DrawTextW( 0, text, -1, &rc, DT_CALCRECT, cl );
    if( 0 == r )
    {
        GN_ERROR( "fail to get text extent!" );
        return;
    }

    // draw text
    OffsetRect( &rc, x, y );
    r = mFont->DrawTextW( 0, text, -1, &rc, DT_LEFT, cl );
    if( 0 == r )
    {
        GN_ERROR( "fail to draw text!" );
    }

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// Private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9Font::createFont()
{
    GN_GUARD;

    GN_ASSERT( !mFont );

    // Get font description
    LOGFONTW lf;
    ::GetObjectW( GetStockObject(SYSTEM_FIXED_FONT), sizeof(lf), &lf );

    // create d3dx font
    GN_DX9_CHECK_RV(
        D3DXCreateFontW(
            getRenderer().getDevice(),
            lf.lfHeight,
            lf.lfWidth,
            lf.lfWeight,
            0,
            lf.lfItalic,
            lf.lfCharSet,
            lf.lfOutPrecision,
            lf.lfQuality,
            lf.lfPitchAndFamily,
            lf.lfFaceName,
            &mFont ),
        false );

    HWND hwnd = (HWND)getRenderer().getDispDesc().windowHandle;

    // get window DC
    HDC dc;
    GN_MSW_CHECK_RV( ( dc = ::GetDC( hwnd ) ), false );

    // select default fixed font
    HGDIOBJ oldfont = ::SelectObject( dc, ::GetStockObject(SYSTEM_FIXED_FONT) );

    // get text height
    SIZE sz;
    if( !::GetTextExtentPoint32W(dc, L"��", 1, &sz) )
    {
        GN_ERROR( "Fail to get text height : %s!", getOSErrorInfo() );
        ::SelectObject( dc, oldfont );
        ::ReleaseDC( hwnd, dc );
        return false;
    }
    mFontHeight = sz.cy;

    // release local variables
    ::SelectObject( dc, oldfont );
    ::ReleaseDC( hwnd, dc );

    // success
    return true;

    GN_UNGUARD;
}

#endif
