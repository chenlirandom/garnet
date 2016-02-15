#ifndef __GN_INPUT_INPUTMSW_H__
#define __GN_INPUT_INPUTMSW_H__
// *****************************************************************************
/// \file
/// \brief   ʹ��Windows�ı�׼��Ϣ���Ƶ�����ģ��
/// \author  chenlee (2005.10.25)
// *****************************************************************************

#include "basicInputMsw.h"

#if GN_WINPC

namespace GN { namespace input
{
    ///
    /// ʹ��Windows�ı�׼��Ϣ���Ƶ�����ģ��
    ///
    class InputMsw : public BasicInputMsw
    {
        KeyCode mKeyMap[0x200]; ///< windows vkcode to garnet keycode

    public:

        ///
        /// Ctor
        ///
        InputMsw();

    protected:

        // inherited from BasicInputMsw
        virtual void msgHandler( UINT msg, WPARAM wp, LPARAM lp );
    };
}}

#endif

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_INPUT_INPUTMSW_H__
