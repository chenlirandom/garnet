#ifndef __GN_INPUT_BASICINPUT_H__
#define __GN_INPUT_BASICINPUT_H__
// *****************************************************************************
/// \file
/// \brief   ʵ��inputģ���ͨ�û���
/// \author  chenlee (2005.10.24)
// *****************************************************************************

#include "garnet/GNinput.h"
#include <queue>

namespace GN { namespace input
{
    ///
    /// inputģ��Ļ��࣬ʵ����inputģ���ͨ�ù���.
    ///
    class BasicInput : public Input
    {
    public:

        ///
        /// Ctor
        ///
        BasicInput() { resetInputStates(); }

        // ********************************
        // from Input
        // ********************************
    public:

        KeyEvent popLastKeyEvent();
        const KeyStatus * getKeyboardStatus() const { return mKeyboardStatus; }
        const int * getAxisStatus() const { return mAxisStatus; }

        // ********************************
        //     custom protected functions
        // ********************************
    protected:

        ///
        /// Reset to initial state
        ///
        void resetInputStates()
        {
            memset( mKeyboardStatus, 0, sizeof(mKeyboardStatus) );
            memset( mAxisStatus, 0, sizeof(mAxisStatus) );
            mKeyFlags.u8 = 0;
            mHalfWideChar = false;
        }

        ///
        /// Update internal mouse position.
        ///
        /// \param x, y
        ///     New mouse position
        /// \param notify
        ///     If true, the function will trigger axis move signal, while
        ///     new position differs from the old one.
        ///
        void updateMousePosition( int x, int y, bool notify = true );

        /// \name signal triggers
        ///
        /// sub class should call these function when key/axis event occurs
        //@{
        void triggerKeyPress( KeyCode key, bool keydown );
        void triggerCharPress( char ch );
        void triggerAxisMove( Axis axis, int distance )
        {
            GN_ASSERT( 0 <= axis && axis < NUM_AXISES );
            mAxisStatus[axis] += distance;
            sigAxisMove(axis,distance);
        }
        void triggerAxisMoveAbs( Axis axis, int value, int deadZone )
        {
            GN_ASSERT( 0 <= axis && axis < NUM_AXISES );

            // handle dead zone
            if( -deadZone <= value && value <= deadZone ) value = 0;

            if( value != mAxisStatus[axis] )
            {
                int old = mAxisStatus[axis];
                mAxisStatus[axis] = value;
                sigAxisMove(axis, value - old );
            }
        }
        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        std::queue<KeyEvent> mKeyEventQueue;
        SpinLoop             mKeyEventQueueMutex;

        ///
        /// ��¼���̵�״̬����������/������ƥ��İ�������
        ///
        KeyStatus mKeyboardStatus[NUM_KEYS];

        ///
        /// axis positions
        ///
        int mAxisStatus[NUM_AXISES];

        ///
        /// ��¼��CTRL/ALT/SHIFT��״̬
        ///
        KeyStatus mKeyFlags;

        ///
        /// True�������Ѿ������˰��UNICODE�ַ������ڵȴ��°���ַ���
        ///
        bool mHalfWideChar;

        ///
        /// �ݴ������İ��UNICODE�ַ�
        ///
        char mHalfBytes[2];
    };
}}

#include "basicInput.inl"

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_INPUT_BASICINPUT_H__
