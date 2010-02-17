#ifndef __GN_BASE_LOG_H__
#define __GN_BASE_LOG_H__
// *****************************************************************************
/// \file
/// \brief   log functions and macros
/// \author  chenlee (2005.4.17)
// *****************************************************************************

/// General log macros, with user specified source code location
//@{
#if GN_BUILD_ENABLE_LOG
#define GN_LOG_EX( logger, level, func, file, line ) if( logger->IsOff( level ) ) {} else GN::Logger::LogHelper( logger, level, func, file, line ).DoLog
#define GN_LOG_BEGIN( logger, level )                if( logger->IsOn( level ) ) {
#define GN_LOG_END()                                 }
#else
#define GN_LOG_EX( logger, level, func, file, line ) if( 1 ) {} else GN::Logger::sFakeLog
#define GN_LOG_BEGIN( logger, level )                if(0) {
#define GN_LOG_END()                                 }
#endif
//@}

///
/// General DoLog macro, with automatic source code location
///
#define GN_LOG( logger, level ) GN_LOG_EX( logger, level, GN_FUNCTION, __FILE__, __LINE__ )

///
/// output fatal error message
///
#define GN_FATAL( logger )  GN_LOG( logger, GN::Logger::FATAL )

///
/// output error message
///
#define GN_ERROR( logger )  GN_LOG( logger, GN::Logger::ERROR_ )

///
/// output warning message
///
#define GN_WARN( logger )  GN_LOG( logger, GN::Logger::WARN )

///
/// output informational message
///
#define GN_INFO( logger )  GN_LOG( logger, GN::Logger::INFO )

///
/// output verbose message
///
#define GN_VERBOSE( logger ) GN_LOG( logger, GN::Logger::VERBOSE )

///
/// output very-verbose message
///
#define GN_VVERBOSE( logger ) GN_LOG( logger, GN::Logger::VVERBOSE )

///
/// Debug only log macros (no effect to non-debug build)
///
//@{
#if GN_BUILD_DEBUG
#define GN_TRACE( logger )   GN_INFO( logger )
#define GN_VTRACE( logger )  GN_VERBOSE( logger )
#define GN_VVTRACE( logger ) GN_VVERBOSE( logger )
#else
#define GN_TRACE( logger )   if( 1 ) {} else ::GN::Logger::sFakeLog
#define GN_VTRACE( logger )  if( 1 ) {} else ::GN::Logger::sFakeLog
#define GN_VVTRACE( logger ) if( 1 ) {} else ::GN::Logger::sFakeLog
#endif
//@}

namespace GN
{
    ///
    /// Logger class
    ///
    class Logger
    {
    public:

        ///
        /// logging level
        ///
        enum LogLevel
        {
            FATAL    = 10,  ///< fatal error message
            ERROR_   = 20,  ///< error message (Note: ERROR is define as a macro)
            WARN     = 30,  ///< warning message
            INFO     = 40,  ///< informational message
            VERBOSE  = 50,  ///< verbose message
            VVERBOSE = 60,  ///< very verbose message
        };

        ///
        /// Log description structure
        ///
        struct LogDesc
        {
            int          level; ///< Log level/severity (required)
            const char * func;  ///< Log location: function name (optional). Set to NULL if you don't need it.
            const char * file;  ///< Log location: file name (optional). Set to NULL if you don't need it.
            int          line;  ///< Log location: line number (optional). Set to NULL if you don't need it.

            ///
            /// Default constructor. Do nothing.
            ///
            LogDesc() {}

            ///
            /// Construct DoLog descriptor
            ///
            LogDesc(
                int          lvl_,
                const char * func_,
                const char * file_,
                int          line_ )
                : level(lvl_)
                , func(func_)
                , file(file_)
                , line(line_)
            {}
        };

        ///
        /// DoLog helper
        ///
        struct LogHelper
        {
            Logger * mLogger; ///< Logger instance pointer
            LogDesc  mDesc;   ///< Logging descriptor

        public:

            ///
            /// Construct DoLog helper
            ///
            LogHelper( Logger * logger, int level, const char * func, const char * file, int line )
                : mLogger(logger), mDesc(level,func,file,line)
            {
                GN_ASSERT( mLogger );
            }

            ///
            /// Do DoLog
            ///
            void DoLog( const char * fmt, ... );

            ///
            /// Do DoLog (UNICODE)
            ///
            void DoLog( const wchar_t * fmt, ... );
        };

        ///
        /// Log message receiver
        ///
        struct Receiver
        {
            ///
            /// virtual destructor
            ///
            virtual ~Receiver() {}

            ///
            /// deal with incoming log message
            ///
            virtual void OnLog( Logger &, const LogDesc &, const StrA & ) = 0;

            ///
            /// deal with incoming UNICODE log message
            ///
            virtual void OnLog( Logger &, const LogDesc &, const StrW & ) = 0;
        };

        ///
        /// virtual destructor
        ///
        virtual ~Logger() {}

        ///
        /// Do log
        ///
        virtual void DoLog( const LogDesc & desc, const StrA & msg ) = 0;

        ///
        /// Do log (UNICODE)
        ///
        virtual void DoLog( const LogDesc & desc, const StrW & msg ) = 0;

        ///
        /// change logger level.
        ///
        /// \param level    Log level.
        ///     - level>0 : output log message with level <= this value.
        ///     - level=0 : muted
        ///     - level<0 : output log messge with level == -(this value)
        ///
        virtual void SetLevel( int level ) = 0;

        ///
        /// enable or disable this logger
        ///
        virtual void SetEnabled( bool ) = 0;

        /// \name receiver management
        //@{
        virtual void AddReceiver( Receiver * r ) = 0;
        virtual void RemoveReceiver( Receiver * r ) = 0;
        virtual void RemoveAllReceivers() = 0;
        //@}

        ///
        /// get logger name
        ///
        const StrA & GetName() const { return mName; }

        ///
        /// get logger level
        ///
        int GetLevel() const { return mLevel; }

        ///
        /// get logger enable/disable status
        ///
        bool isEnabled() const { return mEnabled; }

        ///
        /// is logging in effective?
        ///
        bool IsOn( int level ) const
        {
            GN_ASSERT( level > 0 );
            return
                mEnabled &&
                0 != mLevel &&
                ( level <= mLevel || level == -mLevel );
        }

        ///
        /// is logging muted?
        ///
        bool IsOff( int level ) const
        {
            GN_ASSERT( level > 0 );
            return
                !mEnabled ||
                0 == mLevel ||
                ( level > mLevel && level != -mLevel );
        }

        ///
        /// Fake logging. Do nothing.
        ///
        static inline void sFakeLog(...) {}

    protected:

        ///
        /// protective constructor
        ///
        Logger( const StrA & name ) : mName(name) {}

        int  mLevel;   ///< DoLog level
        bool mEnabled; ///< logger enabled or not.

    private:
        StrA mName;
    };

    /// \name Global DoLog functions
    //@{

    ///
    /// Get logger by name. Set name to NULL or empty string to get root logger.
    ///
    /// \param name
    ///     Logger name (case insensitive)
    ///
    GN_PUBLIC Logger * GetLogger( const char * name );

    ///
    /// Get root logger
    ///
    inline Logger * GetRootLogger() { return GetLogger( 0 ); }

    //@}
} // end of namespace GN


// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_LOG_H__
