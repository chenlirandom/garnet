#include "pch.h"

//
//
// -----------------------------------------------------------------------------
void
GN::detail::LogHelper::log( const char * fmt, ... )
{
    StrA s;
    va_list arglist;
    va_start( arglist, fmt );
    s.format( fmt, arglist );
    va_end( arglist );
    GN::doLog( mDesc, s.cstr() );
}

//
//
// -----------------------------------------------------------------------------
void
GN::detail::LogHelper::logc( const char * cate, const char * fmt, ... )
{
    StrA s;
    va_list arglist;
    va_start( arglist, fmt );
    s.format( fmt, arglist );
    va_end( arglist );
    mDesc.cate = cate ? cate : "";
    GN::doLog( mDesc, s.cstr() );
}

//
//
// -----------------------------------------------------------------------------
void
GN::detail::LogHelper::loglc(
    int level, const char * cate, const char * fmt, ... )
{
    StrA s;
    va_list arglist;
    va_start( arglist, fmt );
    s.format( fmt, arglist );
    va_end( arglist );
    mDesc.level = level;
    mDesc.cate = cate ? cate : "";
    GN::doLog( mDesc, s.cstr() );
}

//
//
// -----------------------------------------------------------------------------
static inline GN::StrA levelStr( int l )
{
    if( 0 <= l && l < GN::LOGLEVEL_TRACE_0 )
    {
        static const char * table[] = {
            "GN_FATAL",
            "GN_ERROR",
            "GN_WARN",
            "GN_INFO",
        };
        return table[l];
    }
    else 
    {
        return GN::strFormat( "GN_TRACE_%d", l );
    }
}

class ConsoleColor
{
#if GN_MSWIN & GN_PC
    HANDLE       mConsole;
    WORD         mAttrib;
public:
    ConsoleColor( int level )
    {
        // store console attributes
        mConsole = GetStdHandle(
            GN::LOGLEVEL_INFO <= level ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE );
        CONSOLE_SCREEN_BUFFER_INFO csbf;
        GetConsoleScreenBufferInfo( mConsole, &csbf );
        mAttrib = csbf.wAttributes;

        // change console color
        WORD attrib;
        switch( level )
        {
            case GN::LOGLEVEL_FATAL:
            case GN::LOGLEVEL_ERROR:
                attrib = FOREGROUND_RED;
                break;

            case GN::LOGLEVEL_WARN:
                attrib = FOREGROUND_RED | FOREGROUND_GREEN;
                break;

            case GN::LOGLEVEL_INFO:

            default: // GN_TRACE_###
                attrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
        }
        SetConsoleTextAttribute( mConsole, attrib | FOREGROUND_INTENSITY );
    }

    ~ConsoleColor()
    {
        // restore console attributes
        SetConsoleTextAttribute( mConsole, mAttrib );
    }


#else
public:
    ConsoleColor( int ) {}
    ~ConsoleColor()     {}
#endif
};

//
//
// -----------------------------------------------------------------------------
void GN::detail::defaultLogImpl( const LogDesc & desc, const char * msg )
{
    if( 0 == msg || 0 == msg[0] ) return;

    bool logDisabled = getEnvBoolean( "GN_LOG_DISABLED" );
    if( logDisabled ) return;

    StrA logFileName = getEnv( "GN_LOG_FILENAME" );
    bool logToScreen = !getEnvBoolean( "GN_LOG_QUIET" );

    const char * cate = desc.cate ? desc.cate : "";
    const char * file = desc.file ? desc.file : "UNKNOWN FILE";

    ConsoleColor cc(desc.level);

    if( LOGLEVEL_INFO == desc.level )
    {
        if( !logFileName.empty() )
        {
            AnsiFile fp;
            if( fp.open( logFileName, "at" ) )
            {
                fprintf( fp, "%s\n", msg );
            }
        }

        if( logToScreen )
        {
            std::cout << msg << std::endl;
        }
    }
    else
    {
        if( !logFileName.empty() )
        {
            AnsiFile fp;
            if( fp.open( logFileName, "at" ) )
            {
                ::fprintf(
                    fp,
                    "%s(%d) : %s : %s : %s\n",
                    file, desc.line,
                    cate, levelStr(desc.level).cstr(),
                    msg );
            }
        }

#if !GN_XENON // Xenon has no console output
        if( logToScreen )
        {
            // output to console
            ::fprintf(
                desc.level > GN::LOGLEVEL_INFO ? stdout : stderr,
                "%s(%d) : %s : %s : %s\n",
                file, desc.line,
                cate, levelStr(desc.level).cstr(),
                msg );
        }
#endif

        // output to debugger
#if GN_MSWIN
        char buf[16384];
        _snprintf( buf, 16384,
            "%s(%d) : %s : %s : %s\n",
            file, desc.line,
            cate, levelStr(desc.level).cstr(),
            msg );
        buf[16383] = 0;
        ::OutputDebugStringA( buf );
#endif
    }
}
