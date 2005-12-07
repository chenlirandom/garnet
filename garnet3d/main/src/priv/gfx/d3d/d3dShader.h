#ifndef __GN_GFXD3D_D3DSHADER_H__
#define __GN_GFXD3D_D3DSHADER_H__
// *****************************************************************************
//! \file    d3d/d3dShader.h
//! \brief   D3D shader classes
//! \author  chenlee (2005.11.26)
// *****************************************************************************

#include "d3dResource.h"

namespace GN { namespace gfx
{
    //!
    //! Basic D3D shader class
    //!
    struct D3DBasicShader : public Shader
    {
        //!
        //! Apply shader as well as shader constants to D3D device
        //!
        virtual void apply() const = 0;

        //!
        //! Apply only dirty uniforms to D3D device
        //!
        virtual void applyDirtyUniforms() const = 0;

    protected:

        //!
        //! protected ctor
        //!
        D3DBasicShader( ShaderType type, ShadingLanguage lang ) : Shader(type,lang) {}
    };

    // *************************************************************************
    // ASM shaders
    // *************************************************************************

    //!
    //! Basic D3D asm shader class
    //!
    struct D3DShaderAsm : public D3DBasicShader
    {
        enum
        {
            CONST_F = 1, //!< float const
            CONST_I = 2, //!< integer const
            CONST_B = 3, //!< bool const
        };

        //!
        //! Asm shader constant descriptor
        //!
        union D3DAsmConstDesc
        {
            //!
            //! shader constant descriptor as uint32
            //!
            uint32_t u32;

            struct
            {
                uint16_t type;  //!< should be one of CONST_F, CONST_I, CONST_B
                uint16_t index; //!< const index.
            };
        };

    protected:

        //!
        //! protected ctor
        //!
        D3DShaderAsm( ShaderType type) : D3DBasicShader(type,LANG_D3D_ASM) {}
    };

    //!
    //! D3D asm vertex shader class
    //!
    class D3DVtxShaderAsm : public D3DShaderAsm, public D3DResource, public StdClass
    {
         GN_DECLARE_STDCLASS( D3DVtxShaderAsm, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        D3DVtxShaderAsm( D3DRenderer & r )
            : D3DShaderAsm(VERTEX_SHADER)
            , D3DResource(r)
        { clear(); }
        virtual ~D3DVtxShaderAsm() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const StrA & code );
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() { mCode.clear(); mMachineCode = 0; mD3DShader = 0; }
        //@}

        // ********************************
        // from Shader
        // ********************************
    public:

        // ********************************
        // from D3DResource
        // ********************************
    public:
        bool deviceCreate();
        bool deviceRestore() { return true; }
        void deviceDispose() {}
        void deviceDestroy();

        // ********************************
        // from D3DBasicShader
        // ********************************
    public:

        void apply() const;
        void applyDirtyUniforms() const;

        // ********************************
        // private variables
        // ********************************
    private:
        StrA                    mCode;
        LPD3DXBUFFER            mMachineCode;
        LPDIRECT3DVERTEXSHADER9 mD3DShader;

        size_t mMaxConstF, mMaxConstI, mMaxConstB;

        // ********************************
        // private functions
        // ********************************
    private:
        bool compileShader();
        bool analyzeUniforms();
        void applyUniform( LPDIRECT3DDEVICE9, const Uniform & ) const;
        bool queryDeviceUniform( const char * name, HandleType & userData ) const;
    };

    //!
    //! D3D asm pixel shader class
    //!
    class D3DPxlShaderAsm : public D3DShaderAsm, public D3DResource, public StdClass
    {
         GN_DECLARE_STDCLASS( D3DPxlShaderAsm, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        D3DPxlShaderAsm( D3DRenderer & r )
            : D3DShaderAsm(PIXEL_SHADER)
            , D3DResource(r)
        { clear(); }
        virtual ~D3DPxlShaderAsm() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const StrA & code );
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() { mCode.clear(); mMachineCode = 0; mD3DShader = 0; }
        //@}

        // ********************************
        // from Shader
        // ********************************
    public:

        // ********************************
        // from D3DResource
        // ********************************
    public:
        bool deviceCreate();
        bool deviceRestore() { return true; }
        void deviceDispose() {}
        void deviceDestroy();

        // ********************************
        // from D3DBasicShader
        // ********************************
    public:

        void apply() const;
        void applyDirtyUniforms() const;

        // ********************************
        // private variables
        // ********************************
    private:
        StrA                   mCode;
        LPD3DXBUFFER           mMachineCode;
        LPDIRECT3DPIXELSHADER9 mD3DShader;

        size_t mMaxConstF, mMaxConstI, mMaxConstB;

        // ********************************
        // private functions
        // ********************************
    private:
        bool compileShader();
        bool analyzeUniforms();
        void applyUniform( LPDIRECT3DDEVICE9, const Uniform & ) const;
        bool queryDeviceUniform( const char * name, HandleType & userData ) const;
    };

    // *************************************************************************
    // HLSL shaders
    // *************************************************************************

    //!
    //! Basic D3D HLSL shader class
    //!
    struct D3DShaderHlsl : public D3DBasicShader
    {

    protected:

        //!
        //! protected ctor
        //!
        D3DShaderHlsl( ShaderType type) : D3DBasicShader(type,LANG_D3D_ASM) {}

        //!
        //! apply uniform to D3D device
        void applyUniform( LPDIRECT3DDEVICE9 dev, LPD3DXCONSTANTTABLE table, const Uniform & u ) const
        {
            GN_GUARD_SLOW;

            GN_ASSERT( dev && table );

            D3DXHANDLE h = (D3DXHANDLE)u.userData;
            GN_ASSERT( h );

            switch( u.type )
            {
                case UVT_BOOL:
                    if( !u.valueBool.empty() )
                        GN_DX_CHECK( table->SetBoolArray( dev, h, (const BOOL*)&u.valueBool[0], (UINT)u.valueBool.size() ) );
                    break;

                case UVT_INT:
                    if( !u.valueInt.empty() )
                        GN_DX_CHECK( table->SetIntArray( dev, h, (const INT*)&u.valueInt[0], (UINT)u.valueInt.size() ) );
                    break;

                case UVT_FLOAT:
                    if( !u.valueFloat.empty() )
                        GN_DX_CHECK( table->SetFloatArray( dev, h, &u.valueFloat[0], (UINT)u.valueFloat.size() ) );
                    break;

                case UVT_FLOAT4:
                    if( !u.valueVector4.empty() )
                        GN_DX_CHECK( table->SetVectorArray( dev, h, (const D3DXVECTOR4*)&u.valueVector4[0], (UINT)u.valueVector4.size() ) );
                    break;

                case UVT_MATRIX44:
                    if( !u.valueMatrix44.empty() )
                        GN_DX_CHECK( table->SetMatrixTransposeArray( dev, h, (const D3DXMATRIX*)&u.valueMatrix44[0], (UINT)u.valueMatrix44.size() ) );
                    break;

                default:
                    // Program should not reach here.
                    GN_ERROR( "invalid uniform type!" );
                    GN_UNEXPECTED();
            }
            GN_UNGUARD_SLOW;
        }
    };

    //!
    //! D3D HLSL vertex shader class
    //!
    class D3DVtxShaderHlsl : public D3DShaderHlsl, public D3DResource, public StdClass
    {
         GN_DECLARE_STDCLASS( D3DVtxShaderHlsl, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        D3DVtxShaderHlsl( D3DRenderer & r )
            : D3DShaderHlsl(VERTEX_SHADER)
            , D3DResource(r)
        { clear(); }
        virtual ~D3DVtxShaderHlsl() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const StrA & code, const StrA & entry );
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() { mCode.clear(); mConstTable = 0; mD3DShader = 0; }
        //@}

        // ********************************
        // from Shader
        // ********************************
    public:

        // ********************************
        // from D3DResource
        // ********************************
    public:
        bool deviceCreate();
        bool deviceRestore() { return true; }
        void deviceDispose() {}
        void deviceDestroy();

        // ********************************
        // from D3DBasicShader
        // ********************************
    public:

        void apply() const;
        void applyDirtyUniforms() const;

        // ********************************
        // private variables
        // ********************************
    private:
        StrA                    mCode;
        StrA                    mEntry;
        LPD3DXCONSTANTTABLE     mConstTable;
        LPDIRECT3DVERTEXSHADER9 mD3DShader;

        // ********************************
        // private functions
        // ********************************
    private:
        bool queryDeviceUniform( const char * name, HandleType & userData ) const;
    };

    //!
    //! D3D HLSL pixel shader class
    //!
    class D3DPxlShaderHlsl : public D3DShaderHlsl, public D3DResource, public StdClass
    {
         GN_DECLARE_STDCLASS( D3DPxlShaderHlsl, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        D3DPxlShaderHlsl( D3DRenderer & r )
            : D3DShaderHlsl(PIXEL_SHADER)
            , D3DResource(r)
        { clear(); }
        virtual ~D3DPxlShaderHlsl() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init( const StrA & code, const StrA & entry );
        void quit();
        bool ok() const { return MyParent::ok(); }
    private:
        void clear() { mCode.clear(); mConstTable = 0; mD3DShader = 0; }
        //@}

        // ********************************
        // from Shader
        // ********************************
    public:

        // ********************************
        // from D3DResource
        // ********************************
    public:
        bool deviceCreate();
        bool deviceRestore() { return true; }
        void deviceDispose() {}
        void deviceDestroy();

        // ********************************
        // from D3DBasicShader
        // ********************************
    public:

        void apply() const;
        void applyDirtyUniforms() const;

        // ********************************
        // private variables
        // ********************************
    private:
        StrA                   mCode;
        StrA                   mEntry;
        LPD3DXCONSTANTTABLE    mConstTable;
        LPDIRECT3DPIXELSHADER9 mD3DShader;

        // ********************************
        // private functions
        // ********************************
    private:
        bool queryDeviceUniform( const char * name, HandleType & userData ) const;
    };
}}

#if GN_ENABLE_INLINE
#include "d3dVtxShaderAsm.inl"
#include "d3dPxlShaderAsm.inl"
#endif

// *****************************************************************************
//                           End of d3dShader.h
// *****************************************************************************
#endif // __GN_GFXD3D_D3DSHADER_H__
