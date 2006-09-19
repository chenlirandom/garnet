#ifndef __GN_GFX_GFX_H
#define __GN_GFX_GFX_H
// *****************************************************************************
//! \file    GNgfx.h
//! \brief   ������graphicsģ��Ľӿڣ��Լ����õ����ݽṹ�����ڷ�װ�ײ��
//!          ��ȾAPI���ⲿ�û�ֻ��ͨ������ӿ���ʹ��graphics��Ԫ��
//! \author  chenlee(2005.9.30)
//!
//! GFXģ����������ģ����ɣ�
//!     - ��̬��ͼ�ο⣬���ڶ��徲̬��ͼ�����ͺͺ�����
//!     - ��ƽ̨��ͨ����Ⱦ����Ŀǰ֧��D3D9(PC)��OpenGL
//!
// *****************************************************************************

#include "GNbase.h"

namespace GN
{
    //!
    //! namespace for GFX module
    //!
    namespace gfx
    {
        //!
        //! namespace for GFX implemenation details
        //!
        namespace detail
        {
        }
    }
}

// Basic graphics definition and utilities that are mainly building blocks of
// renderer interface and higher level graphics classes.
#include "gfx/colorFormat.h"
#include "gfx/image.h"
#include "gfx/renderState.h"
#include "gfx/vertexFormat.h"
#include "gfx/shape.h"
#include "gfx/misc.h"

// Abstract renderer interfaces.
#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/buffer.h"
#include "gfx/context.h"
#include "gfx/renderer.h"

// More graphics classes and utilities that use renderer interfaces,
// such as mesh effects and etc.

// *****************************************************************************
//                         END of GNgfx.h
// *****************************************************************************
#endif
