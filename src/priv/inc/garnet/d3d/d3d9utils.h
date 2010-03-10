#ifndef __GN_D3D_D3D9UTILS_H__
#define __GN_D3D_D3D9UTILS_H__
// *****************************************************************************
/// \file
/// \brief   d3d9 utils
/// \author  chen@@CHENLI-HOMEPC (2007.4.16)
// *****************************************************************************

#include "../GNinput.h"

#if GN_BUILD_DEBUG
#define D3D_DEBUG_INFO // Enable "Enhanced D3DDebugging"
#endif

#if GN_XENON
#include <xtl.h>
#include <xgraphics.h>
#elif GN_PC
#ifndef NOMINMAX
#define NOMINMAX ///< This is to disable windows min/max macros
#endif
#include <windows.h>
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <xnamath.h>

// Check d3d version
#if DIRECT3D_VERSION < 0x0900
#error "directx 9.0 or higher is required!"
#endif


// Define constants and types that are missing on Xenon platform.
#if GN_XENON

#define D3DLOCK_DISCARD 0
#define D3DUSAGE_DYNAMIC 0
#endif

namespace GN { /* namespace for D3D9 utils */ namespace d3d9
{
    ///
    /// scoped PIX event
    ///
    struct PixPerfScopeEvent
    {
        //@{
        PixPerfScopeEvent( D3DCOLOR color, const char * name )
        {
#if GN_XENON
            PIXBeginNamedEvent( color, name );
#else
            D3DPERF_BeginEvent( color, Mbs2Wcs(name).ToRawPtr() );
#endif
        }
        ~PixPerfScopeEvent()
        {
#if GN_XENON
            PIXEndNamedEvent();
#else
            D3DPERF_EndEvent();
#endif
        }
        //@}
    };

    ///
    /// Compile vertex shader from string
    ///
    LPDIRECT3DVERTEXSHADER9 compileAndCreateVS( LPDIRECT3DDEVICE9 dev, const char * code, size_t len = 0, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 );

    ///
    /// Compile vertex shader from file
    ///
    LPDIRECT3DVERTEXSHADER9 compileAndCreateVSFromFile( LPDIRECT3DDEVICE9 dev, const char * file, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 );

    ///
    /// Assemble vertex shader from string
    ///
    LPDIRECT3DVERTEXSHADER9 assembleAndCreateVS( LPDIRECT3DDEVICE9 dev, const char * code, size_t len = 0, UInt32 flags = 0 );

    ///
    /// Assemble vertex shader from file
    ///
    LPDIRECT3DVERTEXSHADER9 assembleAndCreateVSFromFile( LPDIRECT3DDEVICE9 dev, const char * file, UInt32 flags = 0 );

    ///
    /// Compile pixel shader from string
    ///
    LPDIRECT3DPIXELSHADER9 compileAndCreatePS( LPDIRECT3DDEVICE9 dev, const char * code, size_t len = 0, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 );

    ///
    /// Compile pixel shader from file
    ///
    LPDIRECT3DPIXELSHADER9 compileAndCreatePSFromFile( LPDIRECT3DDEVICE9 dev, const char * file, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 );

    ///
    /// Assemble pixel shader from string
    ///
    LPDIRECT3DPIXELSHADER9 assembleAndCreatePS( LPDIRECT3DDEVICE9 dev, const char * code, size_t len = 0, UInt32 flags = 0 );

    ///
    /// Assemble pixel shader from file
    ///
    LPDIRECT3DPIXELSHADER9 assembleAndCreatePSFromFile( LPDIRECT3DDEVICE9 dev, const char * file, UInt32 flags = 0 );

    //@{

    ///
    /// D3D9 shader compiler template
    ///
    template<class SHADER_CLASS>
    struct ShaderCompiler
    {
    };

    ///
    /// D3D9 vertex shader compiler
    ///
    template<>
    struct ShaderCompiler<IDirect3DVertexShader9>
    {
        //@{

        static inline IDirect3DVertexShader9 *
        assembleAndCreate( IDirect3DDevice9 & dev, const char * code, size_t len = 0, UInt32 flags = 0 )
        {
            return assembleAndCreateVS( &dev, code, len, flags );
        }

        static inline IDirect3DVertexShader9 *
        compileAndCreate( IDirect3DDevice9 & dev, const char * code, size_t len = 0, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 )
        {
            return compileAndCreateVS( &dev, code, len, flags, entryFunc, profile, constTable );
        }

        static inline IDirect3DVertexShader9 *
        compileAndCreateFromFile( IDirect3DDevice9 & dev, const char * file, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 )
        {
            return compileAndCreateVSFromFile( &dev, file, flags, entryFunc, profile, constTable );
        }

        //@}
    };

    ///
    /// D3D9 pixel shader compiler
    ///
    template<>
    struct ShaderCompiler<IDirect3DPixelShader9>
    {
        //@{

        static inline IDirect3DPixelShader9 *
        assembleAndCreate( IDirect3DDevice9 & dev, const char * code, size_t len = 0, UInt32 flags = 0 )
        {
            return assembleAndCreatePS( &dev, code, len, flags );
        }

        static inline IDirect3DPixelShader9 *
        compileAndCreate( IDirect3DDevice9 & dev, const char * code, size_t len = 0, UInt32 flags = 0, const char * entryFunc = "main", const char * profile = 0, LPD3DXCONSTANTTABLE * constTable = 0 )
        {
            return compileAndCreatePS( &dev, code, len, flags, entryFunc, profile, constTable );
        }

        //@}
    };

    ///
    /// Compile effect from string
    ///
    LPD3DXEFFECT compileAndCreateEffect( LPDIRECT3DDEVICE9 dev, const char * code, size_t len = 0, UInt32 flags = 0, LPD3DXEFFECTPOOL pool = 0 );


    ///
    /// Get backbuffer descriptor
    ///
    inline bool getBackBufferDesc( LPDIRECT3DDEVICE9 dev, D3DSURFACE_DESC & desc )
    {
        GN_GUARD_SLOW;
        GN_ASSERT( dev );
        AutoComPtr<IDirect3DSurface9> surf;
        GN_DX_CHECK_RETURN( dev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &surf ), false );
        GN_DX_CHECK_RETURN( surf->GetDesc( &desc ), false );
		return true;
        GN_UNGUARD_SLOW;
    }

    ///
    /// Get backbuffer size
    ///
    inline Vector2<UInt32> getBackBufferSize( LPDIRECT3DDEVICE9 dev )
    {
        GN_GUARD_SLOW;
        D3DSURFACE_DESC desc;
        return getBackBufferDesc( dev, desc )
            ? Vector2<UInt32>( desc.Width, desc.Height )
            : Vector2<UInt32>( 0, 0 );
        GN_UNGUARD_SLOW;
    }

    ///
    /// Get depth buffer descriptor
    ///
    inline bool getDepthStencilDesc( LPDIRECT3DDEVICE9 dev, D3DSURFACE_DESC & desc )
    {
        GN_GUARD_SLOW;
        GN_ASSERT( dev );
        AutoComPtr<IDirect3DSurface9> surf;
        GN_DX_CHECK_RETURN( dev->GetDepthStencilSurface( &surf ), false );
        GN_DX_CHECK_RETURN( surf->GetDesc( &desc ), false );
		return true;
        GN_UNGUARD_SLOW;
    }

    ///
    /// Get depth buffer size
    ///
    inline Vector2<UInt32> getDepthStencilSize( LPDIRECT3DDEVICE9 dev )
    {
        GN_GUARD_SLOW;
        D3DSURFACE_DESC desc;
        return getDepthStencilDesc( dev, desc )
            ? Vector2<UInt32>( desc.Width, desc.Height )
            : Vector2<UInt32>( 0, 0 );
        GN_UNGUARD_SLOW;
    }

    class D3D9RenderStateSaver
    {
        typedef HashMap<D3DRENDERSTATETYPE, DWORD> RenderStateMap;

        IDirect3DDevice9 * m_Device;
        RenderStateMap     m_Values;

    public:

        D3D9RenderStateSaver( IDirect3DDevice9 * dev )
            : m_Device(dev)
        {
        }

        ~D3D9RenderStateSaver()
        {
            RestoreAllRenderStates();
        }

        void StoreRenderState( D3DRENDERSTATETYPE type )
        {
            if( m_Device && m_Values.Find(type) == NULL )
            {
                DWORD value;
                if( SUCCEEDED( m_Device->GetRenderState( type, &value ) ) )
                {
                    m_Values[type] = value;
                }
            }
        }

        void RestoreAllRenderStates()
        {
            RenderStateMap::KeyValuePair * i;
            for( i = m_Values.First(); i != NULL; i = m_Values.Next( i ) )
            {
                D3DRENDERSTATETYPE type = i->key;
                DWORD              value = i->value;
                m_Device->SetRenderState( type, value );
            }
            m_Values.Clear();
        }
    };

    struct ThickLineVertex
    {
        float    x, y, z;
        float    u, v;
        D3DCOLOR color;
    };

    struct ThickLineParameters
    {
        XMMATRIX worldview;          // matrix that transform vertex from object space to view space
        XMMATRIX proj;               // matrix that transform vertex from view space to clip space
        float    width;              // line width
        bool     widthInScreenSpace; // true  : line width is in pixels in screen space
                                     // false : line width is in view space
    };

    class D3D9ThickLineRenderer
    {

    public:

        D3D9ThickLineRenderer();
        ~D3D9ThickLineRenderer() { OnDeviceDispose(); OnDeviceDelete(); }

        bool OnDeviceCreate( IDirect3DDevice9 * dev );
        bool OnDeviceRestore();
        void OnDeviceDispose();
        void OnDeviceDelete();

        bool DrawBegin( const ThickLineParameters & parameters );
        void DrawEnd();

        void Line( const ThickLineVertex & v0, const ThickLineVertex & v1 );
        void Line( float x1, float y1, float z1, float x2, float y2, float z2, D3DCOLOR color );
        void LineList( const ThickLineVertex * vertices, size_t numverts );

    private:

        struct PrivateParameters : public ThickLineParameters
        {
            XMMATRIX wvp; // world * view * proj
            float screenWidth, screenHeight; // screen size in pixels
            float endPointHalfWidth, endPointHalfHeight; // size of line end point in clip space.
        };

        struct PrivateVertex
        {
            XMVECTOR position; // position in clip space
            D3DCOLOR color;
            float    u, v;
            UInt32   _; // padding
        };

        struct EndPoint
        {
            float posl, posr, post, posb, posz, posw;
            float texl, texr, text, texb;
            D3DCOLOR color;

            PrivateVertex * TopLeft( PrivateVertex * v );
            PrivateVertex * TopRight( PrivateVertex * v );
            PrivateVertex * BottomLeft( PrivateVertex * v );
            PrivateVertex * BottomRight( PrivateVertex * v );
        };

    private:

        IDirect3DDevice9 * m_Device;
        IDirect3DVertexShader9 * m_Vs;
        IDirect3DPixelShader9 * m_Ps;
        IDirect3DVertexDeclaration9 * m_Decl;
        IDirect3DVertexBuffer9 * m_Vb;
        IDirect3DIndexBuffer9 * m_Ib;

        bool m_Drawing;
        PrivateParameters m_Parameters;

        static const size_t MAX_VERTICES = 1024;
        PrivateVertex m_Vertices[MAX_VERTICES];
        UInt16        m_Indices[MAX_VERTICES/6*12]; // 12 indices (4 triangles ) per 6 verices
        UInt32        m_NumVertices;


    private:

        void Clear();

        void CalcEndPoint(
            EndPoint              & endpoint,
            const ThickLineVertex & vertex );

        PrivateVertex * NewPolygon6();

        void Flush();
    };

    ///
    /// D3D9 application framework
    ///
    struct D3D9AppOption
    {
        ///
        /// ctor
        ///
        D3D9AppOption()
            : parent(0)
            , monitor(0)
            , refdev(false)
            , vsync(false)
            , fullscreen(false)
            , fsWidth(0)
            , fsHeight(0)
            , fsDepth(0)
            , fsRefrate(0)
            , windowedWidth(640)
            , windowedHeight(480)
        {
        }

        ///
        /// Handle of parent window. Default is zero, means a top-level window.
        ///
        HWND parent;

        ///
        /// Monitor handle.
        ///
        /// - 0 means using the monitor where parent window stays in.
        ///   If monitor and parent are both zero, primary monitor will be used.
        /// - Default is 0.
        ///
        HMONITOR monitor;

        ///
        /// use ref device. Default is false.
        ///
        bool refdev;

        ///
        /// V-SYNC enable or not. Default is false.
        ///
        bool vsync;

        ///
        /// fullscreen or windowed mode.
        /// Default is false.
        ///
        bool fullscreen;

        ///
        /// full screen display mode
        ///
        //@{
        UInt32 fsWidth;   ///< Screen width.
        UInt32 fsHeight;  ///< Screen height.
        UInt32 fsDepth;   ///< Color depth. Ignored for windowed mode.
        UInt32 fsRefrate; ///< Referesh rate. Ignored for windowed mode.
        //@}

        ///
        /// Backbuffer width for windowed mode. Ignored in fullscreen mode.
        /// Zero means using client width of render window. If render window
        /// is also not avaiable, default width 640 will be used.
        /// Default is 0.
        ///
        UInt32 windowedWidth;

        ///
        /// Backbuffer height for windowed mode. Ignored in fullscreen mode.
        /// Zero means using client height of render window. If render window
        /// is also not avaiable, default height 480 will be used.
        /// Default is 0.
        ///
        UInt32 windowedHeight;
    };

    ///
    /// D3D9 application framework
    ///
    class D3D9Application : public SlotBase
    {
    public:

        //@{

        D3D9Application();
        ~D3D9Application();

        HWND               window() const { return mWindow; }
        IDirect3DDevice9 & d3d9dev() const { GN_ASSERT( mDevice ); return *mDevice; }

        int run( const D3D9AppOption * = 0 );

        bool changeOption( const D3D9AppOption & );

        //@}

    protected:

        //@{

        virtual bool onInit( D3D9AppOption & ) { return true; }
        virtual bool onCreate() { return true; }
        virtual bool onRestore() { return true; }
        virtual void onDispose() {}
        virtual void onDestroy() {}
        virtual void onQuit() {}

        virtual void onDraw() {}
        virtual void onUpdate() {}
        virtual void onKeyPress( input::KeyEvent );
        virtual void onCharPress( wchar_t ) {}
        virtual void onAxisMove( input::Axis, int ) {}

        //@}

    private:

        bool Init();
        void Quit();

        bool createDevice();
        bool restoreDevice();
        void disposeDevice();
        void destroyDevice();

    private:

        D3D9AppOption         mOption;
        HWND                  mWindow;
        UINT                  mAdapter;
        D3DDEVTYPE            mDeviceType;
        UINT                  mBehavior;
        D3DPRESENT_PARAMETERS mPresentParameters;
        IDirect3D9          * mD3D;
        IDirect3DDevice9    * mDevice;
        bool                  mRunning;
        bool                  mShutdownInputSystem;
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_D3D_D3D9UTILS_H__
