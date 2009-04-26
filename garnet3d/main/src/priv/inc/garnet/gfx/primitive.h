#ifndef __GN_GFX_PRIMITIVE_H__
#define __GN_GFX_PRIMITIVE_H__
// *****************************************************************************
/// \file
/// \brief   primitive types and related utilities.
/// \author  chenlee (2005.9.30)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// ��ͼԭ��
    ///
    struct PrimitiveType
    {
        enum Enum
        {
            POINT_LIST,     ///< point list
            LINE_LIST,      ///< line list
            LINE_STRIP,     ///< line strip
            TRIANGLE_LIST,  ///< triangle list
            TRIANGLE_STRIP, ///< triangle strip
            QUAD_LIST     , ///< quad list. D3D9 does not support this primitive.
            RECT_LIST     , ///< rect list. This is xenon special primitive.
            NUM_PRIMITIVES  ///< number of available primitive types
        };

        /// conver to string
        const char * toString() const
        {
            static const char * TABLE[]=
            {
                "POINT_LIST",
                "LINE_LIST",
                "LINE_STRIP",
                "TRIANGLE_LIST",
                "TRIANGLE_STRIP",
                "QUAD_LIST",
                "RECT_LIST",
            };

            const Enum & e = *this;
            if( 0 <= e && e < GN_ARRAY_COUNT(TABLE) ) return TABLE[e];
            else return "INVALID_PRIMITIVE_TYPE";
        }

        GN_DEFINE_ENUM_CLASS_HELPERS(PrimitiveType, Enum)
    };

    /// \name Primitive utils
    //@{

    ///
    /// convert primitive type to string
    ///
    inline StrA primitiveType2Str( PrimitiveType pt )
    {
        GN_GUARD_SLOW;
        switch( pt )
        {
            case PrimitiveType::POINT_LIST     : return "PrimitiveType::POINT_LIST";
            case PrimitiveType::LINE_LIST      : return "PrimitiveType::LINE_LIST";
            case PrimitiveType::LINE_STRIP     : return "PrimitiveType::LINE_STRIP";
            case PrimitiveType::TRIANGLE_LIST  : return "PrimitiveType::TRIANGLE_LIST";
            case PrimitiveType::TRIANGLE_STRIP : return "PrimitiveType::TRIANGLE_STRIP";
            case PrimitiveType::QUAD_LIST      : return "PrimitiveType::QUAD_LIST";
            case PrimitiveType::RECT_LIST      : return "PrimitiveType::RECT_LIST";
            default             : return "Invalid primitive type!";
        }
        GN_UNGUARD_SLOW;
    }

    ///
    /// convert string to primitive
    ///
    inline PrimitiveType str2PrimitiveType( const StrA & str )
    {
        GN_GUARD_SLOW;
        if( "PrimitiveType::POINT_LIST" == str ) return PrimitiveType::POINT_LIST;
        else if( "PrimitiveType::LINE_LIST" == str ) return PrimitiveType::LINE_LIST;
        else if( "PrimitiveType::LINE_STRIP" == str ) return PrimitiveType::LINE_STRIP;
        else if( "PrimitiveType::TRIANGLE_LIST" == str ) return PrimitiveType::TRIANGLE_LIST;
        else if( "PrimitiveType::TRIANGLE_STRIP" == str ) return PrimitiveType::TRIANGLE_STRIP;
        else if( "PrimitiveType::QUAD_LIST" == str ) return PrimitiveType::QUAD_LIST;
        else if( "PrimitiveType::RECT_LIST" == str ) return PrimitiveType::RECT_LIST;
        else
        {
            // failed
            static Logger * sLogger = getLogger("GN.gfx.misc");
            GN_ERROR(sLogger)( "invalid primitive string : '%s'!", str.cptr() );
            return PrimitiveType::NUM_PRIMITIVES;
        }
        GN_UNGUARD_SLOW;
    }

    ///
    /// calculate primitive count from vertex count
    ///
    /// \param pt       rendering primitive
    /// \param numvert  vertex count
    /// \return         primitive count
    ///
    inline size_t calcPrimitiveCount( PrimitiveType pt, size_t numvert )
    {
        GN_GUARD_SLOW;

        switch( pt )
        {
            case PrimitiveType::POINT_LIST     : return numvert;
            case PrimitiveType::LINE_LIST      : return (numvert >= 2) ? numvert / 2 : 0;
            case PrimitiveType::LINE_STRIP     : return (numvert >= 2) ? numvert - 1 : 0;
            case PrimitiveType::TRIANGLE_LIST  : return (numvert >= 3) ? numvert / 3 : 0;
            case PrimitiveType::TRIANGLE_STRIP : return (numvert >= 3) ? numvert - 2 : 0;
            case PrimitiveType::QUAD_LIST      : return (numvert >= 4) ? numvert / 4 : 0;
            case PrimitiveType::RECT_LIST      : return (numvert >= 3) ? numvert / 3 : 0;
            default             :
                {
                    static Logger * sLogger = getLogger("GN.gfx.misc");
                    GN_ERROR(sLogger)( "Invalid primitive type!" );
                }
                return 0;
        }
        GN_UNGUARD_SLOW;
    }

    ///
    /// calculate vertex count from primitive count
    ///
    /// \param pt       rendering primitive
    /// \param numprim  primitive count
    /// \return         vertex count
    ///
    inline size_t calcVertexCount( PrimitiveType pt, size_t numprim )
    {
        GN_GUARD_SLOW;
        switch( pt )
        {
            case PrimitiveType::POINT_LIST     : return numprim;
            case PrimitiveType::LINE_LIST      : return numprim * 2;
            case PrimitiveType::LINE_STRIP     : return numprim > 0 ? numprim + 1 : 0;
            case PrimitiveType::TRIANGLE_LIST  : return numprim * 3;
            case PrimitiveType::TRIANGLE_STRIP : return numprim > 0 ? numprim + 2 : 0;
            case PrimitiveType::QUAD_LIST      : return numprim * 4;
            case PrimitiveType::RECT_LIST      : return numprim * 3;
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
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_PRIMITIVE_H__
