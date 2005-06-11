#ifndef __GN_BASE_MATRIX_INL__
#define __GN_BASE_MATRIX_INL__
// *****************************************************************************
//! \file    matrix.inl
//! \brief   inline functions of matrix classes
//! \author  chenlee (2005.5.8)
// *****************************************************************************

namespace GN
{
    // *************************************************************************
    // 3x3 matrix
    // *************************************************************************
    
    //
    // Multiply the Matrix33<T> by another Matrix33<T>
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> & Matrix33<T>::operator *= ( const Matrix33<T> & m )
    {
        Matrix33<T> t;

        #define M33_CALCULATE_ONE_CELL(r, c) t[r][c] = rows[r][0] * m[0][c] + \
                                                        rows[r][1] * m[1][c] + \
                                                        rows[r][2] * m[2][c];

        M33_CALCULATE_ONE_CELL(0, 0);
        M33_CALCULATE_ONE_CELL(0, 1);
        M33_CALCULATE_ONE_CELL(0, 2);

        M33_CALCULATE_ONE_CELL(1, 0);
        M33_CALCULATE_ONE_CELL(1, 1);
        M33_CALCULATE_ONE_CELL(1, 2);

        M33_CALCULATE_ONE_CELL(2, 0);
        M33_CALCULATE_ONE_CELL(2, 1);
        M33_CALCULATE_ONE_CELL(2, 2);

        #undef M33_CALCULATE_ONE_CELL

        *this = t;

        return *this;
    }

    //
    // get inverse of a 3x3 matrix
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> & Matrix33<T>::invert()
    {

        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.
        Matrix33<T> src = *this;
        Matrix33<T>  & result = *this;

        result[0][0] = src[1][1]*src[2][2] - src[1][2]*src[2][1];
        result[0][1] = src[0][2]*src[2][1] - src[0][1]*src[2][2];
        result[0][2] = src[0][1]*src[1][2] - src[0][2]*src[1][1];
        result[1][0] = src[1][2]*src[2][0] - src[1][0]*src[2][2];
        result[1][1] = src[0][0]*src[2][2] - src[0][2]*src[2][0];
        result[1][2] = src[0][2]*src[1][0] - src[0][0]*src[1][2];
        result[2][0] = src[1][0]*src[2][1] - src[1][1]*src[2][0];
        result[2][1] = src[0][1]*src[2][0] - src[0][0]*src[2][1];
        result[2][2] = src[0][0]*src[1][1] - src[0][1]*src[1][0];

        ElementType fDet =
            src[0][0]*result[0][0]+
            src[0][1]*result[1][0]+
            src[0][2]*result[2][0];

        if( ::fabs(fDet) == 0.0f )
        {
            // Invertable matrix is very rare, and normally means error,
            // so we write output a warning here.
            GN_WARN( "Matrix is un-invertable!" );
            return identify();
        }

        ElementType fInvDet = 1.0f/fDet;
        for (int iRow = 0; iRow < 3; iRow++)
        {
            for (int iCol = 0; iCol < 3; iCol++)
                result[iRow][iCol] *= fInvDet;
        }

        // success
        return *this;
    }

    //
    // generate a X-rotate matrix by 'angle' is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> & Matrix33<T>::rotateX( ElementType angle )
    {
        // 1    0    0
        // 0  cos -sin
        // 0  sin  cos

        ElementType s = (ElementType)::sin( angle );
        ElementType c = (ElementType)::cos( angle );

        identify();

        rows[1][1] = c;
        rows[1][2] = -s;
        rows[2][1] = s;
        rows[2][2] = c;

        return *this;
    }

    //
    // generate a Y-rotate matrix by 'angle' is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> & Matrix33<T>::rotateY( ElementType angle )
    {
        //  cos  0  sin
        //  0    1    0
        // -sin  0  cos

        ElementType s = ::sin( angle );
        ElementType c = ::cos( angle );

        identify();

        rows[0][0] = c;
        rows[0][2] = s;
        rows[2][0] = -s;
        rows[2][2] = c;

        return *this;
    }

    //
    // generate a Z-rotate matrix by 'angle' is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> & Matrix33<T>::rotateZ( ElementType angle )
    {
        // cos -sin  0
        // sin  cos  0
        //   0    0  1

        ElementType s = ::sin( angle );
        ElementType c = ::cos( angle );

        identify();

        rows[0][0] = c;
        rows[0][1] = -s;
        rows[1][0] = s;
        rows[1][1] = c;

        return *this;
    }

    //
    // generate a arbitrary rotation matrix, angle is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> & Matrix33<T>::rotate( const Vector3<T> & v, ElementType angle )
    {
        ElementType sinA, cosA;
        ElementType invCosA;
        Vector3<T> nrm = v;
        ElementType x, y, z;
        ElementType xSq, ySq, zSq;

        nrm.normalize();
        sinA = ::sin( angle );
        cosA = ::cos( angle );
        invCosA = 1.0F - cosA;

        x = nrm.x;
        y = nrm.y;
        z = nrm.z;

        xSq = x * x;
        ySq = y * y;
        zSq = z * z;

        rows[0][0] = (invCosA * xSq) + (cosA);
        rows[0][1] = (invCosA * x * y) - (sinA * z );
        rows[0][2] = (invCosA * x * z) + (sinA * y );

        rows[1][0] = (invCosA * x * y) + (sinA * z);
        rows[1][1] = (invCosA * ySq) + (cosA);
        rows[1][2] = (invCosA * y * z) - (sinA * x);

        rows[2][0] = (invCosA * x * z) - (sinA * y);
        rows[2][1] = (invCosA * y * z) + (sinA * x);
        rows[2][2] = (invCosA * zSq) + (cosA);

        return *this;
    }

    //
    //
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> &
    Matrix33<T>::lookAtLh( const Vector3<T> & forward, const Vector3<T> & up )
    {
        Vector3<T> F = forward;
        F.normalize();

        Vector3<T> S = Vector3<T>::cross( Vector3<T>::normalize(up), F );
        S.normalize();

        Vector3<T> U = Vector3<T>::cross( F, S );
        U.normalize();

        rows[0][0] = S.x;
        rows[0][1] = S.y;
        rows[0][2] = S.z;

        rows[1][0] = U.x;
        rows[1][1] = U.y;
        rows[1][2] = U.z;

        rows[2][0] = F.x;
        rows[2][1] = F.y;
        rows[2][2] = F.z;

        // success
        return *this;
    }

    //
    //
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix33<T> &
    Matrix33<T>::lookAtRh( const Vector3<T> & forward, const Vector3<T> & up )
    {
        Vector3<T> F = forward;
        F.normalize();

        Vector3<T> S = Vector3<T>::cross( F, Vector3<T>::normalize(up) );
        S.normalize();

        Vector3<T> U = Vector3<T>::cross( S, F );
        U.normalize();

        rows[0][0] = S.x;
        rows[0][1] = S.y;
        rows[0][2] = S.z;

        rows[1][0] = U.x;
        rows[1][1] = U.y;
        rows[1][2] = U.z;

        rows[2][0] = -F.x;
        rows[2][1] = -F.y;
        rows[2][2] = -F.z;

        // success
        return *this;
    }


    //
    // ����������ݴ�ӡ���ַ�����
    // -------------------------------------------------------------------------
    template < typename T >
    StrA Matrix33<T>::print() const
    {
        return formatStr(
            "%f,\t%f,\t%f\n"
            "%f,\t%f,\t%f\n"
            "%f,\t%f,\t%f\n",
            rows[0][0], rows[0][1], rows[0][2],
            rows[1][0], rows[1][1], rows[1][2],
            rows[2][0], rows[2][1], rows[2][2] );
    }

    // *************************************************************************
    // 4x4 matrix
    // *************************************************************************

    //
    // Multiply the Matrix44<T> by another Matrix44<T>
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> & Matrix44<T>::operator *= ( const Matrix44<T> & m )
    {
        Matrix44<T> t;

        #define M44_CALCULATE_ONE_CELL(r, c)  t[r][c] = rows[r][0] * m[0][c] + \
                                                        rows[r][1] * m[1][c] + \
                                                        rows[r][2] * m[2][c] + \
                                                        rows[r][3] * m[3][c];  \

        M44_CALCULATE_ONE_CELL(0, 0);
        M44_CALCULATE_ONE_CELL(0, 1);
        M44_CALCULATE_ONE_CELL(0, 2);
        M44_CALCULATE_ONE_CELL(0, 3);

        M44_CALCULATE_ONE_CELL(1, 0);
        M44_CALCULATE_ONE_CELL(1, 1);
        M44_CALCULATE_ONE_CELL(1, 2);
        M44_CALCULATE_ONE_CELL(1, 3);

        M44_CALCULATE_ONE_CELL(2, 0);
        M44_CALCULATE_ONE_CELL(2, 1);
        M44_CALCULATE_ONE_CELL(2, 2);
        M44_CALCULATE_ONE_CELL(2, 3);

        M44_CALCULATE_ONE_CELL(3, 0);
        M44_CALCULATE_ONE_CELL(3, 1);
        M44_CALCULATE_ONE_CELL(3, 2);
        M44_CALCULATE_ONE_CELL(3, 3);

        #undef M44_CALCULATE_ONE_CELL

        *this = t;
        return *this;
    }

    //
    // Invert the matrix
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> & Matrix44<T>::invert()
    {
        Matrix44<T> a(*this);
        Matrix44<T>  & b = *this;
        b.identify();

        int r, c;
        int cc;
        int rowMax; // Points to max abs value row in this column
        int row;
        ElementType tmp;

        // Go through columns
        for (c=0; c<4; c++)
        {

            // Find the row with max value in this column
            rowMax = c;
            for (r=c+1; r<4; r++)
            {
                if (::fabs(a[r][c]) > ::fabs(a[rowMax][c]))
                {
                    rowMax = r;
                }
            }

            // Check if the matrix is invertible. If the max value here is 0, we
            // can't invert.  Return identity.
            if (a[rowMax][c] == 0.0F)
            {
                // Invertible matrix is very rare, and normally means error,
                // so we write output a warning here.
                GN_WARN( "Matrix is un-invertable!" );
                return identify();
            }

            // Swap row "rowMax" with row "c"
            for (cc=0; cc<4; cc++)
            {
                tmp = a[c][cc];
                a[c][cc] = a[rowMax][cc];
                a[rowMax][cc] = tmp;
                tmp = b[c][cc];
                b[c][cc] = b[rowMax][cc];
                b[rowMax][cc] = tmp;
            }

            // Now everything we do is on row "c".
            // Set the max cell to 1 by dividing the entire row by that value
            tmp = a[c][c];
            for (cc=0; cc<4; cc++)
            {
                a[c][cc] /= tmp;
                b[c][cc] /= tmp;
            }

            // Now do the other rows, so that this column only has a 1 and 0's
            for (row = 0; row < 4; row++)
            {
                if (row != c)
                {
                    tmp = a[row][c];
                    for (cc=0; cc<4; cc++)
                    {
                        a[row][cc] -= a[c][cc] * tmp;
                        b[row][cc] -= b[c][cc] * tmp;
                    }
                }
            }

        }

        // success
        return *this;
    }

    //
    // generate a rotate matrix by X-axis, angle is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> & Matrix44<T>::rotateX( ElementType angle )
    {
        // cos -sin  0   0
        // sin  cos  0   0
        //   0    0  1   0
        //   0    0  0   1

        ElementType s = ::sin( angle );
        ElementType c = ::cos( angle );

        identify();

        rows[1][1] = c;
        rows[1][2] = -s;
        rows[2][1] = s;
        rows[2][2] = c;

        return *this;
    }

    //
    // generate a rotate matrix by Y-axis, angle is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> & Matrix44<T>::rotateY( ElementType angle )
    {
        //  cos  0  sin   0
        //  0    1    0   0
        // -sin  0  cos   0
        //  0    0    0   1

        ElementType s = ::sin( angle );
        ElementType c = ::cos( angle );

        identify();

        rows[0][0] = c;
        rows[0][2] = s;
        rows[2][0] = -s;
        rows[2][2] = c;

        return *this;
    }

    //
    // generate a rotate matrix by X-axis, angle is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> & Matrix44<T>::rotateZ( ElementType angle )
    {
        // cos -sin   0    0
        // sin  cos   0    0
        //  0    0    1    0
        //  0    0    0    1

        ElementType s = ::sin( angle );
        ElementType c = ::cos( angle );

        identify();

        rows[0][0] = c;
        rows[0][1] = -s;
        rows[1][0] = s;
        rows[1][1] = c;

        return *this;
    }

    //
    // generate a rotate matrix by vector 'v', angle is in radians
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::rotate( const Vector3<T> & v, ElementType angle )
    {
        Vector3<T> nrm = v;
        nrm.normalize();

        ElementType sinA, cosA, invCosA;
        ElementType x, y, z;
        ElementType xSq, ySq, zSq;

        sinA = ::sin( angle );
        cosA = ::cos( angle );
        invCosA = 1.0F - cosA;

        x = nrm.x;
        y = nrm.y;
        z = nrm.z;

        xSq = x * x;
        ySq = y * y;
        zSq = z * z;

        rows[0][0] = (invCosA * xSq) + (cosA);
        rows[0][1] = (invCosA * x * y) - (sinA * z );
        rows[0][2] = (invCosA * x * z) + (sinA * y );
        rows[0][3] = 0.0F;

        rows[1][0] = (invCosA * x * y) + (sinA * z);
        rows[1][1] = (invCosA * ySq) + (cosA);
        rows[1][2] = (invCosA * y * z) - (sinA * x);
        rows[1][3] = 0.0F;

        rows[2][0] = (invCosA * x * z) - (sinA * y);
        rows[2][1] = (invCosA * y * z) + (sinA * x);
        rows[2][2] = (invCosA * zSq) + (cosA);
        rows[2][3] = 0.0F;

        rows[3][0] = 0.0F;
        rows[3][1] = 0.0F;
        rows[3][2] = 0.0F;
        rows[3][3] = 1.0F;

        return *this;
    }

    //
    //
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::lookAtLh( const Vector3<T> & eye,
                           const Vector3<T> & to,
                           const Vector3<T> & up )
    {
        Vector3<T> F = to - eye;
        F.normalize();

        Vector3<T> S = Vector3<T>::cross( Vector3<T>::normalize(up), F );
        S.normalize();

        Vector3<T> U = Vector3<T>::cross( F, S );
        U.normalize();

        rows[0][0] = S.x;
        rows[0][1] = S.y;
        rows[0][2] = S.z;
        rows[0][3] = 0.0f;

        rows[1][0] = U.x;
        rows[1][1] = U.y;
        rows[1][2] = U.z;
        rows[1][3] = 0.0f;

        rows[2][0] = F.x;
        rows[2][1] = F.y;
        rows[2][2] = F.z;
        rows[2][3] = 0.0f;

        rows[3][0] = 0.0f;
        rows[3][1] = 0.0f;
        rows[3][2] = 0.0f;
        rows[3][3] = 1.0f;

        Matrix44 mt;
        mt.translate( -eye );
        *this *= mt;

        return *this;
    }

    //
    //
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::lookAtRh( const Vector3<T> & eye,
                               const Vector3<T> & to,
                               const Vector3<T> & up )
    {
        Vector3<T> F = to - eye;
        F.normalize();

        Vector3<T> S = Vector3<T>::cross( F, Vector3<T>::normalize(up) );
        S.normalize();

        Vector3<T> U = Vector3<T>::cross( S, F );
        U.normalize();

        rows[0][0] = S.x;
        rows[0][1] = S.y;
        rows[0][2] = S.z;
        rows[0][3] = 0.0f;

        rows[1][0] = U.x;
        rows[1][1] = U.y;
        rows[1][2] = U.z;
        rows[1][3] = 0.0f;

        rows[2][0] = -F.x;
        rows[2][1] = -F.y;
        rows[2][2] = -F.z;
        rows[2][3] = 0.0f;

        rows[3][0] = 0.0f;
        rows[3][1] = 0.0f;
        rows[3][2] = 0.0f;
        rows[3][3] = 1.0f;

        Matrix44 mt;
        mt.translate( -eye );
        *this *= mt;

        return *this;
    }

    //
    // ����opengl������ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::orthoOpenGLLh( ElementType left, ElementType right,
                                ElementType bottom, ElementType top,
                                ElementType znear, ElementType zfar )
    {
        GN_ASSERT( left != right && bottom != top && znear != zfar );

        ElementType w = right - left,
                  h = top - bottom,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f / w;
        rows[1][1] = 2.0f / h;
        rows[2][2] = 2.0f / d;                  // �˴���directx��ͬ
        rows[0][3] = - (left + right) / w;
        rows[1][3] = - (bottom + top) / h;
        rows[2][3] = - (znear + zfar) / d;      // �˴���directx��ͬ
        rows[3][3] = 1.0f;

        return *this;
    }

    //
    // ����opengl������ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::orthoOpenGLRh( ElementType left, ElementType right,
                                ElementType bottom, ElementType top,
                                ElementType znear, ElementType zfar )
    {
        GN_ASSERT( left != right && bottom != top && znear != zfar );

        ElementType w = right - left,
                  h = top - bottom,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f / w;
        rows[1][1] = 2.0f / h;
        rows[2][2] = -2.0f / d;                 // �˴���directx��ͬ
        rows[0][3] = - (left + right) / w;
        rows[1][3] = - (bottom + top) / h;
        rows[2][3] = - (znear + zfar) / d;      // �˴���directx��ͬ
        rows[3][3] = 1.0f;

        return *this;
    }

    //
    // ����directx������ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::orthoDirect3DLh( ElementType left, ElementType right,
                                  ElementType bottom, ElementType top,
                                  ElementType znear, ElementType zfar )
    {
        GN_ASSERT( left != right && bottom != top && znear != zfar );

        ElementType w = right - left,
                  h = top - bottom,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f / w;
        rows[1][1] = 2.0f / h;
        rows[2][2] = 1.0f / d;                  // �˴���opengl��ͬ
        rows[0][3] = - (left + right) / w;
        rows[1][3] = - (bottom + top) / h;
        rows[2][3] = - znear / d;               // �˴���opengl��ͬ
        rows[3][3] = 1.0f;

        return *this;
    }

    //
    // ����directx������ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::orthoDirect3DRh( ElementType left, ElementType right,
                                  ElementType bottom, ElementType top,
                                  ElementType znear, ElementType zfar )
    {
        GN_ASSERT( left != right && bottom != top && znear != zfar );

        ElementType w = right - left,
                  h = top - bottom,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f / w;
        rows[1][1] = 2.0f / h;
        rows[2][2] = -1.0f / d;                 // �˴���opengl��ͬ
        rows[0][3] = - (left + right) / w;
        rows[1][3] = - (bottom + top) / h;
        rows[2][3] = - znear / d;               // �˴���opengl��ͬ
        rows[3][3] = 1.0f;

        return *this;
    }

    //
    // ����opengl��͸��ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::perspectiveOpenGLLh( ElementType fovy, ElementType ratio,
                                      ElementType znear, ElementType zfar )
    {
        GN_ASSERT( 0.0f != fovy && 0.0f != ratio && znear != zfar );

        ElementType h = tan( fovy / 2.0f ) * znear * 2.0f,
                  w = h * ratio,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f * znear / w;
        rows[1][1] = 2.0f * znear / h;
        rows[2][2] = (znear + zfar) / d;        // �˴���directx��ͬ
        rows[3][2] = 1.0f;
        rows[2][3] = -2.0f * znear * zfar / d;  // �˴���directx��ͬ
        rows[3][3] = 0.0f;

        return *this;
    }

    //
    // ����opengl��͸��ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::perspectiveOpenGLRh( ElementType fovy, ElementType ratio,
                                      ElementType znear, ElementType zfar )
    {
        GN_ASSERT( 0.0f != fovy && 0.0f != ratio && znear != zfar );

        ElementType h = tan( fovy / 2.0f ) * znear * 2.0f,
                  w = h * ratio,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f * znear / w;
        rows[1][1] = 2.0f * znear / h;
        rows[2][2] = -(znear + zfar) / d;       // �˴���directx��ͬ
        rows[3][2] = -1.0f;
        rows[2][3] = -2.0f * znear * zfar / d;  // �˴���directx��ͬ
        rows[3][3] = 0.0f;

        return *this;
    }

    //
    // ����directx��͸��ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::perspectiveDirect3DLh( ElementType fovy, ElementType ratio,
                                        ElementType znear, ElementType zfar )
    {
        GN_ASSERT( 0.0f != fovy && 0.0f != ratio && znear != zfar );

        ElementType h = tan( fovy/2.0f ) * znear * 2.0f,
                  w = h * ratio,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f * znear / w;
        rows[1][1] = 2.0f * znear / h;
        rows[2][2] = zfar / d;                  // �˴���opengl��ͬ
        rows[3][2] = 1.0f;
        rows[2][3] = -zfar * znear / d;         // �˴���opengl��ͬ
        rows[3][3] = 0.0f;

        return *this;
    }

    //
    // ����directx��͸��ͶӰ����
    // -------------------------------------------------------------------------
    template < typename T >
    Matrix44<T> &
    Matrix44<T>::perspectiveDirect3DRh( ElementType fovy, ElementType ratio,
                                        ElementType znear, ElementType zfar )
    {
        GN_ASSERT( 0.0f != fovy && 0.0f != ratio && znear != zfar );

        ElementType h = tan( fovy/2.0f ) * znear * 2.0f,
                  w = h * ratio,
                  d = zfar - znear;

        identify();
        rows[0][0] = 2.0f * znear / w;
        rows[1][1] = 2.0f * znear / h;
        rows[2][2] = -zfar / d;                 // �˴���opengl��ͬ
        rows[3][2] = -1.0f;
        rows[2][3] = -zfar * znear / d;         // �˴���opengl��ͬ
        rows[3][3] = 0.0f;

        return *this;
    }

    //
    //  ����������ݴ�ӡ���ַ�����
    // -------------------------------------------------------------------------
    template < typename T >
    StrA Matrix44<T>::print() const
    {
        return formatStr(
            "%f,\t%f,\t%f,\t%f,\n"
            "%f,\t%f,\t%f,\t%f,\n"
            "%f,\t%f,\t%f,\t%f,\n"
            "%f,\t%f,\t%f,\t%f,\n",
            rows[0][0], rows[0][1], rows[0][2], rows[0][3],
            rows[1][0], rows[1][1], rows[1][2], rows[1][3],
            rows[2][0], rows[2][1], rows[2][2], rows[2][3],
            rows[3][0], rows[3][1], rows[3][2], rows[3][3] );
    }
}

// *****************************************************************************
//                           End of matrix.inl
// *****************************************************************************
#endif // __GN_BASE_MATRIX_INL__
