#ifndef __GN_BASE_FILE_H__
#define __GN_BASE_FILE_H__
// *****************************************************************************
//! \file    file.h
//! \brief   general file stream class
//! \author  chenlee (2005.4.20)
// *****************************************************************************

#include <vector>
#include <map>

namespace GN
{
    //!
    //! �ļ���ģʽ
    //!
    //! ��Ч��ģʽ������һ����д��־����һ����ʽ��־
    //!
    enum FileOpenMode
    {
        FOPEN_NONE  = 0x00, //!< indicate a invalid value

        // ��д��־
        FOPEN_READ  = 0x01, //!< ֻ��ģʽ���ļ�������ڣ���same as "r"
        FOPEN_WRITE = 0x02, //!< ֻдģʽ��same as "w"
        FOPEN_RW    = 0x03, //!< ��дģʽ���ļ�������ڣ���same as "r+"

        // ��ʽ��־
        FOPEN_BIN   = 0x10, //!< ������ģʽ
        FOPEN_TXT   = 0x20, //!< �ı�ģʽ
    };

    //!
    //! �ļ���λģʽ
    //!
    enum FileSeekMode
    {
        FSEEK_CUR,     //!< same as standard SEEK_CUR
        FSEEK_END,     //!< same as standard SEEK_END
        FSEEK_SET,     //!< same as standard SEEK_SET
        NUM_FSEEKS,    //!< number of avaliable seeking modes
    };

    //!
    //! basic file interface used throughout of the garnet system
    //!
    //! �û�ʵ�ָ��ļ���ʱ����һ��Ҫʵ����������в�����
    struct File : public RefCounter
    {
        //!
        //! ��ȡsize���ֽڵ�buffer�У�����ʵ�ʶ�ȡ���ֽ���,
        //!
        //! \return   -1 means failed.
        //!
        virtual size_t read( void * /*buffer*/, size_t /*size*/ ) { return size_t(-1); }

        //!
        //! ���ļ���д��size���ֽڣ�����ʵ��д�뵽�ֽ���
        //!
        //! \return   -1 means failed
        //!
        virtual size_t write( const void * /*buffer*/, size_t /*size*/ ) { return size_t(-1); }

        //!
        //! �Ƿ��Ѿ����ļ���β
        //!
        virtual bool   eof() const { return false; }

        //!
        //! �趨�ļ���д�α��λ��
        //!
        //! \return   return false if error
        //!
        virtual bool   seek( int /*offset*/, FileSeekMode /*origin*/ ) { return false; }

        //!
        //! ���ص�ǰ�ļ���д�α��λ��
        //!
        //! \return   On error, return -1L
        virtual size_t tell() const { return size_t(-1); }

        //!
        //! �����ļ����ܳ���. If error, return -1L.
        //!
        virtual size_t size() const { return size_t(-1); }

        //!
        //! return file name string
        //!
        const StrA & name() const { return mName; }

    protected :

        // ctor / dtor
        File()          {}
        virtual ~File() {}

        //!
        //! Set file name
        //!
        void setName( const StrA & name ) { mName = name; }

    private:

        StrA mName;
    };

    //!
    //! ��File��װ��standard file stream
    //!
    class StdFile : public File
    {
        FILE * mFile;
    public :

        //!
        //! constructor
        //!
        StdFile( FILE * );

        // from File
    public:
        size_t read( void *, size_t );
        size_t write( const void * buffer, size_t size );
    };

    //!
    //! file class using ANSI file functions
    //!
    class AnsiFile : public File
    {
        FILE * mFile;
        size_t mSize;
    public:

        AnsiFile() : mFile(0), mSize(0) {}
        ~AnsiFile() { close(); }

        //!
        //! open a file
        //!
        //! \param fname File name
        //! \param flags File open flags (ANSI compatible)
        bool open( const StrA & fname, int32_t flags );

        //!
        //! close the file
        //!
        void close() throw();

        // from File
    public:
        size_t read( void * buffer, size_t size );
        size_t write( const void * buffer, size_t size );
        bool   eof() const;
        bool   seek( int offset, FileSeekMode );
        size_t tell() const;
        size_t size() const { return mSize; }
    };

    //!
    //! file class that wraps a memory buffer
    //!
    template< typename T >
    class MemFile : public File
    {
        uint8_t * mStart;
        uint8_t * mPtr;
        size_t    mSize;

    public:

        //! \name ctor/dtor
        //@{
        MemFile( T * buf = 0, size_t size = 0, const StrA & name = "" )
            : mStart((uint8_t*)buf)
            , mPtr((uint8_t*)buf)
            , mSize(size)
        { setName(name); }
        ~MemFile() {}
        //@}

        //!
        //! reset memory buf
        //!
        void reset( T * buf = 0, size_t size = 0, const StrA & name = "" );

        //! \name from File
        //@{
        size_t read( void * buf, size_t size );
        size_t write( const void * buf, size_t size );
        bool   eof() const { return (mStart+mSize) == mPtr; }
        bool   seek( int offset, FileSeekMode origin );
        size_t tell() const { return mPtr - mStart; }
        size_t size() const { return mSize; }
        //@}
    };

    //!
    //! File device
    //!
    struct FileDevice : public RefCounter
    {
        //!
        //! ���ļ�
        //!
        //! \param path  File path (no device name)
        //! \param mode  One or combination of OpenMode
        //!
        virtual AutoRef<File>
        openFile( const StrA & path, int mode ) const = 0;

        //!
        //! �ļ��Ƿ����
        //!
        //! \param path  File path (no device name)
        //!
        virtual bool isExist( const StrA & path ) const = 0;

        //!
        //! if the path points to a directoy?
        //!
        virtual bool isDir( const StrA & path ) const = 0;

        //!
        //! ����ָ���ļ�
        //!
        //! \param result     �����������
        //! \param dirName    ��ʲôĿ¼��
        //! \param pattern    ����ʲô�ļ�
        //! \param recursive  �Ƿ�ݹ�������Ŀ¼
        //! \param useRegex   �Ƿ�ʹ������ƥ��
        //!
        virtual void
        findFiles( std::vector<StrA> & result,
                   const StrA & dirName,
                   const StrA & pattern,
                   bool         recursive,
                   bool         useRegex ) const = 0;

        //!
        //! convert relative path to absolute path
        //!
        //! \return Empty string, if failed
        //!
        virtual StrA rel2abs( const StrA & ) const = 0;
    };
    
    //!
    //! General file system, which contains multiple file devices.
    //!
    //! - Full path of file includes: [device][root][relpath]
    //! - We always use "/" as path separator.
    //! - Device name must be end with "::". Default device is
    //!   "native::", if omitted.
    //! - "root" could be : "c:/", "d:", "/" and etc...
    //!
    class FileSys : public StdClass
    {
         GN_DECLARE_STDCLASS( FileSys, StdClass );

        // ********************************
        //! name  ctor/dtor
        // ********************************

        //@{
    public:
        FileSys()          { clear(); }
        virtual ~FileSys() { quit(); }
        //@}

        // ********************************
        //! name standard init/quit
        // ********************************

        //@{
    public:
        bool init();
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() {}
        //@}

    public:

        // ********************************
        //! \name file device management
        // ********************************

        //@{

        //!
        //! register a file device
        //!
        //! \param deviceName     Device name
        //! \param device         Pointer to device instance, can't be NULL
        //!                       Reference counter of the device will be increased.
        //! \param override       Override the existing file device?
        //!
        bool registerDevice( const StrA & deviceName,
                             const FileDevice * device,
                             bool override = false );

        //!
        //! unregister a file device
        //!
        void unregDevice( const StrA & deviceName );

        //@}

        // ********************************
        //! \name file operations
        // ********************************

        //! This group of functions will do nothing more then forwarding
        //! the file operations to appropriate file device.
        //@{

        //!
        //! ���ļ�
        //!
        //! \param path  File path (w/ or w/o device name)
        //! \param mode  one or combination of FileOpenMode
        //!
        AutoRef<File>
        openFile( const StrA & path, int mode ) const;

        //!
        //! �ļ���Ŀ¼�Ƿ����
        //!
        bool isExist( const StrA & ) const;

        //!
        //! �Ƿ�����Ч��Ŀ¼��
        //!
        bool isDir( const StrA & ) const;

        //!
        //! ����ָ���ļ�
        //!
        //! \param result     �����������
        //! \param dirName    ��ʲôĿ¼��, should be a valid directory locator
        //! \param pattern    ����ʲô�ļ�, in format of regualr expression
        //! \param recursive  �Ƿ�ݹ�������Ŀ¼
        //! \param useRegex   �Ƿ�ʹ������ƥ��
        //!
        void findFiles( std::vector<StrA> & result,
                        const StrA & dirName,
                        const StrA & pattern,
                        bool         recursive,
                        bool         useRegex ) const;

        //@}

        // ********************************
        //! \name garnet file locator operations
        // ********************************

        //@{

        //!
        //! Normalize glf into consistent format
        //!
        StrA normalizePath( const StrA & ) const;

        //!
        //! Covert relative path to absolute path
        //!
        //! \param relPath
        //!     The relative path that'll be converted.
        //!
        //! \param base
        //!     If empty, means current (working) directory.
        //!
        //! \return
        //!     Return empty string, if failed.
        //!
        //! \note Resolving rules:
        //! <pre>
        //! if( path.device() == base.device() )
        //! {
        //!     if( base.empty() )
        //!     {
        //!         resolve path using FileDevice::rel2abs();
        //!     }
        //!     else
        //!     {
        //!         compose full path in the same way as boost::complete()
        //!     }
        //! }
        //! else
        //! {
        //!   failed();
        //! }
        //! </pre>
        //!
        StrA rel2abs( const StrA & relPath,
                      const StrA & base = "" ) const;

        //!
        //! Get the parent path (directory) of the path
        //!
        //! \return Empty string, if failed
        //!
        StrA getParent( const StrA & ) const;

        //!
        //! Get device name of the path
        //!
        //! \return Empty string, if failed
        //!
        StrA getDevice( const StrA & ) const;

        //@}

        // ********************************
        //   private variables
        // ********************************
    private:

        typedef std::map<StrA, AutoRef<const FileDevice> > DeviceMap;

        DeviceMap mDevices;

        // ********************************
        //   private functions
        // ********************************
    private:
    };
}

#include "file.inl"

// *****************************************************************************
//                           End of file.h
// *****************************************************************************
#endif // __GN_BASE_FILE_H__

