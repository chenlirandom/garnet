#ifndef __GN_BASE_STRING_H__
#define __GN_BASE_STRING_H__
// *****************************************************************************
/// \file
/// \brief   custom string implementation
/// \author  chenlee (2005.4.17)
// *****************************************************************************

#include <stdarg.h>
#include <ostream>
#include <string.h>
namespace GN
{
    /// define enumerations for string compare
    struct StringCompareCase
    {
        enum ENUM
        {
            INSENSITIVE, // case insensitive comparision
            SENSITIVE,   // case sensitive comparision
        };

        GN_DEFINE_ENUM_CLASS_HELPERS( StringCompareCase, ENUM );
    };

    ///
    /// Get string length.
    ///
    /// if maxLen > 0, then return math::GetMin(maxLen,realLength).
    ///
    template<typename CHAR>
    inline size_t StringLength( const CHAR * s, size_t maxLen = 0 )
    {
        if( 0 == s ) return 0;
        size_t l = 0;
        if( maxLen > 0 )
        {
            while( 0 != *s && l < maxLen )
            {
                ++l;
                ++s;
            }
        }
        else
        {
            while( 0 != *s )
            {
                ++l;
                ++s;
            }
        }
        return l;
    }

    ///
    /// string comparison (case sensitive)
    ///
    template<typename CHAR>
    inline int StringCompare( const CHAR * s1, const CHAR * s2 )
    {
        if( s1 == s2 ) return 0;
        if( 0 == s1 ) return -1;
        if( 0 == s2 ) return 1;
        while( *s1 && *s2 )
        {
            if( *s1 < *s2 ) return -1;
            if( *s1 > *s2 ) return 1;
            ++s1;
            ++s2;
        }
        if( 0 != *s1 ) return 1;
        if( 0 != *s2 ) return -1;
        return 0;
    }

    ///
    /// string comparison, case sensitive, with limited length
    ///
    template<typename CHAR>
    inline int StringCompare( const CHAR * s1, const CHAR * s2, size_t maxLength )
    {
        if( s1 == s2 ) return 0;
        if( 0 == s1 ) return -1;
        if( 0 == s2 ) return 1;
        size_t len = 0;
        while( *s1 && *s2 && len < maxLength )
        {
            if( *s1 < *s2 ) return -1;
            if( *s1 > *s2 ) return 1;
            ++s1;
            ++s2;
            ++len;
        }
        if( len == maxLength ) return 0;
        if( 0 != *s1 ) return 1;
        if( 0 != *s2 ) return -1;
        return 0;
    }

    ///
    /// string comparison (case insensitive)
    ///
    template<typename CHAR>
    inline int StringCompareI( const CHAR * s1, const CHAR * s2 )
    {
        if( s1 == s2 ) return 0;
        if( 0 == s1 ) return -1;
        if( 0 == s2 ) return 1;
        int a, b;
        while( *s1 && *s2 )
        {
            a = (int)*s1;
            b = (int)*s2;
            if( 'a' <= a && a <= 'z' ) a += 'A'-'a';
            if( 'a' <= b && b <= 'z' ) b += 'A'-'a';
            if( a < b ) return -1;
            if( a > b ) return 1;
            ++s1;
            ++s2;
        }
        if( 0 != *s1 ) return 1;
        if( 0 != *s2 ) return -1;
        return 0;
    }

    ///
    /// string comparison (case insensitive), with limited length
    ///
    template<typename CHAR>
    inline int StringCompareI( const CHAR * s1, const CHAR * s2, size_t maxLength )
    {
        if( s1 == s2 ) return 0;
        if( 0 == s1 ) return -1;
        if( 0 == s2 ) return 1;
        size_t len = 0;
        int a, b;
        while( *s1 && *s2 && len < maxLength )
        {
            a = (int)*s1;
            b = (int)*s2;
            if( 'a' <= a && a <= 'z' ) a += 'A'-'a';
            if( 'a' <= b && b <= 'z' ) b += 'A'-'a';
            if( a < b ) return -1;
            if( a > b ) return 1;
            ++s1;
            ++s2;
            ++len;
        }
        if( len == maxLength ) return 0;
        if( 0 != *s1 ) return 1;
        if( 0 != *s2 ) return -1;
        return 0;
    }

    ///
    /// check for empty string, including NULL.
    ///
    template<typename CHAR>
    inline bool IsStringEmpty( const CHAR * s )
    {
        return 0 == s || 0 == s[0];
    }

    ///
    /// safe sprintf. This function always outputs null-terminated string,
    /// like StringCchPrintf(...)
    ///
    void
    StringPrintf(
        char *       buf,
        size_t       bufSizeInChar,
        const char * fmt,
        ... );

    ///
    /// safe sprintf. This function always outputs null-terminated string,
    /// like StringCchPrintf(...)
    ///
    void
    StringPrintf(
        wchar_t *       buf,
        size_t          bufSizeInWchar,
        const wchar_t * fmt,
        ... );

    ///
    /// safe sprintf. This function always outputs null-terminated string,
    /// like StringCchPrintf(...)
    ///
    void
    StringVarPrintf(
        char *       buf,
        size_t       bufSizeInChar,
        const char * fmt,
        va_list      args );

    ///
    /// printf-like format string (wide-char)
    ///
    void
    StringVarPrintf(
        wchar_t *       buf,
        size_t          bufSizeInWchar,
        const wchar_t * fmt,
        va_list         args );

    ///
    /// string hash function
    ///
    /// set to length to 0 to hash NULL terminated string.
    ///
    template<typename CHAR>
    inline UInt64 StringHash( const CHAR * s, size_t length = 0 )
    {
        unsigned long hash = 5381;

        if( length > 0 )
        {
            for( size_t i = 0; i < length; ++i )
            {
                hash = ((hash << 5) + hash) + *s; /* hash * 33 + c */
                ++s;
            }
        }
        else
        {
            while( *s )
            {
                hash = ((hash << 5) + hash) + *s; /* hash * 33 + c */
                ++s;
            }
        }

        return hash;
    }


    ///
    /// Custom string class
    ///
    template<typename CHAR, typename ALLOCATOR = StlAllocator<CHAR> >
    class Str
    {
        typedef CHAR CharType;
        typedef ALLOCATOR AllocatorType;

        CharType * mPtr;   ///< string buffer pointer.
        size_t     mCount; ///< How many charecters in the string, not including null End.
        size_t     mCaps;  ///< How many characters can we hold, not including null End?

    public:

        ///
        /// Instance of Empty string
        ///
        static Str EMPTYSTR;

        ///
        /// indicate serach failure.
        ///
        static const size_t NOT_FOUND = (size_t)-1;

        ///
        /// default constructor
        ///
        Str() : mCount(0), mCaps(0)
        {
            mPtr = alloc(mCaps);
            mPtr[0] = 0;
        }

        ///
        /// copy constructor
        ///
        Str( const Str & s ) : mCount(s.mCount), mCaps(CalculateCaps(s.mCount))
        {
            mPtr = alloc(mCaps);
            ::memcpy( mPtr, s.mPtr, (mCount+1)*sizeof(CharType) );
        }

        ///
        /// copy constructor from c-style string
        ///
        Str( const CharType * s, size_t l = 0 )
        {
            if( 0 == s )
            {
                mCaps = 0;
                mCount = 0;
                mPtr = alloc(mCaps);
                mPtr[0] = 0;
            }
            else
            {
                l = StringLength<CharType>(s,l);
                mCaps = CalculateCaps(l);
                mPtr = alloc(mCaps);
                mCount = l;
                ::memcpy( mPtr, s, l*sizeof(CharType) );
                mPtr[l] = 0;
            }
        }

        ///
        /// destructor
        ///
        ~Str()
        {
            dealloc( mPtr, mCaps );
        }

        ///
        /// Append to this string
        ///
        void Append( const CharType * s, size_t l = 0 )
        {
            if( 0 == s ) return;
            l = StringLength<CharType>(s,l);
            SetCaps( mCount + l );
            ::memcpy( mPtr+mCount, s, l*sizeof(CharType) );
            mCount += l;
            mPtr[mCount] = 0;
        }

        ///
        /// Append to this string
        ///
        void Append( const Str & s )
        {
            if( s.Empty() ) return;
            size_t l = s.Size();
            SetCaps( mCount + l );
            ::memcpy( mPtr+mCount, s.ToRawPtr(), l*sizeof(CharType) );
            mCount += l;
            mPtr[mCount] = 0;
        }

        ///
        /// Append to this string
        ///
        void Append( CharType ch )
        {
            if( 0 == ch ) return;
            SetCaps( mCount + 1 );
            mPtr[mCount] = ch;
            ++mCount;
            mPtr[mCount] = 0;
        }

        ///
        /// Assign value to string class
        ///
        void Assign( const CharType * s, size_t l = 0 )
        {
            if( 0 == s )
            {
                mPtr[0] = 0;
                mCount = 0;
            }
            else
            {
                l = StringLength<CharType>(s,l);
                SetCaps(l);
                mCount = l;
                ::memcpy( mPtr, s, l*sizeof(CharType) );
                mPtr[l] = 0;
            }
        }

        ///
        /// Begin iterator(1)
        ///
        CharType * Begin() { return mPtr; }

        ///
        /// Begin iterator(2)
        ///
        const CharType * Begin() const { return mPtr; }

        ///
        /// Clear to Empty string
        ///
        void Clear()
        {
            mPtr[0] = 0;
            mCount = 0;
        }

        ///
        /// Empty string or not?
        ///
        bool Empty() const { return 0 == mCount; }

        ///
        /// Begin iterator(1)
        ///
        CharType * End() { return mPtr+mCount; }

        ///
        /// Begin iterator(2)
        ///
        const CharType * End() const { return mPtr+mCount; }

        ///
        /// Searches through a string for the GetFirst character that matches any elements in user specified string
        ///
        /// \param s
        ///     User specified search pattern
        /// \param offset, count
        ///     Range of the search. (count==0) means to the End of input string.
        /// \return
        ///     Return index of the character of GetFirst occurance or NOT_FOUND.
        ///
        size_t FindFirstOf( const CharType * s, size_t offset = 0, size_t count = 0 ) const
        {
            if( 0 == s || 0 == *s ) return NOT_FOUND;
            if( offset >= mCount ) return NOT_FOUND;
            if( 0 == count ) count = mCount;
            if( offset + count > mCount ) count = mCount - offset;
            const CharType * p = mPtr + offset;
            for( size_t i = 0; i < count; ++i, ++p )
            {
                for( const CharType * t = s; *t; ++t )
                {
                    GN_ASSERT( *p && *t );
                    if( *p == *t ) return offset + i;
                }
            }
            return NOT_FOUND;
        }

        ///
        /// Searches through a string for the GetFirst character that not any elements of user specifed string.
        ///
        size_t FindFirstNotOf( const CharType * s, size_t offset = 0, size_t count = 0 ) const
        {
            if( 0 == s || 0 == *s ) return NOT_FOUND;
            if( offset >= mCount ) return NOT_FOUND;
            if( 0 == count ) count = mCount;
            if( offset + count > mCount ) count = mCount - offset;
            const CharType * p = mPtr + offset;
            for( size_t i = 0; i < count; ++i, ++p )
            {
                for( const CharType * t = s; *t; ++t )
                {
                    GN_ASSERT( *p && *t );
                    if( *p != *t ) return offset + i;
                }
            }
            return NOT_FOUND;
        }

        ///
        /// Searches through a string for the GetFirst character that matches users predication
        ///
        template<typename PRED>
        size_t FindFirstOf( PRED pred, size_t offset = 0, size_t count = 0 ) const
        {
            if( offset >= mCount ) return NOT_FOUND;
            if( 0 == count ) count = mCount;
            if( offset + count > mCount ) count = mCount - offset;
            const char * p = mPtr + offset;
            for( size_t i = 0; i < count; ++i, ++p )
            {
                GN_ASSERT( *p );
                if( pred(*p) ) return offset + i;
            }
            return NOT_FOUND;
        }

        ///
        /// Searches through a string for the GetLast character that matches any elements in user specified string
        ///
        size_t FindLastOf( const CharType * s, size_t offset = 0, size_t count = 0 ) const
        {
            if( 0 == s || 0 == *s ) return NOT_FOUND;
            if( offset >= mCount ) return NOT_FOUND;
            if( 0 == count ) count = mCount;
            if( offset + count > mCount ) count = mCount - offset;
            GN_ASSERT( count > 0 );
            const CharType * p = mPtr + offset + count - 1;
            for( size_t i = count; i > 0; --i, --p )
            {
                for( const CharType * t = s; *t; ++t )
                {
                    GN_ASSERT( *p && *t );
                    if( *p == *t ) return offset + i - 1;
                }
            }
            return NOT_FOUND;
        }

        ///
        /// printf-like string formatting
        ///
        const CharType * Format( const CharType * fmt, ... )
        {
            va_list arglist;
            va_start( arglist, fmt );
            FormatV( fmt, arglist );
            va_end( arglist );
            return mPtr;
        }

        ///
        /// printf-like string formatting(2)
        ///
        const CharType * FormatV( const CharType * fmt, va_list args )
        {
            if( IsStringEmpty(fmt) )
            {
                Clear();
            }
            else
            {
                CharType buf[16384];  // 16k should be enough in most cases
                StringVarPrintf( buf, 16384, fmt, args );
                buf[16383] = 0;
                Assign( buf );
            }
            return mPtr;
        }

        ///
        /// get string caps
        ///
        size_t GetCaps() const { return mCaps; }

        ///
        /// get GetFirst character of the string. If string is Empty, return 0.
        ///
        CharType GetFirst() const { return mPtr[0]; }

        ///
        /// get GetLast character of the string. If string is Empty, return 0.
        ///
        CharType GetLast() const { return mCount>0 ? mPtr[mCount-1] : (CharType)0; }

        ///
        /// Insert a character at specific position
        ///
        void Insert( size_t pos, CharType ch )
        {
            if( 0 == ch ) return;
            if( pos >= mCount )
            {
                Append( ch );
            }
            else
            {
                SetCaps( mCount + 1 );
                for( size_t i = mCount+1; i > pos; --i )
                {
                    mPtr[i] = mPtr[i-1];
                }
                mPtr[pos] = ch;
                ++mCount;
            }
        }

        ///
        /// pop GetLast charater
        ///
        void PopBack()
        {
            if( mCount > 0 )
            {
                --mCount;
                mPtr[mCount] = 0;
            }
        }

        ///
        /// Replace specific character with another
        ///
        void Replace( CharType from, CharType to )
        {
            CharType * p = mPtr;
            for( size_t i = 0; i < mCount; ++i, ++p )
            {
                if( from == *p ) *p = to;
            }
        }

        ///
        /// Remove specific charactor at specific location
        ///
        void Remove( size_t pos )
        {
            for( size_t i = pos; i < mCount; ++i )
            {
                mPtr[i] = mPtr[i+1];
            }
            --mCount;
        }

        ///
        /// set string caps
        ///
        void SetCaps( size_t newCaps )
        {
            if( mCaps >= newCaps ) return;
            size_t oldCaps = mCaps;
            mCaps = CalculateCaps( newCaps );
            CharType * newPtr = alloc( mCaps );
            ::memcpy( newPtr, mPtr, sizeof(CharType)*(mCount+1) );
            dealloc( mPtr, oldCaps );
            mPtr = newPtr;
        }

        ///
        /// return string length in character, not including ending zero
        ///
        size_t Size() const { return mCount; }

        ///
        /// Get sub string. (0==length) means to the End of original string.
        ///
        void SubString( Str & result, size_t offset, size_t length ) const
        {
            if( offset >= mCount ) { result.Clear(); return; }
            if( 0 == length ) length = mCount;
            if( offset + length > mCount ) length = mCount - offset;
            result.Assign( mPtr+offset, length );
        }

        ///
        /// Return sub string
        ///
        Str SubString( size_t offset, size_t length ) const
        {
            Str ret;
            SubString( ret, offset, length );
            return ret;
        }

        ///
        /// convert to all lower case
        ///
        void ToLowerCase()
        {
            CHAR * p = mPtr;
            CHAR * e = mPtr + mCount;
            for( ; p < e; ++p )
            {
                if( 'A' <= *p && *p <= 'Z' ) *p = (*p) - 'A' + 'a';
            }
        }

        ///
        /// return c-style const char pointer
        ///
        const CharType * ToRawPtr() const { return mPtr; }

        ///
        /// convert to all upper case
        ///
        void ToUpperCase()
        {
            CHAR * p = mPtr;
            CHAR * e = mPtr + mCount;
            for( ; p < e; ++p )
            {
                if( 'a' <= *p && *p <= 'z' ) *p = (*p) - 'a' + 'A';
            }
        }

        ///
        /// Trim characters for both side
        ///
        void Trim( const CharType * ch, size_t len = 0 )
        {
            if( 0 == ch ) return;
            if( 0 == len ) len = StringLength( ch );
            TrimRight( ch, len );
            TrimLeft( ch, len );
        }

        ///
        /// Trim characters for both side
        ///
        void Trim( CharType ch ) { Trim( &ch, 1 ); }

        ///
        /// Trim left characters
        ///
        void TrimLeft( const CharType * ch, size_t len = 0 )
        {
            if( 0 == ch ) return;
            if( 0 == len ) len = StringLength( ch );
            if( 0 == len ) return;
            CharType * p = mPtr;
            CharType * e = mPtr+mCount;
            while( p < e )
            {
                bool equal = false;
                for( size_t i = 0; i < len; ++i )
                {
                    if( *p == ch[i] )
                    {
                        equal = true;
                        break;
                    }
                }
                if( !equal ) break;
                ++p;
            }
            mCount = e - p;
            for( size_t i = 0; i < mCount; ++i )
            {
                mPtr[i] = p[i];
            }
            mPtr[mCount] = 0;
        }

        ///
        /// Trim left characters
        ///
        void TrimLeft( CharType ch ) { TrimLeft( &ch, 1 ); }

        ///
        /// Trim right characters
        ///
        void TrimRight( const CharType * ch, size_t len = 0 )
        {
            if( 0 == mCount ) return;
            if( 0 == ch ) return;
            if( 0 == len ) len = StringLength( ch );
            if( 0 == len ) return;
            CharType * p = mPtr + mCount - 1;
            while( p >= mPtr )
            {
                bool equal = false;
                for( size_t i = 0; i < len; ++i )
                {
                    if( *p == ch[i] )
                    {
                        equal = true;
                        break;
                    }
                }
                if( !equal ) break;
                *p = 0;
                --p;
            }
            mCount = p - mPtr + 1;
        }

        ///
        /// Trim right characters
        ///
        void TrimRight( CharType ch ) { TrimRight( &ch, 1 ); }

        ///
        /// Trim right characters until meet the predication condition.
        ///
        template<typename PRED>
        void TrimRightUntil( PRED pred )
        {
            if( 0 == mCount ) return;
            CharType * p = mPtr + mCount - 1;
            while( p >= mPtr && !pred(*p) )
            {
                *p = 0;
                --p;
            }
            mCount = p - mPtr + 1;
        }

        ///
        /// type cast to C string
        ///
        operator const CharType *() const { return mPtr; }

        ///
        /// type cast to C string
        ///
        operator CharType *() { return mPtr; }

        ///
        /// Assign operator
        ///
        Str & operator = ( const Str & s )
        {
            Assign( s.mPtr, s.mCount );
            return *this;
        }

        ///
        /// Assign operator
        ///
        Str & operator = ( const CharType * s )
        {
            Assign( s, StringLength<CharType>(s) );
            return *this;
        }

        ///
        /// += operator (1)
        ///
        Str & operator += ( const Str & s )
        {
            Append( s );
            return *this;
        }

        ///
        /// += operator (2)
        ///
        Str & operator += ( const CharType * s )
        {
            Append( s, 0 );
            return *this;
        }

        ///
        /// += operator (3)
        ///
        Str & operator += ( CharType ch )
        {
            Append( ch );
            return *this;
        }

        ///
        /// += operator (4)
        ///
        Str & operator += ( std::basic_string<CharType> & s )
        {
            Append( s.c_str(), 0 );
            return *this;
        }

        ///
        /// equality operator(1)
        ///
        friend bool operator == ( const CharType * s1, const Str & s2 )
        {
            return 0 == StringCompare( s1, s2.mPtr );
        }

        ///
        /// equality operator(2)
        ///
        friend bool operator == ( const Str & s1, const CharType * s2 )
        {
            return 0 == StringCompare( s1.mPtr, s2 );
        }

        ///
        /// equality operator(3)
        ///
        friend bool operator == ( const Str & s1, const Str & s2 )
        {
            return 0 == StringCompare( s1.mPtr, s2.mPtr );
        }

        ///
        /// unequality operator(1)
        ///
        friend bool operator != ( const CharType * s1, const Str & s2 )
        {
            return 0 != StringCompare( s1, s2.mPtr );
        }

        ///
        /// unequality operator(2)
        ///
        friend bool operator != ( const Str & s1, const CharType * s2 )
        {
            return 0 != StringCompare( s1.mPtr, s2 );
        }

        ///
        /// unequality operator(3)
        ///
        friend bool operator != ( const Str & s1, const Str & s2 )
        {
            return 0 != StringCompare( s1.mPtr, s2.mPtr );
        }

        ///
        /// less operator(1)
        ///
        friend bool operator < ( const CharType * s1, const Str & s2 )
        {
            return -1 == StringCompare( s1, s2.mPtr );
        }

        ///
        /// less operator(2)
        ///
        friend bool operator < ( const Str & s1, const CharType * s2 )
        {
            return -1 == StringCompare( s1.mPtr, s2 );
        }

        ///
        /// less operator(3)
        ///
        friend bool operator < ( const Str & s1, const Str & s2 )
        {
            return -1 == StringCompare( s1.mPtr, s2.mPtr );
        }


        ///
        /// concatnate operator(1)
        ///
        friend Str operator + ( const CharType * s1, const Str & s2 )
        {
            Str r(s1);
            r.Append(s2);
            return r;
        }

        ///
        /// concatnate operator(2)
        ///
        friend Str operator + ( const Str & s1, const CharType * s2 )
        {
            Str r(s1);
            r.Append(s2);
            return r;
        }

        ///
        /// concatnate operator(3)
        ///
        friend Str operator + ( const Str & s1, const Str & s2 )
        {
            Str r(s1);
            r.Append(s2);
            return r;
        }

        ///
        /// concatnate operator(4)
        ///
        friend Str operator + ( const Str & s1, const std::basic_string<CharType> & s2 )
        {
            Str r(s1);
            r.Append( s2.c_str() );
            return r;
        }

        ///
        /// concatnate operator(5)
        ///
        friend Str operator + ( const std::basic_string<CharType> & s1, const Str & s2 )
        {
            Str r(s2);
            r.Append( s1.c_str() );
            return r;
        }

        ///
        /// Output to ostream
        ///
        friend std::ostream & operator << ( std::ostream & os, const Str & str )
        {
            os << str.ToRawPtr();
            return os;
        }

        ///
        /// string Hash Functor
        ///
        struct Hash
        {
            UInt64 operator()( const Str & s ) const
            {
                return StringHash( s.mPtr, s.mCount );
            }
        };

    private:

        // align caps to 2^n-1
        size_t CalculateCaps( size_t count )
        {
            #if GN_X64
            count |= count >> 32;
            #endif
            count |= count >> 16;
            count |= count >> 8;
            count |= count >> 4;
            count |= count >> 2;
            count |= count >> 1;
            return count;
        }

        // Allocate a memory buffer that can hold at least 'count' characters, and one extra '\0'.
        static CharType * alloc( size_t count )
        {
            AllocatorType a;
            return a.allocate( count + 1 );
        }

        static void dealloc( CharType * ptr, size_t count )
        {
            AllocatorType a;
            a.deallocate( ptr, count + 1 );
        }

        friend Str<char> Wcs2Mbs( const wchar_t *, size_t );
        friend void Wcs2Mbs( Str<char> &, const wchar_t *, size_t );
        friend Str<wchar_t> Mbs2Wcs( const char *, size_t );
        friend void Mbs2Wcs( Str<wchar_t> &, const char *, size_t );
    };

    // Implement static data member
    template <typename CHAR, typename ALLOC> Str<CHAR,ALLOC> Str<CHAR,ALLOC>::EMPTYSTR;

    ///
    /// multi-byte string class
    ///
    typedef Str<char> StrA;

    ///
    /// wide-char string class
    ///
    typedef Str<wchar_t> StrW;

    ///
    /// Fixed sized string that has no runtime memory allocation.
    ///
    template<size_t N, typename CHAR>
    class StackStr
    {
        typedef CHAR CharType;

        size_t mCount;    ///< How many charecters in the string, not including null end.
        CHAR   mBuf[N+1]; ///< Pre-allocated string buffer

        static size_t sValidateLength( size_t len ) { return len < N ? len : N; }

    public:

        /// \name ctor and dtor
        //@{
        StackStr() : mCount(0) { mBuf[0] = 0; }
        StackStr( const CHAR * s, size_t l = 0 ) : mCount(l)
        {
            if( 0 == s )
            {
                mCount = 0;
                mBuf[0] = 0;
            }
            else
            {
                if( 0 == l ) l = StringLength( s );
                memcpy( mBuf, s, sValidateLength(l) * sizeof(CHAR) );
            }
        }
        StackStr( const StackStr & s ) : mCount( s.mCount )
        {
            memcpy( mBuf, s.mBuf, sizeof(CHAR)*s.mCount );
        }
        StackStr( const Str<CHAR> & s )
        {
            memcpy( mBuf, s.ToRawPtr(), sizeof(CHAR) * sValidateLength(s.Size()) );
        }
        //@}
    };

    //
    // Implmement algorithm described in paper "Fast Algorithms for Sorting and Searching Strings"
    //
    //      http://www.cs.princeton.edu/~rs/strings/)
    //
    // Here are some reference implementations:
    //
    //      http://www.codeproject.com/KB/recipes/tst.aspx
    //      http://meshula.net/wordpress/?p=183
    //
    // TODO: sorted leaf list
    //
    template<class CHAR, class T, StringCompareCase::ENUM CASE_COMPARE = StringCompareCase::SENSITIVE>
    class StringMap
    {
        // *****************************
        // public types
        // *****************************

    public:

        /// public key-value pair type.
        struct KeyValuePair
        {
            const CHAR * const key;   // Note: key is always an const.
            T                  value;

        protected:

            // default constructor
            KeyValuePair(const char * k, const T & v) : key(k), value(v) {}
        };

        // *****************************
        // public methods
        // *****************************

    public:

        /// default constructor
        StringMap() : mRoot(NULL), mCount(0)
        {
        }

        /// copy constructor
        StringMap( const StringMap & sm ) : mRoot(NULL), mCount(0)
        {
            DoClone( sm );
        }

        /// destructor
        ~StringMap()
        {
            Clear();
        }

        /// get first element in the map
        /// \note elements are _NOT_ sorted yet.
        const KeyValuePair * First() const { return DoFirst(); }

        /// get first element in the map
        /// \note elements are _NOT_ sorted yet.
        KeyValuePair * First() { return DoFirst(); }

        /// clear whole map
        void Clear() { DoClear(); }

        /// empty
        bool Empty() const { return 0 == mCount; }

        /// Get next item
        /// \note elements are _NOT_ sorted yet.
        const KeyValuePair * Next( const KeyValuePair * p ) const { return DoNext( p ); }

        /// Get next item
        /// \note elements are _NOT_ sorted yet.
        KeyValuePair * Next( const KeyValuePair * p ) { return DoNext( p ); }

        /// erase by key
        void Remove( const CHAR * text ) { DoRemove( text ); }

        /// Find
        const T * Find( const CHAR * text ) const { return DoFind( text ); }

        /// Find
        T * Find( const CHAR * text ) { return DoFind( text ); }

        /// Find
        KeyValuePair * FindPair( const CHAR * text ) { return DoFindPair( text ); }

        /// Find
        const KeyValuePair * FindPair( const CHAR * text ) const { return DoFindPair( text ); }

        /// insert. Return the inserted key value pair or NULL.
        KeyValuePair * Insert( const CHAR * text, const T & value ) { bool inserted; KeyValuePair * p = DoFindOrInsert( text, value, inserted ); return inserted ? p : NULL; }

        /// return number of items in map
        size_t Size() const { return mCount; }

        // *****************************
        // public operators
        // *****************************

    public:

        /// assignment
        StringMap & operator=( const StringMap & rhs ) { DoClone( rhs ); return *this; }

        /// indexing operator
        T & operator[]( const CHAR * text ) { bool inserted; KeyValuePair * p = DoFindOrInsert( text, T(), inserted ); GN_ASSERT(p); return p->value; }

        /// indexing operator
        const T & operator[]( const CHAR * text ) const { const KeyValuePair * p = DoFindPair( text ); GN_ASSERT(p); return p->value; }

        // *****************************
        // private types
        // *****************************

    private:

        struct Leaf : public KeyValuePair
        {
            // double linked list fields
            Leaf * prev;
            Leaf * next;
            void * owner;

            Leaf( const CHAR * text, size_t textlen, const T & v )
                : KeyValuePair( (const char*)HeapMemory::Alloc(textlen+1), v )
                , prev(NULL)
                , next(NULL)
                , owner(NULL)
            {
                if( NULL != key )
                {
                    memcpy( (char*)key, text, textlen+1 );
                }
            }

            ~Leaf()
            {
                HeapMemory::Free( (void*)key );
            }
        };

        struct Node
        {
            CHAR   splitchar; // 0 means a leaf node
            Node * parent;
            Node * lower;
            Node * equal;
            Node * higher;
            Leaf * leaf;  // valid only when splitchar == 0 (leaf node)
        };

        // *****************************
        // private data
        // *****************************

    private:

        Node * mRoot;
        size_t mCount; // number of items in map
        FixSizedRawMemoryPool<sizeof(Node)> mNodePool;
        ObjectPool<Leaf>                    mLeafPool;
        DoubleLinkedList<Leaf>              mLeafs;

        // *****************************
        // private methods
        // *****************************

    private:

        Node * AllocNode()
        {
            return (Node*)mNodePool.Alloc();
        }

        void FreeNode( Node * n )
        {
            mNodePool.Dealloc( n );
        }

        Leaf * AllocLeaf( const CHAR * text, size_t textlen, const T & value )
        {
            Leaf * p = mLeafPool.AllocUnconstructed();
            if( NULL == p ) return NULL;

            // call in-place new to construct the leaf
            new (p) Leaf( text, textlen, value );
            if( NULL == p->key )
            {
                mLeafPool.FreeWithoutDeconstruct( p );
                return NULL;
            }

            return p;
        }

        void FreeLeaf( Leaf * l )
        {
            GN_ASSERT( l );
            mLeafs.Remove( l );
            mLeafPool.DeconstructAndFree( l );
        }

        /// clear the whole map container
        void DoClear()
        {
            mRoot = NULL;
            mCount = 0;
            mNodePool.FreeAll();
            mLeafPool.DeconstructAndFreeAll();

            // I know this is hacky. But it works.
            memset( &mLeafs, 0, sizeof(mLeafs) );
        }

        /// make itself a clone of another map
        void DoClone( const StringMap & anotherMap )
        {
            // shortcut for cloning itself.
            if( this == &anotherMap ) return;

            // clear myself
            Clear();

            // insert all items in another map to this map.
            for( const KeyValuePair * p = anotherMap.First(); NULL != p; p = anotherMap.Next(p) )
            {
                Insert( p->key, p->value );
            }
        }

        T * DoFind( const CHAR * text ) const
        {
            Leaf * p = DoFindPair( text );
            return p ? &p->value : NULL;
        }

        Leaf * DoFindPair( const CHAR * text ) const
        {
            // check for NULL text pointer
            if( NULL == text )
            {
                static Logger * sLogger = GetLogger("GN.base.StringMap");
                GN_WARN(sLogger)( "StringMap finding warning: NULL text!" );
                return NULL;
            }

            Node * p = mRoot;
            while( p )
            {
                int d;

                if( StringCompareCase::INSENSITIVE == CASE_COMPARE )
                {
                    // conver both to upper case
                    CHAR t = *text;
                    CHAR s = p->splitchar;
                    if( 'a' <= t && t <= 'z' ) t += 'A' - 'a';
                    if( 'a' <= s && s <= 'z' ) s += 'A' - 'a';
                    d = t - s;
                }
                else
                {
                    d = *text - p->splitchar;
                }

                if( 0 == d )
                {
                    if( 0 == *text )
                    {
                        // found!
                        GN_ASSERT( p->leaf );
                        return p->leaf;
                    }
                    else
                    {
                        p = p->equal;
                    }

                    ++text;
                }
                else if( d < 0 )
                {
                    p = p->lower;
                }
                else
                {
                    p = p->higher;
                }
            }

            // not found
            return NULL;
        }

        Leaf * DoFindOrInsert( const CHAR * text, const T & value, bool & inserted )
        {
            inserted = false;

            // check for NULL text pointer
            if( NULL == text )
            {
                static Logger * sLogger = GetLogger("GN.base.StringMap");
                GN_WARN(sLogger)( "Null text is not allowed!" );
                return NULL;
            }

            // store input text pointer
            const char * inputText = text;

            // search in existing nodes
            Node * parentNode = NULL;
            Node ** pp = &mRoot;
            while( NULL != *pp )
            {
                int d;

                if( StringCompareCase::INSENSITIVE == CASE_COMPARE )
                {
                    // conver both to upper case
                    CHAR t = *text;
                    CHAR s = (*pp)->splitchar;
                    if( 'a' <= t && t <= 'z' ) t += 'A' - 'a';
                    if( 'a' <= s && s <= 'z' ) s += 'A' - 'a';
                    d = t - s;
                }
                else
                {
                    d = *text - (*pp)->splitchar;
                }

                if( 0 == d )
                {
                    if( 0 == *text )
                    {
                        // The text exists already. Insertion failed.
                        GN_ASSERT( (*pp)->leaf );
                        inserted = false;
                        return (*pp)->leaf;
                    }
                    else
                    {
                        parentNode = *pp;
                        pp = &((*pp)->equal);
                    }

                    ++text;
                }
                else if( d < 0 )
                {
                    parentNode = *pp;
                    pp = &((*pp)->lower);
                }
                else
                {
                    parentNode = *pp;
                    pp = &((*pp)->higher);
                }
            }

            // No existing text found. Now insert new nodes for
            // each unmatched characters in input text.
            for(;;)
            {
                // create new node
                Node * newNode = AllocNode();
                if( NULL == newNode )
                {
                    static Logger * sLogger = GetLogger("GN.base.StringMap");
                    GN_ERROR(sLogger)( "out of memory!" );
                    return NULL;
                }

                // create new leaf if reaching the end of the text
                if( 0 == *text )
                {
                    // we reach the end of the text. Now create a new leaf.
                    newNode->leaf = AllocLeaf( inputText, text - inputText, value );
                    if( NULL == newNode->leaf )
                    {
                        static Logger * sLogger = GetLogger("GN.base.StringMap");
                        GN_ERROR(sLogger)( "out of memory!" );
                        return NULL;
                    }

                    // TODO: find the real "previous" leaf node
                    Leaf * previousNode = mLeafs.GetTail();

                    // insert the new leaf into linked list
                    // TODO: sort and insert
                    mLeafs.InsertAfter( previousNode, newNode->leaf );

                    ++mCount;
                    inserted = true;
                }
                else
                {
                    newNode->leaf = 0;
                }

                // link new node into node tree
                newNode->parent = parentNode;
                newNode->splitchar = *text;
                newNode->lower = newNode->higher = newNode->equal = 0;
                *pp = newNode;

                // continue with next character or exit
                if( 0 == *text )
                {
                    GN_ASSERT((*pp)->leaf);
                    return (*pp)->leaf;
                }
                else
                {
                    parentNode = *pp;
                    pp = &((*pp)->equal);
                    ++text;
                }
            }
        }

        Leaf * DoFirst() const
        {
            return mLeafs.GetHead();
        }

        Leaf * DoNext( const KeyValuePair * p ) const
        {
            if( NULL == p ) return NULL;

            Leaf * leaf = (Leaf*)p;

            if( leaf->owner != &mLeafs )
            {
                static Logger * sLogger = GetLogger("GN.base.StringMap");
                GN_ERROR(sLogger)( "Input pointer does not belong to this string map." );
                return NULL;
            }

            return leaf->next;
        }

        Node * DoRecursiveErase( Node * n, const CHAR * text )
        {
            GN_ASSERT( text );

            if( NULL == n ) return NULL;

            int d = *text - n->splitchar;

            if( d < 0 )
            {
                n->lower = DoRecursiveErase( n->lower, text );
            }
            else if( d > 0 )
            {
                n->higher = DoRecursiveErase( n->higher, text );
            }
            else if( 0 == *text )
            {
                // We reaches the end of the string. It must contain a leaf node.
                GN_ASSERT( n->leaf );

                // delete leaf node
                FreeLeaf( n->leaf );
                n->leaf = NULL;

                --mCount;
            }
            else
            {
                n->equal = DoRecursiveErase( n->equal, text+1 );
            }

            // free the node if it contains neither children or leaf
            if( 0 == n->lower && 0 == n->higher && 0 == n->equal && 0 == n->leaf )
            {
                FreeNode( n );
                return NULL;
            }
            else
            {
                return n;
            }
        }

        void DoRemove( const CHAR * text )
        {
            // check for NULL text pointer
            if( NULL == text )
            {
                static Logger * sLogger = GetLogger("GN.base.StringMap");
                GN_WARN(sLogger)( "StringMap erasing warning: NULL text!" );
                return;
            }

            mRoot = DoRecursiveErase( mRoot, text );
        }
    }; // End of StringMap class

    /// \name string -> number conversion
    ///
    ///  Returns number of characters that are sucessfully scanned. Return 0 for failure.
    //@{

    size_t String2SignedInteger( SInt64 & result, int bits, int base, const char * s );
    size_t String2UnsignedInteger( UInt64 & result, int bits, int base, const char * s );

    template<typename T> size_t String2Integer( T & i, const char * s, int base = 10 )
    {
        size_t n;

        if( SignedType<T>::value )
        {
            SInt64 s64;
            n = String2SignedInteger( s64, sizeof(T)*8, base, s );
            if( n > 0 ) i = (T)s64;
        }
        else
        {
            UInt64 u64;
            n = String2UnsignedInteger( u64, sizeof(T)*8, base, s );
            if( n > 0 ) i = (T)u64;
        }

        return n;
    }

    template<typename T> T String2Integer( const char * s, T defaultValue, int base = 10 )
    {
        T result;
        if( 0 == String2Integer<T>( result, s, base ) )
        {
            return defaultValue;
        }
        else
        {
            return result;
        }
    }

    size_t String2Float( float & i, const char * s );

    size_t String2Double( double & i, const char * s );

    template<typename T> size_t String2Number( T & i, const char * s ) { return String2Integer<T>( i, s, 10 ); }
    template<> inline size_t String2Number<float>( float & i, const char * s ) { return String2Float( i, s ); }
    template<> inline size_t String2Number<double>( double & i, const char * s ) { return String2Double( i, s ); }

    ///
    /// Convert string to float array. String should be in format like:
    ///    float1, float2, float3, ...
    /// or:
    ///    float1 float2 float3 ...
    ///
    /// \return
    ///     Return count of floating filled into target buffer.
    ///
    size_t String2FloatArray( float * buffer, size_t maxCount, const char * str, size_t stringLength = 0 );

    //@}

    ///
    /// printf-like string format function.
    ///
    /// Similar as StringPrintf(...), but returns a string object
    ///
    inline StrA StringFormat( const char * fmt, ... )
    {
        StrA s;
        va_list arglist;
        va_start( arglist, fmt );
        s.FormatV( fmt, arglist );
        va_end( arglist );
        return s;
    }

    ///
    /// printf-like string format function (wide-char version)
    ///
    /// Similar as StringPrintf(...), but returns a string object
    ///
    inline StrW StringFormat( const wchar_t * fmt, ... )
    {
        StrW s;
        va_list arglist;
        va_start( arglist, fmt );
        s.FormatV( fmt, arglist );
        va_end( arglist );
        return s;
    }
}

// *****************************************************************************
//                 End of string.h
// *****************************************************************************
#endif // __GN_BASE_STRING_H__
