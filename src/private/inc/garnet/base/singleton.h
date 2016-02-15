#ifndef __GN_BASE_SINGLETON_H__
#define __GN_BASE_SINGLETON_H__
// *****************************************************************************
/// \file
/// \brief   singleton classes and macros
/// \author  chenlee (2005.8.13)
// *****************************************************************************

///
/// ʵ�ֿ�ģ�鵥���еľ�̬����
///
#define GN_IMPLEMENT_CROSS_DLL_SINGLETON(T) template<> GN_API T * ::GN::CrossDllSingleton< T >::msInstancePtr = 0;

namespace GN
{
    ///
    /// ����ģ���ڲ�ʹ�õĵ����࣬�޷���DLLʹ�á�
    ///
    template<typename T>
    class Singleton
    {
        static T * msInstancePtr; ///< ָ��singleton��ʵ��

    public:

        ///
        /// Constructor
        ///
        Singleton()
        {
            GN_ASSERT( 0 == msInstancePtr );
            // This is code 64-bit compatible?
            size_t offset = (size_t)(T*)1 - (size_t)(Singleton<T>*)(T*)1;
            msInstancePtr = (T*)((size_t)this+offset);
        }

        ///
        /// Destructor
        ///
        virtual ~Singleton() { GN_ASSERT(msInstancePtr); msInstancePtr = 0; }

        ///
        /// Get the instance
        ///
        static T & sGetInstance() { GN_ASSERT(msInstancePtr); return *msInstancePtr; }

        ///
        /// Get the instance pointer (might be NULL)
        ///
        static T * sGetInstancePtr() { return msInstancePtr; }
    };

    template<typename T> T * Singleton<T>::msInstancePtr = 0;

    ///
    /// Singleton class that can used cross DLL boundary.
    ///
    template<typename T>
    class CrossDllSingleton
    {
        static GN_API T * msInstancePtr; ///< ָ��singleton��ʵ��

    public:

        ///
        /// Constructor
        ///
        CrossDllSingleton()
        {
            GN_ASSERT( 0 == msInstancePtr );
            // This is code 64-bit compatible?
            size_t offset = (const char *)(T*)1 - (const char *)(CrossDllSingleton<T>*)(T*)1;
            msInstancePtr = (T*)( ((const char *)this)+offset );
        }

        ///
        /// Destructor
        ///
        virtual ~CrossDllSingleton() { GN_ASSERT(msInstancePtr); msInstancePtr = 0; }

        ///
        /// Get the instance
        ///
        static T & sGetInstance() { GN_ASSERT(msInstancePtr); return *msInstancePtr; }

        ///
        /// Get the instance pointer (might be NULL)
        ///
        static T * sGetInstancePtr() { return msInstancePtr; }
    };

}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_SINGLETON_H__
