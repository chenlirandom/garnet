#ifndef __GN_BASE_NATIVEFILEDEVICE_H__
#define __GN_BASE_NATIVEFILEDEVICE_H__
// *****************************************************************************
//! \file    NativeFileDevice.h
//! \brief   native file device header
//! \author  chenlee (2005.7.11)
// *****************************************************************************

namespace GN { namespace detail {

    //!
    //! Native file device class
    //!
    class NativeFileDevice : public FileDevice
    {
        // ********************************
        //! \name  ctor/dtor
        // ********************************

        //@{
    public:
        NativeFileDevice();
        virtual ~NativeFileDevice() {}
        //@}

        // ********************************
        //   public functions
        // ********************************
    public:

        GN::AutoRef<GN::File>
        openFile( const StrA & path, int mode ) const;

        bool isExist( const StrA & path ) const;

        bool isDir( const StrA & path ) const;

        void glob( std::vector<StrA> & result,
                   const StrA & dirName,
                   const StrA & pattern,
                   bool recursive,
                   bool useRegex ) const;

        StrA rel2abs( const StrA & relPath, const StrA & base ) const;

        // ********************************
        //   private variables
        // ********************************
    private:

        StrA mStartupDir;

        // ********************************
        //   private functions
        // ********************************
    private:
    };
}}

// *****************************************************************************
//                           End of NativeFileDevice.h
// *****************************************************************************
#endif // __GN_BASE_NATIVEFILEDEVICE_H__
