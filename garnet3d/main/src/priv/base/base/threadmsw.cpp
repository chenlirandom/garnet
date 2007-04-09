#include "pch.h"
#include "garnet/base/thread.h"

#if GN_MSWIN

static GN::Logger * sLogger = GN::getLogger("GN.base.Thread");

using namespace GN;

// *****************************************************************************
// local class and functions
// *****************************************************************************


///
/// convert seconds to milliseconds on MS windows platform
///
UInt32 sec2usec( float time )
{
    if( INFINITE_TIME == time ) return INFINITE;
    else return (UInt32)( time * 1000000.0f );
}

static int sPriorityTable[] =
{
    THREAD_PRIORITY_TIME_CRITICAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_IDLE,
};
GN_CASSERT( GN_ARRAY_COUNT(sPriorityTable) == NUM_THREAD_PRIORITIES );

///
/// convert thread priority to WIN32 constant
///
static int sPriority2Msw( ThreadPriority p )
{
    GN_ASSERT( p < NUM_THREAD_PRIORITIES );

    return sPriorityTable[p];
}

///
/// thread class on MS Windows
///
class ThreadMsw : public Thread, public StdClass
{
    GN_DECLARE_STDCLASS( ThreadMsw, StdClass );

    // ********************************
    // ctor/dtor
    // ********************************

    //@{
public:
    ThreadMsw()          { clear(); }
    virtual ~ThreadMsw() { quit(); }
    //@}

    // ********************************
    // from StdClass
    // ********************************

    //@{
public:
    bool init(
        const ThreadProcedure & proc,
        void * param,
        ThreadPriority priority,
        bool initialSuspended,
        const char * )
    {
        GN_GUARD;

        // standard init procedure
        GN_STDCLASS_INIT( ThreadMsw, () );

        // check parameter
        if( priority < 0 || priority >= NUM_THREAD_PRIORITIES )
        {
            GN_ERROR(sLogger)( "invalid thread priority." );
            return failure();
        }

        mProc = proc;
        mParam.instance = this;
        mParam.userparam = param;
        mPriority = priority;

        mHandle = ::CreateThread(
            0, // security
            0, // default stack size
            &sProcDispatcher,
            &mParam,
            initialSuspended ? CREATE_SUSPENDED : 0,
            &mId );
        GN_MSW_CHECK_RV( mHandle, failure() );

        // success
        return success();

        GN_UNGUARD;
    }

    void quit()
    {
        GN_GUARD;

        // standard quit procedure
        GN_STDCLASS_QUIT();

        GN_UNGUARD;
    }

private:
    void clear()
    {
        mHandle = 0;
        mId = 0;
    }
    //@}

    // ********************************
    // inherited from Thread
    // ********************************
public:

    virtual ThreadPriority getPriority() const
    {
        return mPriority;
    }

    virtual void setPriority( ThreadPriority p )
    {
        if( p < 0 || p >= NUM_THREAD_PRIORITIES )
        {
            GN_ERROR(sLogger)( "invalid thread priority!" );
            return;
        }

        GN_MSW_CHECK_R( ::SetThreadPriority( mHandle, sPriority2Msw(p) ) );

        mPriority = p;
    }

    bool isCurrentThread() const
    {
        return ::GetCurrentThreadId() == mId;
    }

    virtual void suspend()
    {
        if( (DWORD)-1 == ::SuspendThread( mHandle ) )
        {
            GN_ERROR(sLogger)( getOSErrorInfo() );
        }
    }

    virtual void resume()
    {
        if( (DWORD)-1 == ::ResumeThread( mHandle ) )
        {
            GN_ERROR(sLogger)( getOSErrorInfo() );
        }
    }

    virtual bool waitForTermination( float seconds, UInt32 * threadProcReturnValue )
    {
        // can't wait for self termination
        GN_ASSERT( !isCurrentThread() );

        UInt32 ret = ::WaitForSingleObject( mHandle, sec2usec( seconds ) );

        if( WAIT_TIMEOUT == ret )
        {
            GN_TRACE(sLogger)( "time out!" );
            return false;
        }
        else if( WAIT_OBJECT_0 == ret )
        {
            if( threadProcReturnValue )
            {
                GN_MSW_CHECK( GetExitCodeThread( mHandle, (LPDWORD)threadProcReturnValue ) );
            }
            return true;
        }
        else
        {
            GN_ERROR(sLogger)( getOSErrorInfo() );
            return false;
        }
    }

    // ********************************
    // private variables
    // ********************************
private:

    struct ThreadParam
    {
        ThreadMsw * instance;
        void      * userparam;
    };

    ThreadProcedure mProc;
    ThreadParam     mParam;
    ThreadPriority  mPriority;

    HANDLE mHandle;
    DWORD  mId;

    // ********************************
    // private functions
    // ********************************
private:

    ///
    /// thread procedure dispather
    ///
    static DWORD WINAPI sProcDispatcher( void * parameter )
    {
        GN_ASSERT( parameter );

        ThreadParam * p = (ThreadParam*)parameter;

        GN_ASSERT( p->instance );

        return p->instance->mProc( p->userparam );
    };

};

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::Thread *
GN::createThread(
    const ThreadProcedure & proc,
    void * param,
    ThreadPriority priority,
    bool initialSuspended,
    const char * name )
{
    GN_GUARD;

    AutoObjPtr<ThreadMsw> s( new ThreadMsw );

    if( !s->init( proc, param, priority, initialSuspended, name ) ) return 0;

    return s.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::sleepCurrentThread( float seconds )
{
   ::Sleep( sec2usec( seconds ) );
}

#endif
