#include "pch.h"
#include "d3dRenderer.h"
#if !GN_ENABLE_INLINE
#include "d3dBufferMgr.inl"
#endif
#include "d3dVtxBuf.h"
#include "d3dIdxBuf.h"

// *****************************************************************************
// local functions
// *****************************************************************************

//!
//! d3d vertex semantic structure
//!
struct VtxSemDesc
{
    D3DDECLUSAGE usage; //!< D3D vertex usage
    BYTE         index; //!< register index
};

//!
//! vertex semantic convert table
//!
static VtxSemDesc sVtxSem2D3D[GN::gfx::NUM_VTXSEMS] =
{
    #define GNGFX_DEFINE_VTXSEM( tag, d3ddecl, d3dindex, glname, glindex, cgname ) \
        { static_cast<D3DDECLUSAGE>(d3ddecl), d3dindex },
    #include "garnet/gfx/vertexSemanticMeta.h"
    #undef  GNGFX_DEFINE_VTXSEM
};

//!
//! convert vertex format to d3d-decl usage
// -----------------------------------------------------------------------------
static inline D3DDECLTYPE sClrFmt2D3D( GN::gfx::ClrFmt fmt )
{
    GN_ASSERT( 0 <= fmt && fmt < GN::gfx::NUM_CLRFMTS );
    switch ( fmt )
    {
        case GN::gfx::FMT_FLOAT1    : return D3DDECLTYPE_FLOAT1;
        case GN::gfx::FMT_FLOAT2    : return D3DDECLTYPE_FLOAT2;
        case GN::gfx::FMT_FLOAT3    : return D3DDECLTYPE_FLOAT3;
        case GN::gfx::FMT_FLOAT4    : return D3DDECLTYPE_FLOAT4;
        case GN::gfx::FMT_D3DCOLOR  : return D3DDECLTYPE_D3DCOLOR;
        case GN::gfx::FMT_SHORT2    : return D3DDECLTYPE_SHORT2;
        case GN::gfx::FMT_UBYTE4N   : return D3DDECLTYPE_UBYTE4N;
        case GN::gfx::FMT_SHORT2N   : return D3DDECLTYPE_SHORT2N;
        case GN::gfx::FMT_SHORT4N   : return D3DDECLTYPE_SHORT4N;
        case GN::gfx::FMT_USHORT2N  : return D3DDECLTYPE_USHORT2N;
        case GN::gfx::FMT_USHORT4N  : return D3DDECLTYPE_USHORT4N;
        case GN::gfx::FMT_UDEC3     : return D3DDECLTYPE_UDEC3;
        case GN::gfx::FMT_DEC3N     : return D3DDECLTYPE_DEC3N;
        case GN::gfx::FMT_FLOAT16_2 : return D3DDECLTYPE_FLOAT16_2;
        case GN::gfx::FMT_FLOAT16_4 : return D3DDECLTYPE_FLOAT16_4;
#if GN_XENON
        case GN::gfx::FMT_INT1      : return D3DDECLTYPE_INT1;
        case GN::gfx::FMT_INT2      : return D3DDECLTYPE_INT2;
        case GN::gfx::FMT_INT4      : return D3DDECLTYPE_INT4;
        case GN::gfx::FMT_UINT1     : return D3DDECLTYPE_UINT1;
        case GN::gfx::FMT_UINT2     : return D3DDECLTYPE_UINT2;
        case GN::gfx::FMT_UINT4     : return D3DDECLTYPE_UINT4;
        case GN::gfx::FMT_INT1N     : return D3DDECLTYPE_INT1N;
        case GN::gfx::FMT_INT2N     : return D3DDECLTYPE_INT2N;
        case GN::gfx::FMT_INT4N     : return D3DDECLTYPE_INT4N;
        case GN::gfx::FMT_UINT1N    : return D3DDECLTYPE_UINT1N;
        case GN::gfx::FMT_UINT2N    : return D3DDECLTYPE_UINT2N;
        case GN::gfx::FMT_UINT4N    : return D3DDECLTYPE_UINT4N;
        case GN::gfx::FMT_UBYTE4    : return D3DDECLTYPE_UBYTE4;
        case GN::gfx::FMT_BYTE4     : return D3DDECLTYPE_BYTE4;
        case GN::gfx::FMT_BYTE4N    : return D3DDECLTYPE_BYTE4N;
        case GN::gfx::FMT_SHORT4    : return D3DDECLTYPE_SHORT4;
        case GN::gfx::FMT_USHORT2   : return D3DDECLTYPE_USHORT2;
        case GN::gfx::FMT_USHORT4   : return D3DDECLTYPE_USHORT4;
        case GN::gfx::FMT_DEC3      : return D3DDECLTYPE_DEC3;
        case GN::gfx::FMT_UDEC3N    : return D3DDECLTYPE_UDEC3N;
        case GN::gfx::FMT_UDEC4     : return D3DDECLTYPE_UDEC4;
        case GN::gfx::FMT_DEC4      : return D3DDECLTYPE_DEC4;
        case GN::gfx::FMT_UDEC4N    : return D3DDECLTYPE_UDEC4N;
        case GN::gfx::FMT_DEC4N     : return D3DDECLTYPE_DEC4N;
        case GN::gfx::FMT_UHEND3    : return D3DDECLTYPE_UHEND3;
        case GN::gfx::FMT_HEND3     : return D3DDECLTYPE_HEND3;
        case GN::gfx::FMT_UHEND3N   : return D3DDECLTYPE_UHEND3N;
        case GN::gfx::FMT_HEND3N    : return D3DDECLTYPE_HEND3N;
        case GN::gfx::FMT_UDHEN3    : return D3DDECLTYPE_UDHEN3;
        case GN::gfx::FMT_DHEN3     : return D3DDECLTYPE_DHEN3;
        case GN::gfx::FMT_UDHEN3N   : return D3DDECLTYPE_UDHEN3N;
        case GN::gfx::FMT_DHEN3N    : return D3DDECLTYPE_DHEN3N;
#endif
        default:
            GN_ERROR( "Invalid color format: %d!", fmt );
            return D3DDECLTYPE(MAXD3DDECLTYPE+1);
    }
}

//!
//! element sorting function
// -----------------------------------------------------------------------------
static bool
sSortByOffset( const D3DVERTEXELEMENT9 & a, const D3DVERTEXELEMENT9 & b )
{
    return a.Offset < b.Offset;
}

//!
//! convert vertdecl structure to a D3D vertex declaration array
// -----------------------------------------------------------------------------
static bool
sVtxFmtDesc2D3DDecl( std::vector<D3DVERTEXELEMENT9> & elements, const GN::gfx::VtxFmtDesc & decl )
{
    GN_GUARD;

    elements.clear();

    for( uint8_t i = 0; i < GN::gfx::NUM_VTXSEMS; ++i )
    {
        const GN::gfx::VtxFmtDesc::AttribDesc & va = decl.attribs[i];

        // ignore unused attribute
        if ( !va.used ) continue;

        D3DVERTEXELEMENT9 elem;

        // set stream index
        elem.Stream = va.stream;

        // set attrib offset
        elem.Offset = va.offset;

        // set method ( no tessellator is used )
        elem.Method = D3DDECLMETHOD_DEFAULT;

        // set attrib semantic
        elem.Usage      = (BYTE)sVtxSem2D3D[i].usage;
        elem.UsageIndex = (BYTE)sVtxSem2D3D[i].index;

        // set attrib format
#if GN_XENON
        elem.Type = sClrFmt2D3D( va.format );
#else
        elem.Type = (BYTE)sClrFmt2D3D( va.format );
#endif

        // add to element array
        elements.push_back( elem );
    }

    // sort elements by offset
    std::sort( elements.begin(), elements.end(), &sSortByOffset );

    // end tag
    D3DVERTEXELEMENT9 endtag = D3DDECL_END();
    elements.push_back( endtag );

    // success
    return true;

    GN_UNGUARD;
}

//!
//! create D3D decl from vertex format structure
// -----------------------------------------------------------------------------
static LPDIRECT3DVERTEXDECLARATION9
sCreateD3DVertexDecl( LPDIRECT3DDEVICE9 dev, const GN::gfx::VtxFmtDesc & format )
{
    GN_GUARD;

    GN_ASSERT( dev );

    std::vector<D3DVERTEXELEMENT9> elements;
    if( !sVtxFmtDesc2D3DDecl( elements, format ) ) return false;
    GN_ASSERT( !elements.empty() );

    LPDIRECT3DVERTEXDECLARATION9 decl;
    GN_DX_CHECK_RV( dev->CreateVertexDeclaration( &elements[0], &decl ), 0 );

    // success
    return decl;

    GN_UNGUARD;
}

struct EqualFormat
{
    const GN::gfx::VtxFmtDesc & format;
    EqualFormat( const GN::gfx::VtxFmtDesc & f ) : format(f) {}
    bool operator()( const GN::gfx::D3DVtxBindingDesc & vbd ) const { return format == vbd.format; }
};

// *****************************************************************************
// device management
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3DRenderer::bufferDeviceRestore()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // recreate all D3D decls
    uint32_t h = mVtxBindings.first();
    while( h )
    {
        mVtxBindings[h].decl.attach( sCreateD3DVertexDecl( mDevice, mVtxBindings[h].format ) );
        if( !mVtxBindings[h].decl ) return false;

        h = mVtxBindings.next( h );
    }

    // create a default binding, if there's no one
    if( mVtxBindings.empty() )
    {
        VtxFmtDesc fmt;
        fmt.addAttrib( 0, 0, VTXSEM_COORD, FMT_FLOAT3 );
        h = createVtxBinding( fmt );
        if( 0 == h ) return false;
        bindVtxBinding( h );
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::bufferDeviceDispose()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    // release all D3D decls
    uint32_t h = mVtxBindings.first();
    while( h )
    {
        mVtxBindings[h].decl.reset();
        h = mVtxBindings.next( h );
    }

    GN_UNGUARD;
}

// *****************************************************************************
// from Renderer
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
uint32_t GN::gfx::D3DRenderer::createVtxBinding( const VtxFmtDesc & format )
{
    GN_GUARD;

    uint32_t h = mVtxBindings.findIf( EqualFormat(format) );

    if( 0 == h )
    {
        // create new vertex decl
        D3DVtxBindingDesc vbd;
        vbd.format = format;
        vbd.decl.attach( sCreateD3DVertexDecl( mDevice, format ) );
        if( !vbd.decl ) return 0;

        h = mVtxBindings.add( vbd );
    }

    // success
    return h;

    GN_UNGUARD;
}


//
//
// -----------------------------------------------------------------------------
GN::gfx::VtxBuf * GN::gfx::D3DRenderer::createVtxBuf(
    size_t bytes, bool dynamic, bool sysCopy, const VtxBufLoader & loader )
{
    GN_GUARD;

    AutoRef<D3DVtxBuf> buf( new D3DVtxBuf(*this) );

    if( !buf->init( bytes, dynamic, sysCopy, loader ) ) return 0;

    return buf.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::IdxBuf * GN::gfx::D3DRenderer::createIdxBuf(
    size_t numIdx, bool dynamic, bool sysCopy, const IdxBufLoader & loader )
{
    GN_GUARD;

    AutoRef<D3DIdxBuf> buf( new D3DIdxBuf(*this) );

    if( !buf->init( numIdx, dynamic, sysCopy, loader ) ) return 0;

    return buf.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::bindVtxBinding( uint32_t handle )
{
    GN_GUARD;

    if( !mVtxBindings.validHandle(handle) )
    {
        GN_ERROR( "invalid vertex binding handle : %d", handle );
        return;
    }

    mDrawState.bindVtxBinding( handle );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::bindVtxBufs( const VtxBuf * const buffers[], size_t start, size_t count )
{
    GN_GUARD_SLOW;

    size_t stage = start;

    for( size_t i = 0; i < count; ++i, ++stage )
    {
        mDrawState.bindVtxBuf( stage, buffers[i], 0 );
    }

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::bindVtxBuf( size_t index, const VtxBuf * buffer, size_t stride )
{
    GN_GUARD_SLOW;

    mDrawState.bindVtxBuf( index, buffer, stride );

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::bindIdxBuf( const IdxBuf * buf )
{
    GN_GUARD_SLOW;

    mDrawState.bindIdxBuf( buf );

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3DRenderer::applyVtxBuffers()
{
    GN_GUARD_SLOW;

    const VtxFmtDesc & vtxFmt = mVtxBindings[mDrawState.vtxBinding].format;
    uint16_t flag = mDrawState.dirtyFlags.vtxBufs;
    UINT i = 0;
    while( flag && i < vtxFmt.numStreams )
    {
        if( flag & 1 )
        {
            // vertex buffer i is dirty, we need to rebind it to device

            const D3DDrawState::VtxBufDesc & vbd = mDrawState.vtxBufs[i];

            if( !vbd.buf )
            {
                GN_DX_CHECK( mDevice->SetStreamSource( i, 0, 0, 0 ) );
            }
            else
            {
                GN_DX_CHECK( mDevice->SetStreamSource(
                    i,
                    safeCast<const D3DVtxBuf*>(vbd.buf.get())->getD3DVb(),
                    0,
                    (UINT)( vbd.stride ? vbd.stride : vtxFmt.streams[i].stride ) ) );
            }
        }
        flag >>= 1;
        ++i;
    }

    GN_UNGUARD_SLOW;
}
