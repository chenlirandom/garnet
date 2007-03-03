#include "pch.h"
#include "oglTexture.h"
#include "oglRenderer.h"

GN::Logger * GN::gfx::OGLTexture::sLogger = GN::getLogger("GN.gfx.rndr.OGL");
static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.OGL");

// *****************************************************************************
// local var/types/functions
// *****************************************************************************

///
/// Automatically delete OGL textures while out of scope.
///
class AutoDeleteTexture
{
    GLuint mTex;

public:

    ///
    /// Ctor
    ///
    AutoDeleteTexture( GLuint tex ) : mTex(tex) {}

    ///
    /// Dtor
    ///
    ~AutoDeleteTexture()
    {
        if( mTex ) glDeleteTextures( 1, &mTex );
    }

    ///
    /// Dismiss the auto-deletetion
    ///
    void dismiss() { mTex = 0; }
};

///
/// map filter type to opengl constant
// -----------------------------------------------------------------------------
static GN_INLINE GLenum sTexFilter2OGL( GN::gfx::TexFilter f )
{
    switch(f)
    {
    case GN::gfx::TEXFILTER_NEAREST : return GL_NEAREST;
    case GN::gfx::TEXFILTER_LINEAR  : return GL_LINEAR;
    case GN::gfx::TEXFILTER_N_MIP_N : return GL_NEAREST_MIPMAP_NEAREST;
    case GN::gfx::TEXFILTER_N_MIP_L : return GL_NEAREST_MIPMAP_LINEAR;
    case GN::gfx::TEXFILTER_L_MIP_N : return GL_LINEAR_MIPMAP_NEAREST;
    case GN::gfx::TEXFILTER_L_MIP_L : return GL_LINEAR_MIPMAP_LINEAR;
    default : GN_ASSERT_EX( 0, "invaid filter type!" );
        return GL_LINEAR;
    }
}

///
/// map wrap mode to opengl constant
// -----------------------------------------------------------------------------
static GN_INLINE GLenum sTexWrap2OGL( GN::gfx::TexWrap w )
{
    if( GN::gfx::TEXWRAP_REPEAT == w ) return GL_REPEAT;
    else if( GN::gfx::TEXWRAP_CLAMP == w ) return GL_CLAMP;
    else if( GN::gfx::TEXWRAP_CLAMP_TO_EDGE == w )
    {
        if( GLEW_EXT_texture_edge_clamp )
            return GL_CLAMP_TO_EDGE_EXT;
        else if (GLEW_SGIS_texture_edge_clamp )
            return GL_CLAMP_TO_EDGE_SGIS;
        else
        {
            GN_ERROR(sLogger)( "do not support clamp to edge!" );
            return GL_CLAMP;
        }
    }
    else
    {
        GN_ASSERT_EX( 0, "invaid wrap type!" );
        return GL_REPEAT;
    }
}

///
/// convert garnet color format to OpenGL format
// -----------------------------------------------------------------------------
static GN_INLINE bool sColorFormat2OGL(
    GLint & gl_internalformat,
    GLuint & gl_format,
    GLuint & gl_type,
    bool & gl_compressed,
    GN::gfx::ClrFmt clrfmt )
{
    gl_compressed = false;
    switch( clrfmt )
    {
        case GN::gfx::FMT_RGBA_32_32_32_32_FLOAT:
            if( !GLEW_ARB_texture_float )
            {
                GN_WARN(sLogger)( "current hardware do not support floating point texture format!" );
                return false;
            }
            gl_internalformat = GL_RGBA32F_ARB;
            gl_format         = GL_RGBA;
            gl_type           = GL_FLOAT;
            return true;

        case GN::gfx::FMT_RG_32_32_FLOAT:
            if( !GLEW_ARB_texture_float )
            {
                GN_WARN(sLogger)( "current hardware do not support floating point texture format!" );
                return false;
            }
            gl_internalformat = GL_LUMINANCE_ALPHA32F_ARB;
            gl_format         = GL_RGBA;
            gl_type           = GL_FLOAT;
            return true;

        case GN::gfx::FMT_RGBA_16_16_16_16_FLOAT:
            if( !GLEW_ARB_texture_float )
            {
                GN_WARN(sLogger)( "current hardware do not support floating point texture format!" );
                return false;
            }
            gl_internalformat = GL_RGBA16F_ARB;
            gl_format         = GL_RGBA;
            gl_type           = GL_FLOAT;
            return true;

        case GN::gfx::FMT_RG_16_16_FLOAT:
            if( !GLEW_ARB_texture_float )
            {
                GN_WARN(sLogger)( "current hardware do not support floating point texture format!" );
                return false;
            }
            gl_internalformat = GL_LUMINANCE_ALPHA16F_ARB;
            gl_format         = GL_RGBA;
            gl_type           = GL_FLOAT;
            return true;
        
        case GN::gfx::FMT_RGBA_16_16_16_16_UNORM:
            gl_internalformat  = GL_RGBA16;
            gl_format          = GL_RGBA;
            gl_type            = GL_UNSIGNED_SHORT;
            return true;

        case GN::gfx::FMT_RGBX_16_16_16_16_UNORM:
            gl_internalformat  = GL_RGB16;
            gl_format          = GL_RGBA;
            gl_type            = GL_UNSIGNED_SHORT;
            return true;

        case GN::gfx::FMT_RGBA_8_8_8_8_UNORM :
            gl_internalformat  = GL_RGBA8;
            gl_format          = GL_RGBA;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_BGRA_8_8_8_8_UNORM :
            gl_internalformat  = GL_RGBA8;
            gl_format          = GL_BGRA_EXT;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_BGRX_8_8_8_8_UNORM :
            gl_internalformat  = GL_RGB8;
            gl_format          = GL_BGRA_EXT;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_RGB_8_8_8_UNORM :
            gl_internalformat  = GL_RGB8;
            gl_format          = GL_RGB;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_BGR_8_8_8_UNORM :
            gl_internalformat  = GL_RGB8;
            gl_format          = GL_BGR_EXT;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_BGRA_5_5_5_1_UNORM :
            gl_internalformat  = GL_RGB5_A1;
            gl_format          = GL_BGRA_EXT;
            gl_type            = GL_UNSIGNED_SHORT_5_5_5_1;
            return true;

        case GN::gfx::FMT_BGR_5_6_5_UNORM :
            gl_internalformat  = GL_RGB5;
            gl_format          = GL_BGR_EXT;
            gl_type            = GL_UNSIGNED_SHORT_5_6_5_REV;
            return true;

        case GN::gfx::FMT_RG_8_8_SNORM :
            if( GLEW_ATI_envmap_bumpmap )
            {
                gl_internalformat  = GL_DU8DV8_ATI;
                gl_format          = GL_DUDV_ATI;
                gl_type            = GL_BYTE;
            }
            else
            {
                GN_WARN(sLogger)( "current hardware do not support UV texture format!" );
                gl_internalformat  = 2;
                gl_format          = GL_RGBA;
                gl_type            = GL_BYTE;
            }
            return true;

        case GN::gfx::FMT_LA_16_16_UNORM :
            gl_internalformat  = GL_LUMINANCE16_ALPHA16;
            gl_format          = GL_LUMINANCE_ALPHA;
            gl_type            = GL_UNSIGNED_SHORT;
            return true;

        case GN::gfx::FMT_LA_8_8_UNORM :
            gl_internalformat  = GL_LUMINANCE8_ALPHA8;
            gl_format          = GL_LUMINANCE_ALPHA;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_L_16_UNORM :
            gl_internalformat  = GL_LUMINANCE16;
            gl_format          = GL_LUMINANCE;
            gl_type            = GL_UNSIGNED_SHORT;
            return true;

        case GN::gfx::FMT_L_8_UNORM :
            gl_internalformat  = GL_LUMINANCE8;
            gl_format          = GL_LUMINANCE;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_A_8_UNORM :
            gl_internalformat  = GL_ALPHA8;
            gl_format          = GL_ALPHA;
            gl_type            = GL_UNSIGNED_BYTE;
            return true;

        case GN::gfx::FMT_D_16 :
            if( !GLEW_ARB_depth_texture )
            {
                GN_ERROR(sLogger)( "does not support GL_ARB_depth_texture." );
                return false;
            }
            gl_internalformat  = GL_DEPTH_COMPONENT;
            gl_format          = GL_DEPTH_COMPONENT;
            gl_type            = GL_UNSIGNED_SHORT;
            return true;

        case GN::gfx::FMT_D_24 :
            if( !GLEW_ARB_depth_texture )
            {
                GN_ERROR(sLogger)( "does not support GL_ARB_depth_texture." );
                return false;
            }
            gl_internalformat  = GL_DEPTH_COMPONENT;
            gl_format          = GL_DEPTH_COMPONENT;
            gl_type            = GL_UNSIGNED_INT;
            return true;

        case GN::gfx::FMT_D_32 :
            if( !GLEW_ARB_depth_texture )
            {
                GN_ERROR(sLogger)( "does not support GL_ARB_depth_texture." );
                return false;
            }
            gl_internalformat  = GL_DEPTH_COMPONENT;
            gl_format          = GL_DEPTH_COMPONENT;
            gl_type            = GL_UNSIGNED_INT;
            return true;

        case GN::gfx::FMT_DXT1 :
            if( GLEW_ARB_texture_compression &&
                GLEW_EXT_texture_compression_s3tc )
            {
                gl_internalformat  = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                gl_format          = GL_RGBA;
                gl_type            = GL_UNSIGNED_BYTE;
                gl_compressed      = true;
                return true;
            }
            break;

        case GN::gfx::FMT_DXT3 :
            if( GLEW_ARB_texture_compression &&
                GLEW_EXT_texture_compression_s3tc )
            {
                gl_internalformat  = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                gl_format          = GL_RGBA;
                gl_type            = GL_UNSIGNED_BYTE;
                gl_compressed      = true;
                return true;
            }
            break;

        case GN::gfx::FMT_DXT5 :
            if( GLEW_ARB_texture_compression &&
                GLEW_EXT_texture_compression_s3tc )
            {
                gl_internalformat  = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                gl_format          = GL_RGBA;
                gl_type            = GL_UNSIGNED_BYTE;
                gl_compressed      = true;
                return true;
            }
            break;

        default : break;
    }

    // failed
    GN_ERROR(sLogger)( "invalid or unsupported format '%s'!", GN::gfx::clrFmt2Str(clrfmt) );
    return false;
}

///
/// generate 2D mipmaps
// ------------------------------------------------------------------------
static bool sGen2DMipmap( GLenum target,
                          GLsizei width, GLsizei height,
                          GLint levels,
                          GLint internalformat,
                          GLenum format, GLenum type )
{
    GN_GUARD;

    GN_ASSERT( width > 0 && height > 0 );

    for( GLint i = 0; i < levels; ++i )
    {
        GN_OGL_CHECK_RV(
            glTexImage2D( target, i, internalformat, width, height, 0, format, type, 0 ),
            false );
        if( width > 1 ) width >>= 1;
        if( height > 1 ) height >>= 1;
    }

    // success
    return true;

    GN_UNGUARD;
}

// *****************************************************************************
// OGL texture creation routines
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GLuint sNew2DTexture(
    GLint   internalformat,
    GLsizei size_x,
    GLsizei size_y,
    GLint   levels,
    GLenum  format,
    GLenum  type )
{
    GN_GUARD;

    // generate new texture
    GLuint result;
    GN_OGL_CHECK_RV( glGenTextures(1, &result), 0 );
    AutoDeleteTexture autoDel( result );

    GN_OGL_CHECK( glBindTexture( GL_TEXTURE_2D, result ) );
    if( !sGen2DMipmap(
            GL_TEXTURE_2D, size_x, size_y, levels,
            internalformat, format, type ) )
        return 0;

    // success
    autoDel.dismiss();
    return result;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static GLuint sNew3DTexture(
    GLint   /*internalformat*/,
    GLsizei /*size_x*/,
    GLsizei /*size_y*/,
    GLsizei /*size_z*/,
    GLint   /*levels*/,
    GLenum  /*format*/,
    GLenum  /*type*/ )
{
    GN_GUARD;

    GN_ERROR(sLogger)( "no implementation" );
    return 0;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static GLuint sNewCubeTexture(
    GLint   internalformat,
    GLsizei size_x,
    GLint   levels,
    GLenum  format,
    GLenum  type )
{
    GN_GUARD;

    // generate new texture
    GLuint result;
    GN_OGL_CHECK_RV( glGenTextures(1, &result), 0 );
    AutoDeleteTexture autoDel( result );

    if( !GLEW_ARB_texture_cube_map )
    {
        GN_ERROR(sLogger)( "do not support cube map!" );
        return 0;
    }

    GN_OGL_CHECK( glBindTexture( GL_TEXTURE_CUBE_MAP_ARB, result ) );
    for( GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
         face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
         ++face )
    {
        if( !sGen2DMipmap(
                face, size_x, size_x, levels,
                internalformat, format, type ) )
            return 0;
    }

    // success
    autoDel.dismiss();
    return result;

    GN_UNGUARD;
}

// *****************************************************************************
// OGLTexture implementation
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLTexture::init( TextureDesc desc )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( OGLTexture, () );

    OGLAutoAttribStack autoAttribStack; // auto-restore OGL states

    // determine pixelformat
    if( FMT_DEFAULT == desc.format )
    {
        if( TEXUSAGE_DEPTH == desc.usage )
        {
            desc.format = FMT_D_32; // default depth format
        }
        else
        {
            desc.format = FMT_BGRA_8_8_8_8_UNORM; // default color format
        }
    }

    // store texture properties
    if( !setDesc( desc ) ) return failure();

    // determine gl texture type
    switch( getDesc().dim )
    {
        case TEXDIM_1D   :
        case TEXDIM_2D   :
            mOGLTarget = GL_TEXTURE_2D;
            break;
        case TEXDIM_3D   :
            if ( !GLEW_EXT_texture3D )
            {
                GN_ERROR(sLogger)( "do not support 3D texture!" );
                return failure();
            }
            mOGLTarget = GL_TEXTURE_3D;
            break;
        case TEXDIM_CUBE :
            if ( !GLEW_ARB_texture_cube_map )
            {
                GN_ERROR(sLogger)( "do not support CUBE texture!" );
                return failure();
            }
            mOGLTarget = GL_TEXTURE_CUBE_MAP_ARB;
            break;
        default :
            GN_ERROR(sLogger)( "invalid texture type!" );
            return failure();
    }

    // convert format to opengl paramaters
    if( !sColorFormat2OGL( mOGLInternalFormat,
                           mOGLFormat,
                           mOGLType,
                           mOGLCompressed,
                           getDesc().format ) )
        return failure();

    // create new opengl texture object
    const TextureDesc & desc = getDesc();
    switch( getDesc().dim )
    {
        case TEXDIM_1D :
        case TEXDIM_2D :
            mOGLTexture = sNew2DTexture(
                mOGLInternalFormat, desc.width, desc.height, desc.levels,
                mOGLFormat, mOGLType );
            break;

        case TEXDIM_3D :
            mOGLTexture = sNew3DTexture(
                mOGLInternalFormat, desc.width, desc.height, desc.depth, desc.levels,
                mOGLFormat, mOGLType );
            break;

        case TEXDIM_CUBE :
            mOGLTexture = sNewCubeTexture(
                mOGLInternalFormat, desc.width, desc.levels,
                mOGLFormat, mOGLType );
            break;

        case TEXDIM_STACK :
            GN_ERROR(sLogger)( "OpenGL does not support STACK texture." );
            mOGLTexture = 0;
            break;

        default:
            GN_UNEXPECTED();
            mOGLTexture = 0;
    }
    if( 0 == mOGLTexture ) return failure();

    // enable/disable mipmap autogeneration
    if( TEXDIM_CUBE != getDesc().dim && GLEW_SGIS_generate_mipmap )
    {
        if( TEXUSAGE_AUTOGEN_MIPMAP & desc.usage )
        {
            GN_OGL_CHECK( glTexParameteri( mOGLTarget,GL_GENERATE_MIPMAP_SGIS, GL_TRUE) );
        }
        else
        {
            GN_OGL_CHECK( glTexParameteri( mOGLTarget,GL_GENERATE_MIPMAP_SGIS, GL_FALSE) );
        }
    }

    // setup mip size array
    for( size_t i = 0; i < getDesc().levels; ++i )
    {
        GLint sx, sy, sz;
        switch( getDesc().dim )
        {
            case TEXDIM_1D :
            case TEXDIM_2D :
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_2D, (GLint)i, GL_TEXTURE_WIDTH, &sx ) );
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_2D, (GLint)i, GL_TEXTURE_HEIGHT, &sy ) );
                sz = 1;
                break;

            case TEXDIM_3D :
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_3D_EXT, (GLint)i, GL_TEXTURE_WIDTH, &sx ) );
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_3D_EXT, (GLint)i, GL_TEXTURE_HEIGHT, &sy ) );
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_3D_EXT, (GLint)i, GL_TEXTURE_DEPTH_EXT, &sz ) );
                break;

            case TEXDIM_CUBE :
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, (GLint)i, GL_TEXTURE_WIDTH, &sx ) );
                GN_OGL_CHECK( glGetTexLevelParameteriv(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, (GLint)i, GL_TEXTURE_HEIGHT, &sy ) );
                sz = 1;
                break;

            default:
                GN_UNEXPECTED();
                return failure();
        }

        setMipSize( i, sx, sy, sz );
    }

    // setup default filters and wrap modes
    GN_OGL_CHECK( glTexParameteri( mOGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
    GN_OGL_CHECK( glTexParameteri( mOGLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
    GN_OGL_CHECK( glTexParameteri( mOGLTarget, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
    GN_OGL_CHECK( glTexParameteri( mOGLTarget, GL_TEXTURE_WRAP_T, GL_REPEAT ) );
    if( GLEW_EXT_texture3D )
    {
        GN_OGL_CHECK( glTexParameteri( mOGLTarget, GL_TEXTURE_WRAP_R, GL_REPEAT ) );
    }

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLTexture::quit()
{
    GN_GUARD;

    // check if locked
    if( isLocked() )
    {
        GN_WARN(sLogger)( "call You are destroying a locked texture!" );
        unlock();
    }

    // delete opengl texture
    if (mOGLTexture) glDeleteTextures( 1, &mOGLTexture ), mOGLTexture = 0;


    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLTexture::setFilter( TexFilter min, TexFilter mag ) const
{
    GLenum glmin = sTexFilter2OGL( min );
    GLenum glmag = sTexFilter2OGL( mag );
    if( mOGLFilters[0] != glmin )
    {
        mOGLFilters[0] = glmin;
        mFilterAndWrapDirty = true;
    }

    if( mOGLFilters[1] != glmag )
    {
        mOGLFilters[1] = glmag;
        mFilterAndWrapDirty = true;
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLTexture::setWrap( TexWrap s, TexWrap t, TexWrap r ) const
{
    GN_GUARD_SLOW;

    GLenum gls = sTexWrap2OGL( s );
    GLenum glt = sTexWrap2OGL( t );
    GLenum glr = sTexWrap2OGL( r );
    
    if( mOGLWraps[0] != gls )
    {
        mOGLWraps[0] = gls;
        mFilterAndWrapDirty = true;
    }

    if( mOGLWraps[1] != glt )
    {
        mOGLWraps[1] = glt;
        mFilterAndWrapDirty = true;
    }

    if( TEXDIM_3D == getDesc().dim && mOGLWraps[2] != glr )
    {
        mOGLWraps[2] = glr;
        mFilterAndWrapDirty = true;
    }

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::OGLTexture::lock(
    TexLockedResult & result,
    size_t face,
    size_t level,
    const Boxi * area,
    LockFlag flag )
{
    GN_GUARD_SLOW;

    // call basic lock
    if( !basicLock( face, level, area, flag, mLockedArea ) ) return false;
    AutoScope< Delegate0<bool> > basicUnlocker( makeDelegate(this,&OGLTexture::basicUnlock) );

    // ����pitch
    if( mOGLCompressed )
    {
        switch ( getDesc().format )
        {
            case FMT_DXT1:
                result.rowBytes = ((mLockedArea.w + 3) >> 2) * 8;
                result.sliceBytes = result.rowBytes * ((mLockedArea.h + 3) >> 2);
                mLockedBytes = result.sliceBytes * mLockedArea.d;
                break;

		    case FMT_DXT3:
		    case FMT_DXT5:
                result.rowBytes = ((mLockedArea.w + 3) >> 2) * 16;
                result.sliceBytes = result.rowBytes * ((mLockedArea.h + 3) >> 2);
                mLockedBytes = result.sliceBytes * mLockedArea.d;
                break;

            default:
                GN_ERROR(sLogger)( "unsupport compress format '%s'!", clrFmt2Str(getDesc().format) );
                return false;
        }
    }
    else
    {
        GLint alignment;
        GN_OGL_CHECK( glGetIntegerv( GL_PACK_ALIGNMENT, &alignment ) );
        GN_ASSERT( isPowerOf2(alignment) ); // alignment�ض���2^n
        size_t bpp = getClrFmtDesc(getDesc().format).bits / 8;
        // �����ֵ����alignment�Ĵ�С����
#define _GN_ALIGN(X,A) X = ( (X & -A) + (X & (A - 1) ? A : 0) )
        _GN_ALIGN(result.rowBytes,alignment);
#undef _GN_ALIGN
        result.rowBytes = mLockedArea.w * bpp;
        result.sliceBytes = result.rowBytes * mLockedArea.h;
        mLockedBytes = result.sliceBytes * mLockedArea.d;
    }

    // ���仺����
    mLockedBuffer = new UInt8[mLockedBytes];
    GN_ASSERT( mLockedBuffer );

    // �������ֻд���������ȡ��ǰ����ͼ���ݵ���������
    if( LOCK_RO == flag || LOCK_RW == flag )
    {
        GN_WARN(sLogger)( "Ŀǰ��֧�ִ���ͼ�ж�ȡ����!" );
    }

    // success
    mLockedTarget  = TEXDIM_CUBE == getDesc().dim ? OGLTexture::sCubeface2OGL(face) : mOGLTarget;
    mLockedLevel   = level;
    mLockedFlag    = flag;
    result.data    = mLockedBuffer;
    basicUnlocker.dismiss();
    return true;

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::OGLTexture::unlock()
{
    GN_GUARD_SLOW;

    // call basic unlock
    if( !basicUnlock() ) return;

    // do nothing for read-only lock
    if( LOCK_RO == mLockedFlag ) return;

    // Auto-restore texture binding when exiting this function.
    OGLAutoAttribStack autoAttribStack( GL_TEXTURE_BIT  );

    // bind myself as current texture
    bind();

    GN_ASSERT( mLockedBuffer );

    if( TEXDIM_3D == getDesc().dim )
    {
        GN_UNIMPL_WARNING();
    }
    else
    {
        // ���������е�����д����ͼ
        if( mOGLCompressed )
        {
            if( GLEW_ARB_texture_compression )
            {
                GN_OGL_CHECK( glCompressedTexSubImage2DARB(
                    mLockedTarget, (GLint)mLockedLevel,
                    mLockedArea.x, mLockedArea.y,
                    mLockedArea.w, mLockedArea.h,
                    mOGLInternalFormat,
                    (GLsizei)mLockedBytes, mLockedBuffer ) );
            }
            else
            {
                GN_WARN(sLogger)( "do not support texture compression!" );
            }
        }
        else
        {
            GN_OGL_CHECK( glTexSubImage2D(
                mLockedTarget, (GLint)mLockedLevel,
                mLockedArea.x, mLockedArea.y,
                mLockedArea.w, mLockedArea.h,
                mOGLFormat, mOGLType, mLockedBuffer ) );
        }
    }

    // release mLockedBuffer
    delete [] mLockedBuffer;
    mLockedBuffer = 0;

    GN_UNGUARD_SLOW;
}
