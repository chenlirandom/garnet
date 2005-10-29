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
        BasicInput() {}

        //!
        //! Get keybord status
        //!
        //! \sa Input::getKeyStates()
        //!
        const bool * getKeyStates() const { return mKeyStates; }

        // ********************************
        //     custom protected functions
        // ********************************
    protected:

        //!
        //! Reset to initial state
        //!
        void resetInputStates()
        {
            mKeyFlags.set( KEY_NONE );
            memset( mKeyStates, 0, sizeof(mKeyStates) );
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
        bool mKeyStates[KEY_NUM_OF_KEYS];

        //!
        //! ��¼��CTRL/ALT/SHIFT��״̬
        //!
        KeyEvent mKeyFlags;

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
