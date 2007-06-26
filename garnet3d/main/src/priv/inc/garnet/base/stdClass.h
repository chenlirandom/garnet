#ifndef __GN_BASE_STDCLASS_H__
#define __GN_BASE_STDCLASS_H__
// *****************************************************************************
/// \file
/// \brief   basic class for standard init/quit procedures.
/// \author  chenlee (2005.8.13)
// *****************************************************************************

///
/// ����stdclass����
///
/// \param self    �������������
/// \param parent  ��������ĸ��������
/// \note �����л���stdclass���͵�������ʹ�������
///
/// ���������������������MySelf��MyParent
///
#define GN_DECLARE_STDCLASS(self, parent) \
            private : typedef self MySelf; typedef parent MyParent;


///
/// stdclass���͵ı�׼��ʼ�����̡�
///
/// \param class_name ��ǰ��������������
/// \param param_list ���磺(param1, param2, param3,...)�ģ�
///                   �ҷ��ϸ���init()Ҫ��Ĳ����б����Ų���ʡ�ԣ�
/// \note �����л���stdclass���͵�init������ʹ�������
///
/// ���磺GN_STDCLASS_INIT( myobject_c, (param1, param2) );
/// ������Ϣ�μ����������
#if GN_DEBUG_BUILD
#define GN_STDCLASS_INIT( class_name, param_list )                               \
    /* call parent's init() */                                                   \
    if( !MyParent::init param_list ) return failure();                           \
    preInit();                                                                   \
    /* define sanity checker to ensure calling either failure() or success(). */ \
    SanityChecker __GN_sc(*this);
#else
#define GN_STDCLASS_INIT( class_name, param_list )     \
    /* call parent's init() */                         \
    if( !MyParent::init param_list ) return failure(); \
    preInit();
#endif

///
/// stdclass���͵ı�׼�˳�����
///
/// \note �����л���stdclass���͵�quit()������ʹ�������
///
/// ʹ�÷����μ����������
#define GN_STDCLASS_QUIT()  { clear(); MyParent::quit(); }

namespace GN
{
    ///
    /// standard class
    ///
    /// \par
    /// ���� StdClass ��ʹ��˵���Լ�һЩע���������������е�ע�͡�
    /// \par
    /// �κ���Ҫ��Ҫ��ʼ��/�˳��������඼Ӧ���� StdClass �̳С�
    /// \par
    /// StdClassʵ���˱�׼��init/quit�ӿڣ�����ӿڰ���6����׼������\n
    /// - StdClass()  : ���캯����
    ///   - ͨ��clear()��˽�б�������
    ///   - һ������£����� clear() ���⣬���캯�����ٲ�Ӧ�����������룬
    ///   - �κ��п���ʧ�ܵĳ�ʼ�����붼Ӧ�ŵ� init() �С�
    /// \n
    /// - ~StdClass() : ������������
    ///   - ���� quit() �ͷ���Դ��
    /// \n
    /// - init()      : ��ʼ��������
    ///   - �ɹ��򷵻�true�����򷵻�false��
    /// \n
    /// - quit()      : �麯����
    ///   - �ͷ����е���Դ��ͬʱ��˽�б������㣨ͨ������ clear() ����
    ///   - �����ʵ����deleteʱ�������������Զ����� quit() ��
    ///   - quit() ����Ӧ�����Ա���ȫ�ġ���εĵ��ã�Ҳ����˵��
    ///   - ���ͷ���Դʱ�������ȼ����Դ����Ч�ԡ�
    /// \n
    /// - ok()        : ��⺯����
    ///   - ��������Ƿ��Ѿ���ʼ�������ɹ����� init() �󷵻�true��
    ///     ���� quit() �󷵻�false��
    /// \n
    /// - clear()     : ˽�к�����
    ///   - ���ڽ�˽�г�Ա�������㣬�����캯���� quit() ����
    ///
    class StdClass
    {
    public:
        ///
        /// ctor
        ///
        StdClass() { clear(); }
        ///
        /// dtor
        ///
        virtual ~StdClass() { quit(); }

    public :

        ///
        /// ��ʼ������
        ///
        bool init()
        {
            if( StdClass::ok() )
            {
                GN_ERROR(getLogger("GN.base.StdClass"))( "u call init() twice!" );
                return failure();
            }

            return success();
        }

        ///
        /// �˳�����
        ///
        virtual void quit() { clear(); }

        ///
        /// �Ƿ��ʼ������
        ///
        bool ok() const { return IS_SUCCESS == mOK; }

    protected:

#if GN_DEBUG_BUILD
        ///
        /// Sanity checker for calling either failure() or success() in init().
        ///
        class SanityChecker
        {
            ///
            /// Boolean value to indicate whether failure() or success() is called.
            ///
            StdClass & theClass;

        public:

            ///
            /// ctor.
            ///
            SanityChecker( StdClass & c ) : theClass(c)
            {
                GN_ASSERT( StdClass::IS_UNDEFINED == c.mOK );
            }

            ///
            /// dtor. Check the boolean value.
            ///
            ~SanityChecker()
            {
                GN_ASSERT_EX(
                    StdClass::IS_FAILURE == theClass.mOK || StdClass::IS_SUCCESS == theClass.mOK,
                    "Neither failure() nor success() is called!" );
            }
        };
#endif

        ///
        /// helper function called by macro GN_STDCLASS_INIT() to invalidate mOK.
        ///
        void preInit() { GN_ASSERT(IS_SUCCESS == mOK); mOK = IS_UNDEFINED; }

        ///
        /// helper function called in init() to indicate initialization failure.
        ///
        bool failure()
        {
            quit();
            mOK = IS_FAILURE;
            return false;
        }

        ///
        /// helper function called in init() to indicate initialization success.
        ///
        bool success()
        {
            mOK = IS_SUCCESS;
            return true;
        }

    private :

        friend class SanityChecker;

        ///
        /// initialization status
        ///
        enum InitStatus
        {
            IS_FAILURE   = 0, ///< init() failed.
            IS_SUCCESS   = 1, ///< init() succedded.
            IS_UNDEFINED = 2, ///< init() not called.
        };

        ///
        /// initialization flag
        ///
        InitStatus mOK;

        ///
        /// initialize data members
        ///
        void clear() { mOK = IS_UNDEFINED; }
    };
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_STDCLASS_H__
