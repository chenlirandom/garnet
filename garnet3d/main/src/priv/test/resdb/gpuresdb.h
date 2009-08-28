#ifndef __GN_GFX_GPURESDB_H__
#define __GN_GFX_GPURESDB_H__
// *****************************************************************************
/// \file
/// \brief   GPU resource database implementation header
/// \author  chenli@@REDMOND (2009.8.21)
// *****************************************************************************

#include "gpures.h"

namespace GN { namespace gfx
{
    union GpuResourceHandleStruct
    {
        UInt32 u32;
        struct
        {
            UInt32 type           : 8;
            UInt32 internalHandle : 24;
        };

        enum
        {
            MAX_TYPES = (2^8-1)
        };
    };

    ///
    /// GPU resource database implementation class
    ///
    class GpuResourceDatabase::Impl
    {
        // *********************************************************************
        // private types
        // *********************************************************************

    private:

        struct ResourceItem
        {
            GpuResource * resource;
        };

        typedef NamedHandleManager<ResourceItem,UInt32> NamedResourceMapper;

        struct ResourceManager
        {
            Guid                guid;
            StrA                desc;
            size_t              index; // index into manager array
            GpuResourceFactory  factory;
            NamedResourceMapper resources;
        };

        typedef StackArray<ResourceManager, GpuResourceHandleStruct::MAX_TYPES> ManagerArray;

        // *********************************************************************
        // private data
        // *********************************************************************

    private:

        GpuResourceDatabase & mDatabase;
        Gpu                 & mGpu;
        ManagerArray          mManagers;

        // *********************************************************************
        //
        // *********************************************************************

    public:

        //@{

        Impl( GpuResourceDatabase & db, Gpu & );
        virtual ~Impl();

        void clear();

        //@}

        //@{
        bool registerResourceFactory( const Guid & type, const char * desc, GpuResourceFactory factory );
        bool hasResourceFactory( const Guid & type );
        //@}

        //@{
        GpuResourceHandle    createResource( const Guid & type, const char * name, const void * parameters );
        void                 deleteResource( GpuResourceHandle );
        void                 deleteAllResources();
        bool                 checkHandle( GpuResourceHandle handle ) const { return NULL != getResourceItem( handle, true ); }
        GpuResourceHandle    findResource( const Guid & type, const char * name ) const;
        const char *         getResourceName( GpuResourceHandle ) const;
        const Guid         * getResourceType( GpuResourceHandle ) const;
        GpuResource        * getResource( GpuResourceHandle );
        //@}

    private:

        const ResourceManager * getManager( const Guid & type ) const;
        ResourceManager       * getManager( const Guid & type );
        ResourceItem          * getResourceItem( GpuResourceHandle handle, bool silent = false ) const;
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_GPURESDB_H__
