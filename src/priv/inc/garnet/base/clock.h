#ifndef __GN_BASE_CLOCK_H__
#define __GN_BASE_CLOCK_H__
// *****************************************************************************
/// \file
/// \brief   high resolution clock system.
/// \author  chenlee (2005.7.1)
// *****************************************************************************

namespace GN
{
    ///
    /// high resolution clock system
    ///
    class Clock
    {
        // ********************************
        /// name  ctor/dtor
        // ********************************

        //@{
    public:
        Clock()  { Reset(); }
        ~Clock() {}
        //@}

        // ********************************
        //   public interface
        // ********************************
    public:

        ///
        /// Clock cycle type
        ///
        typedef SInt64 CycleType;

        ///
        /// һ���߾��ȵļ�ʱ����
        ///
        /// ���شӿ��������ڼ�ʱ��������cycle����
        /// �÷���ֵ���Լ�ʱ������Ƶ���Ϳ��Ի�����롣
        ///
        static CycleType sGetSystemCycleCount();

        ///
        /// ��õ�ǰʱ�����
        ///
        CycleType GetCycleCount() const
        {
            return GetCleanCycleCount();
        }

        ///
        /// ��õ�ǰʱ��, in seconds
        ///
        double GetTimeD() const
        {
            double c1 = static_cast<double>( GetCleanCycleCount() );
            double c2 = static_cast<double>( mSystemCycleFrequency );
            return c1 / c2;
        }

        ///
        /// ��õ�ǰʱ��, in seconds
        ///
        float GetTimef() const
        {
            return (float)GetTimeD();
        }

        ///
        /// ����ʱ�ӵ���ʼ״̬
        ///
        void Reset();

        ///
        /// �ָ�ʱ������
        ///
        void Resume();

        ///
        /// ��ͣʱ��
        ///
        void Pause();

        ///
        /// ��ǰʱ���Ƿ���ͣ
        ///
        bool Paused() const { return mPaused; }

        // ********************************
        //   private variables
        // ********************************
    private:

        CycleType mResetTime;    ///< ��ʱ����λʱ��cycle��
        CycleType mPauseTime;    ///< ��ʱ����ͣʱ��cycle��
        CycleType mPauseElapsed; ///< ��ʱ���ܼ���ͣ��cycle��
        bool      mPaused;       ///< ��ʱ���Ƿ���ͣ

        ///< ϵͳ��ʱ����Ƶ�ʣ�ÿ���ӵ�cycle����
        static CycleType mSystemCycleFrequency;

        // ********************************
        //   private functions
        // ********************************
    private:

        ///
        /// �õ����ϴμ�ʱ����λ��������ȥ����ͣʱ�����ʵ�ʾ����ľ�cycle��
        ///
        CycleType GetCleanCycleCount() const
        {
            if (mPaused)
                return mPauseTime;
            else
                return sGetSystemCycleCount() - mResetTime - mPauseElapsed;
        }
    };
}


// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_CLOCK_H__

