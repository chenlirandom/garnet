// *****************************************************************************
//! \file    pragma.h
//! \brief   Define global compiler pragma
//! \author  chenlee (2005.7.28)
//!
//! Compiler like msvc and icl have bugs when dealing with pragmas in
//! precompiled header. Sometimes pragma defined in PCH loose their effects.
//!
//! To overcome this, all pragmas are collected into a separate header, which
//! can be included DIRECTLY in source file.
//!
//! \note
//!     No include protection here.
//!     This file is mean to be included multiple times.
//!
// *****************************************************************************

#if !defined(GN_MSVC)
#error "No common garnet defines found. Please include GnBase.h!"
#endif

// *****************************************************************************
// Disable MSVC warnings
// *****************************************************************************

#if GN_MSVC && !GN_ICL
#pragma warning( disable : 4127 ) // 条件表达式是常数
#pragma warning( disable : 4201 ) // nameless struct/union
//#pragma warning( disable : 4251 ) // need DLL interface ...
#pragma warning( disable : 4511 ) // can't generate copy constructor
#pragma warning( disable : 4512 ) // can't generate assignment operator
#pragma warning( disable : 4714 ) // 标记为 __forceinline 的函数未内联
#pragma warning( disable : 4786 ) // symbol trunc to 255 ...
#if defined(NDEBUG)
#pragma warning( disable : 4702 ) // 无法访问的代码
#endif
#endif

// *****************************************************************************
// Disable ICL warnings
// *****************************************************************************

#if GN_ICL
#pragma warning(disable:9)        // nested comment is not allowed
#pragma warning(disable:271)      // trailing comma is nonstandard
#pragma warning(disable:279)      // controlling expression is constant
#pragma warning(disable:373)      // xxx is inaccessible
#pragma warning(disable:383)      // reference to temporary
#pragma warning(disable:424)      // extra ";" ignored
#pragma warning(disable:444)      // destructor for xxx is not virtual
//#pragma warning(disable:858)      // type qualifier on return type is meaningless
#pragma warning(disable:981)      // operands are evaluated in unspecified order
#pragma warning(disable:985)      // debug symbol greater than 255 chars
#pragma warning(disable:1418)     // external definition with no prior declaration overridden
#pragma warning(disable:1419)     // external declaration in primary source file
#pragma warning(disable:1572)     // floating-point comparison
#endif

// *****************************************************************************
// 定义语言属性
// *****************************************************************************
#if GN_MSVC
#pragma setlocale( GN_LOCALE )
#endif

// *****************************************************************************
//                           End of pragma.h
// *****************************************************************************
