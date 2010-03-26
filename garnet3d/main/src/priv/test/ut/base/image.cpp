#include "../testCommon.h"
#include "garnet/GNgfx.h"
#include <png.h>

static UInt8              gBuf[10000];
static GN::MemFile<UInt8> gFile(gBuf,10000,"a.png");

class ImageTest : public CxxTest::TestSuite
{
public:
    void testInvalidPNG()
    {
        png_byte png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
        ::memcpy( gBuf, png_signature, 8 );

        GN::gfx::ImageDesc id;
        GN::gfx::ImageReader ir;

        TS_ASSERT( !ir.ReadHeader( id ) );
        TS_ASSERT( !ir.ReadImage( gBuf ) );

        ir.Reset( gFile );
        TS_ASSERT( !ir.ReadHeader( id ) );
        TS_ASSERT( !ir.ReadImage( gBuf ) );
    }

    void testInvalidJPG()
    {
        gBuf[6] = 'J';
        gBuf[7] = 'F';
        gBuf[8] = 'I';
        gBuf[9] = 'F';

        GN::gfx::ImageDesc id;
        GN::gfx::ImageReader ir;

        gFile.Seek( 0, GN::FileSeek::SET );

        ir.Reset( gFile );
        TS_ASSERT( !ir.ReadHeader( id ) );
        TS_ASSERT( !ir.ReadImage( gBuf ) );
    }
};
