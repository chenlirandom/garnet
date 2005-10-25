#ifndef __GN_NTINPUT_NTINPUT_H__
#define __GN_NTINPUT_NTINPUT_H__
// *****************************************************************************
//! \file    ntInput.h
//! \brief   ʹ��Windows�ı�׼��Ϣ���Ƶ�����ģ��
//! \author  chenlee (2005.10.25)
// *****************************************************************************

#include "../common/basicWinInput.h"

namespace GN { namespace input {
    //!
    //! ʹ��Windows�ı�׼��Ϣ���Ƶ�����ģ��
    //!    
    //!
    //! 
    //!
    class NTInput : public BasicWinInput
    {
        KeyCode mKeyMap[0x200]; //!< windows vkcode to garnet keycode

    public:

        //!
        //! Constructor
        //!
        NTInput();

        // inherited from Input
        virtual void processInputEvents() { /* do nothing */ }

    protected:

        // inherited from BasicWinInput
        virtual void msgHandler( UINT, WPARAM, LPARAM );
    };
}}

// *****************************************************************************
//                           End of ntInput.h
// *****************************************************************************
#endif // __GN_NTINPUT_NTINPUT_H__
