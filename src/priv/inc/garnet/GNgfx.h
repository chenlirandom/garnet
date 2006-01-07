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
    }
}

// Common graphics definition and utilities.
// Link to GNgfxLib.lib (or libGNgfxLib.o) to use it.
#include "gfx/colorFormat.h"
#include "gfx/image.h"
#include "gfx/renderState.h"
#include "gfx/vertexFormat.h"
#include "gfx/misc.h"
#include "gfx/d3dToString.h"

// Platform-independent renderer.
// Load GNgfxD3D.dll and GNgfxOGL.dll (or libGNgfxOGL.so) to use it.
#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/buffer.h"
#include "gfx/renderer.h"

// *****************************************************************************
//                         END of GNgfx.h
// *****************************************************************************
#endif
