#ifndef __GN_BASE_SINGLETON_H__
#define __GN_BASE_SINGLETON_H__
// *****************************************************************************
//! \file    singleton.h
//! \brief   singleton classes and macros
//! \author  chenlee (2005.8.13)
// *****************************************************************************

namespace GN
{
    //!
    //! ������. This class can not be used accross DLL boundary. If you want to
    //! implement a cross-DLL singleton class, please use singleton macros.
    //!
    template<typename T>
    class Singleton
    {
        GN_PUBLIC static T * msInstancePtr; //!< ָ��singleton��ʵ��

    public:

        //!
        //! Constructor
        //!
        Singleton()
        {
            GN_ASSERT( 0 == msInstancePtr );
            // This is code 64-bit compatible?
            size_t offset = (size_t)(T*)1 - (size_t)(Singleton<T>*)(T*)1;
            msInstancePtr = (T*)((size_t)this+offset);
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
}

//!
//! ʵ�ֵ������еľ�̬����
//!
#define GN_IMPLEMENT_SINGLETON(T) template<> T * ::GN::Singleton< T >::msInstancePtr = 0;

// *****************************************************************************
//                           End of singleton.h
// *****************************************************************************
#endif // __GN_BASE_SINGLETON_H__
