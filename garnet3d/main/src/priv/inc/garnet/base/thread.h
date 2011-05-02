#ifndef __GN_BASE_THREAD_H__
#define __GN_BASE_THREAD_H__
// *****************************************************************************
/// \file
/// \brief   platform independent thread wrapper
/// \author  chen@@CHENLI-HOMEPC (2007.4.8)
// *****************************************************************************

namespace GN
{
    ///
    /// abstract thread interface
    ///
    struct Thread
    {
        //@{

        /// Thread ID.
        ///
        /// Unless we have run out of all identifier values (which is very unlikly),
        /// thread id is never reused in one OS life time. And you can always use
        /// the id to query the status of the thread you want, without worrying about
        /// it being redirected to another thread suddenly.
        typedef uint64 Identifier;

        /// Thread priority
        enum Priority
        {
            REALTIME,       ///< The highest priority, for time-critical task only.
            HIGH,           ///< high priority
            NORMAL,         ///< normal priority, suitable for most of application.
            LOW,            ///< low priority
            IDLE,           ///< The lowest priority
            NUM_PRIORITIES, ///< number of thread priorities
        };

        ///
        /// thread procedure functor
        ///
        typedef Delegate1<void,void*> Procedure;

        //@}

        /// \name thread operations
        //@{

        /// Create a new thread. Return 0 on failure.
        ///
        /// \note
        ///     All resources associated with the thread will
        ///     be released automatically when the thread exits normally.
        ///     So there's no need to explictly "destroy" a thread ID.
        static Identifier sCreate(
            const Procedure & proc,
            void            * param,
            const char      * name = 0 );

        /// Terminate a thread by force.
        /// Note that killing a thread could potentially harm the whole process.
        /// So use this function only as the last resort.
        static void sKill( Identifier );

        static void sSleepCurrentThread( TimeInNanoSecond sleepTime );

        /// Wait for termination of the thread (join operation).
        /// COMPLETED: the thread is terminated, either normally or being killed.
        /// TIMEOUT  : time out before the thread is either terminated or killed; or the caller is current thread.
        /// FAILED   : wait operation failed for unspecified reason.
        static WaitResult sWaitForTermination(
            Identifier       thread,
            TimeInNanoSecond timeoutTime = INFINITE_TIME );

        /// Alias for sWaitForTermination
        static WaitResult sJoin(
            Identifier       thread,
            TimeInNanoSecond timeoutTime = INFINITE_TIME )
        {
            return sWaitForTermination( thread, timeoutTime );
        }

        //@}

        /// \name thread properties
        //@{
        static Identifier sGetCurrentThread();
        static bool       sIsCurrentThread( Identifier );
        static Priority   sGetPriority( Identifier ); ///< Return NORMAL if the thread ID is invalid.
        static void       sSetPriority( Identifier, Priority );
        static void       sSetAffinity( Identifier, uint32 hardwareThread ); //< Meaning of hardwareThread is platform specific.
        //@}
    };
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_THREAD_H__
