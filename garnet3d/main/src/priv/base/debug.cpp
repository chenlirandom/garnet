#include "pch.h"

//
//
// -----------------------------------------------------------------------------
bool
GN::assertFunc(
    const char * msg,
    const char * file,
    int          line,
    bool *       ignore ) throw()
{
#if GN_PC
#if GN_MSWIN
    char buf[1024];
    strFormat( buf, 1023,
        "%s(%d)\n"
        "%s\n\n"
        "ת���������\n"
        "�����ѡ��Cancel�����Ժ����Assertʧ��ʱ�����ٵ����Ի���",
        file?file:"", line, msg?msg:"" );
    buf[1023] = 0;
    int ret = ::MessageBoxA(
        0,
        buf,
        "Assertʧ��",
        MB_YESNOCANCEL|MB_ICONQUESTION
        );

    if(ignore) *ignore = ( IDCANCEL == ret );
    return IDYES == ret;
#else
    ::fprintf(
        stderr,
        "\n"
        "======================== ASSERT FAILURE ========================\n"
        "%s(%d)\n"
        "----------------------------------------------------------------\n"
        "%s\n"
        "----------------------------------------------------------------\n"
        "              Break(B), Continue(C), Ignore(I)\n"
        "      (Choose Ignore to suppress further assert warnings)\n"
        "================================================================\n",
        file?file:"",
        line,
        msg?msg:"" );
    if( *ignore ) *ignore = false;
    int  ch = 0;
    for(;;)
    {
        ch = ::getc(stdin);
        if( 'b' == ch || 'B' == ch )
        {
            return true;
        }
        if( 'c' == ch || 'C' == ch )
        {
            return false;
        }
        if( 'i' == ch || 'I' == ch )
        {
            if( *ignore ) *ignore = true;
            return false;
        }
    }
#endif
#else
    GN_UNUSED_PARAM( msg );
    GN_UNUSED_PARAM( file );
    GN_UNUSED_PARAM( line );
    if( *ignore ) *ignore = false;
    return true;
#endif
}

#if !GN_X86
//
//
// -----------------------------------------------------------------------------
void GN::debugBreak()
{
#if GN_MSVC
	::DebugBreak();
#else
#error "Debug break unimplemented!"
#endif
}
#endif

//
//
// -----------------------------------------------------------------------------
#if GN_MSWIN
const char *
GN::getOSErrorInfo() throw()
{
    static char info[4096];

#if GN_XENON
    // TODO: unimplemented
    info[0] = 0;
#else
    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        ::GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        info,
        4096,
        NULL );
    info[4095] = 0;

    // ��ȥ��Ϣĩβ����Ļس���
    size_t n = strLen(info);
    while( n > 0 && '\n' != info[n-1] )
    {
        --n;
    }
    info[n] = 0;
#endif

    // success
    return info;
}
#endif
