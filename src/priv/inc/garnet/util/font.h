#ifndef __GN_UTIL_FONT_H__
#define __GN_UTIL_FONT_H__
// *****************************************************************************
/// \file
/// \brief   font utilities
/// \author  chen@@CHENLI-HOMEPC (2007.3.14)
// *****************************************************************************

#include "garnet/GNgfx.h"

namespace GN { namespace util
{
    ///
    /// bitmap image of single character
    ///
    struct FontImage
    {
        size_t        width;  ///< bitmap width in pixel
        size_t        height; ///< bitmap height in pixel
        const uint8 * buffer; ///< bitmap data (8 bits gray image)

        /// \name per character metrics in unit of pixels,
        ///
        /// X coordinate points RIGHT; Y coordinate points DOWN.
        ///
        /// For Glyph metrics detail, check out http://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
        //@{
        float horiBearingX;
        float horiBearingY; // usually negative for horizontal font
        float horiAdvance;

        float vertAdvance;
        //@}
    };

    ///
    /// font quality
    ///
    enum FontFaceQuality
    {
        FFQ_MONOCHROM,      ///< default quality, suitable for small size font.
        FFQ_ANTIALIASED,    ///< suitable for larger size font.
        NUM_FONT_QUALITIES, ///< number of font quality enumerations
    };

    ///
    /// Font face descriptor
    ///
    struct FontFaceDesc
    {
        ///
        /// font file name. Normally would be something like "font::/xxxx"
        ///
        StrA fontname;

        ///
        /// font quality
        ///
        FontFaceQuality quality;

        /// Metrics that are defined for all glyphs in a given font.
        //@{

        // bounding box in pixles. X points RIGHT; Y points DOWN
        float xmin; // left
        float xmax; // right
        float ymin; // top
        float ymax; // bottom

        /// The distance that must be placed between two lines of text
        float linegap;

        /// max height in pixels of all the glyphs.
        uint16 maxGlyphWidth() const { return (uint16)ceil(xmax - xmin); }

        /// max height in pixels of all the glyphs.
        uint16 maxGlyphHeight() const { return (uint16)ceil(ymax - ymin); }

        /// baseline-to-baseline distance of 2 rows of text
        float baseLineDistance() const { return ymax - ymin + linegap; }

        //@}
    };

    ///
    /// abstract font interface
    ///
    struct FontFace : public RefCounter
    {
        ///
        /// get descriptor
        ///
        virtual const FontFaceDesc & getDesc() const = 0;

        ///
        /// load font data of specific unicode character.
        ///
        /// \note This function will erase previously loaded character (invalidate previous FontImage::buffer)
        ///
        virtual bool loadFontImage( FontImage & result, wchar_t ch ) = 0;

        ///
        /// get font kerning data in pixel
        ///
        virtual void getKerning( int & dx, int & dy, wchar_t ch1, wchar_t ch2 ) = 0;
    };

    ///
    /// Font face creation descriptor
    ///
    struct FontFaceCreationDesc
    {
        ///
        /// font file name. Usually would be something like "font::/xxxx"
        ///
        StrA fontname;

        ///
        /// Character width in pixel.
        ///
        uint16 width;

        ///
        /// get character height in pixel
        ///
        uint16 height;

        ///
        /// font quality
        ///
        FontFaceQuality quality;
    };

    ///
    /// create font face from a font file
    ///
    FontFace * createFontFace( const FontFaceCreationDesc & cd );

    ///
    /// Mixed font face creation descriptor
    ///
    struct MixedFontCreationDesc
    {
        wchar_t              firstChar;
        size_t               numChars;
        FontFaceCreationDesc font;
    };

    ///
    /// Create font face with mixed font face
    ///
    FontFace * createMixedFontFace(
        const FontFaceCreationDesc  & defaultFont,
        const MixedFontCreationDesc * additionalFonts,
        size_t                        numAdditionalFonts );

    ///
    /// create simple ASCII only font with size of 8x16, without external font file dependency.
    ///
    FontFace * createSimpleAsciiFontFace();

    ///
    /// font alignment flags
    ///
    struct TextAlignment
    {
        enum ENUM
        {
            HORI_LEFT      = 0,

            VERT_TOP       = 0,
            VERT_CENTER    = 1,
            VERT_BASELINE  = 2,
            VERT_BOTTOM    = 3,
        };
    };

    ///
    /// text descriptor
    ///
    struct TextDesc
    {
        const wchar_t * text;       ///< text string
        size_t          len;        ///< text lengh. Set to 0 for NULL-terminated string.
        float           x;          ///< pen position of the first character
        float           y;          ///< pen position of the first character
        float           z;          ///< pen position of the first character
        uint32          rgba;       ///< text color in R-G-B-A format.
        uint32          alignment;  ///< combination of TextAlignment flags
        bool            background; ///< draw background
        bool            kerning;    ///< enable text kerning
    };

    ///
    /// bitmap font renderer.
    ///
    class BitmapFont : public StdClass
    {
        GN_DECLARE_STDCLASS( BitmapFont, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        BitmapFont() { clear(); }
        virtual ~BitmapFont() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        /// \param sr           Pointer to sprite renderer
        /// \param ff           Pointer to the font face object. Its reference count will be increased by one after this function call.
        /// \param maxchars     Maxinum different characters allowed.
        bool init( gfx::SpriteRenderer * sr, FontFace * ff, size_t maxchars = 4096 );
        void quit();
    private:
        void clear()
        {
            mFontSlots = NULL;
            for( size_t i = 0; i < MAX_TEXTURES; ++i )
            {
                mCharList[i] = NULL;
            }
        }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        ///
        /// Get default font pointer. The refcounter of the fount is _NOT_ increased.
        ///
        FontFace * getDefaultFontFace() const { return mFont.get(); }

        ///
        /// draw UNICODE text
        ///
        void drawText( const TextDesc & td );

        ///
        /// draw UNICODE text
        ///
        void drawText( const wchar_t * text, float x, float y )
        {
            TextDesc td;
            td.text = text;
            td.len = 0;
            td.x = x;
            td.y = y;
            td.z = 0;
            td.rgba = 0xFFFFFFFF;
            td.alignment = TextAlignment::HORI_LEFT | TextAlignment::VERT_TOP;
            td.background = true;
            td.kerning = false;
            drawText( td );
        }

        // ********************************
        // private variables
        // ********************************
    private:

        ///
        /// font slot structure.
        ///
        /// "font slot" is a square area inside the font texture that
        /// representing a single character's image.
        ///
        struct FontSlot
        {
            ///
            /// character code
            ///
            wchar_t ch;

            ///
            /// texture index
            ///
            uint8 texidx;

            ///
            /// offset value ( in pixel unit )
            ///
            float offx, offy;

            ///
            /// advance value ( in pixel unit )
            ///
            float advx, advy;

            ///
            /// texture coord square ( in texture unit )
            ///
            float u, v, tw, th;

            ///
            /// texture square( in pixel unit )
            ///
            uint16 x, y, w, h;
        };

        // private constants
        enum
        {
            MAX_TEXTURES = gfx::GpuContext::MAX_TEXTURES,
        };

        // character information
        struct CharInfo
        {
            const FontSlot * fs;   // font slot of the character
            float            x, y; // screen position of the character
        };

        typedef StackArray<AutoRef<gfx::Texture>,MAX_TEXTURES> TextureArray;

        typedef HashMap<wchar_t,size_t> SlotMap;

        // graphics resources
        gfx::SpriteRenderer       * mSpriteRenderer;

        // font face data
        AutoRef<FontFace>           mFont;

        // font slot
        FontSlot                  * mFontSlots;
        size_t                      mMaxSlots;
        size_t                      mNumUsedSlots;  // number of used slots

        SlotMap                     mSlotMap;       // map that convert charcode to slot index

        // font textures
        TextureArray                mTextures;

        // character list
        CharInfo                  * mCharList[MAX_TEXTURES];
        size_t                      mNumChars[MAX_TEXTURES];

        // ********************************
        // private functions
        // ********************************
    private:

        ///
        /// get slot of specific character
        ///
        /// \return Return null, if failed.
        ///
        inline const FontSlot * getSlot( wchar_t ch );

        ///
        /// create slot of specific character
        ///
        /// \return Return null, if failed.
        ///
        const FontSlot * createSlot( wchar_t ch );

        ///
        /// initialize font slots and font textures
        ///
        bool slotInit(
            gfx::Gpu & gpu,
            uint16     fontw,
            uint16     fonth,
            size_t     maxchars );
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_UTIL_FONT_H__
