#ifndef __GN_PCH_H__
#define __GN_PCH_H__
// *****************************************************************************
// \file    pch.h
// \brief   PCH header
// \author  chenlee (2005.10.1)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "garnet/GNcore.h"
#include "garnet/GNgfx.h"
#include "garnet/GNapp.h"

#if GN_MSWIN
#define NOMINMAX
#include <windows.h>
#endif

#include <glew.h>

#include "garnet/base/pragma.h"

// *****************************************************************************
//                           End of pch.h
// *****************************************************************************
#endif // __GN_PCH_H__
