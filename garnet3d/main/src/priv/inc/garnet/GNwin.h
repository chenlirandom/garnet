#ifndef __GN_WIN_GNWIN_H__
#define __GN_WIN_GNWIN_H__
// *****************************************************************************
//! \file    GNwin.h
//! \brief   public interface of WIN module
//! \author  chenlee (2006.2.22)
// *****************************************************************************

namespace GN
{
    //!
    //! Namespace for window module
    //!
    namespace win
    {
        typedef HandleType DisplayHandle; //!< Display ID on X11. No use on platform other then X11.
        typedef HandleType MonitorHandle; //!< Screen ID on X11; Monitor handle on MS Windows.
        typedef HandleType WindowHandle;  //!< Window handle/ID.

        typedef Delegate2<void,void*,bool&> WindowEventHandler; //!< Window event handler.

        //!
        //! General window class
        //!
        class Window : public NoCopy
        {
            // ********************************
            // public functions
            // ********************************
        public:

            //! \name window properties
            //@{

            virtual DisplayHandle getDisplayHandle() const = 0;
            virtual MonitorHandle getMonitorHandle() const = 0;
            virtual WindowHandle getWindowHandle() const = 0;
            virtual Vector2<size_t> getClientSize() const = 0;

            //@}

            //! \name window manipulation
            //@{
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual void minimize() = 0;
            virtual void moveTo( int x, int y ) = 0;
            virtual void resize( size_t clientWidth, size_t clientHeight ) = 0;
            virtual void repaint() = 0;
            //@}

            //! \name window events
            //@{

            //!
            //! Run the application until the window is closed.
            //!
            virtual void run() = 0;

            //!
            //! Run the application while there's events in event queue.
            //!
            virtual void runWhileEvents() = 0;

            //!
            //! Step one event. Block the application if there's no event.
            //!
            virtual void stepOneEvent() = 0;

            virtual void attachEventHandler( const StrA & eventName, const WindowEventHandler & ) = 0;
            virtual void removeEventHandler( const StrA & eventName, const WindowEventHandler & ) = 0;

            //@}
        };

        //!
        //! Window creation parameters
        //!
        struct WindowCreationParams
        {
            StrA caption; //!< window title text.
            WindowHandle parent; //!< Parent window
            size_t clientWidth;  //!< client width. 0 means default width
            size_t clientHeight; //!< client height. 0 means default height
            bool hasBorder; //!< has border or not
            bool hasTitleBar; //!< has title bar or not
            bool topMost; //!< top-most(always on top) or not
        };

        //!
        //! Default parameters to create main render window.
        //!
        //! border, title, no parent, 640x480
        //!
        extern const WindowCreationParams WCP_WINDOWED_RENDER_WINDOW;

        //!
        //! Default parameters to create main render window.
        //!
        //! no border, no title, no parent, topmost, 640x480
        //!
        extern const WindowCreationParams WCP_FULLSCREEN_RENDER_WINDOW;

        //!
        //! Create window instance
        //!
        Window * createWindow( const WindowCreationParams & );

        //!
        //! Get monitor handle by index. 0 is always primary screen. Return 0 for invalid index.
        //!
        MonitorHandle getMonitorByIndex( size_t );

        //!
        //! Process windows messages. No effects on platform other than MS Windows.
        //!
        void processWindowMessages( WindowHandle window, bool blockWhileMinized );
    }
}

// *****************************************************************************
//                           End of GNwin.h
// *****************************************************************************
#endif // __GN_WIN_GNWIN_H__
