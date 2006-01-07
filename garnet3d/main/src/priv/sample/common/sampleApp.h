#ifndef __GN_SAMPLE_SAMPLEAPP_H__
#define __GN_SAMPLE_SAMPLEAPP_H__
// *****************************************************************************
//! \file    common/sampleApp.h
//! \brief   Common sample framework
//! \author  chenlee (2006.1.7)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "garnet/GNcore.h"
#include "garnet/GNgfx.h"
#include "garnet/GNinput.h"
#include "garnet/GNwin.h"

namespace GN
{
    namespace sample
    {
        //!
        //! Sample application framework
        //!
        class SampleApp : public StdClass, public SlotBase
        {
             GN_DECLARE_STDCLASS( SampleApp, StdClass );

            // ********************************
            // ctor/dtor
            // ********************************

            //@{
        public:
            SampleApp()          { clear(); }
            virtual ~SampleApp() { quit(); }
            //@}

            // ********************************
            // from StdClass
            // ********************************

            //@{
        public:
            bool init( int argc, const char * argv[] );
            void quit();
            bool ok() const { return MyParent::ok(); }
        private:
            void clear() {}
            //@}

            // ********************************
            // sample framework
            // ********************************
        public:

            //@{

            virtual int run();

            virtual bool onAppInit() { return true; }
            virtual void onAppQuit() {}
            virtual bool onRendererCreate() { return true; }
            virtual bool onRendererRestore() { return true; }
            virtual void onRendererDispose() {}
            virtual void onRendererDestroy() {}
            virtual void onUpdate() {}
            virtual void onRender() {}
            virtual void onKeyPress( input::KeyEvent );
            virtual void onCharPress( wchar_t ) {}
            virtual void onAxisMove( input::Axis, int ) {}

            void postExistEvent() { mDone = true; }

            bool switchRenderer();

            gfx::Renderer * getRenderer() { return mGfxRenderer.get(); }

            //@}

            // ********************************
            // private variables
            // ********************************
        private:

            struct InitParam
            {
                bool useD3D;
                bool useDInput;
                gfx::RendererOptions ro;
            };

            InitParam mInitParam;

            GN::SharedLib mGfxLib;
            GN::gfx::CreateRendererFunc mGfxCreator;
            GN::AutoObjPtr<GN::gfx::Renderer> mGfxRenderer;

            GN::AutoObjPtr<GN::input::Input> mInput;

            GN::Clock mClock;

            bool mDone;

            // ********************************
            // private functions
            // ********************************
        private:

            bool checkCmdLine( int argc, const char * argv[] );
            bool doInit();
            bool initRenderer();
            void quitRenderer();
            bool initInput();
            void quitInput();
        };
    }
}

// *****************************************************************************
//                           End of sampleApp.h
// *****************************************************************************
#endif // __GN_SAMPLE_SAMPLEAPP_H__
