#ifndef __GN_UTIL_CAMERA_H__
#define __GN_UTIL_CAMERA_H__
// *****************************************************************************
//! \file    util/camera.h
//! \brief   commonly used camera classes
//! \author  chenli@@FAREAST (2007.1.16)
// *****************************************************************************

#include "garnet/GNinput.h"

namespace GN { namespace util
{
    ///
    /// Arcball that normally used to rotate a 3D model.
    ///
    class ArcBall : public SlotBase
    {
        Quaternionf mQuat;
        Matrix33f   mRotation3x3;
        Matrix44f   mRotation4x4;

        Matrix44f   mTransView; //< trans(view), that is invtrans( inv(view) ). Used to transform vector from view space to model space.
        Vector2i    mWindowCenter;
        Vector2i    mWindowHalfSize;
        float       mHandness; ///< -1 for left-hand, 1 for left-hand

        Quaternionf mQuatBase;
        Vector3f    mMoveBase;
        bool        mMoving;

    public:

        ///
        /// arcball handness
        ///
        enum Handness
        {
            LEFT_HAND  = 1, ///< left-hand arcball
            RIGHT_HAND = 2, ///< right-hand arcball
        };

        ///
        /// ctor
        ///
#if GN_LEFT_HAND
        ArcBall( Handness h = LEFT_HAND );
#else
        ArcBall( Handness h = RIGHT_HAND );
#endif

        /// \name set arcball properties
        ///
        /// \note Changing properties during arcball moving may produce undefined result.
        //@{

        ///
        /// set the mouse moving range of the arcball. X is left-ward, Y is down-ward.
        ///
        /// Initial is (-1,-1,2,2)
        ///
        void setMouseMoveWindow( int l, int t, int w, int h )
        {
            mWindowHalfSize.set( w / 2, h / 2 );
            mWindowCenter.set( l + mWindowHalfSize.x, t + mWindowHalfSize.y );
        }

        void setRotation( const Quaternionf & q ) { mQuat = q; }

        void setViewMatrix( const Matrix44f & v ) { mTransView = Matrix44f::sTranspose( v ); }

        void setHandness( Handness h ) { mHandness = (LEFT_HAND==h) ? -1.0f : 1.0f ; }

        ///
        /// rotate the arcball. dx and dy are in screen space.
        ///
        void rotate( float dx, float dy );

        //@}

        /// \name get arcball properties
        //@{

        const Vector2i & getWindowCenter() const { return mWindowCenter; }

        const Quaternionf & getRotation() const { return mQuat; }

        const Matrix33f & getRotationMatrix33() const { return mRotation3x3; }

        const Matrix44f & getRotationMatrix44() const { return mRotation4x4; }

        //@}

        /// \name mouse action handler
        //@{

        void connectToInput();
        void disconnectFromInput();

        void beginDrag( int x, int y );
        void beginDrag( const Vector2i & pos ) { beginDrag( pos.x, pos.y ); }
        void endDrag();
        void onDrag( int x, int y );

        void onMouseButtonDown( int x, int y ) { beginDrag( x, y ); }
        void onMouseButtonUp() { endDrag(); }
        void onMouseMove( int x, int y ) { onDrag( x, y ); }

        //@}

    private:

        void onKeyPress( input::KeyEvent );
        void onAxisMove( input::Axis, int );
    };
}}

// *****************************************************************************
//                           End of camera.h
// *****************************************************************************
#endif // __GN_UTIL_CAMERA_H__
