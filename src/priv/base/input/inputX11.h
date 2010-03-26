#ifndef __GN_INPUT_INPUTX11_H__
#define __GN_INPUT_INPUTX11_H__
// *****************************************************************************
/// \file
/// \brief   Input system on X11 platform
/// \author  chenlee (2005.11.10)
// *****************************************************************************

#if GN_POSIX

#include "basicInput.h"
#include <X11/Xlib.h>

namespace GN { namespace input
{
    ///
    /// Input system on X11 platform
    ///
    class InputX11 : public BasicInput, public StdClass
    {
         GN_DECLARE_STDCLASS( InputX11, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        InputX11();
        virtual ~InputX11() { Quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool Init();
        void Quit();
    private:
        void Clear()
        {
            mDisplay = 0;
            mWindow = 0;
        }
        //@}

        // ********************************
        // from Input
        // ********************************
    public:

        virtual bool AttachToWindow( HandleType, HandleType );
        virtual void ProcessInputEvents();
        virtual void GetMousePosition( int &, int & ) const;

        // ********************************
        // public functions
        // ********************************
    public:

        // ********************************
        // private variables
        // ********************************
    private:

        Display * mDisplay;
        Window    mWindow;

        enum { MAX_SCAN_CODE = 0x300 };

        KeyCode mKeyMap[MAX_SCAN_CODE]; ///< scancode to garnet keycode

        // ********************************
        // private functions
        // ********************************
    private:
    };
}}

#endif // GN_POSIX

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_INPUT_INPUTX11_H__
