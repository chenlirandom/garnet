#ifndef __GN_GNOGL_GNOGL_H__
#define __GN_GNOGL_GNOGL_H__
// *****************************************************************************
//! \file    GNogl.h
//! \brief   Public interface for opengl module, to ease quick-prototype
//!          of OpenGL application.
//! \author  chenlee (2005.8.26)
// *****************************************************************************

#include "GNbase.h"
#include "GNwin.h"

#if GN_MSWIN
#include <windows.h>
#endif

#include <glew.h>

//!
//! ogl module error log macro
//!
#define GNOGL_ERROR GN_ERROR

//!
//! ogl module warning log macro
//!
#define GNOGL_WARN GN_WARN

//!
//! ogl module informational log macro
//!
#define GNOGL_INFO GN_INFO

//!
//! Global OGL object. Must instantiate and initialize it before using ogl module.
//!
#define gOGL (::GN::ogl::OGL::getInstance())

namespace GN
{
    //!
    //! Namespace for ogl module
    //!
    namespace ogl
    {
        //!
        //! OGL initialization parameters
        //!
        struct OGLInitParams
        {
            bool fullScreen; //!< use fullscreen mode.
            bool showWindow; //!< show render window auotmatically after starting up.
            uint32_t width;  //!< render window width.
            uint32_t height; //!< render window height.
            const char * winTitle; //!< render window title.

            //!
            //! Default constructor
            //!
            OGLInitParams()
                : fullScreen(false)
                , showWindow(true)
                , width(640)
                , height(480)
                , winTitle("OGLApp")
            {}
        };

        //!
        //! Main ogl class
        //!
        class OGL : public StdClass, public Singleton<OGL>
        {
             GN_DECLARE_STDCLASS( OGL, StdClass );

            // ********************************
            // ctor/dtor
            // ********************************

            //@{
        public:
            OGL()          { clear(); }
            virtual ~OGL() { quit(); }
            //@}

            // ********************************
            // from StdClass
            // ********************************

            //@{
        public:
            bool init( const OGLInitParams & param );
            void quit();
            bool ok() const { return MyParent::ok(); }
        private:
            void clear()
            {
#if GN_MSWIN
                mDC = 0;
                mRC = 0;
                mFullscreenModeInitialized = false;
#endif
            }
            //@}

            // ********************************
            // public functions
            // ********************************
        public:

#if GN_MSWIN
            //!
            //! Get render window handle
            //!
            HWND getWindow() const { return mWindow.getWindow(); }

            //!
            //! Get OGL render context
            //!
            HGLRC getRC() const { GN_ASSERT(mRC); return mRC; }
#endif
            //!
            //! Is render window closed?
            //!
            bool windowClosed() const { return mClosed; }

            //!
            //! Do present. Please always call this function,
            //! instead of glSwapBuffer().
            //!
            //! This function also handles windows messages.
            //!
            //! \return Return false, present failed.
            //!
            bool present();

            // ********************************
            // private variables
            // ********************************
        private:

            OGLInitParams   mInitParams;
#if GN_MSWIN
            win::MswWindow  mWindow;
            HDC             mDC;
            HGLRC           mRC;
            bool            mFullscreenModeInitialized;
            bool            mMinimized;
            bool            mInsideSizeMove;
            bool            mSizeChanged;
#endif
            bool            mClosed;

            // ********************************
            // private functions
            // ********************************
        private:

            bool createWindow();
            bool createOGL();
            bool setupDisplayMode();
            void restoreDisplayMode();
#if GN_MSWIN
            LRESULT windowProc( HWND wnd, UINT msg, WPARAM wp, LPARAM lp );
#endif
        };

        //! \name Util functions
        //@{

        //!
        //! Get OpenGL error string
        //!
        inline const char * getGLErrorInfo( GLenum errCode )
        {
            static StrA sErrInfo;
            sErrInfo = (const char*)gluErrorString( errCode );
            return sErrInfo.cstr();
        }

        //@}
    }
}

// *****************************************************************************
//                           End of GnGL.h
// *****************************************************************************
#endif // __GN_GNOGL_GNOGL_H__
