#ifndef __GN_GFX_GFX_H
#define __GN_GFX_GFX_H
// *****************************************************************************
/// \file
/// \brief   ������graphicsģ��Ľӿڣ��Լ����õ����ݽṹ�����ڷ�װ�ײ��
///          ��ȾAPI���ⲿ�û�ֻ��ͨ������ӿ���ʹ��graphics��Ԫ��
/// \author  chenlee(2005.9.30)
///
/// GFXģ����������ģ����ɣ�
///     - ��̬��ͼ�ο⣬���ڶ��徲̬��ͼ�����ͺͺ�����
///     - ��ƽ̨��ͨ����Ⱦ����Ŀǰ֧��D3D9(PC)��OpenGL
///
// *****************************************************************************

#include "GNcore.h"

namespace GN
{
    ///
    /// namespace for GFX module
    ///
    namespace gfx
    {
        ///
        /// namespace for GFX implemenation details
        ///
        namespace detail
        {
        }
    }
}

// Basic graphics definition and utilities that are building blocks of
// higher level graphics system.
#include "gfx/colorFormat.h"
#include "gfx/image.h"
#include "gfx/renderState.h"
#include "gfx/sampler.h"
#include "gfx/vertexFormat.h"
#include "gfx/uniform.h"
#include "gfx/shape.h"
#include "gfx/misc.h"
#include "gfx/d3d9utils.h"
#include "gfx/d3d10utils.h"
#include "gfx/oglutils.h"

// Abstract renderer interfaces.
#include "gfx/shader.h"
#include "gfx/surface.h"
#include "gfx/context.h"
#include "gfx/renderer.h"

// *****************************************************************************
//                         END of GNgfx.h
// *****************************************************************************
#endif
