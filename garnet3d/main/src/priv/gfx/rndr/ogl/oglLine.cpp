#include "pch.h"
#include "oglLine.h"
#include "oglRenderer.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.OGL");

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLLine::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::OGLLine, () );

    // create vertex buffer
    GN_ASSERT( !mVtxBuf );
    mVtxBuf = new LineVertex[MAX_LINES*2];
    if( !mVtxBuf )
    {
        GN_ERROR(sLogger)( "out of memory!" );
        return failure();
    }

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLLine::quit()
{
    GN_GUARD;

    safeDeleteArray( mVtxBuf );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLLine::drawLines(
    BitFields options,
    const float * positions,
    size_t stride,
    size_t count,
    UInt32 rgba,
    const Matrix44f & model,
    const Matrix44f & view,
    const Matrix44f & proj )
{
    GN_GUARD_SLOW;

    GN_ASSERT( ok() );

    if( 0 == positions )
    {
        GN_ERROR(sLogger)( "NULL parameter(s)!" );
        return;
    }

    if( 0 == stride )
    {
        GN_ERROR(sLogger)( "stride can't be zero!" );
        return;
    }

    GN_ASSERT( mNextLine < MAX_LINES );

    // handle large amount of array
    while( count + mNextLine > MAX_LINES )
    {
        size_t n = MAX_LINES - mNextLine;
        GN_ASSERT( n > 0 );
        drawLines( options, positions, stride, n, rgba, model, view, proj );
        positions = (const float*)( ((const UInt8*)positions) + n * stride * 2 );
        count -= n;
    }

    // fill vertex data
    GN_ASSERT( mVtxBuf );
    size_t vertexCount = ( DL_LINE_STRIP & options ) ? count + 1 : count * 2;
    for( size_t i = 0; i < vertexCount; ++i )
    {
        mVtxBuf[i].p.set( positions[0], positions[1], positions[2] );
        mVtxBuf[i].c = rgba;
        positions = (const float*)( ((const UInt8*)positions) + stride );
    }

    // determine attributes that need to be restored.
    GLbitfield attribs = GL_TEXTURE_BIT;
    if( !(DL_USE_CURRENT_RS & options ) )
        attribs |= GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_DEPTH_BITS | GL_ENABLE_BIT;
    if( !(DL_USE_CURRENT_VS & options ) )
        attribs |= GL_TRANSFORM_BIT;
    if( !(DL_USE_CURRENT_PS & options ) )
        attribs |= GL_CURRENT_BIT | GL_ENABLE_BIT;

    // push OGL attributes
    GN_OGL_CHECK( glPushAttrib( attribs ) );
    GN_OGL_CHECK( glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT ) );

    // apply render states
    if( !( DL_USE_CURRENT_RS & options ) )
    {
        glDisable( GL_BLEND );
        glDepthMask( GL_TRUE );
        glEnable( GL_DEPTH_TEST );
    }

    // apply vertex shader
    if( !( DL_USE_CURRENT_VS & options ) )
    {
        // enable color material
        //glEnable( GL_COLOR_MATERIAL );

        // setup OGL matrices
        if( DL_WINDOW_SPACE & options )
        {
            GLdouble vp[4];
            GN_OGL_CHECK( glGetDoublev( GL_VIEWPORT, vp ) );

            // position is in screen space (0,0)->(width,height)
            GN_OGL_CHECK( glMatrixMode( GL_PROJECTION ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadIdentity() );
            GN_OGL_CHECK( glOrtho( 0, vp[2], vp[3], 0, 0, 1 ) );

            GN_OGL_CHECK( glMatrixMode( GL_MODELVIEW ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadIdentity() );
        }
        else
        {
            // position is in object space
            GN_OGL_CHECK( glMatrixMode( GL_PROJECTION ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadMatrixf( Matrix44f::sTranspose(proj)[0] ) );

            GN_OGL_CHECK( glMatrixMode( GL_MODELVIEW ) );
            GN_OGL_CHECK( glPushMatrix() );
            GN_OGL_CHECK( glLoadMatrixf( Matrix44f::sTranspose(view*model)[0] ) );
        }
    }

    // disable texture
    if( !(DL_USE_CURRENT_PS & options ) )
    {
        mRenderer.disableTextureStage( 0 );
        glDisable( GL_LIGHTING );
    }

    // disable VBO
    if( GLEW_ARB_vertex_buffer_object )
    {
        GN_OGL_CHECK( glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 ) );
    }

    // apply vertex binding
    GN_OGL_CHECK( glInterleavedArrays( GL_C4UB_V3F, sizeof(LineVertex), mVtxBuf ) );

    // do draw
    GN_OGL_CHECK( glDrawArrays(
        ( DL_LINE_STRIP & options ) ? GL_LINE_STRIP : GL_LINES,
        0,
        (GLsizei)vertexCount ) );

    if( !( DL_USE_CURRENT_VS & options ) )
    {
        // restore OGL matrices
        GN_OGL_CHECK( glMatrixMode( GL_PROJECTION ) );
        GN_OGL_CHECK( glPopMatrix() );
        GN_OGL_CHECK( glMatrixMode( GL_MODELVIEW ) );
        GN_OGL_CHECK( glPopMatrix() );
    }

    // restore OGL attributes
    GN_OGL_CHECK( glPopClientAttrib() );
    GN_OGL_CHECK( glPopAttrib() );

    // update mNextLine
    mNextLine += count;
    GN_ASSERT( mNextLine <= MAX_LINES );
    if( MAX_LINES == mNextLine ) mNextLine = 0;

    GN_UNGUARD_SLOW;
}
