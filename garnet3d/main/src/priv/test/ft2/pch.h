#ifndef __GN_PCH_H__
#define __GN_PCH_H__
// *****************************************************************************
// \file    pch.h
// \brief   PCH header
// \author  chenlee (2005.10.1)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "garnet/GNcore.h"
#include "garnet/GNwin.h"

#include <ft2build.h>

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
//                           End of pch.h
// *****************************************************************************
#endif // __GN_PCH_H__
