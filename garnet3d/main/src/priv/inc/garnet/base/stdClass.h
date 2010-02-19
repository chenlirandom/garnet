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
///                   �ҷ��ϸ���Init()Ҫ��Ĳ����б����Ų���ʡ�ԣ�
/// \note �����л���stdclass���͵�Init������ʹ�������
///
/// ���磺GN_STDCLASS_INIT( myobject_c, (param1, param2) );
/// ������Ϣ�μ����������
#if GN_BUILD_DEBUG
#define GN_STDCLASS_INIT( class_name, param_list )                               \
    /* call parent's Init() */                                                   \
    if( !MyParent::Init param_list ) return Failure();                           \
    PreInit();                                                                   \
    /* define sanity checker to ensure calling either Failure() or success(). */ \
    SanityChecker __GN_sc(*this);
#else
#define GN_STDCLASS_INIT( class_name, param_list )     \
    /* call parent's Init() */                         \
    if( !MyParent::Init param_list ) return Failure(); \
    PreInit();
#endif

///
/// stdclass���͵ı�׼�˳�����
///
/// \note �����л���stdclass���͵�Quit()������ʹ�������
///
/// ʹ�÷����μ����������
#define GN_STDCLASS_QUIT()  { Clear(); MyParent::Quit(); }

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
    ///   - һ������£����� Clear() ���⣬���캯�����ٲ�Ӧ�����������룬
    ///   - �κ��п���ʧ�ܵĳ�ʼ�����붼Ӧ�ŵ� Init() �С�
    /// \n
    /// - ~StdClass() : ������������
    ///   - ���� Quit() �ͷ���Դ��
    /// \n
    /// - Init()      : ��ʼ��������
    ///   - �ɹ��򷵻�true�����򷵻�false��
    /// \n
    /// - Quit()      : �麯����
    ///   - �ͷ����е���Դ��ͬʱ��˽�б������㣨ͨ������ Clear() ����
    ///   - �����ʵ����deleteʱ�������������Զ����� Quit() ��
    ///   - Quit() ����Ӧ�����Ա���ȫ�ġ���εĵ��ã�Ҳ����˵��
    ///   - ���ͷ���Դʱ�������ȼ����Դ����Ч�ԡ�
    /// \n
    /// - Ok()        : ��⺯����
    ///   - ��������Ƿ��Ѿ���ʼ�������ɹ����� Init() �󷵻�true��
    ///     ���� Quit() �󷵻�false��
    /// \n
    /// - Clear()     : ˽�к�����
    ///   - ���ڽ�˽�г�Ա�������㣬�����캯���� Quit() ����
    ///
    class StdClass
    {
    public:
        ///
        /// ctor
        ///
        StdClass() { Clear(); }
        ///
        /// copy ctor
        ///
        StdClass( const StdClass & o ) : mOK( o.mOK ) {}
        ///
        /// dtor
        ///
        virtual ~StdClass() { Quit(); }

    public :

        ///
        /// ��ʼ������
        ///
        bool Init()
        {
            if( StdClass::Ok() )
            {
                GN_ERROR(GetLogger("GN.base.StdClass"))( "u call Init() twice!" );
                return Failure();
            }

            return Success();
        }

        ///
        /// �˳�����
        ///
        virtual void Quit() { Clear(); }

        ///
        /// �Ƿ��ʼ������
        ///
        bool Ok() const { return IS_SUCCESS == mOK; }

        ///
        /// assignment
        ///
        StdClass & operator=( const StdClass & rhs ) { mOK = rhs.mOK; return *this; }

    protected:

#if GN_BUILD_DEBUG
        ///
        /// Sanity checker for calling either Failure() or Success() in Init().
        ///
        class SanityChecker
        {
            ///
            /// Boolean value to indicate whether Failure() or Success() is called.
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
                    "Neither Failure() nor Success() is called!" );
            }
        };
#endif

        ///
        /// helper function called by macro GN_STDCLASS_INIT() to invalidate mOK.
        ///
        void PreInit() { GN_ASSERT(IS_SUCCESS == mOK); mOK = IS_UNDEFINED; }

        ///
        /// helper function called in Init() to indicate initialization failure.
        ///
        bool Failure()
        {
            Quit();
            mOK = IS_FAILURE;
            return false;
        }

        ///
        /// helper function called in Init() to indicate initialization success.
        ///
        bool Success()
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
            IS_FAILURE   = 0, ///< Init() failed.
            IS_SUCCESS   = 1, ///< Init() succedded.
            IS_UNDEFINED = 2, ///< Init() not called.
        };

        ///
        /// initialization flag
        ///
        InitStatus mOK;

        ///
        /// initialize data members
        ///
        void Clear() { mOK = IS_UNDEFINED; }
    };
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_STDCLASS_H__
