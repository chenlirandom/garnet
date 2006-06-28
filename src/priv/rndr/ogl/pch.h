#ifndef __GN_PCH_H__
#define __GN_PCH_H__
// *****************************************************************************
// \file    pch.h
// \brief   PCH header
// \author  chenlee (2005.10.1)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "garnet/GNcore.h"
#include "garnet/GNgfx.h"

#if GN_MSWIN && !GN_XENON
#define NOMINMAX //!< This is to disable windows min/max macros
#include <windows.h>
#endif

#include <glew.h>
#if GN_MSWIN
#include <wglew.h>
#elif GN_POSIX
#include <glxew.h>
#endif
#include <GL/glu.h>

//!
//! Automatically pop OGL attributes while out of scope.
//!
class OGLAutoAttribStack
{
    GLuint mBits;
    GLuint mClientBits;

public:
    //!
    //! Ctor
    //!
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

    //!
    //! Dtor
    //!
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


#include "garnet/base/pragma.h"

// *****************************************************************************
//                           End of pch.h
// *****************************************************************************
#endif // __GN_PCH_H__
