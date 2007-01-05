#ifndef __GN_BASE_MATH_H__
#define __GN_BASE_MATH_H__
// *****************************************************************************
//! \file    math.h
//! \brief   basic mathmatic functions
//! \author  chenlee (2005.4.17)
// *****************************************************************************

#include <math.h>

//!
//! Բ����
//!
#define GN_PI       3.1415926535897932385f

//!
//! Բ����/2
//!
#define GN_HALF_PI  (GN_PI / 2.0f)

//!
//! Բ����*2
//!
#define GN_TWO_PI   (GN_PI * 2.0f)

namespace GN
{
    //!
    //! floating equality check
    // ------------------------------------------------------------------------
    template<typename T>
    GN_FORCE_INLINE bool
    floatEqual( const T & a, const T & b )
    {
        const T epsilon = (T)0.0000001;
        T diff = a - b;
        return -epsilon < diff && diff < epsilon;
    }

    //!
    //! �Ƕ�->����
    // ------------------------------------------------------------------------
    template<typename T>
    GN_FORCE_INLINE T deg2rad( T a ) { return a*(T)0.01745329252f; }

    //!
    //! ����->�Ƕ�
    // ------------------------------------------------------------------------
    template<typename T>
    GN_FORCE_INLINE T rad2deg( T a ) { return a*(T)57.29577951f;   }

    //!
    //! ���n�Ƿ�Ϊ2^n
    // ------------------------------------------------------------------------
    GN_FORCE_INLINE bool isPowerOf2( UInt32 n )
    {
        return ( 0 == (n & (n - 1)) ) && ( 0 != n );
    }

    //!
    //! ���ز�С��n����С��2������
    // ------------------------------------------------------------------------
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

    //!
    //! ���ز�����n������2������
    // ------------------------------------------------------------------------
    GN_FORCE_INLINE UInt32 floorPowerOf2( UInt32 n )
    {
        n |= n >> 16;
        n |= n >> 8;
        n |= n >> 4;
        n |= n >> 2;
        n |= n >> 1;

        return (n + 1) >> 1;
    }
}

// *****************************************************************************
//                           End of math.h
// *****************************************************************************
#endif // __GN_BASE_MATH_H__
