#ifndef __GN_D3DAPP_PCH_H__
#define __GN_D3DAPP_PCH_H__
// *****************************************************************************
// \file    pch.h
// \brief   Precompiled header for d3dapp module
// \author  chenlee (2005.8.7)
// *****************************************************************************

#define _GN_CORE

#include "garnet/GnBase.h"

#ifdef GN_DEBUG
#define D3D_DEBUG_INFO // Enable "Enhanced D3DDebugging"
#endif

#define NOMINMAX
#include <windows.h>
#include <atlbase.h> // COM smart pointer
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#define ATL_NO_LEAN_AND_MEAN

#include "garnet/base/pragma.h"

// *****************************************************************************
//                           End of pch.h
// *****************************************************************************
#endif // __GN_D3DAPP_PCH_H__
