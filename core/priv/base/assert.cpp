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
#if GN_WINPC
    char buf[1024];
    ::_snprintf( buf, 1023,
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
}

//
//
// -----------------------------------------------------------------------------
void GN::debugBreak()
{
	::DebugBreak();
}

//
//
// -----------------------------------------------------------------------------
#if GN_WINNT
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
