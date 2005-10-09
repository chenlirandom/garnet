#ifndef __GN_BASE_CLOCK_H__
#define __GN_BASE_CLOCK_H__
// *****************************************************************************
//! \file    clock.h
//! \brief   high resolution clock system.
//! \author  chenlee (2005.7.1)
// *****************************************************************************

namespace GN
{
    //!
    //! high resolution clock system
    //!
    class Clock
    {
        // ********************************
        //! name  ctor/dtor
        // ********************************

        //@{
    public:
        Clock()  { reset(); }
        ~Clock() {}
        //@}

        // ********************************
        //   public interface
        // ********************************
    public:

        //!
        //! Clock cycle type
        //!
        typedef int64_t CycleType;

        //!
        //! ��õ�ǰʱ�����
        //!
        CycleType getCycleCount() const
        {
            return getCleanCycleCount();
        }

        //!
        //! ��õ�ǰʱ��
        //!
        double getTimeD() const
        {
            double c1 = static_cast<double>( getCleanCycleCount() );
            double c2 = static_cast<double>( mSystemCycleFrequency );
            return c1 / c2;
        }

        //!
        //! ��õ�ǰʱ��
        //!
        float getTimef() const
        {
            return (float)getTimeD();
        }

        //!
        //! ����ʱ�ӵ���ʼ״̬
        //!
        void reset();

        //!
        //! �ָ�ʱ������
        //!
        void resume();

        //!
        //! ��ͣʱ��
        //!
        void pause();

        //!
        //! ��ǰʱ���Ƿ���ͣ
        //!
        bool paused() const { return mPaused; }

        // ********************************
        //   private variables
        // ********************************
    private:

        CycleType mResetTime;    //!< ��ʱ����λʱ��cycle��
        CycleType mPauseTime;    //!< ��ʱ����ͣʱ��cycle��
        CycleType mPauseElapsed; //!< ��ʱ���ܼ���ͣ��cycle��
        bool      mPaused;       //!< ��ʱ���Ƿ���ͣ

        //!< ϵͳ��ʱ����Ƶ�ʣ�ÿ���ӵ�cycle����
        static CycleType mSystemCycleFrequency;

        // ********************************
        //   private functions
        // ********************************
    private:

        //!
        //! �õ����ϴμ�ʱ����λ��������ȥ����ͣʱ�����ʵ�ʾ����ľ�cycle��
        //!
        CycleType getCleanCycleCount() const
        {
            if (mPaused)
                return mPauseTime;
            else
                return getSystemCycleCount() - mResetTime - mPauseElapsed;
        }

        //!
        //! һ���߾��ȵļ�ʱ����
        //!
        //! ���شӿ��������ڼ�ʱ��������cycle����
        //! �÷���ֵ���Լ�ʱ������Ƶ���Ϳ��Ի�����롣
        //!
        CycleType getSystemCycleCount() const;
    };
}


// *****************************************************************************
//                           End of clock.h
// *****************************************************************************
#endif // __GN_BASE_CLOCK_H__

