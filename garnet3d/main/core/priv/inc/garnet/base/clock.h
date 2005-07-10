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
    class Clock : public StdClass
    {
         GN_DECLARE_STDCLASS( Clock, StdClass );

        // ********************************
        //! name  ctor/dtor
        // ********************************

        //@{
    public:
        Clock()          { clear(); }
        virtual ~Clock() { quit(); }
        //@}

        // ********************************
        //! name standard init/quit
        // ********************************

        //@{
    public:
        bool init();
        void quit() { GN_STDCLASS_QUIT(); }
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() {}
        //@}

        // ********************************
        //   public functions
        // ********************************
    public:

        //!
        //! ��õ�ǰʱ��
        //!
        float getTime() const
        {
            double c1 = static_cast<double>( getCleanCycleCount() );
            double c2 = static_cast<double>( mSystemCycleFrequency );
            return static_cast<float>( c1 / c2 );
        }

        //!
        //!
        //!
        void reset();

        //!
        //!
        //!
        void resume();

        //!
        //!
        //!
        void pause();

        //!
        //!
        //!
        bool paused() const { return mPaused; }

        // ********************************
        //   private variables
        // ********************************
    private:

        typedef int64_t CycleType;

        CycleType mSystemCycleFrequency; //!< ϵͳ��ʱ����Ƶ�ʣ�ÿ���ӵ�cycle����
        CycleType mResetTime;            //!< ��ʱ����λʱ��cycle��
        CycleType mPauseTime;            //!< ��ʱ����ͣʱ��cycle��
        CycleType mPauseElapsed;         //!< ��ʱ���ܼ���ͣ��cycle��
        bool      mPaused;               //!< ��ʱ���Ƿ���ͣ

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

        //!
        //! ��ü���������Ƶ
        //!
        CycleType getSystemCycleFrequency() const;
    };
}


// *****************************************************************************
//                           End of clock.h
// *****************************************************************************
#endif // __GN_BASE_CLOCK_H__

