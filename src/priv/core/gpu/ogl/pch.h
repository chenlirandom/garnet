#ifndef __GN_PCH_H__
#define __GN_PCH_H__
// *****************************************************************************
// \file    pch.h
// \brief   PCH header
// \author  chenlee (2005.10.1)
// *****************************************************************************

#include "../gpupch.h"

#if GN_MSWIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <GL/glew.h>
#if GN_MSWIN
#include <GL/wglew.h>
#elif GN_POSIX
#include <GL/glxew.h>
#endif
#include <GL/glu.h>

#ifdef HAS_CG_OGL
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif

// Note: redefine GN_OGL_CHECK macro to do nothing.
#undef GN_OGL_CHECK
#define GN_OGL_CHECK(X) X

///
/// Automatically pop OGL attributes while out of scope.
///
class OGLAutoAttribStack
{
    GLuint mBits;
    GLuint mClientBits;

public:
    ///
    /// Ctor
    ///
    OGLAutoAttribStack( GLuint bits = GL_ALL_ATTRIB_BITS, GLuint clientBits = GL_CLIENT_ALL_ATTRIB_BITS )
        : mBits( bits ), mClientBits( clientBits )
    {
        if( bits )
        {
            GN_OGL_CHECK( glPushAttrib( bits ) );
        }
        if( clientBits )
        {
            GN_OGL_CHECK( glPushClientAttrib( clientBits ) );
        }
    }

    ///
    /// Dtor
    ///
    ~OGLAutoAttribStack()
    {
        if( mBits )
        {
            GN_OGL_CHECK( glPopAttrib() );
        }
        if( mClientBits )
        {
            GN_OGL_CHECK( glPopClientAttrib() );
        }
    }
};

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_PCH_H__
