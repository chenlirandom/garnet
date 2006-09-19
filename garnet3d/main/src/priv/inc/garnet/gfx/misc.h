#ifndef __GN_GFX_MISC_H__
#define __GN_GFX_MISC_H__
// *****************************************************************************
//! \file    gfx/misc.h
//! \brief   misc. types and functions of GFX module
//! \author  chenlee (2005.9.30)
// *****************************************************************************

namespace GN { namespace gfx
{
    //!
    //! Display mode structure
    //!
    struct DisplayMode
    {
        uint32_t width;   //!< Screen width. Zero means using current screen width. Default value is zero.
        uint32_t height;  //!< Screen height. Zero means using current screen height. Defualt value is zero.
        uint32_t depth;   //!< Color depth. Zero means using current color depth. Default value is zero.
        uint32_t refrate; //!< Referesh rate. Zero means using adapter default rate. Default value is zero.

        //!
        //! Set display mode parameters
        //!
        void set( uint32_t w, uint32_t h, uint32_t d, uint32_t r )
        {
            width = w; height = h; depth = d; refrate = r;
        }
    };

    //!
    //! ������־
    //!
    enum LockFlag
    {
        LOCK_RW,           //!< read-write lock, the slowest one.
        LOCK_RO,           //!< read-only lock.
        LOCK_WO,           //!< write-only lock
        LOCK_DISCARD,      //!< write-only lock, discard old value.
        LOCK_NO_OVERWRITE, //!< write-only lock, promise not to modify any section of the buffer being used.
        NUM_LOCK_FLAGS     //!< number of lock flags.
    };

    //!
    //! ��ͼԭ��
    //!
    enum PrimitiveType
    {
        POINT_LIST,     //!< point list
        LINE_LIST,      //!< line list
        LINE_STRIP,     //!< line strip
        TRIANGLE_LIST,  //!< triangle list
        TRIANGLE_STRIP, //!< triangle strip
        QUAD_LIST     , //!< quad list. D3D9 does not support this primitive.
        RECT_LIST     , //!< rect list. This is xenon special primitive.
        NUM_PRIMITIVES  //!< number of available primitive types
    };

    //! \name Primitive utils
    //@{

    //!
    //! convert primitive type to string
    //!
    inline StrA primitiveType2Str( PrimitiveType pt )
    {
        GN_GUARD_SLOW;
        switch( pt )
        {
            case POINT_LIST     : return "POINT_LIST";
            case LINE_LIST      : return "LINE_LIST";
            case LINE_STRIP     : return "LINE_STRIP";
            case TRIANGLE_LIST  : return "TRIANGLE_LIST";
            case TRIANGLE_STRIP : return "TRIANGLE_STRIP";
            case QUAD_LIST      : return "QUAD_LIST";
            case RECT_LIST      : return "RECT_LIST";
            default             : return "Invalid primitive type!";
        }
        GN_UNGUARD_SLOW;
    }

    //!
    //! convert string to primitive
    //!
    inline PrimitiveType str2PrimitiveType( const StrA & str )
    {
        GN_GUARD_SLOW;
        if( "POINT_LIST" == str ) return POINT_LIST;
        else if( "LINE_LIST" == str ) return LINE_LIST;
        else if( "LINE_STRIP" == str ) return LINE_STRIP;
        else if( "TRIANGLE_LIST" == str ) return TRIANGLE_LIST;
        else if( "TRIANGLE_STRIP" == str ) return TRIANGLE_STRIP;
        else if( "QUAD_LIST" == str ) return QUAD_LIST;
        else if( "RECT_LIST" == str ) return RECT_LIST;
        else
        {
            // failed
            static Logger * sLogger = getLogger("GN.gfx.misc");
            GN_ERROR(sLogger)( "invalid primitive string : '%s'!", str.cptr() );
            return NUM_PRIMITIVES;
        }
        GN_UNGUARD_SLOW;
    }

    //!
    //! calculate primitive count from vertex count
    //!
    //! \param pt       rendering primitive
    //! \param numvert  vertex count
    //! \return         primitive count
    //!
    inline size_t calcPrimitiveCount( PrimitiveType pt, size_t numvert )
    {
        GN_GUARD_SLOW;

        switch( pt )
        {
            case POINT_LIST     : return numvert;
            case LINE_LIST      : return (numvert >= 2) ? numvert / 2 : 0;
            case LINE_STRIP     : return (numvert >= 2) ? numvert - 1 : 0;
            case TRIANGLE_LIST  : return (numvert >= 3) ? numvert / 3 : 0;
            case TRIANGLE_STRIP : return (numvert >= 3) ? numvert - 2 : 0;
            case QUAD_LIST      : return (numvert >= 4) ? numvert / 4 : 0;
            case RECT_LIST      : return (numvert >= 3) ? numvert / 3 : 0;
            default             :
                {
                    static Logger * sLogger = getLogger("GN.gfx.misc");
                    GN_ERROR(sLogger)( "Invalid primitive type!" );
                }
                return 0;
        }
        GN_UNGUARD_SLOW;
    }

    //!
    //! calculate vertex count from primitive count
    //!
    //! \param pt       rendering primitive
    //! \param numprim  primitive count
    //! \return         vertex count
    //!
    inline size_t calcVertexCount( PrimitiveType pt, size_t numprim )
    {
        GN_GUARD_SLOW;
        switch( pt )
        {
            case POINT_LIST     : return numprim;
            case LINE_LIST      : return numprim * 2;
            case LINE_STRIP     : return numprim > 0 ? numprim + 1 : 0;
            case TRIANGLE_LIST  : return numprim * 3;
            case TRIANGLE_STRIP : return numprim > 0 ? numprim + 2 : 0;
            case QUAD_LIST      : return numprim * 4;
            case RECT_LIST      : return numprim * 3;
            default :
                {
                    static Logger * sLogger = getLogger("GN.gfx.misc");
                    GN_ERROR(sLogger)( "Invalid primitive type!" );
                }
                return 0;
        }
        GN_UNGUARD_SLOW;
    }

    //@}

}}

// *****************************************************************************
//                           End of misc.h
// *****************************************************************************
#endif // __GN_GFX_MISC_H__
