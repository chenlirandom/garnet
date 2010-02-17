#include "pch.h"

#if GN_MSVC
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#endif // GN_MSVC


static GN::Logger * sLogger = GN::GetLogger("GN.d3d11.d3d11ShaderUtils");

// *****************************************************************************
// local function
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
static GN::StrA sAddLineCount( const GN::StrA & in )
{
    using namespace GN;

    GN::StrA out( "(  1) : " );

    int line = 1;
    for( const char * s = in.GetRawPtr(); *s; ++s )
    {
        if( '\n' == *s )
        {
            out.Append( StringFormat( "\n(%3d) : ", ++line ) );
        }
        else
        {
            out.Append( *s );
        }
    }

    return out;
}

//
//
// -----------------------------------------------------------------------------
static UInt32 sRefineFlags( UInt32 flags )
{
#if GN_BUILD_DEBUG
    flags |= D3D10_SHADER_DEBUG;
#endif
    return flags;
}

//
//
// -----------------------------------------------------------------------------
static void sPrintShaderCompileError( const char * code, ID3D10Blob * err )
{
    GN_GUARD;

    GN_ERROR(sLogger)(
        "\n================== Shader compile failure ===============\n"
        "%s\n"
        "\n---------------------------------------------------------\n"
        "%s\n"
        "\n=========================================================\n",
        code ? sAddLineCount(code).GetRawPtr() : "Shader code: <EMPTY>",
        err ? (const char*)err->GetBufferPointer() : "Error: <EMPTY>" );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
static void sPrintShaderCompileInfo( const char * hlsl, ID3D10Blob * bin )
{
    GN_GUARD;

    GN_ASSERT( hlsl && bin );

    using namespace GN;

    // get ASM code
    AutoComPtr<ID3D10Blob> asm_;

    D3D10DisassembleShader(
        bin->GetBufferPointer(),
        bin->GetBufferSize(),
        false,
        NULL,
        &asm_ );

    GN_VTRACE(sLogger)(
        "\n================== Shader compile success ===============\n"
        "%s\n"
        "\n---------------------------------------------------------\n"
        "%s\n"
        "\n=========================================================\n",
        sAddLineCount(hlsl).GetRawPtr(),
        sAddLineCount((const char*)asm_->GetBufferPointer()).GetRawPtr() );

    GN_UNGUARD;
}

// *****************************************************************************
// public function
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
ID3D10Blob * GN::d3d11::compileShader(
    const char   * profile,
    const char   * source,
    size_t         len,
    UInt32         flags,
    const char   * entry )
{
    AutoComPtr<ID3D10Blob> bin, err;

    if( NULL == source )
    {
        GN_ERROR(sLogger)( "NULL source pointer." );
        return NULL;
    }

    // determine source length
    if( 0 == len ) len = StringLength(source);

    // generate temporary file to store shader source
    StrA filename;
#if GN_BUILD_DEBUG
    TempFile file;
    if( file.open( "D3D11_shader_source", "wt", TempFile::MANUAL_DELETE ) )
    {
        filename = file.name();
        file.write( source, len, NULL );
        file.close();
    }
#endif

    // Note: D3DX11CompileFromMemory() is a more up to date compiler than
    //       D3D11CompileShader(), since D3D11CompileShader() ships
    //       with runtime only, but D3DXCompileFromMemory() ships with
    //       each DXSDK update.
    if( FAILED( D3DX11CompileFromMemory(
        source,
        len,
        filename.GetRawPtr(),
        0, // defines
        0, // includes
        entry,
        profile,
        sRefineFlags(flags),
        0, // effect compile flags
        0, // thread pump
        &bin,
        &err,
        0 ) ) )
    {
        sPrintShaderCompileError( source, err );
        return NULL;
    }

    sPrintShaderCompileInfo( source, bin );

    return bin.detach();
}

//
//
// -----------------------------------------------------------------------------
ID3D11VertexShader * GN::d3d11::compileAndCreateVS(
    ID3D11Device & dev,
    const char   * source,
    size_t         len,
    UInt32         flags,
    const char   * entry,
    const char   * profile,
    ID3D10Blob  ** signature )
{
    GN_GUARD;

    AutoComPtr<ID3D10Blob> bin( compileShader( profile, source, len, flags, entry ) );
    if( !bin ) return NULL;

    ID3D11VertexShader * vs = createDumpableVS( dev, bin->GetBufferPointer(), bin->GetBufferSize() );
    if( 0 == vs ) return NULL;

    // success
    if( signature ) *signature = bin.detach();
    return vs;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
ID3D11GeometryShader * GN::d3d11::compileAndCreateGS(
    ID3D11Device & dev,
    const char   * source,
    size_t         len,
    UInt32         flags,
    const char   * entry,
    const char   * profile,
    ID3D10Blob  ** signature )
{
    GN_GUARD;

    AutoComPtr<ID3D10Blob> bin( compileShader( profile, source, len, flags, entry ) );
    if( !bin ) return NULL;

    ID3D11GeometryShader * gs = createDumpableGS( dev, bin->GetBufferPointer(), bin->GetBufferSize() );
    if( 0 == gs ) return NULL;

    // success
    if( signature ) *signature = bin.detach();
    return gs;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
ID3D11PixelShader * GN::d3d11::compileAndCreatePS(
    ID3D11Device & dev,
    const char   * source,
    size_t         len,
    UInt32         flags,
    const char   * entry,
    const char   * profile,
    ID3D10Blob  ** signature )
{
    GN_GUARD;

    AutoComPtr<ID3D10Blob> bin( compileShader( profile, source, len, flags, entry ) );
    if( !bin ) return NULL;

    ID3D11PixelShader * ps = createDumpablePS( dev, bin->GetBufferPointer(), bin->GetBufferSize() );
    if( 0 == ps ) return 0;

    // success
    if( signature ) *signature = bin.detach();
    return ps;

    GN_UNGUARD;
}
