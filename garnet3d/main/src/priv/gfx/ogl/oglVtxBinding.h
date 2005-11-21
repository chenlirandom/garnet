#ifndef __GN_GFXOGL_OGLVTXBINDING_H__
#define __GN_GFXOGL_OGLVTXBINDING_H__
// *****************************************************************************
//! \file    ogl/oglVtxBinding.h
//! \brief   OGL vertex binding class
//! \author  chenlee (2005.11.21)
// *****************************************************************************

#include "oglTypes.h"

namespace GN { namespace gfx
{
    //!
    //! OGL vertex binding class
    //!
    class OGLVtxBinding : public OGLResource, public StdClass
    {
         GN_DECLARE_STDCLASS( OGLVtxBinding, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        OGLVtxBinding( OGLRenderer & r ) : OGLResource(r) { clear(); }
        virtual ~OGLVtxBinding() { quit(); }
        //@}

        // ********************************
        // standard init/quit
        // ********************************

        //@{
    public:
        bool init( const VtxFmtDesc & );
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() {}
        //@}

        // ********************************
        // From OGLResource
        // ********************************
    public:

        bool deviceCreate() { return true; }
        bool deviceRestore() { return true; }
        void deviceDispose() {}
        void deviceDestroy() {}

        // ********************************
        // public functions
        // ********************************
    public:

        //!
        //! Get vertex format descriptor
        //!
        const VtxFmtDesc & getFormat() const { return mFormat; }

        // ********************************
        // private variables
        // ********************************
    private:

        VtxFmtDesc mFormat;

        // ********************************
        // private functions
        // ********************************
    private:
    };
}}

// *****************************************************************************
//                           End of oglVtxBinding.h
// *****************************************************************************
#endif // __GN_GFXOGL_OGLVTXBINDING_H__
