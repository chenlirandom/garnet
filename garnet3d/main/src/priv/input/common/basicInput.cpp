#include "pch.h"
#include "basicInput.h"
#include <algorithm>
#include <mbstring.h>

//
// �Ƿ���һ�������ַ���ͷ����ֽ�
// -----------------------------------------------------------------------------
static GN_INLINE bool sIsLeadingByte( char ch )
{
    return 0 != ::_ismbblead( ch );
}

//
// �Ƿ���һ�������ַ��ĺ����ֽ�
// -----------------------------------------------------------------------------
static GN_INLINE bool sIsTrailingByte( char ch )
{
    return 0 != ::_ismbbtrail( ch );
}

// *****************************************************************************
//                  protected functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::input::BasicInput::triggerKeyPress( KeyCode code, bool keydown )
{
    GN_GUARD;

    // �������ԳƵļ�
    if( keydown == mKeyStates[code] )
    {
        if( keydown )
        {
            // insert addtional keyup, between consecutive keydown(s)
            triggerKeyPress( code, false );
        }
        else
        {
            // ignore redundent keyup(s)
            return;
        }
    }

    // ����״̬���ı�־
    if     ( KEY_LCTRL  == code ) mKeyFlags.lctrl  = keydown;
    else if( KEY_RCTRL  == code ) mKeyFlags.rctrl  = keydown;
    else if( KEY_LALT   == code ) mKeyFlags.lalt   = keydown;
    else if( KEY_RALT   == code ) mKeyFlags.ralt   = keydown;
    else if( KEY_LSHIFT == code ) mKeyFlags.lshift = keydown;
    else if( KEY_RSHIFT == code ) mKeyFlags.rshift = keydown;

    // ���������ź�
    KeyEvent k( mKeyFlags );
    k.code = static_cast<uint8_t>(code);
    k.down = keydown;
    sigKeyPress( k );

    // ���¼���״̬����
    mKeyStates[code] = keydown;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::input::BasicInput::triggerCharPress( char ch )
{
    GN_GUARD;

    if( mHalfWideChar )
    {
        // ��ǰ�Ѿ�������ǰ��������ַ����������������chӦ���Ǻ��������ַ�
        if( sIsTrailingByte(ch) )
        {
            // ��ǰ�������ַ���ϳ���������������
            mHalfBytes[1] = ch;
            wchar_t wch[2];
            ::mbstowcs( wch, mHalfBytes, 2 );
            sigCharPress( wch[0] );
        }
        else GN_WARN( "ֻ�����˰�������ַ���" );

        // ��������ַ�����־
        mHalfWideChar = false;
    }
    else if( sIsLeadingByte(ch) )
    {
        mHalfWideChar = true;
        mHalfBytes[0] = ch;
    }
    else sigCharPress( ch ); // �������е����˵��chӦ����һ��Ӣ���ַ�

    GN_UNGUARD;
}
