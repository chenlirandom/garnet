#ifndef __GN_BASE_GEOMETRY_H__
#define __GN_BASE_GEOMETRY_H__
// *****************************************************************************
//! \file    geometry.h
//! \brief   geometry classes
//! \author  chenlee (2005.4.17)
// *****************************************************************************

namespace GN
{
    namespace detail
    {
        //!
        //! Swap to elements
        //!
        template<typename T>
        inline void swap( T & a, T & b )
        {
            T tmp;
            tmp = a;
            a = b;
            b = tmp;
        }
    }

    //!
    //! ��ʾһ����άʸ����������ͼ���꣩��
    //!
    template < typename T >
    class Vector2
    {
    public :
        //!
        //! element type
        //!
        typedef T ElementType;

        // ********************************
        //      public data members
        // ********************************

    public :

        //!
        //! X coordinate
        //!
        union
        {
            ElementType x;
            ElementType u;
            ElementType s;
            ElementType width;
        };

        //!
        //! Y coordinate
        //!
        union
        {
            ElementType y;
            ElementType v;
            ElementType t;
            ElementType height;
        };

        // ********************************
        //! \name  constructors
        // ********************************

    public:
        //@{
        Vector2()                                      {}
        Vector2(const Vector2 & v) : x(v.x), y(v.y)    {}
        Vector2(ElementType ix, ElementType iy) : x(ix), y(iy) {}
        //@}

        // ********************************
        //         operators
        // ********************************

    public:
        //!
        //! Convert to T *
        //!
        operator T *() { return &x; }
        //!
        //! Convert to const T *
        //!
        operator const T *() const { return &x; }
        //!
        //! Are these two Vector2's equal?
        //!
        bool operator == ( const Vector2 & a ) const
        {
            return((x == a.x) && (y == a.y));
        }
        //!
        //! Are these two Vector2's not equal?
        //!
        bool operator != ( const Vector2 & a ) const
        {
            return((a.x != x) || (a.y != y));
        }
        //!
        //! less operator
        //!
        bool operator < ( const Vector2 & a ) const
        {
            return x < a.x || y < a.y;
        }
        //!
        //! Add a Vector2 to this one
        //!
        Vector2 & operator += ( const Vector2 & v )
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        //!
        //! Subtract a Vector2 from this one
        //!
        Vector2 &operator -= ( const Vector2 & v )
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        //!
        //! Multiply the Vector2 by a ElementType
        //!
        Vector2 & operator *= ( ElementType f )
        {
            x *= f;
            y *= f;
            return *this;
        }
        //!
        //! Multiplied by another vector
        //!
        Vector2 & operator *= ( const Vector2 & v )
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }
        //!
        //! Divide the Vector2 by a ElementType
        //!
        Vector2 & operator /= ( ElementType f )
        {
            GN_ASSERT( 0.0f != f );
            x /= f;
            y /= f;
            return *this;
        }
        //!
        //! Divided by another vector
        //!
        Vector2 & operator /= ( const Vector2 & v )
        {
            GN_ASSERT( v.x != 0.0f && v.y != 0.0f );
            x /= v.x;
            y /= v.y;
            return *this;
        }
        //!
        //! Negate this vector
        //!
        friend Vector2 operator - ( const Vector2 & a )
        {
            return Vector2(-a.x, -a.y);
        }
        //!
        //! Add two Vector2's
        //!
        friend Vector2 operator + ( const Vector2 & a, const Vector2 & b )
        {
            Vector2 ret(a);
            ret += b;
            return ret;
        }
        //!
        //! Subtract one Vector2 from another
        //!
        friend Vector2 operator - ( const Vector2 & a, const Vector2 & b )
        {
            Vector2 ret(a);
            ret -= b;
            return ret;
        }
        //!
        //! Multiply two vectors
        //!
        friend Vector2 operator * ( const Vector2 & a, const Vector2 & b )
        {
            return Vector2( a.x * b.x, a.y * b.y );
        }
        //!
        //! Multiply Vector2 by a ElementType
        //!
        friend Vector2 operator * ( const Vector2 & v, ElementType f )
        {
            return Vector2(f * v.x, f * v.y);
        }
        //!
        //! Multiply Vector2 by a ElementType
        //!
        friend Vector2 operator * ( ElementType f, const Vector2 & v )
        {
            return Vector2(f * v.x, f * v.y);
        }
        //!
        //! Divide Vector2 by a ElementType
        //!
        friend Vector2 operator / ( const Vector2 & v, ElementType f )
        {
            return Vector2( v.x / f, v.y / f );
        }
        //!
        //! Divide by another vector
        //!
        friend Vector2 operator / ( const Vector2 & a, const Vector2 & b )
        {
            return Vector2( a.x / b.x, a.y / b.y );
        }

        // ********************************
        //      public functions
        // ********************************

    public:
        //!
        //! type conversion
        //!
        const T * toPtr() const
        {
            return &x;
        }
        //!
        //! type conversion
        //!
        T * toPtr()
        {
            return &x;
        }
        //!
        //! Set Values
        //!
        Vector2 & set( ElementType ix, ElementType iy )
        {
            x = ix;
            y = iy;
            return *this;
        }
        //!
        //! Get length of a Vector2
        //!
        ElementType length() const
        {
            return(ElementType) sqrt(x*x + y*y);
        }
        //!
        //! Get squared length of a Vector2
        //!
        ElementType lengthSqr() const
        {
            return(x*x + y*y);
        }
        //!
        //! Does Vector2 equal (0, 0)?
        //!
        bool isZero() const
        {
            return((x == 0.0F) && (y == 0.0F));
        }
        //!
        //! Normalize a Vector2
        //!
        Vector2 & normalize()
        {
			normalize( *this, *this );
            return *this;
        }

        //!
        //! Normalize a Vector2
        //!
        static Vector2 normalize( const Vector2 & v )
        {
            Vector2 r;
            normalize( r, v );
            return r;
        }

        //!
        //! Normalize a Vector2
        //!
        static void normalize( Vector2 & o, const Vector2 & i )
        {
            ElementType m = i.length();

            if( m > 0.0F )
            {
                m = 1.0F / m;
                o.x = i.x * m;
                o.y = i.y * m;
            }
			else
			{
				o.x = 0.0f;
				o.y = 0.0f;
			}
        }

        //!
        //! dot production
        //!
        static ElementType dot( const Vector2 & v1, const Vector2 & v2 )
        {
            return v1.x * v2.x + v1.y * v2.y;
        }
    };

    //!
    //! ��ʾһ��ʸ�������ǿռ��е�һ���㡣
    //!
    template < typename T >
    class Vector3
    {
        // ********************************
        //    public data members
        // ********************************
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! X-coordinate
        //!
        union
        {
            ElementType x;
            ElementType r;
            ElementType pitch;
        };

        //!
        //! Y-coordinate
        //!
        union
        {
            ElementType y;
            ElementType g;
            ElementType yaw;
        };

        //!
        //! Z-coordinate
        //!
        union
        {
            ElementType z;
            ElementType b;
            ElementType roll;
        };

        // ********************************
        //! \name    constructors
        // ********************************
    public:

        //@{
        Vector3()
        {}
        Vector3( ElementType ix, ElementType iy, ElementType iz )
            : x(ix), y(iy), z(iz)
        {}
        Vector3( const Vector3 & v )
            : x(v.x), y(v.y), z(v.z)
        {}
        Vector3( const Vector2<ElementType> & v, ElementType iz )
            : x(v.x), y(v.y), z(iz)
        {}
        //@}

        // ********************************
        //        operators
        // ********************************
    public:

        //!
        //! Convert to T *
        //!
        operator T *() { return &x; }
        //!
        //! Convert to const T *
        //!
        operator const T *() const { return &x; }
        //!
        //! less operator
        //!
        bool operator < ( const Vector3 & a ) const
        {
            return x < a.x || y < a.y || z < a.z;
        }
        //!
        //! Add a Vector3 to this one
        //!
        Vector3 & operator += ( const Vector3 & v )
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        //!
        //! Subtract a Vector3 from this one
        //!
        Vector3 & operator -= ( const Vector3 & v )
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        //!
        //! Multiply the Vector3 by a ElementType
        //!
        Vector3 &operator *= ( ElementType f )
        {
            x *= f;
            y *= f;
            z *= f;
            return *this;
        }
        //!
        //! Multiplied by another vector
        //!
        Vector3 & operator *= ( const Vector3 & v )
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }
        //!
        //! Divide the Vector3 by a ElementType
        //!
        Vector3 & operator /= ( ElementType f )
        {
            GN_ASSERT( 0.0f != f );
            x /= f;
            y /= f;
            z /= f;
            return *this;
        }
        //!
        //! Divided by another vector
        //!
        Vector3 & operator /= ( const Vector3 & v )
        {
            GN_ASSERT( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }
        //!
        //! Are these two Vector3's equal?
        //!
        friend bool operator == ( const Vector3 & a, const Vector3 & b )
        {
            return((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
        }
        //!
        //! Are these two Vector3's not equal?
        //!
        friend bool operator != ( const Vector3 & a, const Vector3 & b )
        {
            return((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
        }
        //!
        //! Negate a Vector3
        //!
        friend Vector3 operator - ( const Vector3 & a )
        {
            return Vector3(-a.x, -a.y, -a.z);
        }
        //!
        //! Add two Vector3's
        //!
        friend Vector3 operator + ( const Vector3 & a, const Vector3 & b )
        {
            Vector3 ret(a);
            ret += b;
            return ret;
        }
        //!
        //! Subtract one Vector3 from another
        //!
        friend Vector3 operator - ( const Vector3 & a, const Vector3 & b )
        {
            Vector3 ret(a);
            ret -= b;
            return ret;
        }
        //!
        //! Multiply two vectors (not dot production!!!)
        //!
        friend Vector3 operator * ( const Vector3 & a, const Vector3 & b )
        {
            return Vector3( a.x * b.x, a.y * b.y, a.z*b.z );
        }
        //!
        //! Multiply Vector3 by a ElementType
        //!
        friend Vector3 operator * ( const Vector3 & v, ElementType f )
        {
            return Vector3(f * v.x, f * v.y, f * v.z);
        }
        //!
        //! Multiply Vector3 by a ElementType
        //!
        friend Vector3 operator * ( ElementType f, const Vector3 & v )
        {
            return Vector3(f * v.x, f * v.y, f * v.z);
        }
        //!
        //! Divide Vector3 by a ElementType
        //!
        friend Vector3 operator / ( const Vector3 & v, ElementType f )
        {
            return Vector3(v.x / f, v.y / f, v.z / f);
        }
        //!
        //! Divide by another vector
        //!
        friend Vector3 operator / ( const Vector3 & a, const Vector3 & b )
        {
            return Vector3( a.x / b.x, a.y / b.y, a.z / b.z );
        }

        // ********************************
        //       public functions
        // ********************************
    public:

        //!
        //! type conversion
        //!
        const T * toPtr() const
        {
            return &x;
        }
        //!
        //! type conversion
        //!
        T * toPtr()
        {
            return &x;
        }
        //!
        //! Set Values
        //!
        Vector3 & set( ElementType ix, ElementType iy, ElementType iz )
        {
            x = ix;
            y = iy;
            z = iz;
            return *this;
        }
        //!
        //! Set Values
        //!
        Vector3 & set( const Vector2<T> & ixy, ElementType iz )
        {
            x = ixy.x;
            y = ixy.y;
            z = iz;
            return *this;
        }
        //!
        //! Get length of a Vector3
        //!
        ElementType length() const
        {
            return(ElementType) sqrt(x*x + y*y + z*z);
        }
        //!
        //! Get squared length of a Vector3
        //!
        ElementType lengthSqr() const
        {
            return(x*x + y*y + z*z);
        }
        //!
        //! Does Vector3 equal (0, 0, 0)?
        //!
        bool isZero() const
        {
            return((x == 0.0F) && (y == 0.0F) && (z == 0.0F));
        }
        //!
        //! Normalize a Vector3
        //!
        Vector3 & normalize()
        {
			normalize( *this, *this );
            return *this;
        }
        //!
        //! Normalize a Vector3
        //!
        static Vector3 normalize( const Vector3 & v )
        {
            Vector3 r;
            normalize( r, v );
            return r;
        }
        //!
        //! Normalize a Vector3
        //!
        static void normalize( Vector3 & o, const Vector3 & i )
        {
            ElementType m = i.length();

            if( m > 0.0F )
            {
                m = 1.0F / m;
                o.x = i.x * m;
                o.y = i.y * m;
				o.z = i.z * m;
            }
			else
			{
				o.x = 0.0f;
				o.y = 0.0f;
				o.z = 0.0f;
			}
        }
        //!
        //! dot production
        //!
        static ElementType dot( const Vector3 & v1, const Vector3 & v2 )
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }
        //!
        //! get cross product of v1 and v2, and store the result into this one
        //!
        static Vector3 cross( const Vector3 & v1, const Vector3 & v2 )
        {
            return Vector3 ( v1.y * v2.z - v1.z * v2.y,
                             v1.z * v2.x - v1.x * v2.z,
                             v1.x * v2.y - v1.y * v2.x);
        }
        //!
        //! get cross product of v1 and v2, and store the result into o
        //!
        static void cross( Vector3 & o, const Vector3 & v1, const Vector3 & v2 )
        {
            o.set( v1.y * v2.z - v1.z * v2.y,
                   v1.z * v2.x - v1.x * v2.z,
                   v1.x * v2.y - v1.y * v2.x);
        }
    };

    //!
    //! ��ά���꣨��ά������꣩
    //!
    template < typename T >
    class Vector4
    {
        // ********************************
        //  public data members
        // ********************************
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! X-coordinate
        //!
        union
        {
            ElementType x;
            ElementType r;
        };

        //!
        //! Y-coordinate
        //!
        union
        {
            ElementType y;
            ElementType g;
        };

        //!
        //! Z-coordinate
        //!
        union
        {
            ElementType z;
            ElementType b;
        };

        //!
        //! W-coordinate
        //!
        union
        {
            ElementType w;
            ElementType a;
        };

        // ********************************
        //! \name    constructors
        // ********************************
    public:
        //@{
        Vector4() {}
        //
        Vector4( ElementType ix, ElementType iy, ElementType iz, ElementType iw )
            : x(ix), y(iy), z(iz), w(iw)
        {}
        //
        Vector4( const Vector4 & v )
            : x(v.x), y(v.y), z(v.z), w(v.w)
        {}
        //
        Vector4( const Vector2<ElementType> & v, ElementType iz, ElementType iw )
            : x(v.x), y(v.y), z(iz), w(iw)
        {}
        //
        Vector4( const Vector3<ElementType> & v, ElementType iw )
            : x(v.x), y(v.y), z(v.z), w(iw)
        {}
        //@}

        // ********************************
        //        operators
        // ********************************
    public:

        //!
        //! Convert to T *
        //!
        operator T *() { return &x; }
        //!
        //! Convert to const T *
        //!
        operator const T *() const { return &x; }
        //!
        //! less operator
        //!
        bool operator < ( const Vector4 & a ) const
        {
            return x < a.x || y < a.y || z < a.z || w < a.w;
        }
        //!
        //! Add a Vector4 to this one
        //!
        Vector4 & operator += ( const Vector4 & v )
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        //!
        //! Subtract a Vector4 from this one
        //!
        Vector4 & operator -= ( const Vector4 & v )
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        //!
        //! Multiply the Vector4 by a ElementType
        //!
        Vector4 & operator *= ( ElementType f )
        {
            x *= f;
            y *= f;
            z *= f;
            w *= f;
            return *this;
        }
        //!
        //! Multiplied by another vector
        //!
        Vector4 & operator *= ( const Vector4 & v )
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }
        //!
        //! Divide the Vector4 by a ElementType
        //!
        Vector4 & operator /= ( ElementType f )
        {
            GN_ASSERT( 0.0f != f );
            x /= f;
            y /= f;
            z /= f;
            w /= f;
            return *this;
        }
        //!
        //! Divided by another vector
        //!
        Vector4 & operator /= ( const Vector4 & v )
        {
            GN_ASSERT( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w != 0.0f );
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }
        //!
        //! Are these two Vector4's equal?
        //!
        friend bool operator == ( const Vector4 & a, const Vector4 & b )
        {
            return((a.x == b.x) && (a.y == b.y) &&
                (a.z == b.z) && (a.w == b.w));
        }
        //!
        //! Are these two Vector4's not equal?
        //!
        friend bool operator != ( const Vector4 & a, const Vector4 & b )
        {
            return((a.x != b.x) || (a.y != b.y) ||
                (a.z != b.z) || (a.w != b.w));
        }
        //!
        //! Negate a Vector4
        //!
        friend Vector4 operator - ( const Vector4 & a )
        {
            return Vector4(-a.x, -a.y, -a.z, -a.w);
        }
        //!
        //! Add two Vector4's
        //!
        friend Vector4 operator + ( const Vector4 & a, const Vector4 & b )
        {
            Vector4 ret(a);
            ret += b;
            return ret;
        }
        //!
        //! Subtract one Vector4 from another
        //!
        friend Vector4 operator - ( const Vector4 & a, const Vector4 & b )
        {
            Vector4 ret(a);
            ret -= b;
            return ret;
        }
        //!
        //! Multiply two vectors (not dot production!!!)
        //!
        friend Vector4 operator * ( const Vector4 & a, const Vector4 & b )
        {
            return Vector4( a.x * b.x, a.y * b.y, a.z*b.z, a.w*b.w );
        }
        //!
        //! Multiply Vector4 by a ElementType
        //!
        friend Vector4 operator * ( const Vector4 & v, ElementType f )
        {
            return Vector4(f * v.x, f * v.y, f * v.z, f * v.w);
        }
        //!
        //! Multiply Vector4 by a ElementType
        //!
        friend Vector4 operator * ( ElementType f, const Vector4 & v )
        {
            return Vector4(f * v.x, f * v.y, f * v.z, f * v.w);
        }
        //!
        //! Divide Vector4 by a ElementType
        //!
        friend Vector4 operator / ( const Vector4 & v, ElementType f )
        {
            return Vector4(v.x / f, v.y / f, v.z / f, v.w / f);
        }
        //!
        //! Divide by another vector
        //!
        friend Vector4 operator / ( const Vector4 & a, const Vector4 & b )
        {
            return Vector4( a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w );
        }

        // ********************************
        //        public functions
        // ********************************
    public:

        //!
        //! type conversion
        //!
        const T * toPtr() const
        {
            return &x;
        }
        //!
        //! type conversion
        //!
        T * toPtr()
        {
            return &x;
        }
        //!
        //! Set Values
        //!
        Vector4 & set( ElementType ix, ElementType iy, ElementType iz, ElementType iw )
        {
            x = ix;
            y = iy;
            z = iz;
            w = iw;
            return *this;
        }
        //!
        //! Set Values
        //!
        Vector4 & set( const Vector2<ElementType> & ixy, ElementType iz, ElementType iw )
        {
            x = ixy.x;
            y = ixy.y;
            z = iz;
            w = iw;
            return *this;
        }
        //!
        //! Set Values
        //!
        Vector4 & set( const Vector3<ElementType> & ixyz, ElementType iw )
        {
            x = ixyz.x;
            y = ixyz.y;
            z = ixyz.z;
            w = iw;
            return *this;
        }
        //!
        //! Get length of a Vector4
        //!
        ElementType length() const
        {
            return(ElementType) sqrt(x*x + y*y + z*z + w*w);
        }
        //!
        //! Get squared length of a Vector4
        //!
        ElementType lengthSqr() const
        {
            return(x*x + y*y + z*z + w*w);
        }
        //!
        //! Normalize a Vector4
        //!
        Vector4 & normalize()
        {
			normalize( *this, *this );
            return *this;
        }
        //!
        //! Normalize a Vector4
        //!
        static Vector4 normalize( const Vector4 & v )
        {
            Vector4 r;
            normalize( r, v );
            return r;
        }
        //!
        //! Normalize a Vector4
        //!
        static void normalize( Vector4 & o, const Vector4 & i )
        {
            ElementType m = i.length();

            if( m > 0.0F )
            {
                m = 1.0F / m;
                o.x = i.x * m;
                o.y = i.y * m;
				o.z = i.z * m;
				o.w = i.w * m;
            }
			else
			{
				o.x = 0.0f;
				o.y = 0.0f;
				o.z = 0.0f;
				o.w = 0.0f;
			}
        }
        //!
        //! Convert to a 3D vector (divide by w)
        //!
        Vector3<ElementType> toVec3() const
        {
            Vector3<ElementType> ret( x / w, y / w, z / w );
            return ret;
        }
        //!
        //! Convert to a 3D vector (divide by w)
        //!
        Vector3<ElementType> & toVec3( Vector3<ElementType> & o ) const
        {
            o.set( x / w, y / w, z / w );
            return o;
        }
        //!
        //! dot production
        //!
        static ElementType dot( const Vector4 & v1, const Vector4 & v2 )
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
        }
    };

    //!
    //! 3x3���� ( row major )
    //!
    template< typename T >
    class Matrix33
    {
        // ********************************
        //    public data members
        // ********************************
    public:

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! matrix rows
        //!
        Vector3<T> rows[3];

        // ********************************
        //    static data members
        // ********************************
    public:

        //!
        //! identity matrix
        //!
        static Matrix33 IDENTITY;

        // ********************************
        //! \name constructors
        // ********************************
    public:
        //@{
        Matrix33() {}
        //
        Matrix33(
            ElementType _00, ElementType _01, ElementType _02,
            ElementType _10, ElementType _11, ElementType _12,
            ElementType _20, ElementType _21, ElementType _22 )
        {
            rows[0].set(_00,_01,_02);
            rows[1].set(_10,_11,_12);
            rows[2].set(_20,_21,_22);
        }
        //
        Matrix33( const Matrix33 & m )
        {
            rows[0] = m[0];
            rows[1] = m[1];
            rows[2] = m[2];
        }
        //@}

        // ********************************
        //         operators
        // ********************************
    public:

        //!
        //! type conversion
        //!
        operator const Vector3<T> * () const
        {
            return rows;
        }
        //!
        //! type conversion
        //!
        operator Vector3<T> * ()
        {
            return rows;
        }
        //!
        //! assignment
        //!
        Matrix33 &operator = ( const Matrix33 & m )
        {
            if (this == &m) return *this;

            rows[0] = m[0];
            rows[1] = m[1];
            rows[2] = m[2];
            return *this;
        }
        //!
        //! Add a Matrix33 to this one
        //!
        Matrix33 & operator += ( const Matrix33 & m )
        {
            rows[0] += m[0];
            rows[1] += m[1];
            rows[2] += m[2];
            return *this;
        }
        //!
        //! Subtract a Matrix33 from this one
        //!
        Matrix33 & operator -= ( const Matrix33 & m )
        {
            rows[0] -= m[0];
            rows[1] -= m[1];
            rows[2] -= m[2];
            return *this;
        }
        //!
        //! Multiply the Matrix33 by another Matrix33
        //!
        Matrix33<T> & operator *= ( const Matrix33<T> & m );
        //!
        //! Multiply the Matrix33 by a ElementType
        //!
        Matrix33 & operator *= ( ElementType f )
        {
            rows[0] *= f;
            rows[1] *= f;
            rows[2] *= f;
            return *this;
        }
        //!
        //! Are these two Matrix33's equal?
        //!
        friend bool operator == ( const Matrix33 & a, const Matrix33 & b )
        {
            return((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
        }
        //!
        //! Are these two Matrix33's not equal?
        //!
        friend bool operator != ( const Matrix33 & a, const Matrix33 & b )
        {
            return((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]));
        }
        //!
        //! Add two Matrix33's
        //!
        friend Matrix33 operator + ( const Matrix33 & a, const Matrix33 & b )
        {
            Matrix33 ret(a);
            ret += b;
            return ret;
        }
        //!
        //! Subtract one Matrix33 from another
        //!
        friend Matrix33 operator - ( const Matrix33 & a, const Matrix33 & b )
        {
            Matrix33 ret(a);
            ret -= b;
            return ret;
        }
        //!
        //! Multiply Matrix33 by another Matrix33
        //!
        friend Matrix33   operator * ( const Matrix33 & a, const Matrix33 & b )
        {
            Matrix33 ret(a);
            ret *= b;
            return ret;
        }

        ////! Multiply a row Vector3<T> by this matrix
        //friend Vector3<T> operator * ( const Vector3<T> & v, const Matrix33 & m )
        //{
        //    Vector3<T> ret;
        //    ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
        //    ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
        //    ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
        //    return ret;
        //}

        //!
        //! Multiply this matrix by a colume vector
        //!
        friend Vector3<T> operator * ( const Matrix33 & m, const Vector3<T> & v )
        {
            Vector3<T> ret;
            ret.x = Vector3<T>::dot( m[0], v );
            ret.y = Vector3<T>::dot( m[1], v );
            ret.z = Vector3<T>::dot( m[2], v );
            return ret;
        }
        //!
        //! Multiply Matrix33 by a ElementType
        //!
        friend Matrix33 operator * ( const Matrix33 & m, ElementType f )
        {
            Matrix33 ret(m);
            ret *= f;
            return ret;
        }
        //!
        //! Multiply Matrix33 by a ElementType
        //!
        friend Matrix33 operator * ( ElementType f, const Matrix33 & m )
        {
            Matrix33 ret(m);
            ret *= f;
            return ret;
        }

        // ********************************
        //       public functions
        // ********************************
    public:

        //!
        //! ���������һ
        //!
        Matrix33 & identify()
        {
            rows[0].set(1,0,0);
            rows[1].set(0,1,0);
            rows[2].set(0,0,1);
            return *this;
        }
        //!
        //! ��������ת��
        //!
        Matrix33 & transpose()
        {
            detail::swap( rows[0][1], rows[1][0] );
            detail::swap( rows[0][2], rows[2][0] );
            detail::swap( rows[1][2], rows[2][1] );
            return *this;
        }
        //!
        //! ��ת�þ���
        //!
        static void transpose( Matrix33 & dst, const Matrix33 & src )
        {
            dst = src;
            dst.transpose();
        }
        //!
        //! ��ת�þ���
        //!
        static Matrix33 transpose( const Matrix33 & src )
        {
            Matrix33 r(src);
            r.transpose();
            return r;
        }
        //!
        //! ������������
        //!
        Matrix33 & invert();
        //!
        //! �������
        //!
        static void invert( Matrix33 & dst, const Matrix33 & src )
        {
            dst = src;
            dst.invert();
        }
        //!
        //! �������
        //!
        static Matrix33 invert( const Matrix33 & src )
        {
            Matrix33 r(src);
            r.invert();
            return r;
        }
        //!
        //! ���ž���
        //!
        Matrix33 & scale( const Vector3<T> & f )
        {
            identify();
            rows[0][0] = f[0];
            rows[1][1] = f[1];
            rows[2][2] = f[2];
            return *this;
        }
        //!
        //! ����ת����
        //!
        //! \param angle Angle of rotation in radians
        //!
        Matrix33 & rotateX( ElementType angle );
        //!
        //! ����ת����
        //!
        //! \param angle Angle of rotation in radians
        //!
        Matrix33 & rotateY( ElementType angle );
        //!
        //! ����ת����
        //!
        //! \param angle Angle of rotation in radians
        //!
        Matrix33 & rotateZ( ElementType angle );
        //!
        //! ����ת����
        //!
        //! \param v     Axis of rotation
        //! \param angle Angle of rotation in radians
        //!
        Matrix33 & rotate( const Vector3<T> & v, ElementType angle );
        //!
        //! ������lookat����
        //!
        //! \param forward  Forward vector
        //! \param up       Up vector
        //!
        Matrix33 & lookAtLh( const Vector3<T> & forward, const Vector3<T> & up );
        //!
        //! ������lookat����
        //!
        //! \param forward  Forward vector
        //! \param up       Up vector
        //!
        Matrix33 & lookAtRh( const Vector3<T> & forward, const Vector3<T> & up );
        //!
        //! ��lookat����
        //!
        //! \param forward  Forward vector
        //! \param up       Up vector
        //!
        Matrix33 & lookAt( const Vector3<T> & forward, const Vector3<T> & up )
        {
#if GN_LEFT_HAND
            return lookAtLh( forward, up );
#else
            return lookAtRh( forward, up );
#endif
        }
        //!
        //! ��ӡ�������ݵ��ַ�����, mainly for debug purpose.
        //!
        void print( StrA & ) const;
    };

    // static member
    template< typename T >
    Matrix33<T> Matrix33<T>::IDENTITY( 1,0,0,
                                       0,1,0,
                                       0,0,1 );

    //!
    //! 4x4 matrix ( row major )
    //!
    template < typename T >
    class Matrix44
    {
        // ********************************
        //  public data members
        // ********************************
    public:

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! matrix rows
        //!
        Vector4<T> rows[4];

        // ********************************
        //   static data members
        // ********************************
    public:

        //!
        //! identity matrix
        //!
        static Matrix44 IDENTITY;

        // ********************************
        //! \name constructors
        // ********************************
    public:
        //@{
        Matrix44() {};
        //
        Matrix44(
            ElementType _00, ElementType _01, ElementType _02, ElementType _03,
            ElementType _10, ElementType _11, ElementType _12, ElementType _13,
            ElementType _20, ElementType _21, ElementType _22, ElementType _23,
            ElementType _30, ElementType _31, ElementType _32, ElementType _33 )
        {
            rows[0].set(_00,_01,_02,_03);
            rows[1].set(_10,_11,_12,_13);
            rows[2].set(_20,_21,_22,_23);
            rows[3].set(_30,_31,_32,_33);
        }
        //
        Matrix44( const Matrix44 & m )
        {
            rows[0] = m[0];
            rows[1] = m[1];
            rows[2] = m[2];
            rows[3] = m[3];
        }
        //
        explicit Matrix44(
            const Matrix33<T> & m33,
            const Vector3<T>  & col3 = Vector3<T>(0,0,0),
            const Vector3<T>  & row3 = Vector3<T>(0,0,0),
            ElementType        _33   = 1.0f )
        {
            rows[0].set( m33[0], col3.x );
            rows[1].set( m33[1], col3.y );
            rows[2].set( m33[2], col3.z );
            rows[3].set( row3, _33 );
        }
        //@}

        // ********************************
        //          operators
        // ********************************
    public:

        //!
        //! type conversion
        //!
        operator const Vector4<T> * () const
        {
            return rows;
        }
        //!
        //! type conversion
        //!
        operator Vector4<T> * ()
        {
            return rows;
        }
        //!
        //! Assign
        //!
        Matrix44 & operator = ( const Matrix44 & m )
        {
            if (this == &m) return *this;
            rows[0] = m[0];
            rows[1] = m[1];
            rows[2] = m[2];
            rows[3] = m[3];
            return *this;
        }
        //!
        //! Assign a 3x3 matrix to the Matrix44
        //!
        Matrix44 & operator = ( const Matrix33<T> & m )
        {
            rows[0].set( m[0], 0.0f );
            rows[1].set( m[1], 0.0f );
            rows[2].set( m[2], 0.0f );
            rows[3].set( 0.0f, 0.0f, 0.0f, 1.0f );
            return *this;
        }
        //!
        //! Add a Matrix44 to this one
        //!
        Matrix44 & operator += ( const Matrix44 & m )
        {
            rows[0] += m[0];
            rows[1] += m[1];
            rows[2] += m[2];
            rows[3] += m[3];
            return *this;
        }
        //!
        //! Subtract a Matrix44 from this one
        //!
        Matrix44 & operator -= ( const Matrix44 & m )
        {
            rows[0] -= m[0];
            rows[1] -= m[1];
            rows[2] -= m[2];
            rows[3] -= m[3];
            return *this;
        }
        //!
        //! Multiply the Matrix44 by another Matrix44
        //!
        Matrix44<T> & operator *= ( const Matrix44<T> & m );
        //!
        //! Multiply the Matrix44 by a ElementType
        //!
        Matrix44 & operator *= ( ElementType f )
        {
            rows[0] *= f;
            rows[1] *= f;
            rows[2] *= f;
            rows[3] *= f;
            return *this;
        }
        //!
        //! Are these two Matrix44's equal?
        //!
        friend bool operator == ( const Matrix44 & a, const Matrix44 & b )
        {
            return( (a[0] == b[0]) && (a[1] == b[1]) &&
                    (a[2] == b[2]) && (a[3] == b[3]) );
        }
        //!
        //! Are these two Matrix44's not equal?
        //!
        friend bool operator != ( const Matrix44 & a, const Matrix44 & b )
        {
            return( (a[0] != b[0]) || (a[1] != b[1]) ||
                    (a[2] != b[2]) || (a[3] != b[3]) );
        }
        //!
        //! Add two Matrix44's
        //!
        friend Matrix44
        operator + ( const Matrix44 & a, const Matrix44 & b )
        {
            Matrix44 ret(a);
            ret += b;
            return ret;
        }
        //!
        //! Subtract one Matrix44 from another
        //!
        friend Matrix44
        operator - ( const Matrix44 & a, const Matrix44 & b )
        {
            Matrix44 ret(a);
            ret -= b;
            return ret;
        }
        //!
        //! Multiply Matrix44 by another Matrix44
        //!
        friend Matrix44
        operator * ( const Matrix44 & a, const Matrix44 & b )
        {
            Matrix44 ret(a);
            ret *= b;
            return ret;
        }

        ////! Multiply a row 4-D vector by this matrix
        //friend Vector4<T> operator * ( const Vector4<T> & v,
        //    const Matrix44 & m )
        //{
        //    Vector4<T> ret;
        //    ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
        //    ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
        //    ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
        //    ret.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
        //    return ret;
        //}

        //!
        //! Multiply this matrix by a column 4-D vector
        //!
        friend Vector4<T>
        operator * ( const Matrix44 & m, const Vector4<T> & v )
        {
            Vector4<T> ret;
            ret.x = Vector4<T>::dot( m[0], v );
            ret.y = Vector4<T>::dot( m[1], v );
            ret.z = Vector4<T>::dot( m[2], v );
            ret.w = Vector4<T>::dot( m[3], v );
            return ret;
        }
        //!
        //! Multiply Matrix44 by a ElementType
        //!
        friend Matrix44 operator * ( const Matrix44 & m, ElementType f )
        {
            Matrix44 ret(m);
            ret *= f;
            return ret;
        }
        //!
        //! Multiply Matrix44 by a ElementType
        //!
        friend Matrix44 operator * ( ElementType f, const Matrix44 & m )
        {
            Matrix44 ret(m);
            ret *= f;
            return ret;
        }

        // ********************************
        //       public functions
        // ********************************
    public:

        //!
        //! set the content of the matrix
        //!
        Matrix44 & set( const Matrix33<T> & m33,
                        const Vector3<T> & col3 = Vector3<T>(0,0,0),
                        const Vector3<T> & row3 = Vector3<T>(0,0,0),
                        ElementType         _33 = 1.0f )
        {
            rows[0].set( m33[0], col3.x );
            rows[1].set( m33[1], col3.y );
            rows[2].set( m33[2], col3.z );
            rows[3].set( row3, _33 );
            return *this;
        }
        //!
        //! identify the matrix
        //!
        Matrix44 & identify()
        {
            rows[0].set( 1.0f, 0.0f, 0.0f, 0.0f );
            rows[1].set( 0.0f, 1.0f, 0.0f, 0.0f );
            rows[2].set( 0.0f, 0.0f, 1.0f, 0.0f );
            rows[3].set( 0.0f, 0.0f, 0.0f, 1.0f );
            return *this;
        }
        //!
        //! ��������ת��
        //!
        Matrix44 & transpose()
        {
            detail::swap( rows[0][1], rows[1][0] );
            detail::swap( rows[0][2], rows[2][0] );
            detail::swap( rows[0][3], rows[3][0] );
            detail::swap( rows[1][2], rows[2][1] );
            detail::swap( rows[1][3], rows[3][1] );
            detail::swap( rows[2][3], rows[3][2] );
            return *this;
        }
        //!
        //! ��ת�þ���
        //!
        static void transpose( Matrix44 & dst, const Matrix44 & src )
        {
            dst = src;
            dst.transpose();
        }
        //!
        //! ��ת�þ���
        //!
        static Matrix44 transpose( const Matrix44 & src )
        {
            Matrix44 r(src);
            r.transpose();
            return r;
        }
        //!
        //! ������������
        //!
        Matrix44 & invert();
        //!
        //! �������
        //!
        static void invert( Matrix44 & dst, const Matrix44 & src )
        {
            dst = src;
            dst.invert();
        }
        //!
        //! �������
        //!
        static Matrix44 invert( const Matrix44 & src )
        {
            Matrix44 r(src);
            r.invert();
            return r;
        }
        //!
        //! inverse and tranpose
        //!
        Matrix44 & invtrans()
        {
            invert();
            transpose();
        }
        //!
        //! inverse and tranpose
        //!
        static void invtrans( Matrix44 & dst, const Matrix44 & src )
        {
            dst = src;
            dst.invtrans();
        }
        //!
        //! inverse and tranpose
        //!
        static Matrix44 invtrans( const Matrix44 & src )
        {
            Matrix44 r(src);
            r.invtrans();
            return r;
        }
        //!
        //! ����ת����
        //!
        //! \param angle Angle of rotation in radians
        //!
        Matrix44 & rotateX( ElementType angle );
        //!
        //! ����ת����
        //!
        //! \param angle Angle of rotation in radians
        //!
        Matrix44 & rotateY( ElementType angle );
        //!
        //! ����ת����
        //!
        //! \param angle Angle of rotation in radians
        Matrix44 & rotateZ( ElementType angle );
        //!
        //! ����ת����
        //!
        //! \param v     Axis of rotation
        //! \param angle Angle of rotation in radians
        Matrix44 & rotate( const Vector3<T> & v, ElementType angle );
        //!
        //! ��ƽ�ƾ���
        //!
        //! This function will generate a translate matrix like this:\n
        //! \n
        //! 1, 0, 0, x   \n
        //! 0, 1, 0, y   \n
        //! 0, 0, 1, z   \n
        //! 0, 0, 0, 1
        //!
        Matrix44 & translate( ElementType x, ElementType y = .0f, ElementType z = .0f )
        {
            identify();
            rows[0][3] = x;
            rows[1][3] = y;
            rows[2][3] = z;
            return *this;
        }
        //!
        //! ��ƽ�ƾ���
        //!
        Matrix44 & translate( const Vector3<T> & v )
        {
            return translate( v.x, v.y, v.z );
        }
        //!
        //! ��ƽ�ƾ���
        //!
        Matrix44 & translate( const Vector2<T> & v )
        {
            return translate( v.x, v.y, .0f );
        }
        //!
        //! ��Lookat����
        //!
        //! \param eye �ӵ�����
        //! \param to  Ŀ�������
        //! \param up  ������
        //!
        Matrix44 & lookAtLh( const Vector3<T> & eye,
                             const Vector3<T> & to,
                             const Vector3<T> & up );
        //!
        //! ������Lookat����
        //!
        //! \param eye �ӵ�����
        //! \param to  Ŀ�������
        //! \param up  ������
        //!
        Matrix44 & lookAtRh( const Vector3<T> & eye,
                             const Vector3<T> & to,
                             const Vector3<T> & up );
        //!
        //! ��Lookat����
        //!
        //! \param eye �ӵ�����
        //! \param to  Ŀ�������
        //! \param up  ������
        //!
        Matrix44 & lookAt( const Vector3<T> & eye,
                           const Vector3<T> & to,
                           const Vector3<T> & up )
        {
#if GN_LEFT_HAND
            return lookAtLh( eye, to, up );
#else
            return lookAtRh( eye, to, up );
#endif
        }
        //!
        //! ����������ͶӰ����
        //!
        //! ��ͶӰ������Ļ���½�ӳ��Ϊԭ��(left, bottom)�����Ͻ�Ϊ(right, top)
        //! Z�᷶Χ��znear��zfar
        //! left���ܵ���right, bottom���ܵ���top, znear���ܵ���zfar�������
        //! �����0����
        //!
        //! \note
        //!     �ڱ�׼������3άͼ��ѧ�У�ͶӰ�任��Z��ķ�ΧӦ����[1, -1]��
        //!     ��directx��opengl�ֱ�ʹ�õ���[0, 1]��[-1, 1]��
        //!     ��ˣ�ͼ��ѧ�̿����и����ı�׼ͶӰ�任�����޷���opengl��directx
        //!     ��ʹ�ã��Ҷ���ͬ����ͶӰ�任Ч������directx��opengl�б���ʹ�ò�
        //!     ͬ�ı任���� ����ر����������׷ֱ�������opengl��directx�ļ���
        //!     ͶӰ����ĺ�����
        //!
        Matrix44 & orthoOGLLh( ElementType left, ElementType right,
                               ElementType bottom, ElementType top,
                               ElementType znear, ElementType zfar );

        //!
        //! ����������ͶӰ����
        //!
        //! \sa orthoOGLLh
        //!
        Matrix44 & orthoOGLRh( ElementType left, ElementType right,
                               ElementType bottom, ElementType top,
                               ElementType znear, ElementType zfar );

        //!
        //! ������ͶӰ����
        //!
        //! \sa orthoOGLLh
        //!
        Matrix44 & orthoOGL( ElementType left, ElementType right,
                             ElementType bottom, ElementType top,
                             ElementType znear, ElementType zfar )
        {
#if GN_LEFT_HAND
            return orthoOGLLh( left, right, bottom, top, znear, zfar );
#else
            return orthoOGLRh( left, right, bottom, top, znear, zfar );
#endif
        }
        //!
        //! ����������ͶӰ����
        //!
        //! \sa orthoOGLLh
        //!
        Matrix44 & orthoD3DLh( ElementType left, ElementType right,
                               ElementType bottom, ElementType top,
                               ElementType znear, ElementType zfar );
        //!
        //! ����������ͶӰ����
        //!
        //! \sa orthoOGLLh
        //!
        Matrix44 & orthoD3DRh( ElementType left, ElementType right,
                               ElementType bottom, ElementType top,
                               ElementType znear, ElementType zfar );
        //!
        //! ������ͶӰ����
        //!
        //! \sa orthoOGLLh
        //!
        Matrix44 & orthoD3D( ElementType left, ElementType right,
                             ElementType bottom, ElementType top,
                             ElementType znear, ElementType zfar )
        {
#if GN_LEFT_HAND
            return orthoD3DLh( left, right, bottom, top, znear, zfar );
#else
            return orthoD3DRh( left, right, bottom, top, znear, zfar );
#endif
        }
        //!
        //! ������͸��ͶӰ����
        //!
        //! \param fovy        ��ֱ�����ϵ��ӽǴ�С���Ի���Ϊ��λ
        //! \param ratio       �ӳ��Ŀ�Ⱥ͸߶ȵı�ֵ
        //! \param znear, zfar �ɼ���Z�᷶Χ
        //!
        //! \note fovy��ratio����Ϊ0��znear���ܵ���zfar������������0����
        //!
        Matrix44 & perspectiveOGLLh( ElementType fovy, ElementType ratio,
                                     ElementType znear, ElementType zfar );
        //!
        //! ������͸��ͶӰ����
        //!
        //! \sa perspectiveOGLLh
        //!
        Matrix44 & perspectiveOGLRh( ElementType fovy, ElementType ratio,
                                     ElementType znear, ElementType zfar );
        //!
        //! ��͸��ͶӰ����
        //!
        //! \sa perspectiveOGLLh
        //!
        Matrix44 & perspectiveOGL( ElementType fovy, ElementType ratio,
                                   ElementType znear, ElementType zfar )
        {
#if GN_LEFT_HAND
            return perspectiveOGLLh( fovy, ratio, znear, zfar );
#else
            return perspectiveOGLRh( fovy, ratio, znear, zfar );
#endif
        }
        //!
        //! ������͸��ͶӰ����
        //!
        //! \sa perspectiveOGLLh
        //!
        Matrix44 & perspectiveD3DLh( ElementType fovy, ElementType ratio,
                                     ElementType znear, ElementType zfar );
        //!
        //! ������͸��ͶӰ����
        //!
        //! \sa perspectiveOGLLh
        //!
        Matrix44 & perspectiveD3DRh( ElementType fovy, ElementType ratio,
                                     ElementType znear, ElementType zfar );
        //!
        //! ��͸��ͶӰ����
        //!
        //! \sa perspectiveOGLLh
        //!
        Matrix44 & perspectiveD3D( ElementType fovy, ElementType ratio,
                                   ElementType znear, ElementType zfar )
        {
#if GN_LEFT_HAND
            return perspectiveD3DLh( fovy, ratio, znear, zfar );
#else
            return perspectiveD3DRh( fovy, ratio, znear, zfar );
#endif
        }
        //!
        //! transform a 3-D point by this matrix
        //!
        void transformPoint( Vector3<T> & dst, const Vector3<T> & src ) const
        {
            dst.x = rows[0].x * src.x + rows[0].y * src.y + rows[0].z * src.z + rows[0].w;
            dst.y = rows[1].x * src.x + rows[1].y * src.y + rows[1].z * src.z + rows[1].w;
            dst.z = rows[2].x * src.x + rows[2].y * src.y + rows[2].z * src.z + rows[2].w;

            float k = rows[3].x*src.x + rows[3].y*src.y + rows[3].z*src.z + rows[3].w;

            if( .0f != k ) dst /= k;
            else GN_WARN( "the vertex is transformed to infinite place" );
        }
        //!
        //! transform a 3-D point by this matrix
        //!
        Vector3<T> transformPoint( const Vector3<T> & src ) const
        {
			Vector3<T> dst;
			transformPoint( dst, src );
			return dst;
        }
        //!
        //! transform a vector by this matrix
        //!
        //! \note
        //!     To transform an normal, you should use invtrans of the desired matrix.
        //!
        void transformVector( Vector3<T> & dst, const Vector3<T> & src ) const
        {
            dst.x = rows[0].x * src.x + rows[0].y * src.y + rows[0].z * src.z;
            dst.y = rows[1].x * src.x + rows[1].y * src.y + rows[1].z * src.z;
            dst.z = rows[2].x * src.x + rows[2].y * src.y + rows[2].z * src.z;
        }
        //!
        //! transform a vector by this matrix
        //!
        //! \note
        //!     To transform an normal, you should use invtrans of the desired matrix.
        //!
        Vector3<T> transformVector( const Vector3<T> & src ) const
        {
            Vector3<T> dst;
			transformVector( dst, src );
            return dst;
        }
        //!
        //! ��ӡ�������ݵ��ַ���
        //!
        void print( StrA & ) const;
        //!
        //! ��ӡ�������ݵ��ַ���
        //!
        StrA print() const
        {
			StrA s;
			print(s);
			return s;
        }
    };

    // static member
    template < typename T >
    Matrix44<T> Matrix44<T>::IDENTITY( 1,0,0,0,
                                       0,1,0,0,
                                       0,0,1,0,
                                       0,0,0,1 );

    //!
    //! ��Ԫ��
    //!
    template < typename T >
    class Quaternion
    {
        // ********************************
        //    data members
        // ********************************
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        Vector3<T>  v; //!< axis
        ElementType w; //!< W-value

        static Quaternion IDENTITY; //!< identity quaternion

        // ********************************
        //! \name    constructors
        // ********************************
    public:

        //@{
        Quaternion() {};
        //
        Quaternion( const Quaternion & q ) : v(q.v), w(q.w) {}
        //
        Quaternion( ElementType x_, ElementType y_, ElementType z_, ElementType w_ )
            :v(x_,y_,z_), w(w_)
        {}
        //@}

        // ********************************
        //          operators
        // ********************************
    public:

        //!
        //! assign
        //!
        Quaternion & operator = ( const Quaternion & q )
        {
            v = q.v;
            w = q.w;
            return *this;
        }
        //!
        //! Array indexing
        //!
        ElementType operator [] ( size_t i ) const
        {
            GN_ASSERT( i < 4 );
            return *(&v.x + i);
        }
        //!
        //! concatnate
        //!
        Quaternion & operator *= ( const Quaternion & q )
        {
            Quaternion q1 = *this;
            Quaternion q2 = q;

            w = q1.w * q2.w - Vector3<T>::dot( q1.v, q2.v );
            v = Vector3<T>::cross( q1.v, q2.v );
            v += q1.w * q2.v + q2.w * q1.v;

            return *this;
        }
        //!
        //! concatnate
        //!
        friend Quaternion
        operator * ( const Quaternion & q1, const Quaternion & q2 )
        {
            Quaternion result;
            result.w = q1.w * q2.w - Vector3<T>::dot( q1.v, q2.v );
            result.v = Vector3<T>::cross( q1.v, q2.v );
            result.v += q1.w * q2.v + q2.w * q1.v;
            return result;
        }

    private :

        //!
        //! index operator
        //!
        ElementType & operator [] ( unsigned int i )
        {
            GN_ASSERT( i < 4 );
            return *(&v.x + i);
        }

        // ********************************
        //        operations
        // ********************************
    public:

        //!
        //! ��һ��
        //!
        Quaternion & identify()
        {
            w = 1.0f; v.set(0, 0, 0); return *this;
        }
        //!
        //! get norm
        //!
        ElementType getNormal() const
        {
            return w * w + Vector3<T>::dot( v, v );
        }
        //!
        //! normalize a quaternion
        //!
        Quaternion & normalize()
        {
            ElementType n = (ElementType)sqrt( getNormal() );
            if (n > 0.0f) n = 1/n;
            else n = 0.0f;

            v *= n;
            w *= n;
            return *this;
        }
        //!
        //! normalize a quaternion
        //!
        static void normalize( Quaternion & dst, const Quaternion & src )
        {
            dst = src;
            dst.normalize();
        }
        //!
        //! normalize a quaternion
        //!
        static Quaternion normalize( const Quaternion & src )
        {
            Quaternion r(src);
            r.normalize();
            return r;
        }
        //!
        //! conjugate (����)
        //!
        Quaternion & conjugate()
        {
            v = -v; return *this;
        }
        //!
        //! conjugate
        //!
        static void conjugate( Quaternion & dst, const Quaternion & src )
        {
            dst = src;
            dst.conjugate();
        }
        //!
        //! conjugate
        //!
        static Quaternion conjugate( const Quaternion & src )
        {
            Quaternion r(src);
            r.conjugate();
            return r;
        }
        //!
        //! invert
        //!
        Quaternion & invert()
        {
            ElementType l = getNormal();

            if (l > 0.0f)
            { this->conjugate(); v /= l; w /= l; }
            else
            { w = 0.0f; v.set(0, 0, 0); }

            return *this;
        }
        //!
        //! invert
        //!
        static void invert( Quaternion & dst, const Quaternion & src )
        {
            dst = src;
            dst.invert();
        }
        //!
        //! invert
        //!
        static Quaternion invert( const Quaternion & src )
        {
            Quaternion r(src);
            r.invert();
            return r;
        }
        //!
        //! get from a UNIT rotation axis and an angle
        //!
        //! \param axis  Axis of rotation
        //! \param angle Angle of rotation, in radians
        //!
        Quaternion &
        fromRotation( const Vector3<T> & axis, const ElementType angle )
        {
            ElementType r = angle / 2.0f;
            v = axis * sin(r);
            w = cos(r);

            return *this;
        }
        //!
        //! construct from two <b>UNIT</b> vectors
        //!
        //! \note v1 and v2 should be <b>UNIT</b> vectors
        //!
        Quaternion & fromArc( const Vector3<T> & v1, const Vector3<T> & v2 )
        {
            // make sure v1 and v2 are unit vector
            GN_ASSERT( abs( 1.0f - v1.length() ) < 0.0001f &&
                       abs( 1.0f - v2.length() ) < 0.0001f );

            ElementType  d = dot( v1, v2 );
            ElementType  s = (ElementType)sqrt( ( 1 + d ) * 2 );
            Vector3<T> c = cross( v1, v2 );

            v.x = c.x / s;
            v.y = c.y / s;
            v.z = c.z / s;
            w   = s / 2.0f;

            return *this;
        }
        //!
        //! Construct from euler angles
        //!
        //! \param pitch, yaw, roll   Euler angles in radians
        //!
        Quaternion & fromEuler( ElementType pitch,
                                ElementType yaw,
                                ElementType roll )
        {
            float cy = cos( yaw / 2.0f );
            float sy = sin( yaw / 2.0f );
            float cp = cos( pitch / 2.0f );
            float sp = sin( pitch / 2.0f );
            float cr = cos( roll / 2.0f );
            float sr = sin( roll / 2.0f );
            v.set( cr * sp * cy + sr * cp * sy,
                   cr * cp * sy - sr * sp * cy,
                   sr * cp * cy - cr * sp * sy );
            w =    cr * cp * cy + sr * sp * sy;
            return *this;
        }
        //!
        //! construct from euler angles
        //!
        Quaternion & fromEuler( const Vector3<T> & euler )
        {
            return fromEuler( euler.x, euler.y, euler.z );
        }
        //!
        //! construct from matrix33
        //!
        Quaternion & fromMatrix33( const Matrix33<T> & mat );
        //!
        //! convert to matrix33
        //!
        void toMatrix33( Matrix33<T> & out ) const;
        //!
        //! convert to matrix33
        //!
        Matrix33<T> toMatrix33() const
        {
			Matrix33<T> out;
			toMatrix33( out );
			return out;
        }
    };

    // static member
    template < typename T >
    Quaternion<T> Quaternion<T>::IDENTITY( 0, 0, 0, 1 );

    //!
    //! 3D plane class
    //!
    //! plane equation : a * X + b * Y + c * Z + d = 0,
    //!             or : n * V + d = 0;
    //!
    template < typename T >
    class Plane3
    {
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        Vector3<T>  n; //!< normal (X:a, Y:b, Z:c)
        ElementType d; //!< constant factor

    public :
        //! \name constructors
        //@{
        Plane3() {}
        Plane3( ElementType a_, ElementType b_, ElementType c_, ElementType d_ )
            : n(a_,b_,c_), d(d_) {}
        Plane3( const Plane3 & p ) : n(p.n), d(p.d) {}
        Plane3( const Vector3<T> & point, const Vector3<T> & normal )
        {
            from_pointnormal( point, normal );
        }
        Plane3( const Vector3<T> & v1,
                   const Vector3<T> & v2,
                   const Vector3<T> & v3 )
        {
            from_3points( v1, v2, v3 );
        }
        //@}

        //! \name operators
        //@{
    public:
        //!
        //! type conversion
        //!
        operator const ElementType * () const
        {
            return n;
        }
        //!
        //! type conversion
        //!
        operator ElementType * ()
        {
            return n;
        }
        //!
        //! equality
        //!
        bool operator == ( const Plane3 & p ) const
        {
            return n == p.n && d == p.d;
        }
        //!
        //! inequality
        //!
        bool operator != ( const Plane3 & p ) const
        {
            return n != p.n || d != p.d;
        }
        //!
        //! Negate a plane
        //!
        friend Plane3 operator - ( const Plane3 & p )
        {
            return Plane3( -p.n.x, -p.n.y, -p.n.z, -p.d );
        }
        //!
        //! dot production with vector3 ( ax + by + cz + d )
        //!
        friend ElementType operator * ( const Plane3 & p, const Vector3<T> & v )
        {
            return Vector3<T>::dot( p.n, v ) + p.d;
        }
        //!
        //! dot production with 3D vector
        //!
        friend ElementType operator * ( const Vector3<T> & v, const Plane3 & p )
        {
            return p * v;
        }
        //!
        //! dot production with vector4 ( ax + by + cz + dw )
        //!
        friend ElementType operator * ( const Plane3 & p, const Vector4<T> & v )
        {
            return p.n.x * v.x + p.n.y * v.y + p.n.z * v.z + p.d * v.w;
        }
        //!
        //! dot production with 4D vector
        //!
        friend ElementType operator * ( const Vector4<T> & v, const Plane3 & p )
        {
            return p * v;
        }
        //@}

        //! \name operations
        //@{
    public:
        //!
        //! update plane parameters
        //!
        Plane3 & set( T a_, T b_, T c_, T d_ )
        {
            n.set( a_, b_, c_ );
            d = d_;
            return *this;
        }
        //!
        //! update plane parameters
        //!
        Plane3 & set( const Vector3<T> & n_, T d_ )
        {
            n = n_;
            d = d_;
            return *this;
        }
        //!
        //! Normalize the plane (so that |a,b,c| == 1)
        //!
        Plane3 & normalize()
        {
            ElementType f = n.length();
            if( f ) { n /= f; d /= f; }
            return *this;
        }
        //!
        //! Normalize the plane (so that |a,b,c| == 1)
        //!
        static void normalize( Plane3 & dst, const Plane3 & src )
        {
            dst = src;
            dst.normalize();
            return dst;
        }
        //!
        //! Normalize the plane (so that |a,b,c| == 1)
        //!
        static Plane3 normalize( const Plane3 & src )
        {
            Plane3 r(src);
            r.normalize();
            return r;
        }
        //!
        //! construct a plane from a point and a normal
        //!
        Plane3 & fromPointnormal( const Vector3<T> & point,
                                  const Vector3<T> & normal )
        {
            n = normal;
            d = Vector3<T>::dot( -n, point );
            return *this;
        }
        //!
        //! Construct a plane from 3 points
        //!
        Plane3 & fromPoints( const Vector3<T> & v1,
                             const Vector3<T> & v2,
                             const Vector3<T> & v3 )
        {
            n = Vector3<T>::cross( v2 - v1, v3 - v1 );
            d = Vector3<T>::dot( -n, v1 );
            return *this;
        }
        //!
        //! treate plane as an 4-D vector
        //!
        Vector4<T> & toVec4()
        {
            return *reinterpret_cast<Vector4<T>*>(this);
        }
        //!
        //! treate plane as an 4-D vector
        //!
        const Vector4<T> & toVec4() const
        {
            return *reinterpret_cast<const Vector4<T>*>(this);
        }
        //@}
    };

    //!
    //! rectangle structure
    //!
    //! \note  ������εĶ����Windows�еľ���RECT��ͬ
    //!
    template < typename T >
    class Rect
    {
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! point type
        //!
        typedef Vector2<T> PointType;

        //!
        //! rect coordinates
        //!
        //@{
        T x, y, w, h;
        //@}

        //!
        //! Default constructor
        //!
        Rect() {}
        //!
        //! Constructor
        //!
        Rect( T x_, T y_, T w_, T h_ ) : x(x_), y(y_), w(w_), h(h_) {}

        //!
        //! Set rectangle values
        //!
        Rect & set( T x_, T y_, T w_, T h_ )
        {
            x = x_;
            y = y_;
            w = w_;
            h = h_;
            return *this;
        }
        //!
        //! Get rectangle origin
        //!
        const PointType & origin() const
        {
            return *reinterpret_cast<const PointType*>(this);
        }
        //!
        //! get rectangle origin
        //!
        PointType & origin()
        {
            return *reinterpret_cast<PointType*>(this);
        }
        //! get rectangle size
        const PointType & size() const
        {
            return *reinterpret_cast<const PointType*>(&w);
        }
        //! get rectangle size
        PointType & size()
        {
            return *reinterpret_cast<PointType*>(&w);
        }
        //! Check if the point is inside the rectangle
        bool Contain( const PointType & p ) const
        {
            // make sure r is normal rectangle
            GN_ASSERT( isNormalized() );

            return x <= p.x && p.x < (x + w)
                && y <= p.y && p.y < (y + h);
        }
        //!
        //! Is the rectangle normalized? (width and height both > 0)
        //!
        bool isNormalized() const
        {
            return w > T(0) && h > T(0);
        }
        //!
        //! Normalize the rectangle
        //!
        Rect & normalize()
        {
            if( w < 0 )
            {
                x += w;
                w = -w;
            }
            if( h < 0 )
            {
                y += h;
                h = -h;
            }
            return *this;
        }
        //!
        //! Get normalize rectangle
        //!
        static void normalize( Rect<T> & dst, const Rect<T> & src )
        {
			dst = src;
            dst.normalize();
        }
        //!
        //! Get normalize rectangle
        //!
        static Rect<T> normalize( const Rect<T> & src )
        {
            Rect<T> dst(src);
            dst.normalize();
            return dst;
        }
    };

    //!
    //! box class
    //!
    template < typename T >
    class Box
    {
        // ********************************
        // public data
        // ********************************
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! point type
        //!
        typedef Vector3<T> PointType;

        //!
        //! Box position and size
        //@{
        T x, y, z, w, h, d;
        //@}

        // ********************************
        // constructors
        // ********************************
    public :

        //!
        //! default constructor
        //!
        Box() {}
        //!
        //! construct from 2 points
        //!
        Box( const PointType & v1, const PointType & v2 )
            : x(v1.x), y(v1.y), z(v1.z) , w(v2.x-v1.x), h(v2.y-v1.y), d(v2.z-v1.z)
        {}
        //!
        //! copy constructor
        //!
        Box( const Box & b ) : x(b.x), y(b.y), z(b.z), w(b.w), h(b.h), d(b.d) {}

        // ********************************
        // public operators
        // ********************************
    public :

        //!
        //! assign operator
        //!
        Box & operator = ( const Box & b )
        {
            x = b.x;
            y = b.y;
            z = b.z;
            w = b.w;
            h = b.h;
            d = b.d;
            return *this;
        }
        //!
        //! equal operator
        //!
        bool operator == ( const Box & b ) const
        {
            return x == b.x
                && y == b.y
                && z == b.z
                && w == b.w
                && h == b.h
                && d == b.d;
        }
        //!
        //! non-equal operator
        //!
        bool operator != ( const Box & b ) const
        {
            return x != b.x
                || y != b.y
                || z != b.z
                || w != b.w
                || h != b.h
                || d != b.d;
        }

        // ********************************
        // public operations
        // ********************************
    public :

        //!
        //! return box position
        //!
        const PointType & pos() const { return *(const PointType*)this; }

        //!
        //! return box position
        //!
        PointType & pos() { return *(PointType*)this; }

        //!
        //! return box size
        //!
        const PointType & size() const { return *(const PointType*)&w; }

        //!
        //! return box size
        //!
        PointType & size() { return *(PointType*)&w; }

        //!
        //! return box center
        //!
        PointType center() const { return pos() + size() / 2.0f; }

        //!
        //! return box center
        //!
        void center( PointType & out ) const { out = pos() + size() / 2.0f; }

        //!
        //! from two points
        //!
        void fromPoints( const PointType & v1, const PointType & v2 )
        {
            pos() = v1;
            size() = v2 - v1;
        }
        //!
        //! Normalization (that is, positive size)
        //!
        Box & normalize()
        {
            if( w < 0 ) { x += w; w = -w; }
            if( h < 0 ) { y += h; h = -h; }
            if( d < 0 ) { z += d; d = -d; }
			return *this;
        }
        //!
        //! normalization (store result into another variable)
        //!
        static void normalize( Box & dst, const Box & src )
        {
			dst = src;
            dst.normalize();
        }
        //!
        //! normalization (store result into another variable)
        //!
        static Box normalize( const Box & src )
        {
            Box dst(src);
            dst.normalize();
            return dst;
        }
        //!
        //! Return true if point is inside the box.
        //!
        bool checkPoint( const PointType & v )
        {
            Box b;

            normalize( b, *this );

            return b.x <= v.x && v.x < (b.x+b.w)
                && b.y <= v.y && v.y < (b.y+b.h)
                && b.z <= v.z && v.z < (b.z+b.d);
        }
        //!
        //! get union of two boxes
        //!
        static void getUnion( Box & result, const Box & b1, const Box & b2 )
        {
            Box a(b1), b(b2);
            a.normalize();
            b.normalize();
            result.pos() = min( b1.pos(), b2.pos() );
            result.size() = max( b1.pos+b1.size(), b2.pos+b2.size() ) - result.pos();
        }
    };

    //!
    //! template 3D sphere class
    //!
    template < typename T >
    class Sphere
    {
    public :

        //!
        //! element type
        //!
        typedef T ElementType;

        //!
        //! sphere center
        //!
        Vector3<T> center;

        //!
        //! sphere radius
        //!
        T          radius;

        // ********************************
        //! \name constructors
        // ********************************
    public :
        //@{
        Sphere() {}
        Sphere( const Vector3<T> & c, ElementType r ) : center(c), radius(r) {}
        Sphere( const Sphere & s ) : center(s.center), radius(s.radius) {}
        //!
        //! construct from a box
        //!
        explicit Sphere( const Box<T> & b )
            : center( b.center() )
            , radius( (b.vmax-b.vmin).length() / 2.0f )
        {}
        //@}

        // ********************************
        // operators
        // ********************************
    public :

        //!
        //! assign operator
        //!
        Sphere & operator = ( const Sphere & rhs )
        {
            center = rhs.center;
            radius = rhs.radius;
            return *this;
        }

        // ********************************
        // operations
        // ********************************
    public :

        //!
        //! value set
        //!
        Sphere & set( const Vector3<T> & c, ElementType r )
        {
            center = c; radius = r;
            return *this;
        }
    };
}

#include "matrix.inl"
#include "quaternion.inl"

namespace GN
{
    //! \name Alias for commonly used geometry classes.
    //@{
    typedef Vector2<float>      Vector2f;
    typedef Vector2<double>     Vector2d;
    typedef Vector2<int>        Vector2i;

    typedef Vector3<float>      Vector3f;
    typedef Vector3<double>     Vector3d;
    typedef Vector3<int>        Vector3i;

    typedef Vector4<float>      Vector4f;
    typedef Vector4<double>     Vector4d;
    typedef Vector4<int>        Vector4i;

    typedef Matrix33<float>     Matrix33f;
    typedef Matrix33<double>    Matrix33d;
    typedef Matrix33<int>       Matrix33i;

    typedef Matrix44<float>     Matrix44f;
    typedef Matrix44<double>    Matrix44d;
    typedef Matrix44<int>       Matrix44i;

    typedef Plane3<float>       Plane3f;
    typedef Plane3<double>      Plane3d;
    typedef Plane3<int>         Plane3i;

    typedef Rect<float>         Rectf;
    typedef Rect<double>        Rectd;
    typedef Rect<int>           Recti;

    typedef Box<float>          Boxf;
    typedef Box<double>         Boxd;
    typedef Box<int>            Boxi;

    typedef Sphere<float>       Spheref;
    typedef Sphere<double>      Sphered;
    typedef Sphere<int>         Spherei;
    //@}
}

// *****************************************************************************
//                           End of geometry.h
// *****************************************************************************
#endif // __GN_BASE_GEOMETRY_H__
