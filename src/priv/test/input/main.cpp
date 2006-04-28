#include "pch.h"

//!
//! input module test application
//!
class InputTest
{
    GN::AutoObjPtr<GN::win::Window> mWin;
    GN::AutoObjPtr<GN::input::Input> mInput;

    bool mDone;

    GN::input::KeyEvent mLastKeyEvent;

    bool createWindow()
    {
        mWin.attach( GN::win::createWindow( GN::win::WCP_WINDOWED_RENDER_WINDOW ) );
        if( mWin.empty() ) return false;
        mWin->show();
        return true;
    }

    bool createInput( const char * api )
    {
        mInput.attach( GN::input::createInputSystem(
            0 == GN::strCmp("DI",api) ? GN::input::API_DINPUT : GN::input::API_NATIVE ) );
        if( !mInput || !mInput->attachToWindow( 0, mWin->getWindowHandle() ) ) return false;

        // connect to input signals
        mInput->sigKeyPress.connect( this, &InputTest::onKeyPress );
        mInput->sigCharPress.connect( this, &InputTest::onCharPress );
        mInput->sigAxisMove.connect( this, &InputTest::onAxisMove );

        return true;
    }

    /*LRESULT winProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
    {
        switch(msg)
        {
            case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    HDC dc = BeginPaint( hwnd, &ps );

                    GN::StrA txt;
                    txt.format(
                        "%s%s%s%s %s",
                        mLastKeyEvent.status.ctrlDown()?"CTRL-":"",
                        mLastKeyEvent.status.shiftDown()?"SHIFT-":"",
                        mLastKeyEvent.status.altDown()?"ALT-":"",
                        GN::input::kc2str(mLastKeyEvent.code),
                        mLastKeyEvent.status.down?"DOWN":"UP" );
                    TextOutA( dc, 0, 0, txt.cptr(), (INT)txt.size() );

                    if( mInput )
                    {
                        int x, y;
                        mInput->getMousePosition( x, y );
                        txt.format( "Mouse: %d, %d", x, y );
                        TextOutA( dc, 0, 20, txt.cptr(), (INT)txt.size() );
                    }

                    EndPaint( hwnd, &ps );
                }
                break;

            case WM_CLOSE: mDone = true; break;
            default:; // do nothing
        }

        return ::DefWindowProc( hwnd, msg, wp, lp );
    }*/

    void onKeyPress( GN::input::KeyEvent ke )
    {
        mLastKeyEvent = ke;
        mWin->repaint();
        if( !ke.status.down )
        {
            if( GN::input::KEY_ESCAPE == ke.code ) mDone = true;
        }
    }

    void onCharPress( wchar_t )
    {
    }

    void onAxisMove( GN::input::Axis, int  )
    {
        mWin->repaint();
    }

public:

    //!
    //! Default constructor
    //!
    InputTest() {}

    //!
    //! Destructor
    //!
    ~InputTest() { quit(); }

    //!
    //! Initialize test application
    //!
    bool init( const char * api )
    {
        if( !createWindow() || !createInput(api) ) return false;

        mDone = false;

        // success
        return true;
    }

    //!
    //! Quit test application
    //!
    void quit()
    {
        mInput.clear();
        mWin.clear();
    }

    //!
    //! Run test application
    //!
    int run()
    {
        if( !mInput )
        {
            GN_ERROR( "InputTest is not initialized!" );
            return -1;
        }

        while(!mDone)
        {
            mWin->runWhileEvents();
            mInput->processInputEvents();
        }

        return 0;
    }
};

//!
//! Print usage
//!
void usage( const char * appName )
{
    printf(
        "Input module test application.\n"
        "\n"
        "Usage: %s [MSW|DI]\n"
        "\n"
        "Note: default module is NT\n",
        appName );
}

//!
//! Main entry point
//!
int main( int argc, const char * argv[] )
{
    const char * module;

    if( argc < 2 )
    {
        usage( argv[0] );
        module = "MSW";
    }
    else
    {
        module = argv[1];
    }

    InputTest app;
    if( !app.init( module ) ) return -1;
    return app.run();
}
