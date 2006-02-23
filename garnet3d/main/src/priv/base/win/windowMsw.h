#ifndef __GN_WIN_WINDOWMSW_H__
#define __GN_WIN_WINDOWMSW_H__
// *****************************************************************************
//! \file    win/windowMsw.h
//! \brief   Window class on MS Windows
//! \author  chenlee (2006.2.23)
// *****************************************************************************

namespace GN { namespace win
{
    //!
    //! Window class on MS Windows
    //!
    class WindowMsw : public Window, public StdClass
    {
         GN_DECLARE_STDCLASS( WindowMsw, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        WindowMsw()          { clear(); }
        virtual ~WindowMsw() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const WindowCreationParams & );
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() { mClassName.clear(); mModuleInstance = 0; mWindow = 0; }
        //@}

        // ********************************
        // from Window
        // ********************************
    public:

        //@{

        DisplayHandle getDisplayHandle() const;
        WindowHandle getWindowHandle() const { return mWindow; }
        Vector2<size_t> getClientSize() const;
        void show();
        void hide();
        void minimize() {}
        void moveTo( int, int );
        void resize( size_t, size_t );
        void repaint() {}
        void run() {}
        void runWhileEvents() { processWindowMessages(mWindow,false); }
        void stepOneEvent() {}
        void attachEventHandler( const StrA &, const WindowEventHandler & ) {}
        void removeEventHandler( const StrA &, const WindowEventHandler & ) {}

        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        StrA mClassName;
        HINSTANCE mModuleInstance;
        HWND mWindow;

        static std::map<void*,WindowMsw*> msInstanceMap;

        // ********************************
        // private functions
        // ********************************
    private:
        bool createWindow( const WindowCreationParams & wcp );

        LRESULT windowProc( HWND wnd, UINT msg, WPARAM wp, LPARAM lp );
        static LRESULT CALLBACK staticWindowProc( HWND wnd, UINT msg, WPARAM wp, LPARAM lp );
    };
}}

// *****************************************************************************
//                           End of windowMsw.h
// *****************************************************************************
#endif // __GN_WIN_WINDOWMSW_H__
