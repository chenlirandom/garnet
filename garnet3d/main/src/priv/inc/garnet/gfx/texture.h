#ifndef __GN_GFX_TEXTURE_H__
#define __GN_GFX_TEXTURE_H__
// *****************************************************************************
//! \file    texture.h
//! \brief   Texture interface
//! \author  chenlee (2005.9.30)
// *****************************************************************************

//!
//! global texture dictionary
//!
#define gTexDict (GN::gfx::TextureDictionary::sGetInstance())

//!
//! pointer to global texture dictionary
//!
#define gTexDictPtr (GN::gfx::TextureDictionary::sGetInstancePtr())

namespace GN { namespace gfx
{
    //!
    //! ��ͼ����
    //!
    enum TexType
    {
        TEXTYPE_1D,    //!< 1D texture
        TEXTYPE_2D,    //!< 2D texture
        TEXTYPE_3D,    //!< 3D texture
        TEXTYPE_CUBE,  //!< Cube texture
        TEXTYPE_STACK, //!< Stack texture
        NUM_TEXTYPES   //!< Number of avaliable texture types.
    };

    //!
    //! Texture usage.
    //!
    enum TextureUsage
    {
        TEXUSAGE_DYNAMIC        = 1<<0, //!< dynamic texture
        TEXUSAGE_AUTOGEN_MIPMAP = 1<<1, //!< texture that can auto-generate mipmaps
        TEXUSAGE_RENDER_TARGET  = 1<<2, //!< render target texture
        TEXUSAGE_DEPTH          = 1<<3, //!< depth texture
        TEXUSAGE_READBACK       = 1<<4, //!< Normally, read data from texture is extremly slow.
                                        //!< Use this flag to increase reading speed, in exchange for rendering speed.
                                        //!< Use this flag when you really need it.
    };

    //!
    //! ��ͼ������
    //!
    enum TexFilter
    {
        TEXFILTER_NEAREST,      //!< nearest
        TEXFILTER_LINEAR,       //!< linear
        TEXFILTER_N_MIP_N,      //!< NEAREST_MIPMAP_NEAREST
        TEXFILTER_N_MIP_L,      //!< NEAREST_MIPMAP_LINEAR
        TEXFILTER_L_MIP_N,      //!< LINEAR_MIPMAP_NEAREST
        TEXFILTER_L_MIP_L,      //!< LINEAR_MIPMAP_LINEAR
        NUM_TEXFILTERS
    };

    //!
    //! ��ͼWrap Mode
    //!
    enum TexWrap
    {
        TEXWRAP_REPEAT,
        TEXWRAP_CLAMP,
        TEXWRAP_CLAMP_TO_EDGE,
        NUM_TEXWRAPS
    };

    //!
    //! cube texture face
    //!
    enum TexFace
    {
        TEXFACE_PX, //!< positive X
        TEXFACE_NX, //!< negative X
        TEXFACE_PY, //!< positive Y
        TEXFACE_NY, //!< negative Y
        TEXFACE_PZ, //!< positive Z
        TEXFACE_NZ, //!< negative Z
        NUM_TEXFACES
    };

    struct Texture;

    //!
    //! Texture content loader
    //!
    typedef Functor1<bool,Texture&> TextureLoader;

    //!
    //! Texture descriptor
    //!
    struct TextureDesc
    {
        TexType  type;      //!< texture type
        uint32_t width;     //!< basemap width
        uint32_t height;    //!< basemap height
        uint32_t depth;     //!< basemap depth
        uint32_t faces;     //!< face count. When used as parameter of Renderer::createTexture(),
                            //!< you may set it to 0 to use default face count: 6 for cubemap, 1 for others.
        uint32_t levels;    //!< mipmap level count. When used as parameter of Renderer::createTexture(),
                            //!< you may set it to 0 to create full mipmap tower (down to 1x1).
        ClrFmt   format;    //!< pixel format. When used as parameter of Renderer::createTexture(),
                            //!< you may set it to FMT_DEFAULT. To use default texture format.
        BitField usage;     //!< usage
    };

    //!
    //! ��ͼ�����ķ��ؽ��
    //!
    struct TexLockedResult
    {
        size_t rowBytes;   //!< bytes per line
        size_t sliceBytes; //!< bytes per slice
        void * data;       //!< ָ������ͼ��ĵ�һ���ֽ�
    };

    //!
    //! ��ͼ����
    //!
    struct Texture : public RefCounter
    {
        //!
        //! Get texture descriptor
        //!
        const TextureDesc & getDesc() const { return mDesc; }

        //!
        //! get size of base map
        //!
        const Vector3<uint32_t> & getBaseSize() const { return *(const Vector3<uint32_t>*)&mDesc.width; }

        //!
        //! get size of base map
        //!
        template<typename T>
        void getBaseSize( T * sx, T * sy = 0, T * sz = 0 ) const
        {
            const Vector3<uint32_t> & baseSize = getBaseSize();
            if( sx ) *sx = (T)baseSize.x;
            if( sy ) *sy = (T)baseSize.y;
            if( sz ) *sz = (T)baseSize.z;
        }

        //!
        //! get size of specific mip level
        //!
        virtual Vector3<uint32_t> getMipSize( size_t level ) const = 0;

        //!
        //! get size of specific mip level
        //!
        template<typename T>
        void getMipSize( size_t level, T * sx, T * sy = 0, T * sz = 0 ) const
        {
            Vector3<uint32_t> mipSize = getMipSize( level );
            if( sx ) *sx = (T)mipSize.x;
            if( sy ) *sy = (T)mipSize.y;
            if( sz ) *sz = (T)mipSize.z;
        }

        //!
        //! set texture filters
        //!
        //! \note default is [linear, linear]
        //!
        virtual void setFilter( TexFilter min, TexFilter mag ) const = 0;

        //!
        //! set texture address mode
        //!
        //! \note default is TEXWRAP_REPEAT
        //!
        virtual void setWrap( TexWrap s,
                              TexWrap t = TEXWRAP_REPEAT,
                              TexWrap r = TEXWRAP_REPEAT ) const = 0;

        //!
        //! Set content loader
        //!
        void setLoader( const TextureLoader & loader ) { mLoader = loader; }

        //! \name lock/unlock functions
        //@{

        //!
        //! Lock specific level of the texture. Can be used to all kind of texture.
        //!
        //! \param result   return locking result
        //! \param face     Specify face you want to lock.
        //! \param level    Specify mipmap level you want to lock.
        //! \param area     Specify locking area in the mipmap. Null means whole level.
        //! \param flag     Locking flag. See LockFlag for details.
        //!
        //! \return         Return false, if locking failed.
        //!
        virtual bool lock(
            TexLockedResult & result,
            size_t face,
            size_t level,
            const Boxi * area,
            LockFlag flag ) = 0;

        //!
        //! lock a 1D texture.
        //!
        void * lock1D( size_t level, size_t offset, size_t length, LockFlag flag )
        {
            GN_ASSERT( TEXTYPE_1D == getDesc().type );
            TexLockedResult result;
            Boxi area;
            area.x = (int)offset;
            area.y = 0;
            area.z = 0;
            area.w = (int)length;
            area.h = 0;
            area.d = 0;
            if( !lock( result, 0, level, &area, flag ) ) return 0;
            return result.data;
        }

        //!
        //! unlock previous lock
        //!
        virtual void unlock() = 0;

        //@}

        //! \name misc. functions
        //@{

        //!
        //! update contents of sub-mipmap-levels based on level 0
        //!
        virtual void updateMipmap() = 0;

        //!
        //! Get low-level device handle of the texture. LPDIRECT3DBASETEXTURE9 for
        //! DirectX; name of texture object(GLuint) for OpenGL.
        //!
        virtual void * getAPIDependentData() const = 0;

        //@}

    protected :

        //!
        //! Set texture descriptor. Subclass must call this function to set 
        //! all texture properities to valid value.
        //!
        bool setDesc( const TextureDesc & desc )
        {
            // check type
            if( desc.type < 0 || desc.type >= NUM_TEXTYPES )
            {
                GN_ERROR( "invalid texture type!" );
                return false;
            }
            mDesc.type = desc.type;

            // initiate texture size
            mDesc.width =desc.width;
            switch( desc.type )
            {
                case TEXTYPE_1D :
                {
                    mDesc.height = 1;
                    mDesc.depth = 1;
                    break;
                }

                case TEXTYPE_2D :
                case TEXTYPE_CUBE :
                case TEXTYPE_STACK :
                {
                    mDesc.height = desc.height;
                    mDesc.depth = 1;
                    break;
                }

                case TEXTYPE_3D :
                {
                    mDesc.height = desc.height;
                    mDesc.depth = desc.depth;
                    break;
                }

                default : GN_UNEXPECTED();
            }

            // initialize face count
            if( TEXTYPE_CUBE == desc.type )
            {
                if( 0 != desc.faces && 6 != desc.faces )
                {
                    GN_WARN( "Cubemap must have 6 desc.faces." );
                }
                mDesc.faces = 6;
            }
            else if( TEXTYPE_STACK == desc.type )
            {
                mDesc.faces = 0 == desc.faces ? 1 : desc.faces;
            }
            else
            {
                if( 0 != desc.faces && 1 != desc.faces )
                {
                    GN_WARN( "Texture other then cube/stack texture can have only 1 face." );
                }
                mDesc.faces = 1;
            }

            // calculate maximum mipmap levels
            uint32_t nx = 0, ny = 0, nz = 0;
            uint32_t maxLevels;

            maxLevels = mDesc.width;
            while( maxLevels > 0 ) { maxLevels >>= 1; ++nx; }

            maxLevels = mDesc.height;
            while( maxLevels > 0 ) { maxLevels >>= 1; ++ny; }

            maxLevels = mDesc.depth;
            while( maxLevels > 0 ) { maxLevels >>= 1; ++nz; }

            maxLevels = max( max(nx, ny), nz );

            mDesc.levels = ( 0 == desc.levels ) ? maxLevels : min( maxLevels, desc.levels );

            // store format
            if( ( desc.format < 0 || desc.format >= NUM_CLRFMTS ) &&
                FMT_DEFAULT != desc.format )
            {
                GN_ERROR( "invalid texture format: %s", clrFmt2Str(desc.format) );
                return false;
            }
            mDesc.format = desc.format;

            // store usage flags
            mDesc.usage = desc.usage;

            // success
            return true;
        }

        //!
        //! Get content loader
        //!
        const TextureLoader & getLoader() const { return mLoader; }

    private :
        TextureDesc   mDesc;   //!< descriptor
        TextureLoader mLoader; //!< content loader
    };

    //!
    //! Texture manager class (singleton)
    //!
    typedef ResourceManager<Texture*,true> TextureDictionary;

    //! \name convert between texture tags and string
    //@{

    //
    //
    // -------------------------------------------------------------------------
    inline bool
    texType2Str( StrA & str, TexType textype )
    {
        static const char * sTable [] =
        { "1D", "2D", "3D", "CUBE", "STACK" };

        if( 0 <= textype && textype < NUM_TEXTYPES )
        {
            str = sTable[textype];
            return true;
        }
        else return false;
    }
    //
    inline const char *
    texType2Str( TexType textype )
    {
        static const char * sTable [] =
        { "1D", "2D", "3D", "CUBE", "STACK" };

        if( 0 <= textype && textype < NUM_TEXTYPES )
        {
            return sTable[textype];
        }
        else return "BAD_TEXTURE_TYPE";
    }
    //
    inline bool
    str2TexType( TexType & value, const char * name )
    {
        static const char * sTable[] =
        { "1D", "2D", "3D", "CUBE", "STACK" };

        if( name )
        {
            for( int i = 0; i < NUM_TEXTYPES; ++i )
            {
                if( 0 == ::strcmp(sTable[i],name) )
                {
                    value = static_cast<TexType>(i);
                    return true;
                }
            }
        }
        return false;
    }

    //
    //
    // -------------------------------------------------------------------------
    inline bool
    texFilter2Str( StrA & str, TexFilter filter )
    {
        static const char * sTable [] =
        {
            "NEAREST", "LINEAR",
            "N_MIP_N", "N_MIP_L",
            "L_MIP_N", "L_MIP_L",
        };

        if( 0 <= filter && filter < NUM_TEXFILTERS )
        {
            str = sTable[filter];
            return true;
        }
        else return false;
    }
    //
    inline const char *
    texFilter2Str( TexFilter filter )
    {
        static const char * sTable [] =
        {
            "NEAREST", "LINEAR",
            "N_MIP_N", "N_MIP_L",
            "L_MIP_N", "L_MIP_L",
        };

        if( 0 <= filter && filter < NUM_TEXFILTERS )
        {
            return sTable[filter];
        }
        else return "BAD_TEXTURE_FILTER";
    }
    //
    inline bool
    str2TexFilter( TexFilter & value, const char * name )
    {
        static const char * sTable [] =
        {
            "NEAREST", "LINEAR",
            "N_MIP_N", "N_MIP_L",
            "L_MIP_N", "L_MIP_L",
        };

        if( name )
        {
            for( size_t i = 0; i < NUM_TEXFILTERS; ++i )
            {
                if( 0 ==::strcmp(sTable[i],name) )
                {
                    value = static_cast<TexFilter>(i);
                    return true;
                }
            }
        }
        return false;
    }

    //
    //
    // -------------------------------------------------------------------------
    inline bool
    texWrap2Str( StrA & str, TexWrap texwrap )
    {
        static const char * sTable [] =
        { "REPEAT", "CLAMP", "CLAMP_TO_EDGE" };

        if( 0 <= texwrap && texwrap < NUM_TEXWRAPS )
        {
            str = sTable[texwrap];
            return true;
        }
        else return false;
    }
    //
    inline const char *
    texWrap2Str( TexWrap texwrap )
    {
        static const char * sTable [] =
        { "REPEAT", "CLAMP", "CLAMP_TO_EDGE" };

        if( 0 <= texwrap && texwrap < NUM_TEXWRAPS )
        {
            return sTable[texwrap];
        }
        else return "BAD_TEXTURE_WRAP";
    }
    //
    inline bool
    str2TexWrap( TexWrap & value, const char * name )
    {
        static const char * sTable [] =
        { "REPEAT", "CLAMP", "CLAMP_TO_EDGE" };

        if( name )
        {
            for( size_t i = 0; i < NUM_TEXWRAPS; ++i )
            {
                if( 0 ==::strcmp(sTable[i],name) )
                {
                    value = static_cast<TexWrap>(i);
                    return true;
                }
            }
        }
        return false;
    }

    //@}
}}

// *****************************************************************************
//                           End of texture.h
// *****************************************************************************
#endif // __GN_GFX_TEXTURE_H__
