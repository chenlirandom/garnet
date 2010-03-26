#ifndef __GN_GPUCOMMON_BASICSURFACE_H__
#define __GN_GPUCOMMON_BASICSURFACE_H__
// *****************************************************************************
/// \file
/// \brief   surface base classes
/// \author  chenlee (2005.10.1)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// ��ͼ��Ļ���
    ///
    class BasicTexture : public Texture
    {
        // ********************************
        /// \name constructor
        // ********************************

        //@{
    protected :
        BasicTexture() {}
        //@}

        // ********************************
        //  protected functions
        // ********************************
    protected:

        ///
        /// Validate update parameters.
        ///
        bool validateUpdateParameters( size_t face, size_t level, const Box<UInt32> * area, SurfaceUpdateFlag flag, Box<UInt32> & clippedArea )
        {
            // check face
            if( face >= GetDesc().faces )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicTexture");
                GN_ERROR(sLogger)( "invalid lock face : %d", face );
                return false;
            }

            // check level
            if( level >= GetDesc().levels )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicTexture");
                GN_ERROR(sLogger)( "invalid lock level : %d", level );
                return false;
            }

            // check flag
            if( flag >= SurfaceUpdateFlag::NUM_FLAGS )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicTexture");
                GN_ERROR(sLogger)( "invalid lock flag : %d", flag.ToRawEnum() );
                return false;
            }

            // get texture size
            const Vector3<UInt32> & sz = GetMipSize( level );

            // make sure lock area is valid
            if( area )
            {
                clippedArea = *area;
                if( !sAdjustOffsetAndRange( clippedArea.x, clippedArea.w, sz.x ) ||
                    !sAdjustOffsetAndRange( clippedArea.y, clippedArea.h, sz.y ) ||
                    !sAdjustOffsetAndRange( clippedArea.z, clippedArea.d, sz.z ) )
                    return false;
            }
            else
            {
                clippedArea.x = 0;
                clippedArea.y = 0;
                clippedArea.z = 0;
                clippedArea.w = sz.x;
                clippedArea.h = sz.y;
                clippedArea.d = sz.z;
            }

            // success
            return true;
        }

        // ********************************
        //  private variables
        // ********************************
    private:

        // ********************************
        //  private functions
        // ********************************
    private:

        template<typename T>
        static inline bool
        sAdjustOffsetAndRange( T & offset, T & length, T maxLength )
        {
            if( offset >= maxLength )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicTexture");
                GN_ERROR(sLogger)( "offset is beyond the end of valid range." );
                return false;
            }
            if( 0 == length ) length = maxLength;
            if( offset + length > maxLength ) length = maxLength - offset;
            return true;
        }
    };

    ///
    /// Basic vertex buffer class
    ///
    class BasicVtxBuf : public VtxBuf
    {
        // ********************************
        /// \name constructor
        // ********************************

        //@{
    protected :
        BasicVtxBuf() {}
        //@}

        // ********************************
        //  protected functions
        // ********************************
    protected:

        ///
        /// Validate update parameters. Note that this function may modify "length" parameter
        ///
        bool validateUpdateParameters( size_t offset, size_t * length, const void * data, SurfaceUpdateFlag flag )
        {
            const VtxBufDesc & desc = GetDesc();

            if( NULL == data )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicVtxBuffer");
                GN_ERROR(sLogger)( "NULL data pointer!" );
                return false;
            }

            if( flag >= SurfaceUpdateFlag::NUM_FLAGS )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicVtxBuffer");
                GN_ERROR(sLogger)( "Invalid update flag: %d.", flag.ToRawEnum() );
                return false;
            }

            if( offset >= desc.length )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicVtxBuffer");
                GN_ERROR(sLogger)( "offset is beyond the end of vertex buffer!" );
                return false;
            }

            // adjust length
            if( 0 == *length ) *length = desc.length;
            if( offset + *length > desc.length ) *length = desc.length - offset;

            // success
            return true;
        }

        // ********************************
        //  private variables
        // ********************************
    private:

        // ********************************
        //  private functions
        // ********************************
    private:
    };

    ///
    /// Basic index buffer class
    ///
    class BasicIdxBuf : public IdxBuf
    {
        // ********************************
        /// \name constructor
        // ********************************

        //@{
    protected :
        BasicIdxBuf() {}
        //@}

        // ********************************
        //  protected functions
        // ********************************
    protected:

        ///
        /// Validate update parameters.
        ///
        bool validateUpdateParameters( size_t startidx, size_t * numidx, const void * data, SurfaceUpdateFlag flag )
        {
            GN_ASSERT( numidx );

            const IdxBufDesc & desc = GetDesc();

            if( NULL == data )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicIdxBuffer");
                GN_ERROR(sLogger)( "NULL data pointer!" );
                return false;
            }

            if( flag >= SurfaceUpdateFlag::NUM_FLAGS )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicIdxBuffer");
                GN_ERROR(sLogger)( "Invalid update flag: %d.", flag.ToRawEnum() );
                return false;
            }

            if( startidx >= desc.numidx )
            {
                static Logger * sLogger = GetLogger("GN.gfx.gpu.common.BasicIdxBuffer");
                GN_ERROR(sLogger)( "startidx is beyond the end of index buffer!" );
                return false;
            }

            // adjust startidx and numidx
            if( 0 == *numidx ) *numidx = desc.numidx;
            if( startidx + *numidx > desc.numidx ) *numidx = desc.numidx - startidx;

            // success
            return true;
        }

        // ********************************
        //  private variables
        // ********************************
    private:

        // ********************************
        //  private functions
        // ********************************
    private:
    };

    /// Add "<" operator, by memory compare, to any type that doesn't have one.
    template<typename T>
    struct MemCmpLessOperator
    {
        T value;

        MemCmpLessOperator() {}

        MemCmpLessOperator( T & t ) : value(t) {}

        operator       T &()       { return value; }
        operator const T &() const { return value; }

        bool operator<( const MemCmpLessOperator & rhs ) const
        {
            return memcmp( &value, &rhs, sizeof(T) ) < 0;
        }
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GPUCOMMON_BASICSURFACE_H__
