#ifndef __GN_GFXCOMMON_BASICRENDERER_H__
#define __GN_GFXCOMMON_BASICRENDERER_H__
// *****************************************************************************
//! \file    basicRenderer.h
//! \brief   Basic renderer class
//! \author  chenlee (2005.10.1)
// *****************************************************************************

#include "renderWindowMsw.h"
#include "renderWindowX11.h"

//!
//! trace the call sequence of device reset/recreate
//!
#define _GNGFX_DEVICE_TRACE()  //GN_TRACE( GN_FUNCTION_NAME )

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

        static Logger * sLogger;

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
                && resOk()
                && contextOk()
                && drawOk();
        }
    private :
        void clear()
        {
            drawClear();
            contextClear();
            resClear();
            dispClear();
        }
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
        RenderWindowMsw mWindow;  //!< Render window instance.
    private:
        WinProp         mWinProp; //!< Render window properites.
#elif GN_POSIX
    private:
        RenderWindowX11 mWindow;  //!< Render window instance
#endif

        //@}

        // *****************************************************************************
        //
        //! \name                   Caps Manager
        //
        // *****************************************************************************

        //@{

        //@}

        // *****************************************************************************
        //
        //! \name                   Resource Manager
        //
        // *****************************************************************************

        //@{

    public :

        virtual Texture * createTextureFromFile( File & );

    private:

        bool resInit() { return true; }
        void resQuit() {}
        bool resOk() const { return true; }
        void resClear() {}

        //@}

        // *****************************************************************************
        //
        //! \name                   Context Manager
        //
        // *****************************************************************************

        //@{

    private:

        bool contextInit() { return true; }
        void contextQuit() {}
        bool contextOk() const { return true; }
        void contextClear() { clearContextResources(); }

    protected:

        void holdContextResources( const RendererContext & ); //!< hold reference to resources in context state
        void clearContextResources();

    private:

        template<class T,size_t COUNT>
        struct AutoRefArray
        {
            enum { MAX_COUNT = COUNT };
            AutoRef<const T> data[COUNT];
            size_t           count;
            AutoRefArray() : count(0) {}
            void clear() { for( size_t i = 0; i < COUNT; ++i ) data[i].clear(); count = 0; }
        };

        struct ResourceHolder
        {
            AutoRefArray<Shader,NUM_SHADER_TYPES>    shaders;
            AutoRefArray<Texture,MAX_RENDER_TARGETS> colorBuffers;
            AutoRef<const Texture>                   depthBuffer;
            AutoRefArray<Texture,MAX_TEXTURE_STAGES> textures;
            AutoRefArray<VtxBuf,MAX_VERTEX_STREAMS>  vtxBufs;
            AutoRef<const IdxBuf>                    idxBuf;
        };

        //
        // Use to hold references to currently binded resources, in case client user
        // delete those resources.
        //
        ResourceHolder mResourceHolder;

        //@}

        // *****************************************************************************
        //
        //! \name                   Draw Manager
        //
        // *****************************************************************************

        //@{

    private:
        bool drawInit()     { return true; }
        void drawQuit()     {}
        bool drawOk() const { return true; }
        void drawClear()    { mNumPrims = 0; mNumDraws = 0; }

        //@}
    };
}}

// *****************************************************************************
//                           End of basicRenderer.h
// *****************************************************************************
#endif // __GN_GFXCOMMON_BASICRENDERER_H__