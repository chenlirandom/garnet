#ifndef __GN_INPUT_BASICINPUT_H__
#define __GN_INPUT_BASICINPUT_H__
// *****************************************************************************
//! \file    basicInput.h
//! \brief   ʵ��inputģ���ͨ�û���
//! \author  chenlee (2005.10.24)
// *****************************************************************************

#include "garnet/GNinput.h"

namespace GN { namespace input
{
    //!
    //! inputģ��Ļ��࣬ʵ����inputģ���ͨ�ù���.
    //!
    class BasicInput : public Input
    {
    public:

        //!
        //! Ctor
        //!
        BasicInput() { resetInputStates(); }

        // ********************************
        // from Input
        // ********************************
    public:

        const KeyStatus * getKeyboardStatus() const { return mKeyboardStatus; }

        // ********************************
        //     custom protected functions
        // ********************************
    protected:

        //!
        //! Reset to initial state
        //!
        void resetInputStates()
        {
            memset( mKeyboardStatus, 0, sizeof(mKeyboardStatus) );
            mKeyFlags.u8 = 0;
            mHalfWideChar = false;
        }

        //!
        //! Update internal mouse position.
        //!
        //! \param x, y
        //!     New mouse position
        //! \param notify
        //!     If true, the function will trigger axis move signal, while
        //!     new position differs from the old one.
        //!
        void updateMousePosition( int x, int y, bool notify = true );

        //! \name signal triggers
        //!
        //! sub class should call these function when key/axis event occurs
        //@{
        void triggerKeyPress( KeyCode key, bool keydown );
        void triggerCharPress( char ch );
        void triggerAxisMove( Axis axis, int distance ) { sigAxisMove(axis,distance); }
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        Vector2i mMousePosition; //! store current mouse position

        //!
        //! ��¼���̵�״̬����������/������ƥ��İ�������
        //!
        KeyStatus mKeyboardStatus[KEY_NUM_OF_KEYS];

        //!
        //! ��¼��CTRL/ALT/SHIFT��״̬
        //!
        KeyStatus mKeyFlags;

        //!
        //! True�������Ѿ������˰��UNICODE�ַ������ڵȴ��°���ַ���
        //!
        bool mHalfWideChar;

        //!
        //! �ݴ������İ��UNICODE�ַ�
        //!
        char mHalfBytes[2];
    };
}}

#if GN_ENABLE_INLINE
#include "basicInput.inl"
#endif

// *****************************************************************************
//                           End of basicInput.h
// *****************************************************************************
#endif // __GN_INPUT_BASICINPUT_H__
