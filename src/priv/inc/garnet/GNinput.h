#ifndef __GN_INPUT_GNINPUT_H__
#define __GN_INPUT_GNINPUT_H__
// *****************************************************************************
//! \file    GNinput.h
//! \brief   Main header of input module
//! \author  chenlee (2005.10.24)
// *****************************************************************************

namespace GN
{
    //!
    //! Namespace for input module
    //!
    namespace input
    {
        //! define keycode
        #define GNINPUT_DEFINE_KEYCODE( name, dikey, scancode, vkeycode ) name,

        //!
        //! keycode type
        //!
        enum KeyCode
        {
            //!
            //! ���� ( normally indicate a error )
            //!
            KEY_NONE = 0,

            #include "input/keycodeMeta.h"

            //!
            //! number of available keycodes
            //!
            KEY_NUM_OF_KEYS,
        };
        #undef GNINPUT_DEFINE_KEYCODE
        //@}

        //!
        //! axis type
        //!
        enum Axis
        {
            AXIS_0,  //!< Axis 0
            AXIS_1,  //!< Axis 1
            AXIS_2,  //!< Axis 2
            AXIS_3,  //!< Axis 3

            AXIS_X = AXIS_0, //!< Horizonal move
            AXIS_Y = AXIS_1, //!< Vertical move
            AXIS_Z = AXIS_2, //!< Depth move

            AXIS_WHEEL_0 = AXIS_2, //!< Mouse wheel 0
            AXIS_WHEEL_1 = AXIS_3, //!< Mouse whell 1
        };

        // TODO: joystick support

        //!
        //! ���尴����״̬, used by struct KeyEvent
        //!
        enum KeyState
        {
            KS_DOWN   = 1<<0,
            KS_LCTRL  = 1<<1,
            KS_RCTRL  = 1<<2,
            KS_LALT   = 1<<3,
            KS_RALT   = 1<<4,
            KS_LSHIFT = 1<<5,
            KS_RSHIFT = 1<<6,
        };

        //@{
        // These are provided for easy construction of usual key event constants
        #define GNINPUT_KEYDOWN( keycode ) ( keycode | (::GN::input::KS_DOWN<<8) )
        #define GNINPUT_KEYUP( keycode )   ( keycode )
        //@}

        //!
        //! key event structure
        //!
        //! һ��keyevent_sʵ���Ͼ���һ��uint16_t���͵�������
        //! ���8λ��key code����8λ��key state
        //!
        union KeyEvent
        {
            //! key event as unsigned 16bit integer
            uint16_t u16;

            //! key event as signed 16bit integer
            int16_t  s16;

            //! structured key states
            struct
            {
                uint8_t          code;    //!< key code

                //! key state union
                union
                {
                    uint8_t      state;  //!< key state

                    //! key state structure
                    struct
                    {
                        bool     down   : 1; //!< key down
                        bool     lctrl  : 1; //!< left ctrl down
                        bool     rctrl  : 1; //!< right ctrl down
                        bool     lalt   : 1; //!< left alt down
                        bool     ralt   : 1; //!< right alt down
                        bool     lshift : 1; //!< left shift down
                        bool     rshift : 1; //!< right shift down
                    };
                };
            };

            //! \name constructor(s)
            //@{
            KeyEvent() {}
            KeyEvent( const KeyEvent & k )
                : code(k.code), state(k.state)
            {}
            KeyEvent( KeyCode kc, uint8_t ks )
                : code( static_cast<uint8_t>(kc) ), state(ks)
            { GN_ASSERT( kc < KEY_NUM_OF_KEYS ); }
            //@}

            //! \name operators
            //@{
            bool operator == ( const KeyEvent & rhs ) const
            {
                return u16 == rhs.u16;
            }

            bool operator != ( const KeyEvent & rhs ) const
            {
                return u16 != rhs.u16;
            }

            //@}

            //! \name misc functions
            //@{

            //!
            //! set key data
            //!
            const KeyEvent & set( KeyCode kc, uint8_t ks = 0 )
            {
                GN_ASSERT( kc < KEY_NUM_OF_KEYS );
                code = static_cast<uint8_t>(kc); state = ks; return *this;
            }

            bool isCtrlDown()     const { return lctrl && rctrl; }

            bool isAltDown()      const { return lalt && ralt; }

            bool isShiftDdown()    const { return lshift && rshift; }
            //@}
        };

        //!
        //! User options to input system
        //!
        struct UserOptions
        {
            void * windowHandle; //!< Window handle that accepts user inputs.
        };

        //!
        //! main interface of input module
        //!
        struct Input : public NoCopy
        {
            // ************************************************************************
            //      main interface functions
            // ************************************************************************

            //!
            //! Change user options.
            //!
            virtual bool changeUserOptions( const UserOptions & ) = 0;

            //!
            //! ��ȡ���������µ������¼�
            //!
            virtual void processInputEvents() = 0;

            //!
            //! return all keys
            //!
            //! \note Use KeyCode to index the array;
            //!       true means that key is being pressed.
            //!
            virtual const bool * getKeyStates() const = 0;

            //!
            //! �õ���ǰ����λ�ã���Դ��ڵ����Ͻǣ�
            //!
            virtual void getMousePosition( int & x, int & y ) const = 0;

            // ************************************************************************
            //! \name   signals
            //! \note   These signals are used internally by garnet system.
            //          Client user should connect to their counterparts in inputdev_i
            // ************************************************************************

            //@{

            //!
            //! triggered when keyboard, mouse button, joystick button was pressed/released.
            //!
            Signal1<void,KeyEvent> sigKeyPress;

            //!
            //! triggered when a valid unicode character was generated by user input
            //!
            Signal1<void,wchar_t> sigCharPress;

            //!
            //! triggered when mouse/wheel/joystick moving/scrolling
            //!
            //! \note  ����2�з��ص������λ��
            //!
            Signal2<void,Axis,int> sigAxisMove;

            //@}
        };

        //! \name Utils functions
        //@{

        //!
        //! convert string to keycode
        //!
        //! \return ʧ��ʱ���� KEY_NONE
        //!
        KeyCode str2kc( const char * );

        //!
        //! convert keycode to string
        //!
        //! \return  ʧ��ʱ���ؿ��ִ���ע�⣬�ǿ��ִ�""������NULL��
        //!          ������� GN::strEmpty() ����ⷵ��ֵ
        //!
        const char * kc2str( KeyCode );

        //@}
    }
}

#include "input/inputUtils.inl"

// *****************************************************************************
//                           End of GNinput.h
// *****************************************************************************
#endif // __GN_INPUT_GNINPUT_H__
