#ifndef __GN_GFXCOMMON_BASICTEXTURE_H__
#define __GN_GFXCOMMON_BASICTEXTURE_H__
// *****************************************************************************
//! \file    basicTexture.h
//! \brief   ��ͼ����
//! \author  chenlee (2005.10.1)
// *****************************************************************************

namespace GN { namespace gfx
{
    //!
    //! ��ͼ��Ļ���
    //!
    class BasicTexture : public Texture
    {
        // ********************************
        //! \name constructor
        // ********************************

        //@{
    protected :
        BasicTexture() : mLocked( false ) {}
        //@}

        // ********************************
        //  public functions
        // ********************************
    public:

        //!
        //! texture is being locked or not.
        //!
        bool isLocked() const { return mLocked; }

        // ********************************
        //  protected functions
        // ********************************
    protected:

        //!
        //! Basic lock operation. Each lock function must call this function first,
        //! to ensure the lock operation is valid.
        //!
        //! This function will also clip lock area to ensure it is not out of texture size.
        //!
        bool basicLock( size_t face, size_t level, const Boxi * area, Boxi & clippedArea )
        {
            if( isLocked() )
            {
                GN_ERROR( "dupilcate lock! lock/unlock() must be called accordinglly!" );
                return false;
            }

            // check face
            if( face >= getFaces() )
            {
                GN_ERROR( "invalid lock face : %d", face );
                return false;
            }

            // check level
            if( level >= getLevels() )
            {
                GN_ERROR( "invalid lock level : %d", level );
                return false;
            }

            // get texture size
            size_t sx, sy, sz;
            getMipSize( level, &sx, &sy, &sz );

            // make sure lock area is valid
            if( area )
            {
                clippedArea = *area;
                if( !sAdjustOffsetAndRange( clippedArea.x, clippedArea.w, (int)sx ) ||
                    !sAdjustOffsetAndRange( clippedArea.y, clippedArea.h, (int)sy ) ||
                    !sAdjustOffsetAndRange( clippedArea.z, clippedArea.d, (int)sz ) )
                    return false;
            }
            else
            {
                clippedArea.x = 0;
                clippedArea.y = 0;
                clippedArea.z = 0;
                clippedArea.w = (int)sx;
                clippedArea.h = (int)sy;
                clippedArea.d = (int)sz;
            }
            
            // success            
            mLocked = 1;
            return true;
        }

        //!
        //! Basic unlock operation. Should be called in pair with baseLock().
        //!
        bool basicUnlock()
        {
            if( isLocked() )
            {
                mLocked = 0;
                return true;
            }
            else
            {
                GN_ERROR( "dupilcate unlock! lock/unlock() must be called accordinglly!" );
                return false;
            }
        }

        // ********************************
        //  private variables
        // ********************************
    private:

        //!
        //! ��ͼ��������־
        //!
        bool mLocked;

        // ********************************
        //  private functions
        // ********************************
    private:

        static inline bool
        sAdjustOffsetAndRange( int & offset, int & length, int maxLength )
        {
            if( offset >= maxLength )
            {
                GN_ERROR( "offset is beyond the end of valid range." );
                return false;
            }
            if( 0 == length ) length = maxLength;
            if( offset + length > maxLength ) length = maxLength - offset;
            return true;
        }
    };
}}

// *****************************************************************************
//                           End of basicTexture.h
// *****************************************************************************
#endif // __GN_GFXCOMMON_BASICTEXTURE_H__
