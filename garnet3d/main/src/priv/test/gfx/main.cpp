#include "pch.h"
#include "garnet/GNwin.h"
#include "garnet/GNinput.h"

//!
//! GFX module test application
//!
class GfxTest
{
    GN::SharedLib mLib;
    GN::gfx::CreateRendererFunc mCreator;
    GN::AutoObjPtr<GN::gfx::Renderer> mRenderer;
    GN::AutoObjPtr<GN::input::Input> mInput;

    bool mDone;

    //!
    //! Check if specific key is pressed
    //!
    bool keyDown( int keycode )
    {
#if GN_MSWIN
        return !!( 0x8000 & ::GetKeyState(keycode) );
#else
        return false;
#endif
    }

public:

    //!
    //! Default constructor
    //!
    GfxTest() {}

    //!
    //! Destructor
    //!
    ~GfxTest() { quit(); }

    //!
    //! Initialize test application
    //!
    bool init( const char * api )
    {
        GN::StrA libName = GN::StrA("GNgfx") + api;
        if( !mLib.load( libName.cstr() ) ) return false;
        mCreator = (GN::gfx::CreateRendererFunc)mLib.getSymbol( "GNgfxCreateRenderer" );
        if( !mCreator ) return false;

        GN::gfx::UserOptions uo;
        mRenderer.reset( mCreator(uo) );
        if( !mRenderer ) return false;

        mInput.reset( GN::input::createInputSystem() );
        if( !mInput || !mInput->attachToWindow(mRenderer->getDispDesc().windowHandle) ) return false;

        mDone = false;

        // success
        return true;
    }

    //!
    //! Quit test application
    //!
    void quit()
    {
        mInput.reset();
        mRenderer.reset();
        mCreator = 0;
        mLib.free();
    }

    //!
    //! Run test application
    //!
    int run()
    {
        if( !mRenderer )
        {
            GN_ERROR( "GfxTest is not initialized!" );
            return -1;
        }

        while(!mDone)
        {
            GN::win::processMessages( mRenderer->getDispDesc().windowHandle );
            mInput->processInputEvents();
            update();
            if( mRenderer->drawBegin() )
            {
                render();
                mRenderer->drawEnd();
            }
        }

        return 0;
    }

    //!
    //! Frame update
    //!
    void update()
    {
        const GN::input::KeyStatus * kb = mInput->getKeyboardStatus();

        mDone = kb[GN::input::KEY_ESCAPE].down;

        if( kb[GN::input::KEY_RETURN].down && kb[GN::input::KEY_RETURN].altDown() )
        {
            // toggle fullscreen mode
            GN::gfx::UserOptions uo = mRenderer->getUserOptions();
            uo.fullscreen = !uo.fullscreen;
            if( !mRenderer->changeUserOptions(uo) ) mDone = true;
        }
    }

    //!
    //! Frame render
    //!
    void render()
    {
        mRenderer->clearScreen( GN::Vector4f(0,0,1,1) ); // clear to pure blue.
    }
};

#if GN_MSWIN
#define DEFAULT_MODULE "D3D"
#else
#define DEFAULT_MODULE "OGL"
#endif

//!
//! Print usage
//!
void usage( const char * appName )
{
    printf(
        "GFX module test application.\n"
        "\n"
        "Usage: %s [D3D|OGL]\n"
        "\n"
        "Note: default module is %s\n",
        appName,
        DEFAULT_MODULE );
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
        module = DEFAULT_MODULE;
    }
    else
    {
        module = argv[1];
    }

    GfxTest app;
    if( !app.init( module ) ) return -1;
    return app.run();
}
