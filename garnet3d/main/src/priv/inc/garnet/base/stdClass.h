#ifndef __GN_BASE_STDCLASS_H__
#define __GN_BASE_STDCLASS_H__
// *****************************************************************************
//! \file    stdClass.h
//! \brief   basic class for standard init/quit procedures.
//! \author  chenlee (2005.8.13)
// *****************************************************************************

//!
//! ����stdclass����
//!
//! \param self    �������������
//! \param parent  ��������ĸ��������
//! \note �����л���stdclass���͵�������ʹ�������
//!
//! ���������������������MySelf��MyParent���ͱ�׼˽�к����� selfOK()
//!
#define GN_DECLARE_STDCLASS(self, parent)                           \
            private : typedef self MySelf; typedef parent MyParent; \
            private : bool selfOK() const { return MySelf::ok(); }


//!
//! stdclass���͵ı�׼��ʼ�����̡�
//!
//! \param class_name ��ǰ��������������
//! \param param_list ���磺(param1, param2, param3,...)�ģ�
//!                   �ҷ��ϸ���init()Ҫ��Ĳ����б����Ų���ʡ�ԣ�
//! \note �����л���stdclass���͵�init������ʹ�������
//!
//! ���磺GN_STDCLASS_INIT( myobject_c, (param1, param2) );
//! ������Ϣ�μ����������
#define GN_STDCLASS_INIT( class_name, param_list )     \
    /* check for twice init */                         \
    if( selfOK() )                                     \
    {                                                  \
        GN_ERROR(getLogger("GN.base.StdClass"))( "u call init() twice!" ); \
        quit(); return selfOK();                       \
    }                                                  \
    /* call parent's init() */                         \
    if( !MyParent::init param_list )  { quit(); return selfOK(); }

//!
//! stdclass���͵ı�׼�˳�����
//!
//! \note �����л���stdclass���͵�quit()������ʹ�������
//!
//! ʹ�÷����μ����������
#define GN_STDCLASS_QUIT()  { clear(); MyParent::quit(); }

namespace GN
{
    //!
    //! standard class
    //!
    //! \par
    //! ���� StdClass ��ʹ��˵���Լ�һЩע���������������е�ע�͡�
    //! \par
    //! �κ���Ҫ��Ҫ��ʼ��/�˳��������඼Ӧ���� StdClass �̳С�
    //! \par
    //! StdClassʵ���˱�׼��init/quit�ӿڣ�����ӿڰ����߸���׼������\n
    //! - StdClass()  : ���캯����
    //!   - ͨ��clear()��˽�б�������
    //!   - һ������£����� clear() ���⣬���캯�����ٲ�Ӧ�����������룬
    //!   - �κ��п���ʧ�ܵĳ�ʼ�����붼Ӧ�ŵ� init() �С�
    //! \n
    //! - ~StdClass() : ������������
    //!   - ���� quit() �ͷ���Դ��
    //! \n
    //! - init()      : ʵ��ʼ��������
    //!   - �ɹ��򷵻�1�����򷵻�0��
    //! \n
    //! - quit()      : �麯����
    //!   - �ͷ����е���Դ��ͬʱ��˽�б������㣨ͨ������ clear() ����
    //!   - �����ʵ����deleteʱ�������������Զ����� quit() ��
    //!   - quit() ����Ӧ�����Ա���ȫ�ġ���εĵ��ã�Ҳ����˵��
    //!   - ���ͷ���Դʱ�������ȼ����Դ����Ч�ԡ�
    //! \n
    //! - ok()     : �����麯����
    //!   - ��������Ƿ��Ѿ���ʼ�������ɹ����� init() �󷵻�true��
    //!     ���� quit() �󷵻�false��
    //! \n
    //! - selfOK() : ˽�к�����
    //!   - selfOK() �� ok() �ġ����顱�汾��ǿ�Ƶ��ñ���� ok() ������
    //!   - ���� selfOK() ����ϸʹ�ù���������ע�⡣
    //! \n
    //! - clear()     : ˽�к�����
    //!   - ���ڽ�˽�г�Ա�������㣬�����캯���� quit() ����
    //!
    //! \note  selfOK() ���麯�� ok() �ġ����顱�汾���书����ǿ�Ƶ���
    //!        �����͵� ok() ����������ͨ�����麯����������ʵ��������
    //!        ��������Ӧ�� ok() �� selfOK() ��˽�к�����ֻ���ڱ���ĳ�
    //!        Ա������ʹ�á� �ٸ����ӣ�
    //! \code
    //!    class A : public StdClass
    //!    {
    //!        GN_DECLARE_STDCLASS(A, StdClass);
    //!    public:
    //!        virtual bool ok() const { return 1; }
    //!        bool test()             { return ok(); }
    //!        bool test2()            { return selfOK(); }
    //!    }
    //!    class B : public A
    //!    {
    //!        GN_DECLARE_STDCLASS(B, A);
    //!    public:
    //!        virtual bool ok() const { return 0; }
    //!    }
    //!    A * a;
    //!    B b;
    //!    a = \&b;
    //!    a->test();  // ����aָ�����B���͵ı��������԰���
    //!                // �麯���ĵ��ù��򣬽������B::ok()�����Է���0
    //!    a->test2(); // ʹ��selfOk()���ǿ�Ƶ���A::ok()������1
    //! \endcode
    //!
    class StdClass
    {
    public:
        //!
        //! ctor
        //!
        StdClass() { clear(); }
        //!
        //! dtor
        //!
        virtual ~StdClass() { quit(); }

    public :

        //!
        //! ��ʼ������
        //!
        bool init()
        {
            if( selfOK() )
            {
                GN_ERROR(getLogger("GN.base.StdClass"))( "u call init() twice!" );
                quit(); return selfOK();
            }

            mOK = true;
            return selfOK();
        }

        //!
        //! �˳�����
        //!
        virtual void quit() { clear(); }

        //!
        //! �Ƿ��ʼ������
        //!
        virtual bool ok() const { return mOK; }

    private :

        //!
        //! initialize data members
        //!
        void clear() { mOK = false; }

        //!
        //! private initialization check routine
        //!
        bool selfOK() const { return StdClass::ok(); }

        //!
        //! initialization flag
        //!
        bool mOK;
    };

}

// *****************************************************************************
//                           End of stdClass.h
// *****************************************************************************
#endif // __GN_BASE_STDCLASS_H__
