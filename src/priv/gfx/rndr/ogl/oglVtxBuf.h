#ifndef __GN_GFXOGL_OGLVTXBUF_H__
#define __GN_GFXOGL_OGLVTXBUF_H__
// *****************************************************************************
/// \file
/// \brief   OGL vertex buffer classes
/// \author  chenlee (2005.11.21)
// *****************************************************************************

#include "oglResource.h"
#include "../common/basicSurface.h"

namespace GN { namespace gfx
{
    ///
    /// Basic OGL vertex buffer class
    ///
    struct OGLBasicVtxBuf : public BasicVtxBuf, public OGLResource
    {
        ///
        /// ����ָ�򶥵����ݵ�ָ��
        ///
        virtual const UInt8 * getVtxData() const = 0;

    protected :

        /// protected ctor
        OGLBasicVtxBuf( OGLRenderer & r ) : OGLResource( r ) {}
    };

    ///
    /// Normal OGL vertex buffer class
    ///
    class OGLVtxBufNormal : public OGLBasicVtxBuf, public StdClass
    {
         GN_DECLARE_STDCLASS( OGLVtxBufNormal, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        OGLVtxBufNormal( OGLRenderer & r ) : OGLBasicVtxBuf(r) { clear(); }
        virtual ~OGLVtxBufNormal() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const VtxBufDesc & desc );
        void quit();
    private:
        void clear() { mBuffer = 0; }
        //@}

        // ********************************
        // from VtxBuf
        // ********************************
    public:

        virtual void update( size_t offset, size_t length, const void * data, UpdateFlag flag );
        virtual void readback( std::vector<UInt8> & data );

        // ********************************
        // public OGLBasicVtxBuf
        // ********************************
    public:

        const UInt8 * getVtxData() const
        {
            if( GLEW_ARB_vertex_buffer_object )
            {
                GN_OGL_CHECK( glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 ) );
            }
            return mBuffer;
        }

        // ********************************
        // private variables
        // ********************************
    private:

        UInt8 * mBuffer;
    };

    ///
    /// Vertex buffer that use GL_ARB_vertex_buffer_object.
    ///
    class OGLVtxBufVBO : public OGLBasicVtxBuf, public StdClass
    {
         GN_DECLARE_STDCLASS( OGLVtxBufVBO, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        OGLVtxBufVBO( OGLRenderer & r ) : OGLBasicVtxBuf(r) { clear(); }
        virtual ~OGLVtxBufVBO() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const VtxBufDesc & desc );
        void quit();
    private:
        void clear()
        {
            mOGLVertexBufferObject = 0;
            mOGLUsage = 0;
         }
        //@}

        // ********************************
        // from VtxBuf
        // ********************************
    public:

        virtual void update( size_t offset, size_t length, const void * data, UpdateFlag flag );
        virtual void readback( std::vector<UInt8> & data );

        // ********************************
        // public OGLBasicVtxBuf
        // ********************************
    public:

        const UInt8 * getVtxData() const
        {
            GN_OGL_CHECK( glBindBufferARB( GL_ARRAY_BUFFER_ARB, mOGLVertexBufferObject ) );
            return 0;
        }

        // ********************************
        // private variables
        // ********************************
    private:

        GLuint mOGLVertexBufferObject;
        GLenum mOGLUsage;

        // ********************************
        // private functions
        // ********************************
    private:
        bool createVBO();
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFXOGL_OGLVTXBUF_H__
