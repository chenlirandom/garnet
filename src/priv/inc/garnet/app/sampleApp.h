#ifndef __GN_APP_SAMPLEAPP_H__
#define __GN_APP_SAMPLEAPP_H__
// *****************************************************************************
/// \file    app/sampleApp.h
/// \brief   Sample application framework
/// \author  chenlee (2006.1.7)
// *****************************************************************************

#include "garnet/GNgfx.h"
#include "garnet/GNscene.h"
#include "garnet/GNinput.h"

namespace GN { namespace app
{
    ///
    /// Simple FPS counter
    ///
    class FpsCounter
    {
        GN::Clock mClock;
        float     mFpsValue;
        StrW      mFormatString;
        StrW      mFpsString;
        size_t    mFrameCounter;
        double    mLastFrameTime;
        double    mCurrentTime;
        bool      mBeforeFirstUpdate;

    public:

        ///
        /// Constructor
        ///
        FpsCounter( const wchar_t * format = L"FPS: %.2f" ) : mFormatString(format) { reset(); }

        ///
        /// Get time
        ///
        double getCurrentTime() const { return mCurrentTime; }

        ///
        /// reset the counter
        ///
        void reset()
        {
            mFpsValue = 60.0f; // ensure non-zero FPS for the very first frame.
            mFpsString.format( mFormatString.cptr(), 0 );
            mFrameCounter = 0;
            mCurrentTime = mClock.getTimeD();
            mLastFrameTime = mCurrentTime - 1.0f/60.0f;
            mBeforeFirstUpdate = true;
        }

        ///
        /// Update the counter
        ///
        void onFrame()
        {
            mCurrentTime = mClock.getTimeD();
            ++mFrameCounter;
            double duration = mCurrentTime - mLastFrameTime;
            if( duration >= 1.0f )
            {
                mBeforeFirstUpdate = false;
                mFpsValue = (float)( mFrameCounter / duration );
                mFpsString.format( mFormatString.cptr(), mFpsValue );
                mLastFrameTime = mCurrentTime;
                mFrameCounter = 0;
            }
            else if( mBeforeFirstUpdate )
            {
                mFpsValue = (float)( (mCurrentTime - mLastFrameTime) / mFrameCounter );
                mFpsString.format( mFormatString.cptr(), mFpsValue );
            }
        }

        ///
        /// Get FPS value
        ///
        float getFps() const { return mFpsValue; }

        ///
        /// Get FPS string
        ///
        const StrW & getFpsString() const { return mFpsString; }
    };

    ///
    /// Represent raw data block
    ///
    struct RawData : public NoCopy
    {
        ///
        /// get data size
        ///
        virtual size_t size() const = 0;

        ///
        /// get data pointer
        ///
        virtual void * data() const = 0;
    };

    ///
    /// Sample application framework
    ///
    class SampleApp : public SlotBase
    {
        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        SampleApp();
        virtual ~SampleApp() { quit(); }
        //@}

        // ********************************
        // sample framework
        // ********************************
    public:

        //@{

        ///
        /// application initialization structure
        ///
        struct InitParam
        {
            gfx::RendererAPI     rapi; ///< renderer API
            gfx::RendererOptions ro;   ///< renderer options
            input::InputApi      iapi; ///< input API
            scene::FontFaceDesc  ffd;  ///< default font face descriptor
        };

        static float UPDATE_INTERVAL; ///< Time interval for calling onUpdate(), in seconds.

        virtual int  run( int argc, const char * const argv[] );
        ///
        /// \note
        ///     - argv[0] is always application name
        ///     - only non-standard/unknown argument will be send to this function.
        ///
        virtual bool onCheckCmdLine( int argc, const char * const argv[] );
        virtual bool onAppInit() { return true; }
        virtual void onAppQuit() {}
        virtual void onDetermineInitParam( InitParam & ) {}
        virtual bool onRendererCreate() { return true; }
        virtual bool onRendererRestore() { return true; }
        virtual void onRendererDispose() {}
        virtual void onRendererDestroy() {}
        virtual void onUpdate() {}
        virtual void onRender() {}
        virtual void onKeyPress( input::KeyEvent );
        virtual void onCharPress( wchar_t ) {}
        virtual void onAxisMove( input::Axis, int ) {}

        ///
        /// Return seconds since application starts
        ///
        double getCurrentTime() const { return mFps.getCurrentTime(); }

        ///
        /// Return seconds of last frame
        ///
        double getLastFrameTime() const { return mLastFrameTime; }

        ///
        /// Return time since last update
        ///
        double getTimeSinceLastUpdate() const { return mTimeSinceLastUpdate; }

        ///
        /// post exit event. Application will exit at next frame.
        ///
        void postExitEvent() { mDone = true; }

        ///
        /// switch renderer API
        ///
        bool switchRenderer();

		///
		/// show HUD or not?
		///
		void showHUD( bool show ) { mShowHUD = show; }

        ///
        /// get the FPS
        ///
        float getFps() const { return mFps.getFps(); }

        ///
        /// get font renderer
        ///
        scene::BitmapFontRenderer & getFontRenderer() { return mFontRenderer; }

        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        InitParam mInitParam;

        // time stuff
        bool mShowHUD;
        bool mShowHelp;
        FpsCounter mFps;
        double mLastFrameTime;
        double mTimeSinceLastUpdate;

        scene::BitmapFontRenderer mFontRenderer;

        bool mDone; // exit flag

        // ********************************
        // private functions
        // ********************************
    private:

        bool init( int argc, const char *  const argv[] );
        void quit();
        bool initApp();
        void quitApp();
        bool checkCmdLine( int argc, const char * const argv[] );
        bool initRenderer();
        void quitRenderer();
        bool recreateRenderer();
        bool initInput();
        void quitInput();
        bool initFont();
        void quitFont();
        void drawHUD();
    };
}}

// *****************************************************************************
//                           End of sampleApp.h
// *****************************************************************************
#endif // __GN_APP_SAMPLEAPP_H__
