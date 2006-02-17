#ifndef __GN_TEST_TESTCOMMON_H__
#define __GN_TEST_TESTCOMMON_H__
// *****************************************************************************
//! \file    testCommon.h
//! \brief   common test header
//! \author  chenlee (2005.6.5)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "cxxtest/TestSuite.h"

#if GN_XENON
#include <xtl.h>
#elif GN_MSWIN
#define NOMINMAX
#include <windows.h>
#endif

//!
//! namespace of CxxTest framework
//!
namespace CxxTest
{
    CXXTEST_TEMPLATE_INSTANTIATION
    //!
    //! Equality check of multi-byte char c-style string
    //!
    bool equals<const char *>( const char * x, const char * y )
    {
        return 0 == ::GN::strCmp(x,y);
    }

    CXXTEST_TEMPLATE_INSTANTIATION
    //!
    //! Equality check of wide char c-style string
    //!
    bool equals<const wchar_t *>( const wchar_t * x, const wchar_t * y )
    {
        return 0 == ::GN::strCmp(x,y);
    }

    //!
    //! Type traits of multi-byte string class
    //!
    CXXTEST_TEMPLATE_INSTANTIATION
    class ValueTraits< GN::StrA >
    {
        GN::StrA s;
    public:
        /** ctor */
        ValueTraits( const GN::StrA & t ) : s(t) {}
        /** convert to string */
        const char *asString() const { return s.cstr(); }
    };

    //!
    //! Type traits of wide char string class
    //!
    CXXTEST_TEMPLATE_INSTANTIATION
    class ValueTraits< GN::StrW >
    {
        GN::StrA s;
    public:
        /** ctor */
        ValueTraits( const GN::StrW & t ) : s(GN::wcs2mbs(t)) {}
        /** convert to string */
        const char *asString() const { return s.cstr(); }
    };

    //!
    //! Type traits of multi-byte c-style string
    //!
    CXXTEST_TEMPLATE_INSTANTIATION
    class ValueTraits< const wchar_t * >
    {
        GN::StrA s;
    public:
        /** ctor */
        ValueTraits( const wchar_t * t ) : s(GN::wcs2mbs(t,0)) {}
        /** convert to string */
        const char *asString() const { return s.cstr(); }
    };
}

// *****************************************************************************
//                           End of testCommon.h
// *****************************************************************************
#endif // __GN_TEST_TESTCOMMON_H__
