#include "pch.h"
#include "garnet/GNapp.h"
#define SO_ASSERT GN_ASSERT
#include <SimpleOpt.h>

float GN::app::SampleApp::UPDATE_INTERVAL = 1.0f/60.0f;

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::app::SampleApp::SampleApp() : mShowHUD(true)
{
    enableCRTMemoryCheck();
    mFps.reset();
}

//
//
// -----------------------------------------------------------------------------
int GN::app::SampleApp::run( int argc, const char * const argv[] )
{
    GN_GUARD_ALWAYS;

    if( !init( argc, argv ) ) return -1;

    mDone = false;

    double elapsedUpdateTime;
    double lastUpdateTime = mFps.getCurrentTime();

    while( !mDone )
    {
        // process user input
        GN::win::processWindowMessages( gRenderer.getDispDesc().windowHandle, true );
        gInput.processInputEvents();

        // update time stuff
        mFps.onFrame();

        // call update in fixed interval
        elapsedUpdateTime = mFps.getCurrentTime() - lastUpdateTime;
        if( elapsedUpdateTime > UPDATE_INTERVAL )
        {
            int count = (int)( elapsedUpdateTime / UPDATE_INTERVAL );
            static const int MAX_COUNT = (int)( 1.0f / UPDATE_INTERVAL );
            if( count > MAX_COUNT ) count = MAX_COUNT; // make easy of long time debug break.
            for( int i = 0; i < count; ++i )
                onUpdate();
            lastUpdateTime += UPDATE_INTERVAL * count;
        }

        // do render
        if( gRenderer.drawBegin() )
        {
            mLastFrameTime = 1.0 / mFps.getFps();
            mTimeSinceLastUpdate = mFps.getCurrentTime() - lastUpdateTime;
            onRender();
            drawHUD();
            gRenderer.drawEnd();
        }
    }

    // success
    quit();
    return 0;

    // failed
    GN_UNGUARD_ALWAYS_NO_THROW;
    quit();
    return -1;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::onCheckCmdLine( int argc, const char * const argv[] )
{
    if( argc > 1 )
    {
        StrA s = "unknown command line arguments:";
        for( int i = 1; i < argc; ++i )
        {
            s += " ";
            s += argv[i];
        }
        GN_WARN( s.cptr() );
    }
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::onKeyPress( input::KeyEvent ke )
{
    if( input::KEY_ESCAPE == ke.code && !ke.status.down ) mDone = true;
    if( input::KEY_R == ke.code && !ke.status.down ) reloadResources();
    if( input::KEY_RETURN == ke.code && ke.status.down && ke.status.altDown() )
    {
        GN::gfx::RendererOptions ro = gRenderer.getOptions();
        ro.fullscreen = !ro.fullscreen;
        if( !gRenderer.changeOptions(ro) ) postExitEvent();
    }
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::switchRenderer()
{
    GN_GUARD;

    mInitParam.rapi = (gfx::RendererAPI)((mInitParam.rapi+1)%gfx::NUM_RENDERER_API);
    return recreateRenderer();

    GN_UNGUARD;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::init( int argc, const char * const argv[] )
{
    GN_GUARD_ALWAYS;

    if( !checkCmdLine(argc,argv) ) { quit(); return false; }
    if( !initApp() ) { quit(); return false; }
    onDetermineInitParam( mInitParam );
    if( !initRenderer() ) { quit(); return false; }
    if( !initInput() ) { quit(); return false; }

    // success
    return true;

    // failed
    GN_UNGUARD_ALWAYS_NO_THROW;
    quit(); return false;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quit()
{
    GN_GUARD_ALWAYS;

    quitRenderer();
    quitInput();
    quitApp();

    GN_UNGUARD_ALWAYS_NO_THROW;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::checkCmdLine( int argc, const char * const argv[] )
{
    GN_GUARD;

#if GN_XENON
    GN_UNUSED_PARAM( argc );
    GN_UNUSED_PARAM( argv );
    mInitParam.rapi = gfx::API_D3D9;
    mInitParam.ro = gfx::RendererOptions();
    mInitParam.iapi = input::API_NATIVE;
    if( !onCheckCmdLine( argc, argv ) ) return false;
#else
    // setup default parameters
    mInitParam.rapi = gfx::API_AUTO;
    mInitParam.ro = gfx::RendererOptions();
    mInitParam.ro.windowedWidth = 640;
    mInitParam.ro.windowedHeight = 480;
    mInitParam.iapi = input::API_NATIVE;

    DynaArray<const char*> unknownArgs;
    unknownArgs.append( argv[0] );

    static const CSimpleOptA::SOption sOptions[] = {
        { 0, "-d3d9",  SO_NONE    },
        { 0, "-d3d10", SO_NONE    },
        { 0, "-ogl",   SO_NONE    },
        { 0, "-fake",  SO_NONE    },
        { 0, "-fs",    SO_NONE    },
        { 0, "-ref",   SO_NONE    },
        { 0, "-msaa",  SO_NONE    },
        { 0, "-pure",  SO_NONE    },
        { 0, "-m",     SO_REQ_SEP }, // specify monitor index
        { 0, "-di",    SO_NONE    },
        { 1, "-?",     SO_NONE    },
        { 1, "-h",     SO_NONE    },
        { 1, "--help", SO_NONE    },
        SO_END_OF_OPTIONS            // END
    };

    CSimpleOptA so( argc, (char**)argv, sOptions );

    while( so.Next() )
    {
        const char * a = so.OptionText();
        if( SO_SUCCESS == so.LastError() )
        {
            if( 1 == so.OptionId() )
            {
                GN_INFO( "\n"
                    "Usage : %s [options]\n\n"
                    "Options : (options are case-insensitive)\n"
                    "    -h, -?, --help         : Show help screen.\n"
                    "    -d3d9/d3d10/ogl/fake   : Select rendering API.\n"
                    "    -fs                    : Use fullsreen mode.\n"
                    "    -ref                   : Use reference device.\n"
                    "    -msaa                  : Enable MSAA/FSAA.\n"
                    "    -pure                  : Use pure device (D3D only).\n"
                    "    -m [num]               : Specify monitor index. Default is 0.\n"
                    "    -di                    : Use direct input.\n"
                    , GN::path::baseName(argv[0]).cptr() );
                return false;
            }
            else if( 0 == strCmpI( a, "-d3d9" ) ) mInitParam.rapi = gfx::API_D3D9;
            else if( 0 == strCmpI( a, "-d3d10" ) ) mInitParam.rapi = gfx::API_D3D10;
            else if( 0 == strCmpI( a, "-ogl" ) ) mInitParam.rapi = gfx::API_OGL;
            else if( 0 == strCmpI( a, "-fs" ) ) mInitParam.ro.fullscreen = true;
            else if( 0 == strCmpI( a, "-ref" ) ) mInitParam.ro.reference = true;
            else if( 0 == strCmpI( a, "-fake" ) ) mInitParam.rapi = gfx::API_FAKE;
            else if( 0 == strCmpI( a, "-msaa" ) ) mInitParam.ro.msaa = GN::gfx::MSAA_ULTRA;
            else if( 0 == strCmpI( a, "-pure" ) ) mInitParam.ro.pure = true;
            else if( 0 == strCmpI( a, "-m" ) )
            {
                uint32_t idx;
                if( !str2Uint32( idx, so.OptionArg() ) )
                {
                    GN_ERROR( "monitor index must be integer." );
                    return false;
                }
                mInitParam.ro.monitorHandle = win::getMonitorByIndex( idx );
            }
            else if( 0 == strCmpI( a, "-di") ) mInitParam.iapi = input::API_DINPUT;
        }
        else
        {
            unknownArgs.append( a );
        }
    }

    // handle unknown arguments
    for( int i = 0; i < so.FileCount(); ++i )
    {
        unknownArgs.append( so.File( i ) );
    }
    if( !onCheckCmdLine( (int)unknownArgs.size(), unknownArgs.cptr() ) ) return false;
#endif

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initApp()
{
    GN_GUARD;

    if( !onAppInit() ) return false;

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitApp()
{
    GN_GUARD;

    onAppQuit();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initRenderer()
{
    GN_GUARD;

    // connect to renderer signals
    GN::gfx::Renderer::sSigCreate.connect( this, &SampleApp::onRendererCreate );
    GN::gfx::Renderer::sSigRestore.connect( this, &SampleApp::onRendererRestore );
    GN::gfx::Renderer::sSigDispose.connect( this, &SampleApp::onRendererDispose );
    GN::gfx::Renderer::sSigDestroy.connect( this, &SampleApp::onRendererDestroy );
    GN::gfx::Renderer::sSigWindowClosing.connect( this, &SampleApp::postExitEvent );

    // create renderer
    return recreateRenderer();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitRenderer()
{
    GN_GUARD;

    // delete Renderer
    GN::gfx::deleteRenderer();

    // disconnect to renderer signals
    GN::gfx::Renderer::sSigDestroy.disconnect( this );
    GN::gfx::Renderer::sSigDispose.disconnect( this );
    GN::gfx::Renderer::sSigRestore.disconnect( this );
    GN::gfx::Renderer::sSigCreate.disconnect( this );
    GN::gfx::Renderer::sSigWindowClosing.disconnect( this );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::recreateRenderer()
{
    GN_GUARD;

    // (re)create renderer
    GN::gfx::Renderer * r = gfx::createRenderer( mInitParam.rapi );
    if( NULL == r ) return false;
    if( !r->changeOptions( mInitParam.ro ) ) return false;

    // reattach input window
    const GN::gfx::DispDesc & dd = r->getDispDesc();
    if( gInputPtr && !gInput.attachToWindow( dd.displayHandle,dd.windowHandle ) )
    {
        return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initInput()
{
    GN_GUARD;

    // release old input system
    quitInput();

    // create INPUT system
    GN::input::Input * input = GN::input::createInputSystem( mInitParam.iapi );
    if( 0 == input ) return false;

    if( gRendererPtr )
    {
        const GN::gfx::DispDesc & dd = gRenderer.getDispDesc();
        if( !input->attachToWindow(dd.displayHandle,dd.windowHandle) ) return false;
    }

    // connect to input signals
    input->sigKeyPress.connect( this, &SampleApp::onKeyPress );
    input->sigCharPress.connect( this, &SampleApp::onCharPress );
    input->sigAxisMove.connect( this, &SampleApp::onAxisMove );

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitInput()
{
    GN_GUARD;

    if( gInputPtr ) delete gInputPtr;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::drawHUD()
{
    GN_GUARD_SLOW;

    using namespace GN::gfx;

    if( mShowHUD )
    {
        Renderer & r = gRenderer;
        r.drawDebugText( mFps.getFpsString(), 0, 0 );
    }

    GN_UNGUARD_SLOW;
}
