#ifndef __GN_BASE_FILE_H__
#define __GN_BASE_FILE_H__
// *****************************************************************************
//! \file    file.h
//! \brief   general file stream class
//! \author  chenlee (2005.4.20)
// *****************************************************************************

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
    struct File
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
}

#include "file.inl"

// *****************************************************************************
//                           End of file.h
// *****************************************************************************
#endif // __GN_BASE_FILE_H__

