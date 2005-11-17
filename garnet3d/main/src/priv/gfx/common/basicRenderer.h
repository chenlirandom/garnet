#ifndef __GN_GFXCOMMON_BASICRENDERER_H__
#define __GN_GFXCOMMON_BASICRENDERER_H__
// *****************************************************************************
//! \file    basicRenderer.h
//! \brief   Basic renderer class
//! \author  chenlee (2005.10.1)
// *****************************************************************************

#include "ntRenderWindow.h"
#include "xRenderWindow.h"

//!
//! trace the call sequence of device reset/recreate
//!
#define _GNGFX_DEVICE_TRACE()  //GNGFX_TRACE( GN_FUNCTION_NAME )

namespace GN { namespace gfx
{
    //!
    //! Device-dependent render state block
    //!
    struct DeviceRenderStateBlock : public RefCounter
    {
        //!
        //! Apply render states to render device
        //!
        virtual void apply() const = 0;
    };

    //!
    //! basic renderer class
    //!
    class BasicRenderer : public Renderer, public StdClass
    {
        GN_DECLARE_STDCLASS( BasicRenderer, StdClass );

        // ********************************
        // constructor/destructor
        // ********************************

        //@{
    public:
        BasicRenderer() { clear(); }
        virtual ~BasicRenderer() {}
        //@}

        // ********************************
        // standard init / quit
        // ********************************

        //@{
    public:
        bool init();
        void quit();
        bool ok() const
        {
            return MyParent::ok()
                && dispOk()
                && rsbOk()
                && drawOk();
        }
    private :
        void clear()
        {
            drawClear();
            rsbClear();
            dispClear();
        }
        //@}

        // *****************************************************************************
        //
        //! \name                   Device manager
        //
        // *****************************************************************************

        //@{

    protected:
        bool deviceCreate();
        bool deviceRestore();
        void deviceDispose();
        void deviceDestroy();

        //@}

        // *****************************************************************************
        //
        //! \name                   Display Manager
        //
        // *****************************************************************************

        //@{

    private :
        bool dispInit() { return true; }
        void dispQuit() { mWindow.quit(); }
        bool dispOk() const { return true; }
        void dispClear() {}

    protected:

        //!
        //! Called by sub-classes to initialize display descriptor
        //! based on device settings.
        //!
        bool processUserOptions( const RendererOptions & );

        //!
        //! Called by sub class to respond to render window resizing/moving
        //!
        bool handleRenderWindowSizeMove();

#if GN_MSWIN
    protected:
        NTRenderWindow mWindow;  //!< Render window instance.
    private:
        WinProp        mWinProp; //!< Render window properites.
#elif GN_POSIX
    private:
        XRenderWindow mWindow;  //!< Render window instance
#endif

        //@}

        // *****************************************************************************
        //
        //! \name                   Render state manager
        //
        // *****************************************************************************

        //@{

        public :

            virtual uint32_t createRenderStateBlock( const RenderStateBlockDesc & );
            virtual void bindRenderStateBlock( uint32_t );
            virtual void setRenderState( RenderState, RenderStateValue );
            virtual void setTextureState( uint32_t, TextureState, TextureStateValue );

        private :

            bool rsbInit() { return true; }
            void rsbQuit() {}
            bool rsbOk() const { return true; }
            void rsbClear() { mRsbHandles.clear(); mDevRsbMap.clear(); mCurrentRsb = 0; }

            bool rsbDeviceCreate() { return true; }
            bool rsbDeviceRestore();
            void rsbDeviceDispose() { mDevRsbMap.clear(); }
            void rsbDeviceDestroy() { mRsbHandles.clear(); mCurrentRsb = 0; }

            //!
            //! Create device render state block, that can switch device render state
            //! from 'from' to 'to'.
            //!
            virtual DeviceRenderStateBlock *
            createDeviceRenderStateBlock( const RenderStateBlockDesc & from, const RenderStateBlockDesc & to ) = 0;

        private :

            union DevRsbKey
            {
                uint64_t u64;
                struct
                {
                    uint32_t from, to;
                };
                bool operator<( const DevRsbKey & rhs ) const { return u64 < rhs.u64; }
            };
            typedef HandleManager<RenderStateBlockDesc,uint32_t> RsbHandleManager;
            typedef AutoRef<DeviceRenderStateBlock> DeviceRenderStateBlockRefPtr;
            typedef std::map<DevRsbKey,DeviceRenderStateBlockRefPtr> DevRsbMap;

            RsbHandleManager mRsbHandles;
            DevRsbMap        mDevRsbMap;
            uint32_t         mCurrentRsb;
        //@}

        // *****************************************************************************
        //
        //! \name                   Draw Manager
        //
        // *****************************************************************************

        //@{
    public:
        virtual void drawTextA( const char * text, int x, int y, const Vector4f & color );

    private:
        bool drawInit()     { return true; }
        void drawQuit()     {}
        bool drawOk() const { return true; }
        void drawClear()    { mNumPrims = 0; mNumDraws = 0; }

    protected:

        //!
        //! Render target texture descriptor
        //!
        struct RenderTargetTextureDesc
        {
            const Texture * tex;  //!< Render target texture.
            TexFace         face; //!< Effective only when tex is cubemap.

            //!
            //! Ctor
            //!
            RenderTargetTextureDesc() : tex(0) {}

            //!
            //! Equality check
            //!
            bool equal( const Texture * t, TexFace f ) const
            {
                return tex == t && ( 0 == tex || face == f );
            }
        };

        //@}

    };
}}

// *****************************************************************************
//                           End of basicRenderer.h
// *****************************************************************************
#endif // __GN_GFXCOMMON_BASICRENDERER_H__
