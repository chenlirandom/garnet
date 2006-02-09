#ifndef __GN_BASE_SINGLETON_H__
#define __GN_BASE_SINGLETON_H__
// *****************************************************************************
//! \file    singleton.h
//! \brief   singleton classes and macros
//! \author  chenlee (2005.8.13)
// *****************************************************************************

//!
//! ʵ�ֵ������еľ�̬����
//!
#define GN_IMPLEMENT_SINGLETON(T) template<> T * ::GN::Singleton< T >::msInstancePtr = 0;

namespace GN
{
    //!
    //! ������.
    //!
    template<typename T>
    class Singleton
    {
        static GN_PUBLIC T * msInstancePtr; //!< ָ��singleton��ʵ��

    public:

        //!
        //! Constructor
        //!
        Singleton()
        {
            GN_ASSERT( 0 == msInstancePtr );
            // This is code 64-bit compatible?
            size_t offset = (const char *)(T*)1 - (const char *)(Singleton<T>*)(T*)1;
            msInstancePtr = (T*)( ((const char *)this)+offset );
        }

        //!
        //! Destructor
        //!
        virtual ~Singleton() { GN_ASSERT(msInstancePtr); msInstancePtr = 0; }

        //!
        //! Get the instance
        //!
        static T & getInstance() { GN_ASSERT(msInstancePtr); return *msInstancePtr; }

        //!
        //! Get the instance pointer (might be NULL)
        //!
        static T * getInstancePtr() { return msInstancePtr; }
    };

    //!
    //! ���ھֲ�ʹ�õĵ����࣬�޷���DLLʹ�á�
    //!
    template<typename T>
    class LocalSingleton
    {
        static T * msInstancePtr; //!< ָ��singleton��ʵ��

    public:

        //!
        //! Constructor
        //!
        LocalSingleton()
        {
            GN_ASSERT( 0 == msInstancePtr );
            // This is code 64-bit compatible?
            size_t offset = (size_t)(T*)1 - (size_t)(LocalSingleton<T>*)(T*)1;
            msInstancePtr = (T*)((size_t)this+offset);
        }

        //!
        //! Destructor
        //!
        virtual ~LocalSingleton() { GN_ASSERT(msInstancePtr); msInstancePtr = 0; }

        //!
        //! Get the instance
        //!
        static T & getInstance() { GN_ASSERT(msInstancePtr); return *msInstancePtr; }

        //!
        //! Get the instance pointer (might be NULL)
        //!
        static T * getInstancePtr() { return msInstancePtr; }
    };

    template<typename T> T * LocalSingleton<T>::msInstancePtr = 0;
}

// *****************************************************************************
//                           End of singleton.h
// *****************************************************************************
#endif // __GN_BASE_SINGLETON_H__
