#ifndef __GN_GFX_OGLUTILS_H__
#define __GN_GFX_OGLUTILS_H__
// *****************************************************************************
/// \file
/// \brief   Public interface for opengl module (collection of OGL utils).
/// \author  chenlee (2005.8.26)
// *****************************************************************************

#if !GN_XENON

#if GN_MSWIN
#include <windows.h>
#endif

#include <glew.h>

namespace GN { namespace gfx
{
    ///
    /// Namespace for ogl module
    ///
    namespace ogl
    {
        ///
        /// Get OpenGL error string
        ///
        inline const char * getGLErrorInfo( GLenum errCode )
        {
            static StrA sErrInfo;
            sErrInfo = (const char*)gluErrorString( errCode );
            return sErrInfo.ToRawPtr();
        }
    }
}}

#endif

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_OGLUTILS_H__
