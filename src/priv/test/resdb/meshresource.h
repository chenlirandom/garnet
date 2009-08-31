#ifndef __GN_GFX_MESHRESOURCE_H__
#define __GN_GFX_MESHRESOURCE_H__
// *****************************************************************************
/// \file
/// \brief   Mesh resource implementation class
/// \author  chenli@@REDMOND (2009.8.28)
// *****************************************************************************

#include "gpures.h"

namespace GN { namespace gfx
{
    ///
    /// Mesh resource implementation class
    ///
    class MeshResource::Impl : public StdClass
    {
        GN_DECLARE_STDCLASS( Impl, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        Impl( MeshResource & owner ) : mOwner(owner) { clear(); }
        virtual ~Impl() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const MeshResourceDesc & desc );
        void quit();
    private:
        void clear() {}
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        const MeshResourceDesc & getDesc() const { return mDesc; }
        void applyToContext( GpuContext & context ) const;

        // ********************************
        // private variables
        // ********************************
    private:

        struct VertexBuffer
        {
            AutoRef<VtxBuf>  gpudata;
        };

        struct IndexBuffer
        {
            AutoRef<IdxBuf>  gpudata;
        };

        MeshResource    & mOwner;
        MeshResourceDesc  mDesc;
        VertexBuffer      mVtxBufs[GpuContext::MAX_VERTEX_BUFFERS];
        IndexBuffer       mIdxBuf;

        // ********************************
        // private functions
        // ********************************
    private:

        GpuResourceDatabase & database() const { return mOwner.database(); }
        const char *          meshName() const { return mOwner.database().getResourceName( mOwner.handle() ); }
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_MESHRESOURCE_H__
