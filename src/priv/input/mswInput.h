#ifndef __GN_INPUT_MSWINPUT_H__
#define __GN_INPUT_MSWINPUT_H__
// *****************************************************************************
//! \file    mswInput.h
//! \brief   ʹ��Windows�ı�׼��Ϣ���Ƶ�����ģ��
//! \author  chenlee (2005.10.25)
// *****************************************************************************

#include "basicMswInput.h"

#if GN_MSWIN

namespace GN { namespace input
{
    //!
    //! ʹ��Windows�ı�׼��Ϣ���Ƶ�����ģ��
    //!
    class MswInput : public BasicMswInput
    {
        KeyCode mKeyMap[0x200]; //!< windows vkcode to garnet keycode

    public:

        //!
        //! Ctor
        //!
        MswInput();

        void processInputEvents() { /* do nothing here*/ }

    protected:

        // inherited from BasicMswInput
        virtual void msgHandler( UINT msg, WPARAM wp, LPARAM lp );
    };
}}

#endif

// *****************************************************************************
//                           End of mswInput.h
// *****************************************************************************
#endif // __GN_INPUT_MSWINPUT_H__
