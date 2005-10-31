#ifndef __GN_INPUT_BASICINPUT_H__
#define __GN_INPUT_BASICINPUT_H__
// *****************************************************************************
//! \file    basicInput.h
//! \brief   ʵ��inputģ���ͨ�û���
//! \author  chenlee (2005.10.24)
// *****************************************************************************

#include "garnet/GNinput.h"

namespace GN { namespace input {
    //!
    //! inputģ��Ļ��࣬ʵ����inputģ���ͨ�ù���.
    //!
    class BasicInput : public Input
    {
        // ********************************
        // public functions
        // ********************************
    public:

        //!
        //! Ctor
        //!
        BasicInput() { resetInputStates(); }

        //!
        //! Get keybord status
        //!
        //! \sa Input::getKeyStates()
        //!
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

// *****************************************************************************
//                           End of basicInput.h
// *****************************************************************************
#endif // __GN_INPUT_BASICINPUT_H__
