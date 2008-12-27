#ifndef __GN_GFXOGL_OGLVTXFMT_H__
#define __GN_GFXOGL_OGLVTXFMT_H__
// *****************************************************************************
/// \file
/// \brief   OGL vertex binding class
/// \author  chenlee (2005.11.21)
// *****************************************************************************

#include "oglResource.h"

namespace GN { namespace gfx
{
    ///
    /// OGL vertex binding class
    ///
    class OGLVtxFmt : public OGLResource, public StdClass
    {
         GN_DECLARE_STDCLASS( OGLVtxFmt, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        OGLVtxFmt( OGLRenderer & r ) : OGLResource(r) { clear(); }
        virtual ~OGLVtxFmt() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const VertexFormat & );
        void quit();
    private:
        void clear() { mAttribBindings.clear(); mStateBindings.clear(); }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        ///
        /// Get vertex format descriptor
        ///
        const VertexFormat & getFormat() const { return mFormat; }

        ///
        /// Bind the format to device
        ///
        void bindStates() const;

        ///
        /// Bind the buffer to device
        ///
        bool bindBuffers( const UInt8 * const * buffers,
                          const UInt16        * strides,
                          size_t                count ) const;

        // ********************************
        // private variables
        // ********************************
    private:

        struct AttribBindingInfo
        {
            const OGLVtxFmt * self;
            UInt16            offset;
            UInt8             stream; ///< vertex stream index
            UInt8             index;  ///< texture coordinate stage index or vertex attribute index
            GLuint            format;
            GLuint            components;
            GLboolean         normalization;
        };

        typedef void (*FP_setOglVertexBuffer)( const AttribBindingInfo &, const UInt8 * buf, size_t stride );

        struct AttribBinding
        {
            AttribBindingInfo     info;
            FP_setOglVertexBuffer func;
            void bind( const UInt8 * buf, size_t stride ) const
            {
                GN_ASSERT( func );
                func( info, buf, stride );
            }
        };

        struct StateBindingInfo
        {
            const OGLVtxFmt * self;
            size_t            texStage;
            GLenum            semantic;
            GLuint            attribute;
        };

        typedef void (*FP_setOglVertexState)( const StateBindingInfo & );

        struct StateBinding
        {
            StateBindingInfo     info;
            FP_setOglVertexState func;
        };

        VertexFormat             mFormat;
        DynaArray<AttribBinding> mAttribBindings;
        DynaArray<StateBinding>  mStateBindings;

        // ********************************
        // private functions
        // ********************************
    private:

        // state binding utils
        bool setupStateBindings();

        static void sSetVertexPointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );
        static void sSetNormalPointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );
        static void sSetColorPointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );
        static void sSetSecondaryColorPointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );
        static void sSetFogPointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );
        static void sSetTexCoordPointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );
        static void sSetVertexAttributePointer( const AttribBindingInfo &, const UInt8 * buf, size_t stride );

        static void sEnableClientState( const StateBindingInfo & info );
        static void sDisableClientState( const StateBindingInfo & info );
        static void sEnableVAA( const StateBindingInfo & info );
        static void sDisableVAA( const StateBindingInfo & info );
        static void sEnableTexArray( const StateBindingInfo & info );
        static void sDisableTexArray( const StateBindingInfo & info );
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFXOGL_OGLVTXFMT_H__
