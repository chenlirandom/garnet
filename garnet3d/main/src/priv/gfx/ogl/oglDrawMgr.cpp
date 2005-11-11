#include "pch.h"
#include "oglRenderer.h"

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::drawDeviceRestore()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    if( !fontInit() ) return false;

    // success
    return true;

    GN_UNGUARD
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawDeviceDispose()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    fontQuit();

    GN_UNGUARD
}

// *****************************************************************************
// interface functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::drawBegin()
{
    GN_GUARD_SLOW;

    GN_ASSERT( !mDrawBegan );

    // handle render window size move
    if( !handleRenderWindowSizeMove() ) return false;

    mDrawBegan = 1;
    mNumPrims = 0;
    mNumDraws = 0;
    return true;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawEnd()
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );
    mDrawBegan = 0;
#if GN_MSWIN
    GN_MSW_CHECK( ::SwapBuffers( mDeviceContext ) );
#else
    const DispDesc & dd = getDispDesc();
    GN_ASSERT( dd.displayHandle && dd.windowHandle );
    glXSwapBuffers( (Display*)dd.displayHandle, (Window)dd.windowHandle );
#endif

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawFinish()
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );
    GN_OGL_CHECK( glFinish() );

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::clearScreen(
    const GN::Vector4f & c, float z, uint32_t s, uint32_t flags )
{
    GN_GUARD_SLOW;

    GLbitfield glflag = 0;

    // clear color buffer
    if( flags & C_BUFFER )
    {
        glflag |= GL_COLOR_BUFFER_BIT;
        glClearColor( c.r, c.g, c.b, c.a );
        glColorMask( 1,1,1,1 );       // ȷ��COLOR BUFFER��д
    }

    // clean z-buffer
    if( flags & Z_BUFFER )
    {
        glflag |= GL_DEPTH_BUFFER_BIT;
        glClearDepth( z );
        glDepthMask( 1 );             // ȷ��Z-BUFFER��д
    }

    // clearn stencil buffer
    if( flags & S_BUFFER )
    {
        glflag |= GL_STENCIL_BUFFER_BIT;
        glClearStencil( s );
        glStencilMask( 0xFFFFFFFF );  // ȷ��STENCIL BUFFER��д
    }

    // do clear
    GN_OGL_CHECK( glClear( glflag ) );

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawIndexed(
    PrimitiveType prim,
    size_t        numPrims,
    size_t        baseVtx,
    size_t        minVtxIdx,
    size_t        numVtx,
    size_t        startIdx )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    GN_UNUSED_PARAM(prim);
    GN_UNUSED_PARAM(baseVtx);
    GN_UNUSED_PARAM(minVtxIdx);
    GN_UNUSED_PARAM(numVtx);
    GN_UNUSED_PARAM(startIdx);

    // success
    mNumPrims += numPrims;
    ++mNumDraws;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::draw(
    PrimitiveType prim, size_t numPrims, size_t baseVtx )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    GN_UNUSED_PARAM(prim);
    GN_UNUSED_PARAM(baseVtx);

    // success
    mNumPrims += numPrims;
    ++mNumDraws;

    GN_UNGUARD_SLOW;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::drawTextW(
    const wchar_t * s, int x, int y, const Vector4f & c )
{
    GN_GUARD_SLOW;

    GN_ASSERT( mDrawBegan );

    if( strEmpty(s) ) return;

    // TODO: disable programmable shader

    // push attributes
    glPushAttrib(
        GL_TRANSFORM_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT |
        GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_FOG_BIT | GL_TEXTURE_BIT );

    // set render attributes
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    disableTextureStage( 0 );
    glEnable(GL_COLOR_MATERIAL);
    glColor4fv( c );

    // set transform
    const DispDesc & dd = getDispDesc();
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0, dd.width, 0, dd.height, 0, 1 );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    // draw wide char one by one
    int sx = x;
    int sy = dd.height - y - mFontHeight;
    while ( *s )
    {
        wchar_t ch = *s; ++s;

        if( L'\n' == ch )
        {
            // next line
            sx = x;
            sy -= mFontHeight;
        }
        else // normal char
        {
            glRasterPos2i( sx, sy );
            sx += drawChar(ch);
        }
    }

    // end draw text, restore render states
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();

    // check opengl error
    GN_OGL_CHECK(;);

    GN_UNGUARD_SLOW;
}

// ****************************************************************************
// private functions
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::fontInit()
{
    GN_GUARD;

#if GN_MSWIN

    HWND hwnd = (HWND)getDispDesc().windowHandle;

    // get window DC
    HDC dc;
    GN_MSW_CHECK_RV( dc = ::GetDC( hwnd ), false );

    // select default fixed font
    HGDIOBJ oldfont = ::SelectObject( dc, ::GetStockObject(SYSTEM_FIXED_FONT) );

    // get text height
    SIZE sz;
    GN_MSW_CHECK_DO( ::GetTextExtentPoint32W(dc, L"��", 1, &sz),
        SelectObject( dc, oldfont );
        ReleaseDC( hwnd, dc );
        return false; );

    ::SelectObject( dc, oldfont );
    ::ReleaseDC( hwnd, dc );
    mFontHeight = sz.cy;

#else // GN_MSWIN

    mFontHeight = getFontBitmapHeight();

#endif

    // success
    return true;

    GN_UNGUARD;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::OGLRenderer::fontQuit()
{
    GN_GUARD;

    // release all cached wide-char items
    FontMap::iterator i = mFontMap.begin();
    for ( ;i != mFontMap.end(); ++i )
    {
        glDeleteLists( i->second.displayList, 1 );
    }

    // clear fontmap
    mFontMap.clear();

    GN_UNGUARD;
}

//
//
// ----------------------------------------------------------------------------
int GN::gfx::OGLRenderer::drawChar( wchar_t c )
{
    GN_GUARD_SLOW;

    // ��fontmap�в��ҵ�ǰ�ַ�
    FontMap::iterator i = mFontMap.find(c);
    if( i == mFontMap.end() )
    {
        // û�ҵ��������µ��ַ���
        CharDesc cd;
        if( !charInit( c, cd ) ) return 0;

        // ���fontmap��������ɾ��һ���Ѵ��ڵ��ַ���
        // FIXME : �򵥵�ɾ��begin()���ܻ����𶶶�������İ취�����ɾ��
        //         fontmap�е�һ�
        if( mFontMap.size() >= 128 )
        {
            glDeleteLists(mFontMap.begin()->second.displayList, 1);
            mFontMap.erase( mFontMap.begin() );
        }
        // ���µ��ַ������fontmap
        mFontMap[c] = cd;

        // draw this character
        glCallList( cd.displayList );
        return cd.advanceX;
    }
    else
    {
        // �ҵ�
        glCallList( i->second.displayList );
        return i->second.advanceX;
    }

    GN_UNGUARD_SLOW;
}

//
//
// ----------------------------------------------------------------------------
bool GN::gfx::OGLRenderer::charInit( wchar_t c, CharDesc & cd )
{
    GN_GUARD;

    // generate display lists
    GLuint l = glGenLists(1);
    if( 0 == l )
    {
        GNGFX_ERROR( "Fail to generate opengl display list for wchar %s!",
            wcs2mbs(&c,1).cstr() );
        return false;
    }

#if GN_MSWIN

    HWND hwnd = (HWND)getDispDesc().windowHandle;

    // get window DC
    HDC dc;
    GN_MSW_CHECK_DO( dc = ::GetDC( hwnd ),
        glDeleteLists(l, 1);
        return false; );

    // select default fixed font
    HGDIOBJ oldfont = ::SelectObject( dc, ::GetStockObject(SYSTEM_FIXED_FONT) );

    // get char width
    SIZE sz;
    GN_MSW_CHECK_DO( ::GetTextExtentPoint32W( dc, &c, 1, &sz ),
        ::SelectObject( dc, oldfont );
        ::ReleaseDC( hwnd, dc );
        glDeleteLists( l, 1 );
        return false; );

    // create font list
    GN_MSW_CHECK_DO( ::wglUseFontBitmapsW( dc, c, 1, l ),
        ::SelectObject(dc, oldfont);
        ::ReleaseDC( hwnd, dc );
        glDeleteLists( l, 1 );
        return false; );

    // success
    ::SelectObject(dc, oldfont);
    ::ReleaseDC( hwnd, dc );
    cd.advanceX = sz.cx;
    cd.displayList   = l;
    return true;

#else // GN_MSWIN

    glNewList( l, GL_COMPILE );
    drawFontBitmap( (char)c );
    glEndList();
    cd.displayList = l;
    cd.advanceX = getFontBitmapAdvance( (char)c );
    GN_OGL_CHECK_RV( , false );
    return true;

#endif

    GN_UNGUARD;
}
