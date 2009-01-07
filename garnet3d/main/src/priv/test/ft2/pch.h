#ifndef __GN_PCH_H__
#define __GN_PCH_H__
// *****************************************************************************
// \file    pch.h
// \brief   PCH header
// \author  chenlee (2005.10.1)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "garnet/GNwin.h"

#if GN_XENON
#include <xtl.h>
#elif GN_MSWIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif GN_POSIX
#include <X11/Xlib.h>
#endif

#include <exception>
#include <malloc.h>
#include <stdlib.h>

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_PCH_H__
