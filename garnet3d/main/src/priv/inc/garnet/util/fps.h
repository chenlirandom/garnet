#ifndef __GN_UTIL_FPS_H__
#define __GN_UTIL_FPS_H__
// *****************************************************************************
/// \file
/// \brief   FPS calculator
/// \author  chenli@@REDMOND (2009.1.6)
// *****************************************************************************

namespace GN { namespace util
{
    ///
    /// Simple FPS calculator
    ///
    class FpsCalculator
    {
        Clock     mClock;
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
        FpsCalculator( const wchar_t * format = L"FPS: %.2f" ) : mFormatString(format) { reset(); }

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
                static Logger * sLogger = getLogger("GN.util.fps");
                GN_VERBOSE(sLogger)( mFpsString.cptr() );
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
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_UTIL_FPS_H__
