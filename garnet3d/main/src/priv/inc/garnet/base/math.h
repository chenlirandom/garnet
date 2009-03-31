#ifndef __GN_BASE_MATH_H__
#define __GN_BASE_MATH_H__
// *****************************************************************************
/// \file
/// \brief   basic mathmatic functions
/// \author  chenlee (2005.4.17)
// *****************************************************************************

#include <math.h>

///
/// Բ����
///
#define GN_PI       3.1415926535897932385f

///
/// Բ����/2
///
#define GN_HALF_PI  (GN_PI / 2.0f)

///
/// Բ����*2
///
#define GN_TWO_PI   (GN_PI * 2.0f)

///
/// Get element count of C-style array
///
#define GN_ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))

///
/// Get byte offset of class member or struct field
///
#define GN_FIELD_OFFSET( class_, field ) \
    ( (size_t)(UIntPtr) &( ((class_*)(void*)(8))->field ) - 8 )

namespace GN
{
    ///
    /// namespace for math helpers
    ///
    namespace math
    {

        ///
        /// floating equality check with epsilon
        // --------------------------------------------------------------------
        template<typename T>
        GN_FORCE_INLINE bool
        floatEqual( const T & a, const T & b, const T & epsilon = (T)0.0000001 )
        {
            T diff = a - b;
            return -epsilon < diff && diff < epsilon;
        }

        ///
        /// �Ƕ�->����
        // --------------------------------------------------------------------
        template<typename T>
        GN_FORCE_INLINE T deg2rad( T a ) { return a*(T)0.01745329252f; }

        ///
        /// ����->�Ƕ�
        // --------------------------------------------------------------------
        template<typename T>
        GN_FORCE_INLINE T rad2deg( T a ) { return a*(T)57.29577951f;   }

        ///
        /// ���n�Ƿ�Ϊ2^n
        // --------------------------------------------------------------------
        template<typename T>
        GN_FORCE_INLINE bool isPowerOf2( T n )
        {
            return ( 0 == (n & (n - 1)) ) && ( 0 != n );
        }

        ///
        /// ���ز�С��n����С��2������
        // --------------------------------------------------------------------
        GN_FORCE_INLINE UInt32 ceilPowerOf2( UInt32 n )
        {
            n -= 1;

            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;

            return n + 1;
        }

        ///
        /// ���ز�����n������2������
        // --------------------------------------------------------------------
        GN_FORCE_INLINE UInt32 floorPowerOf2( UInt32 n )
        {
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;

            return (n + 1) >> 1;
        }

#if GN_X64

        ///
        /// ���ز�С��n����С��2������
        // --------------------------------------------------------------------
        GN_FORCE_INLINE size_t ceilPowerOf2( size_t n )
        {
            n -= 1;

            n |= n >> 32;
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;

            return n + 1;
        }

        ///
        /// ���ز�����n������2������
        // --------------------------------------------------------------------
        GN_FORCE_INLINE size_t floorPowerOf2( size_t n )
        {
            n |= n >> 32;
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;

            return (n + 1) >> 1;
        }

#endif

        ///
        /// ��value�޶���[vmin, vmax]������
        // --------------------------------------------------------------------
        template < typename T >
        inline void clamp( T & value, const T & vmin, const T & vmax )
        {
            value = vmin > value ? vmin : vmax < value ? vmax : value;
        }

        ///
        /// align numeric value up to next multiple of alignment.
        /// Note that the alignment must be 2^N.
        // --------------------------------------------------------------------
        template< typename T >
        inline T align( const T & value, const T & alignment )
        {
            GN_ASSERT( isPowerOf2( alignment ) );
            return ( value + (alignment-1) ) & ~(alignment-1);
        }

        ///
        /// get minimal value
        ///
        template<typename T> inline const T & getmin( const T & a, const T & b ) { return a < b ? a : b; }

        ///
        /// get minimal value
        ///
        template<typename T> inline const T & getmin( const T & a, const T & b, const T & c )
        {
            return a < b ? ( c < a ? c : a ) : ( c < b ? c : b );
        }

        ///
        /// get maximal value
        ///
        template<typename T> inline const T & getmax( const T & a, const T & b ) { return a > b ? a : b; }

        ///
        /// get maximal value
        ///
        template<typename T> inline const T & getmax( const T & a, const T & b, const T & c )
        {
            return a > b ? ( c > a ? c : a ) : ( c > b ? c : b );
        }
    }
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_MATH_H__
