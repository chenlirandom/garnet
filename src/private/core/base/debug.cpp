#include "pch.h"

#if GN_XBOX2

#include "dxerr9.h"
#if GN_MSVC
#pragma comment( lib, "dxerr9.lib" )
#endif
#define DXERR_FUNC DXGetErrorDescription9A

#elif GN_PLATFORM_HAS_DXERR

#include "dxerr.h"
#define DXERR_FUNC DXGetErrorDescriptionA

#elif GN_MSWIN

static const char * DXERR_FUNC( sint32 error )
{
    __declspec(thread) static char text[] = "0x12345678";
    for(int i = 0; i < 8; ++i)
    {
        char ch = '0' + ((error >> (28-i*4)) & 0xF);
        text[2+i] = ch;
    }
    return text;
}

#else

static const char * DXERR_FUNC( sint32 )
{
    return "";
}

#endif


// Runtime assert behaviour flag.
static GN::RuntimeAssertBehavior sRuntimeAssertBehavior = GN::RAB_BREAK_ALWAYS;

// Assert failure routine
static GN::AssertFailuerUserRoutine sAssertFailureUserRoutine = NULL;
static void * sAssertFailureUserContext = NULL;

//
//
// -----------------------------------------------------------------------------
GN_API GN::RuntimeAssertBehavior GN::setRuntimeAssertBehavior( RuntimeAssertBehavior rab )
{
    RuntimeAssertBehavior old = sRuntimeAssertBehavior;
    sRuntimeAssertBehavior = rab;
    return old;
}

//
//
// -----------------------------------------------------------------------------
GN_API void
GN::setAssertFailerUserRoutine(
    AssertFailuerUserRoutine   newRoutine,
    void                     * newUserContext,
    AssertFailuerUserRoutine * oldRoutine,
    void                    ** oldUserContext )
{
    AssertFailuerUserRoutine oldR = sAssertFailureUserRoutine;
    void                   * oldC = sAssertFailureUserContext;
    sAssertFailureUserRoutine = newRoutine;
    sAssertFailureUserContext = newUserContext;

    if( oldRoutine ) *oldRoutine = oldR;
    if( oldUserContext ) *oldUserContext = oldC;
}

//
//
// -----------------------------------------------------------------------------
GN_API void
GN::internal::handleAssertFailure(
    const char * msg,
    const char * file,
    int          line,
    bool *       ignoreForever ) throw()
{
    if( RAB_CALL_USER_ROUTINE == sRuntimeAssertBehavior && NULL != sAssertFailureUserRoutine )
    {
        sAssertFailureUserRoutine( sAssertFailureUserContext, msg, file, line, ignoreForever );
        return;
    }

    if( RAB_SILENCE == sRuntimeAssertBehavior ) return;

    ::fprintf(
        stderr,
        "\n"
        "======================== ASSERT FAILURE ========================\n"
        "%s(%d)\n"
        "----------------------------------------------------------------\n"
        "%s\n"
        "================================================================\n",
        file?file:"",
        line,
        msg?msg:"" );

    if( RAB_LOG_ONLY == sRuntimeAssertBehavior ) return;

    bool debuggerBreak = false;

    if( RAB_BREAK_ALWAYS == sRuntimeAssertBehavior )
    {
        debuggerBreak = true;
    }
    else
    {
    #if GN_WINPC
        char buf[1024];
        str::formatTo( buf, 1024,
            "%s(%d)\n"
            "%s\n\n"
            "Break into debugger?\n"
            "(If canceled, this specific assert failure will not trigger debug break anymore)",
            file?file:"", line, msg?msg:"" );
        int ret = ::MessageBoxA(
            0,
            buf,
            "Assert Failure",
            MB_YESNOCANCEL|MB_ICONQUESTION
            );

        *ignoreForever = ( IDCANCEL == ret );
        debuggerBreak = IDYES == ret;
    #else
        if( *ignoreForever ) *ignoreForever = false;
        debuggerBreak = true;
    #endif
    }

    if( debuggerBreak )
    {
        GN::breakIntoDebugger();
    }
}

//
//
// -----------------------------------------------------------------------------
GN_API void GN::breakIntoDebugger()
{
#if GN_MSVC
	::DebugBreak();
#elif GN_GCC
    asm("int $3");
#else
#error "Unsupport compiler!"
#endif
}

//
//
// -----------------------------------------------------------------------------
GN_API void GN::printToDebugger(const char * message)
{
#if GN_MSWIN
	::OutputDebugStringA(message);
#else
    fprintf(stderr, "%s", message);
#endif
}

#if GN_MSWIN

//
//
// -----------------------------------------------------------------------------
GN_API const char *
GN::getWin32ErrorInfo( uint32 win32ErrorCode ) throw()
{
    static char info[4096];

#if GN_XBOX2
    // TODO: unimplemented
    info[0] = 0;
#else
    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        win32ErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        info,
        4096,
        NULL );
    info[4095] = 0;

    // 除去信息末尾多余的回车符
    size_t n = str::length(info);
    while( n > 0 && '\n' != info[n-1] )
    {
        --n;
    }
    info[n] = 0;
#endif

    // success
    return info;
}

//
//
// -----------------------------------------------------------------------------
GN_API const wchar_t *
GN::getWin32ErrorInfoW( uint32 win32ErrorCode ) throw()
{
    static wchar_t info[4096];

#if GN_XBOX2
    // TODO: unimplemented
    info[0] = 0;
#else
    ::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        win32ErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        info,
        4096,
        NULL );
    info[4095] = 0;

    // 除去信息末尾多余的回车符
    size_t n = str::length(info);
    while( n > 0 && L'\n' != info[n-1] )
    {
        --n;
    }
    info[n] = 0;
#endif

    // success
    return info;
}

//
//
// -----------------------------------------------------------------------------
GN_API const char *
GN::getWin32LastErrorInfo() throw()
{
    return getWin32ErrorInfo( ::GetLastError() );
}

#endif


//
//
// -----------------------------------------------------------------------------
GN_API const char *
GN::getDXErrorInfo( sint32 hr ) throw()
{
    return DXERR_FUNC( hr );
}

//
//
// -----------------------------------------------------------------------------
GN_API const char *
GN::errno2str( int err )
{
    switch( err )
    {
        case EPERM        : return "Operation not permitted";
        case ENOENT       : return "No such file or directory";
        case ESRCH        : return "No such process";
        case EINTR        : return "Interrupted function";
        case EIO          : return "I/O error";
        case ENXIO        : return "No such device or address";
        case E2BIG        : return "Argument list too long";
        case ENOEXEC      : return "Exec format error";
        case EBADF        : return "Bad file number";
        case ECHILD       : return "No spawned processes";
        case EAGAIN       : return "No more processes or not enough memory or maximum nesting level reached";
        case ENOMEM       : return "Not enough memory";
        case EACCES       : return "Permission denied";
        case EFAULT       : return "Bad address";
        case EBUSY        : return "Device or resource busy";
        case EEXIST       : return "File exists";
        case EXDEV        : return "Cross-device link";
        case ENODEV       : return "No such device";
        case ENOTDIR      : return "Not a directory";
        case EISDIR       : return "Is a directory";
        case EINVAL       : return "Invalid argument";
        case ENFILE       : return "Too many files open in system";
        case EMFILE       : return "Too many open files";
        case ENOTTY       : return "Inappropriate I/O control operation";
        case EFBIG        : return "File too large";
        case ENOSPC       : return "No space left on device";
        case ESPIPE       : return "Invalid seek";
        case EROFS        : return "Read-only file system";
        case EMLINK       : return "Too many links";
        case EPIPE        : return "Broken pipe";
        case EDOM         : return "Math argument";
        case ERANGE       : return "Result too large";
        case EDEADLK      : return "Resource deadlock would occur";
        case ENAMETOOLONG : return "Filename too long";
        case ENOLCK       : return "No locks available";
        case ENOSYS       : return "Function not supported";
        case ENOTEMPTY    : return "Directory not empty";
        case EILSEQ       : return "Illegal byte sequence";
        default           : return "Unknown error.";
    }
}
