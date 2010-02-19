#ifndef __GN_GFX_UTIL_GPU_MTIDXBUF_H__
#define __GN_GFX_UTIL_GPU_MTIDXBUF_H__
// *****************************************************************************
/// \file
/// \brief   multi-thread idxbuf wrappers
/// \author  chenli@@REDMOND (2009.1.3)
// *****************************************************************************

#include "mtgpu.h"

namespace GN { namespace gfx
{
    ///
    /// multi thread index buffer wrapper
    ///
    class MultiThreadIdxBuf : public IdxBuf, public StdClass
    {
        GN_DECLARE_STDCLASS( MultiThreadIdxBuf, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        MultiThreadIdxBuf( MultiThreadGpu & r ) : mGpu(r) { Clear(); }
        virtual ~MultiThreadIdxBuf() { Quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool Init( IdxBuf * );
        void Quit();
    private:
        void Clear() { mIdxBuf = NULL; }
        //@}

        // ********************************
        // public methods
        // ********************************
    public:

        IdxBuf * getRealIdxBuf() const { return mIdxBuf; }

        // ********************************
        // from IdxBuf
        // ********************************
    public:

        virtual void update( size_t offset, size_t length, const void * data, SurfaceUpdateFlag flag );
        virtual void readback( std::vector<UInt8> & data );

        // ********************************
        // private variables
        // ********************************
    private:

        MultiThreadGpu & mGpu;
        IdxBuf              * mIdxBuf;

        // ********************************
        // private functions
        // ********************************
    private:
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_UTIL_GPU_MTIDXBUF_H__
