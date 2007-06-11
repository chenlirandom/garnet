#ifndef __GN_GFX2_GNGFX2_H__
#define __GN_GFX2_GNGFX2_H__
// *****************************************************************************
//! \file    experimental/GNgfx2.h
//! \brief   experimental effect based GFX interface
//! \author  chenli@@FAREAST (2007.6.11)
// *****************************************************************************

namespace GN
{
    ///
    /// namespace for new experimental graphics system
    namespace gfx2
    {
    }
}

namespace GN { namespace gfx2
{
    // *************************************************************************
    // surface
    // *************************************************************************

    enum
    {
        MAX_SURFACE_ELEMENT_ATTRIBUTES = 16,  ///< max attributes in single surface element
        MAX_SUB_SURFACES               = 256, ///< max sub surfaces count in single surface
    };

    ///
    /// surface access flags
    ///
    enum SurfaceAccessFlag
    {
        //@{
        SAF_HOST_READ     = 0x1, ///< can be readen by host application.
        SAF_HOST_WRITE    = 0x2, ///< can be modified by host application.
        SAF_DEVICE_READ   = 0x4, ///< can bind to input port of a effect
        SAF_DEVICE_WRITE  = 0x8, ///< can bind to output port of a effect
        //@}
    };

    ///
    /// syrface element attribute. This is the minimal unit of a surface.
    ///
    struct SurfaceElementAttribute
    {
        FOURCC semantic; ///< FORCC encoded sementic. (must be unique in single surfel)
        SInt16 format;   ///< attribute format. (FMT_XXX).
        UInt16 offset;   ///< offset in element.

        ///
        /// set values in attribute descriptor
        ///
        void set( FOURCC sem_, SInt16 fmt_, UInt16 offset_ )
        {
            semantic = sem_;
            offset   = offset_;
            format   = fmt_;
        }
    };
    GN_CASSERT( sizeof(SurfaceElementAttribute) == 8 );

    ///
    /// Surface element (surfel)
    ///
    struct SurfaceElement
    {
        SurfaceElementAttribute attribs[MAX_SURFACE_ELEMENT_ATTRIBUTES]; ///< surfel attribute list
        UInt32                  count;                                   ///< surfel attribute count
        UInt32                  stride;                                  ///< surfel stride in bytes
    };

    ///
    /// describe sub surface data orgnization
    ///
    struct SubSurfaceLayout
    {
        // all values are in unit of element
        //@{
        UInt32 width;        ///< sub surface width in element
        UInt32 height;       ///< sub surface height in element
        UInt32 depth;        ///< sub surface depth in element
        UInt32 rowBytes;     ///< row pitch in bytes
        UInt32 sliceBytes;   ///< slice pitch in bytes
        //@}
    };

    ///
    /// fully describe surface data orgnization
    ///
    struct SurfaceLayout
    {
        int              orgnization; ///< 1D, 1D array, 2D, 2D array, 3D, 3D array
        int              levels;      ///< LOD levels
        int              faces;       ///< number of faces
        SurfaceElement   element;     ///< element descriptor
        SubSurfaceLayout subsurfaces[MAX_SUB_SURFACES]; ///< indexed by (arrayIndex * mipcount + mipIndex)
    };

    ///
    /// surface descriptor
    ///
    struct SurfaceDesc
    {
        SurfaceLayout layout; ///< surface data layout
        int           access; ///< surface access flags, combination of SurfaceAccessFlag.
    };

    ///
    /// sub surface data
    ///
    struct SubSurfaceData
    {
        //@{
        void * data;         ///< sub surface data pointer
        UInt32 rowBytes;     ///< row pitch in bytes (at least "element stride * sub surface width" )
        UInt32 sliceBytes;   ///< slice pitch in bytes (at least "rowBytes * sub surface height" )
        //@}
    };

    ///
    /// unstructured/opaque surface data in native device format, which is used to fast surface save and load.
    ///
    struct NativeSurfaceData
    {
        ///
        /// surface data chunk
        ///
        struct Chunk
        {
            size_t bytes; ///< chunk bytes
            void * data;  ///< chunk data, may points to external data, or somewhere in DeviceSurfaceData::data
        };

        UInt32           numChunks;                ///< chunk count
        Chunk            chunks[MAX_SUB_SURFACES]; ///< chunks
        DynaArray<UInt8> data;                     ///< raw data
    };

    ///
    /// Surface interface: store device dependant data.
    ///
    struct Surface : public NoCopy
    {
        ///
        /// get surface descriptor
        ///
        virtual const SurfaceDesc & getDesc() const = 0;

        ///
        /// copy data to surface. Surface must have SAF_HOST_WRITE flag.
        ///
        virtual void download(
            UInt32              subsurface,
            const Box<UInt32> & area,
            const void        * source,
            UInt32              rowBytes,
            UInt32              sliceBytes,
            ) = 0;

        ///
        /// transfer data from device to host. Surface must have SAF_HOST_READ flag.
        ///
        /// Note that destination buffer must be large enough to hold the whole sub surface.
        ///
        virtual void upload(
            UInt32              subsurface,
            void              * destination,
            UInt32              bytes ) = 0;

        /// \name save and load surface content in device native format.
        ///
        /// Similar as download() and upload(), but operates on whole surface, and normally faster.
        ///
        //@{
        virtual void save( NativeSurfaceData & ) const = 0;
        virtual void load( const NativeSurfaceData & ) = 0;
        //@}
    };

    // *************************************************************************
    // effect
    //
    // There's 2 way to transter data between device and host: by surface or by
    // raw host data pointer. A effect may support either one of them, or both.
    // send/get data to/from effect by the "unsupported" way has no effect but
    // generate some error messages.
    //
    // *************************************************************************

    ///
    /// define template of the surface data layout that can be used to match one or mutiple data layouts.
    ///
    struct SurfaceLayoutTemplate
    {
        ///
        /// check whether a layout matches the template
        ///
        bool match( const SurfaceLayout & ) const;
    };

    ///
    /// effect port descriptor: describe a input or output port of the effect
    ///
    struct EffectPortDesc
    {
        //@{
        StrA                  sementic;
        SurfaceLayoutTemplate layout;
        //@}
    };

    ///
    /// effect descriptor: fully describe what the effect is.
    ///
    struct EffectDesc
    {
        DynaArray<EffectPortDesc> ports; ///< input output ports. Note that port semantic must be unique.
    };

    ///
    /// define surface to effect binding
    ///
    struct EffectInputOutputLayout
    {
        struct PortBinding
        {
            StrA      port;       ///< effect port name
            Surface * surf;       ///< surface pointer
            StrA      semantic;   ///< semantic of the attribute that will be binded.
            //@{
            UInt32    firstLevel;
            UInt32    numLevels;
            UInt32    firstFace;
            UInt32    numFaces;
            //@}
        };
        DynaArray<PortBinding> bindings;
    };

    ///
    /// effect binding handle
    ///
    typedef UIntPtr EffectBinding;

    ///
    /// effect interface: represents a process kernel function
    ///
    struct Effect : public NoCopy
    {
        ///
        /// check whether a surface is compatible with the effect
        ///
        virtual bool compatible( Surface * surf, const StrA & port ) = 0;

        ///
        /// create a binding handle
        ///
        virtual EffectBinding createBinding( EffectInputOutputLayout & ) = 0;

        ///
        /// delete a binding handle
        ///
        virtual void deleteBinding( EffectBinding ) = 0;

        ///
        /// bind surfaces to effect
        ///
        virtual void bind( EffectBinding ) = 0;
    };

    // *************************************************************************
    // Graphics system
    // *************************************************************************

    ///
    /// surface creation parameters
    ///
    struct SurfaceCreationParameter
    {
        ///
        /// define binding to specific port of specific effect
        ///
        struct EffectBinding
        {
            StrA effect; ///< effect name
            StrA port;   ///< port name
        };

        ///
        /// define required bindings of the resources
        ///
        DynaArray<EffectBinding> binding;

        ///
        /// Force the created surface supports some access flags.
        ///
        /// Normally set to zero to let graphics system to determine the optimal access flags.
        ///
        /// Set to non-zero value may fail the creation process.
        ///
        int forceAccessFlag;
    };

    ///
    /// Major interface of new graphics system
    ///
    struct GraphicsSystem : public NoCopy
    {
        /// \name global states
        //@{

        // ...

        //@}

        /// \name effect management
        //@{

        void     registerEffects( ... );
        Effect * getEffect( const StrA & name );

        //@}

        /// \name surface management
        //@{

        Surface * createSurface( const SurfaceCreationParameter & );

        //@}
    };

    // *************************************************************************
    // Sample code
    // *************************************************************************

    //@{

    GraphicsSystem gs;

    void RenderToCube()
    {
        // render-to-cube effect has 
    }

    //@}
}}

// *****************************************************************************
//                           End of GNgfx2.h
// *****************************************************************************
#endif // __GN_GFX2_GNGFX2_H__
