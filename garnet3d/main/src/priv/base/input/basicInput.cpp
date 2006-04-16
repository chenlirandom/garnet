#include "pch.h"
#include "basicInput.h"
#if !GN_ENABLE_INLINE
#include "basicInput.inl"
#endif
#include <algorithm>

// *****************************************************************************
//                  protected functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::input::BasicInput::triggerKeyPress( KeyCode code, bool keydown )
{
    GN_GUARD;

    // ignore redundant keyup(s)
    if( keydown == mKeyboardStatus[code].down ) return;

    //GN_TRACE( "Key press: %s %s", kc2str(code), keydown?"down":"up" );

    // ����״̬���ı�־
    mKeyFlags.down = keydown;
    if     ( KEY_LCTRL  == code ) mKeyFlags.lctrl  = keydown;
    else if( KEY_RCTRL  == code ) mKeyFlags.rctrl  = keydown;
    else if( KEY_LALT   == code ) mKeyFlags.lalt   = keydown;
    else if( KEY_RALT   == code ) mKeyFlags.ralt   = keydown;
    else if( KEY_LSHIFT == code ) mKeyFlags.lshift = keydown;
    else if( KEY_RSHIFT == code ) mKeyFlags.rshift = keydown;

    KeyEvent k( code, mKeyFlags );

    // ���¼���״̬����
    mKeyboardStatus[code] = k.status;

    // ���������ź�
    sigKeyPress( k );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::input::BasicInput::triggerCharPress( char ch )
{
    GN_GUARD;

    if( (unsigned char)ch < 128 )
    {
        // ASCII character
        //GN_TRACE( "Char press: %s", StrA(&ch,1).cptr() );
        sigCharPress( ch );
    }
    else if( mHalfWideChar )
    {
        mHalfBytes[1] = ch;
        wchar_t wch[2];
        ::mbstowcs( wch, mHalfBytes, 2 );

        //GN_TRACE( "Char press: %s", StrA(mHalfBytes,2).cptr() );
        sigCharPress( wch[0] );

        // ��������ַ�����־
        mHalfWideChar = false;
    }
    else
    {
        mHalfWideChar = true;
        mHalfBytes[0] = ch;
    }

    GN_UNGUARD;
}
