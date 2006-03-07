#include "pch.h"
#if !GN_ENABLE_INLINE
#include "garnet/gfx/image.inl"
#endif
#include "imagePNG.h"
#include "imageJPG.h"
#include "imageDDS.h"

// *****************************************************************************
// ImageDesc
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ImageDesc::valid() const
{
    // check format
    if( format < 0 || format >= NUM_CLRFMTS )
    {
        GN_ERROR( "invalid image format!" );
        return false;
    }

    // check mipmap pointer
    if( numFaces > 0 && numLevels > 0 && 0 == mipmaps )
    {
        GN_ERROR( "Null mipmap array!" );
        return false;
    }

    const GN::gfx::ClrFmtDesc & fd = getClrFmtDesc( format );

    // check mipmaps
    for( size_t f = 0; f < numFaces; ++f )
    for( size_t l = 0; l < numLevels; ++l )
    {
        const MipmapDesc & m = getMipmap( f, l );

        // check image size
        if( 0 == m.width || 0 == m.height || 0 == m.depth )
        {
            GN_ERROR( "mipmaps[%d] size is zero!", l );
            return false;
        }

        // check pitches
        uint32_t w = m.width / fd.blockWidth;
        uint32_t h = m.height / fd.blockHeight;
        if( 0 == w ) w = 1;
        if( 0 == h ) h = 1;
        if( m.rowPitch != w * fd.blockWidth * fd.blockHeight * fd.bits / 8 )
        {
            GN_ERROR( "rowPitch of mipmaps[%d][%d] is incorrect!", f, l );
            return false;
        }
        if( m.slicePitch != m.rowPitch * h )
        {
            GN_ERROR( "slicePitch of mipmaps[%d][%d] is incorrect!", f, l );
            return false;
        }
        if( m.levelPitch != m.slicePitch * m.depth )
        {
            GN_ERROR( "levelPitch of mipmaps[%d][%d] is incorrect!", f, l );
            return false;
        }
    }

    // success
    return true;
}

// *****************************************************************************
// imgreade_c::Impl
// *****************************************************************************

//!
//! implementation class of image reader
//!
class GN::gfx::ImageReader::Impl
{
    //!
    //! file format tag
    //!
    enum FileFormat
    {
        UNKNOWN = 0,
        JPEG,
        BMP,
        PNG,
        DDS,
    };

    //!
    //! image reader state
    //!
    enum ReaderState
    {
        INVALID = 0,
        INITIALIZED,
        HEADER_READEN,
        DATA_READEN,
    };

    std::vector<uint8_t> mSrc;

    JpegReader  mJpg;
    PngReader   mPng;
    DDSReader   mDds;

    FileFormat  mFileFormat;
    ReaderState mState;

public:

    //!
    //! default ctor
    //!
    Impl() : mFileFormat(UNKNOWN), mState(INVALID) {}

    //!
    //! reset image reader
    //!
    bool reset( File & i_file )
    {
        GN_GUARD;

        static const size_t HEADER_BYTES = 10;

        // reset internal states
        mFileFormat = UNKNOWN;
        mState  = INVALID;

        // get file size
        size_t sz = i_file.size();
        if( sz <= HEADER_BYTES )
        {
            GN_ERROR( "Image file size is too small! Must not be a valid image file." );
            return false;
        }
        mSrc.resize( sz );

        // read file header
        sz = i_file.read( &mSrc[0], HEADER_BYTES );
        if( size_t(-1) == sz || sz < HEADER_BYTES )
        {
            GN_ERROR( "Fail to read image header!" );
            return false;
        }

        // detect file format
        if( 'J' == mSrc[6] && 'F' == mSrc[7] &&
            'I' == mSrc[8] && 'F' == mSrc[9] )
        {
            // JPEG format
            mFileFormat = JPEG;
        }
        else if( 'B' == mSrc[0] && 'M' == mSrc[1] )
        {
            // BMP format
            mFileFormat = BMP;
        }
        else if ( 0 == png_sig_cmp(&mSrc[0], 0, 8) )
        {
            // PNG format
            mFileFormat = PNG;
        }
        else if( 'D' == mSrc[0] && 'D' == mSrc[1] &&
                 'S' == mSrc[2] && ' ' == mSrc[3] )
        {
            // DDS format
            mFileFormat = DDS;
        }
        else
        {
            GN_ERROR( "unknown image file format!" );
            return false;
        }

        // read whole file
        sz = i_file.read( &mSrc[HEADER_BYTES], mSrc.size() - HEADER_BYTES );
        if( size_t(-1) == sz ) return false;

        // success
        mState = INITIALIZED;
        return true;

        GN_UNGUARD;
    }

    //!
    //! read image header
    //!
    bool readHeader( ImageDesc & o_desc )
    {
        GN_GUARD;

        if( INITIALIZED != mState )
        {
            GN_ERROR( "image reader is not ready for header reading!" );
            return false;
        }

        GN_ASSERT( !mSrc.empty() );

        #define READ_HEADER( reader ) \
            if( !reader.readHeader( o_desc, &mSrc[0], mSrc.size() ) ) \
            { mState = INVALID; return false; }

        switch( mFileFormat )
        {
            case JPEG : READ_HEADER( mJpg ); break;
            //case BMP  : READ_HEADER( mBmp ); break;
            case PNG  : READ_HEADER( mPng ); break;
            case DDS  : READ_HEADER( mDds ); break;
            default   :
                GN_ERROR( "unknown or unsupport file format!" );
                mState = INVALID;
                return false;
        }

        #undef READ_HEADER

        // success
        mState = HEADER_READEN;
        return true;

        GN_UNGUARD;
    }

    //!
    //! read image data
    //!
    bool readImage( void * o_data )
    {
        GN_GUARD;

        if( 0 == o_data )
        {
            GN_ERROR( "null output buffer!" );
            return false;
        }

        if( HEADER_READEN != mState )
        {
            GN_ERROR( "image reader is not ready for image reading!" );
            return false;
        }

        #define READ_IMAGE( reader ) \
            if( !reader.readImage( o_data ) ) \
            { mState = INVALID; return false; }

        switch( mFileFormat )
        {
            case JPEG : READ_IMAGE( mJpg ); break;
            //case BMP  : READ_IMAGE( mBmp ); break;
            case PNG  : READ_IMAGE( mPng ); break;
            case DDS  : READ_IMAGE( mDds ); break;
            default   :
                GN_ERROR( "unknown or unsupport file format!" );
                mState = INVALID;
                return false;
        }

        #undef READ_IMAGE

        // success
        mState = DATA_READEN;
        return true;

        GN_UNGUARD;
    }
};

// *****************************************************************************
// ImageReader
// *****************************************************************************

//
// ctor/dtor
// -----------------------------------------------------------------------------
GN::gfx::ImageReader::ImageReader() : mImpl( new Impl ) {}
GN::gfx::ImageReader::~ImageReader() { delete mImpl; }

//
// forward call to Impl
// -----------------------------------------------------------------------------
bool GN::gfx::ImageReader::reset( File & i_file )
{
    GN_GUARD;
    return mImpl->reset( i_file );
    GN_UNGUARD;
}
//
bool GN::gfx::ImageReader::readHeader( ImageDesc & o_desc )
{
    GN_GUARD;
    return mImpl->readHeader( o_desc );
    GN_UNGUARD;
}
//
bool GN::gfx::ImageReader::readImage( void * o_data )
{
    GN_GUARD;
    return mImpl->readImage( o_data );
    GN_UNGUARD;
}
