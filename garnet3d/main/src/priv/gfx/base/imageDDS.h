#ifndef __GN_GFX_IMAGEDDS_H__
#define __GN_GFX_IMAGEDDS_H__
// *****************************************************************************
//! \file    imageDDS.h
//! \brief   DDS image reader
//! \author  chenlee (2005.6.2)
// *****************************************************************************

//!
//! DD pixel format
//!
struct DDPixelFormat
{
    uint32_t size;   //!< size of this structure
    uint32_t flags;  //!< pixel format flags
    uint32_t fourcc; //!< fourcc
    uint32_t bits;   //!< bits of the format
    uint32_t rMask;  //!< R, Y
    uint32_t gMask;  //!< G, U
    uint32_t bMask;  //!< B, V
    uint32_t aMask;  //!< A, A
};

//!
//! DDS file header
//!
struct DDSFileHeader
{
    //! \cond NEVER
    uint32_t        size;
    uint32_t        flags;
    uint32_t        height;
    uint32_t        width;
    uint32_t        pitchOrLinearSize;
    uint32_t        depth;
    uint32_t        mipCount;
    uint32_t        reserved[11];
    DDPixelFormat   ddpf;
    uint32_t        caps;
    uint32_t        caps2;
    uint32_t        caps3;
    uint32_t        caps4;
    uint32_t        reserved2;
    //! \endcond
};

//!
//! dds image reader
//!
class DDSReader
{
    DDSFileHeader      mHeader;
    GN::gfx::ImageDesc mImgDesc;

    const uint8_t * mSrc;
    size_t          mSize;

public:

    //!
    //! Constructor
    //!
    DDSReader()
    {
    }

    //!
    //! Destructor
    //!
    ~DDSReader()
    {
    }

    //!
    //! Check file format. Return true if the file is DDS file
    //!
    bool checkFormat( GN::File & );

    //!
    //! Read DDS header
    //!
    bool readHeader(
        GN::gfx::ImageDesc & o_desc, const uint8_t * i_buf, size_t i_size );

    //!
    //! Read DDS image
    //!
    bool readImage( void * o_data ) const;
};

// *****************************************************************************
//                           End of imageDDS.h
// *****************************************************************************
#endif // __GN_GFX_IMAGEDDS_H__
