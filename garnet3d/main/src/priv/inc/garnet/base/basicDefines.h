#ifndef __GN_BASE_BASICDEFINES_H__
#define __GN_BASE_BASICDEFINES_H__
// *****************************************************************************
/// \file
/// \brief   Basic defines and types
/// \author  chenlee (2005.7.24)
// *****************************************************************************

// *****************************************************************************
// 辨识编译器
// *****************************************************************************

#define GN_MSVC  0 ///< If 1, means current compiler is msvc (or icl)
#define GN_MSVC8 0 ///< If 1, means current compiler is msvc 8.x
#define GN_ICL   0 ///< If 1, means current compiler is intel c++ compiler
#define GN_MINGW 0 ///< If 1, means current compiler is MingW
#define GN_GCC   0 ///< If 1, means current compierl is gcc/g++
#define GN_BCB   0 ///< If 1, means current compierl is boland c++ compiler

/// \def GN_COMPILER
/// Indicate current compiler

#if defined(_MSC_VER) && !defined(__ICL)
#undef GN_MSVC
#undef GN_MSVC8
#define GN_MSVC 1
#define GN_MSVC8 (_MSC_VER >= 1400)
#define GN_COMPILER "msvc"

#elif defined(__ICL)
#undef GN_ICL
#undef GN_MSVC
#undef GN_MSVC8
#define GN_ICL  1
#define GN_MSVC 1                  // treat intel compiler as VC compiler
#define GN_MSVC8 (_MSC_VER >= 1400)
#define GN_COMPILER "icl"

#elif defined(__GNUC__) && defined(_WIN32)
#undef GN_MINGW
#undef GN_GCC
#define GN_MINGW 1
#define GN_GCC 1
#define GN_COMPILER "mingw"

#elif defined(__BORLANDC__)
#undef GN_BCB
#define GN_BCB 1
#define GN_COMPILER "bcb"

#elif defined(__GNUC__)
#undef GN_GCC
#define GN_GCC 1
#define GN_COMPILER "gcc"

#else
#error "Unknown compiler!"
#define GN_COMPILER "unknown"
#endif

// *****************************************************************************
// 辨识操作系统
// *****************************************************************************

#define GN_MSWIN  0 ///< If 1, means current platform is Windows serias
#define GN_CYGWIN 0 ///< If 1, means current platform is Cygwin
#define GN_POSIX  0 ///< If 1, means current platform is POSIX compatible, such as Cygwin

/// \def GN_OS
/// Indicate current OS

#if defined( _WIN32 )      // Windows
// Windows platform
#undef GN_MSWIN
#define GN_MSWIN 1
#define GN_OS "mswin"
#elif defined(__CYGWIN__)
// Cygwin platform
#undef GN_CYGWIN
#undef GN_POSIX
#define GN_CYGWIN 1
#define GN_POSIX  1 // cygwin also provides some posix compabilities
#define GN_OS "cygwin"
#elif defined( __unix ) || defined( __unix__ )
// posix-like platform
#undef GN_POSIX
#define GN_POSIX 1
#define GN_OS "posix"
#else
#error "unknown OS!"
#define GN_OS "unknown"
#endif

// *****************************************************************************
// 辨识系统平台
// *****************************************************************************

#define GN_PC 0    ///< PC
#define GN_XBOX1 0 ///< Xbox
#define GN_XENON 0 ///< Xbox 360

/// \def GN_SYSTEM
/// Indicate current system

#ifdef _XBOX_VER
#if _XBOX_VER >= 200
#undef GN_XENON
#define GN_XENON 1
#define GN_SYSTEM "xenon"
#else
#undef GN_XBOX1
#define GN_XBOX1 1
#define GN_SYSTEM "xbox1"
#endif
#else
#undef GN_PC
#define GN_PC 1
#define GN_SYSTEM "pc"
#endif

// *****************************************************************************
// 辨识CPU
// *****************************************************************************

#define GN_X86 0 ///< 32-bit x86
#define GN_X64 0 ///< 64-bit amd64
#define GN_PPC 0 ///< power pc

/// \def GN_CPU
/// Indicate current CPU

#if defined( _WIN64 ) || defined( WIN64 ) || ( 64 == __WORDSIZE )
#undef GN_X64
#define GN_X64 1
#define GN_CPU "x64"
#elif defined( _PPC_ ) || GN_XENON
#undef GN_PPC
#define GN_PPC 1
#define GN_CPU "ppc"
#else
#undef GN_X86
#define GN_X86 1
#define GN_CPU "x86"
#endif

// *****************************************************************************
// 辨识endian
// *****************************************************************************

#if defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__)
#define GN_LITTLE_ENDIAN defined(__LITTLE_ENDIAN__) ///< true on little endian machine
#define GN_BIT_ENDIAN    defined(__BIG_ENDIAN__)    ///< true on big endian machine
#else
#define GN_LITTLE_ENDIAN  ( GN_X64 || GN_X86 ) ///< true on little endian machine
#define GN_BIG_ENDIAN     GN_PPC               ///< true on big endian machine
#endif

#if !( GN_LITTLE_ENDIAN ^ GN_BIG_ENDIAN )
#error Compile time endian detection failed!
#endif

// *****************************************************************************
// 定义函数和变量声明
// *****************************************************************************

///
/// force inline macro
///
#if GN_MSVC
#define GN_FORCE_INLINE   __forceinline
#else
#define GN_FORCE_INLINE   inline
#endif

// Export/Import function/variable tag

/// \def GN_EXPORT
/// Export function tag

/// \def GN_IMPORT
/// Import function tag

#if GN_MSVC && !GN_BUILD_STATIC
#define GN_EXPORT       __declspec(dllexport)
#define GN_IMPORT       __declspec(dllimport)
#else
#define GN_IMPORT
#define GN_EXPORT
#endif

///
/// Public/global function/variable tag
///
#if defined(_GN_CORE)
#define GN_PUBLIC   GN_EXPORT // Only core module exports symbols
#else
#define GN_PUBLIC   GN_IMPORT // All other modules only imports symbols
#endif

// *****************************************************************************
/// \name Call Convension
// *****************************************************************************

//@{

// Note: call convension is only avaliable for MSVC on x86 platform.
#if GN_MSVC && !GN_ICL && GN_X86
#define GN_HAS_FASTCALL 1
#define GN_FASTCALL __fastcall

#define GN_HAS_STDCALL 1
#define GN_STDCALL __stdcall

#define GN_HAS_THISCALL 1
#if _MSC_VER >= 1400
#define GN_THISCALL __thiscall
#else
#define GN_THISCALL
#endif

#define GN_HAS_CDECL 1
#define GN_CDECL __cdecl

#else
#define GN_HAS_FASTCALL 0
#define GN_FASTCALL

#define GN_HAS_STDCALL 0
#define GN_STDCALL

#define GN_HAS_THISCALL 0
#define GN_THISCALL

#define GN_HAS_CDECL 1
#define GN_CDECL
#endif

//@}

// *****************************************************************************
// Misc.
// *****************************************************************************

///
/// Hepers to define enumeration class
///
#define GN_DEFINE_ENUM_CLASS( ENUM_CLASS, ENUM_TYPE ) \
    private: \
        ENUM_TYPE mValue; \
    public: \
        ENUM_CLASS() {} \
        ENUM_CLASS( ENUM_TYPE e ) : mValue( e ) {} \
        operator ENUM_TYPE &() { return mValue; } \
        operator const ENUM_TYPE &() const { return mValue; }

///
/// 函数名称
///
#if GN_MSVC
#define GN_FUNCTION          __FUNCTION__
#elif defined( GN_GNUC )
#define GN_FUNCTION          __PRETTY_FUNCTION__
#else
#define GN_FUNCTION          ""
#endif

#define GN_JOIN(s1, s2)          GN_JOIN_DIRECT(s1, s2) ///< join 2 symbols
#define GN_JOIN3(s1, s2, s3)     GN_JOIN( GN_JOIN( s1, s2 ), s3 ) ///< join 3 symbols
#define GN_JOIN4(s1, s2, s3, s4) GN_JOIN( GN_JOIN3( s1, s2, s3 ), s4 ) ///< join 4 symbols
#define GN_JOIN_DIRECT(s1, s2)   s1##s2 ///< Auxillary macro used by GN_JOIN

///
/// Make wide-char string
///
#define GN_WSTR(X) GN_JOIN( L, X )

///
/// Make FORCC code
///
#if GN_PPC
// big endian
#define GN_MAKE_FOURCC(ch0, ch1, ch2, ch3)                  \
    ((UInt32)(UInt8)(ch3) | ((UInt32)(UInt8)(ch2) << 8) |   \
    ((UInt32)(UInt8)(ch1) << 16) | ((UInt32)(UInt8)(ch0) << 24 ))
#else
// little endian
#define GN_MAKE_FOURCC(ch0, ch1, ch2, ch3)                  \
    ((UInt32)(UInt8)(ch0) | ((UInt32)(UInt8)(ch1) << 8) |   \
    ((UInt32)(UInt8)(ch2) << 16) | ((UInt32)(UInt8)(ch3) << 24 ))
#endif

/// Compile time assert helper templates
//@{
template<bool>  struct COMPILE_TIME_ASSERT_FAILURE;
template<>      struct COMPILE_TIME_ASSERT_FAILURE<true>{ enum { value = 1 }; };
template<int x> struct COMPILE_TIME_ASSERT_TEST {};
//@}

///
/// Compile time assert
///
#define GN_CASSERT( exp ) \
    typedef COMPILE_TIME_ASSERT_TEST<sizeof(COMPILE_TIME_ASSERT_FAILURE<exp>::value)> \
    GN_JOIN(__GN_cassert_,__LINE__)

///
/// define handle type
///
#define GN_DEFINE_HANDLE( type ) typedef struct type##Struct {} * type

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_BASICDEFINES_H__
