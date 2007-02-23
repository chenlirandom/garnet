#ifndef __GN_GFX_BASICBUFFER_H__
#define __GN_GFX_BASICBUFFER_H__
// *****************************************************************************
/// \file    common/basicBuffer.h
/// \brief   Basic vertex/index buffer class
/// \author  chenlee (2006.3.8)
// *****************************************************************************

namespace GN { namespace gfx
{
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
        BasicVtxBuf() : mLocked( false ) {}
        //@}

        // ********************************
        //  public functions
        // ********************************
    public:

        ///
        /// texture is being locked or not.
        ///
        bool isLocked() const { return mLocked; }

        // ********************************
        //  protected functions
        // ********************************
    protected:

        ///
        /// Basic lock operation. Each lock function must call this function first,
        /// to ensure the lock operation is valid.
        ///
        /// This function will also clip lock bytes into valid range.
        ///
        bool basicLock( size_t offset, size_t & bytes, LockFlag flag )
        {
            if( isLocked() )
            {
                GN_ERROR(sLogger)( "dupilcate lock! lock/unlock() must be called accordinglly!" );
                return false;
            }

            const VtxBufDesc & desc = getDesc();

            if( offset >= desc.bytes )
            {
                GN_ERROR(sLogger)( "offset is beyond the end of vertex buffer!" );
                return false;
            }

            if( flag >= NUM_LOCK_FLAGS )
            {
                GN_ERROR(sLogger)( "invalid lock flag: %d!", flag );
                return false;
            }

            // adjust offset and bytes
            if( 0 == bytes ) bytes = desc.bytes;
            if( offset + bytes > desc.bytes ) bytes = desc.bytes - offset;

            // success            
            mLocked = true;
            return true;
        }

        ///
        /// Basic unlock operation. Should be called in pair with baseLock().
        ///
        bool basicUnlock()
        {
            if( isLocked() )
            {
                mLocked = false;
                return true;
            }
            else
            {
                GN_ERROR(sLogger)( "dupilcate unlock! lock/unlock() must be called accordinglly!" );
                return false;
            }
        }

        // ********************************
        //  private variables
        // ********************************
    private:

        ///
        /// ������־
        ///
        bool mLocked;

        static Logger * sLogger;

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
        BasicIdxBuf() : mLocked( false ) {}
        //@}

        // ********************************
        //  public functions
        // ********************************
    public:

        ///
        /// texture is being locked or not.
        ///
        bool isLocked() const { return mLocked; }

        // ********************************
        //  protected functions
        // ********************************
    protected:

        ///
        /// Basic lock operation. Each lock function must call this function first,
        /// to ensure the lock operation is valid.
        ///
        /// This function will also clip numiIdx into valid range.
        ///
        bool basicLock( size_t startIdx, size_t & numIdx, LockFlag flag )
        {
            if( isLocked() )
            {
                GN_ERROR(sLogger)( "dupilcate lock! lock/unlock() must be called accordinglly!" );
                return false;
            }

            const IdxBufDesc & desc = getDesc();

            if( startIdx >= desc.numidx )
            {
                GN_ERROR(sLogger)( "startIdx is beyond the end of index buffer!" );
                return false;
            }

            if( flag >= NUM_LOCK_FLAGS )
            {
                GN_ERROR(sLogger)( "invalid lock flag: %d!", flag );
                return false;
            }

            // adjust startIdx and numIdx
            if( 0 == numIdx ) numIdx = desc.numidx;
            if( startIdx + numIdx > desc.numidx ) numIdx = desc.numidx - startIdx;

            // success            
            mLocked = true;
            return true;
        }

        ///
        /// Basic unlock operation. Should be called in pair with baseLock().
        ///
        bool basicUnlock()
        {
            if( isLocked() )
            {
                mLocked = false;
                return true;
            }
            else
            {
                GN_ERROR(sLogger)( "dupilcate unlock! lock/unlock() must be called accordinglly!" );
                return false;
            }
        }

        // ********************************
        //  private variables
        // ********************************
    private:

        ///
        /// ������־
        ///
        bool mLocked;

        static Logger * sLogger;

        // ********************************
        //  private functions
        // ********************************
    private:
    };
}}

// *****************************************************************************
//                           End of basicBuffer.h
// *****************************************************************************
#endif // __GN_GFX_BASICBUFFER_H__
