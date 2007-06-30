#include "pch.h"
#include "testD3D9Hlsl.h"

using namespace GN;
using namespace GN::gfx;

// *****************************************************************************
// local code
// *****************************************************************************

struct Vertex
{
    float x, y, z;
    float n[3];
    float u, v;
};

static const char * vs_code =
"uniform float4x4 gPvw : register(c0);      \n"
"struct vsi                                 \n"
"{                                          \n"
"    float4 pos : POSITION;                 \n"
"    float3 nml : NORMAL;                   \n"
"    float2 uv  : TEXCOORD0;                \n"
"};                                         \n"
"struct vso                                 \n"
"{                                          \n"
"    float4 pos : POSITION;                 \n"
"    float4 clr : COLOR0;                   \n"
"    float2 uv  : TEXCOORD0;                \n"
"};                                         \n"
"vso main( vsi i )                          \n"
"{                                          \n"
"    vso o;                                 \n"
"    o.pos = mul( i.pos, gPvw );            \n"
"    float3 n = 2*abs(i.nml) + i.nml;       \n"
"    o.clr = float4( n/3.0, 1.0 );          \n"
"    o.uv = i.uv;                           \n"
"    return o;                              \n"
"};";

static const char * ps_code =
"struct vso                     \n"
"{                              \n"
"    float4 pos : POSITION;     \n"
"    float4 clr : COLOR0;       \n"
"};                             \n"
"float4 main( vso i ) : COLOR0  \n"
"{                              \n"
"	return i.clr;               \n"
"}";

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool TestD3D9Hlsl::init( GraphicsSystem & gs )
{
    // initialize matrix
    Matrix44f view, proj;
    view.translate( 0, 0, 200 );
    proj.perspectiveD3DLh( 1.0f, 4.0f/3.0f, 10.0f, 1000.0f );
    mProjView = proj * view;
    mArcBall.setHandness( util::LEFT_HAND );
    mArcBall.setViewMatrix( view );
    mArcBall.connectToInput();
    mArcBall.setMouseMoveWindow( 0, 0, gs.getDesc().width, gs.getDesc().height );

    // create cube mesh
    static Vertex vertices[24];
    static UInt16 indices[36];
    createBox(
        100, 100, 100, // box size
        &vertices[0].x, sizeof(Vertex),
        &vertices[0].u, sizeof(Vertex),
         vertices[0].n, sizeof(Vertex),
        0, 0, 0, 0, // tangend and binormal
        indices,
        0 // quad list
        );

    // create kernel
    mKernel = gs.getKernel( "D3D9_HLSL" );
    if( 0 == mKernel ) return false;

    // create parameters
    mParam = mKernel->createParameterSet();
    if( 0 == mParam ) return false;
    mParam->setParameter( "VS", vs_code );
    mParam->setParameter( "PS", ps_code );
    //mParam->setParameter( "gPvw", ... );
    mParam->setParameter( "PRIM_TYPE", 4 ); // D3DPT_TRIANGLELIST
    mParam->setParameter( "PRIM_COUNT", 12 );
    mParam->setParameter( "BASE_VERTEX", 0 );
    mParam->setParameter( "VERTEX_COUNT", 24 );
    mParam->setParameter( "BASE_INDEX", 0 );

    // create vertex buffer
    SurfaceCreationParameter scp;
    scp.bindings.append( SurfaceBindingParameter("D3D9_HLSL","VTXBUF0") );
    scp.forcedAccessFlags = SURFACE_ACCESS_HOST_WRITE;
    scp.layout.dim = SURFACE_DIMENSION_1D;
    scp.layout.levels = 1;
    scp.layout.faces = 1;
    scp.layout.basemap.width = GN_ARRAY_COUNT(vertices);
    scp.layout.basemap.height = 1;
    scp.layout.basemap.depth = 1;
    scp.layout.basemap.rowBytes = sizeof(vertices);
    scp.layout.basemap.sliceBytes = scp.layout.basemap.rowBytes;
    scp.layout.format.attribs[0].semantic.set( "POS0" );
    scp.layout.format.attribs[0].offset = 0;
    scp.layout.format.attribs[0].format = FMT_FLOAT3;
    scp.layout.format.attribs[1].semantic.set( "NML0" );
    scp.layout.format.attribs[1].offset = 12;
    scp.layout.format.attribs[1].format = FMT_FLOAT3;
    scp.layout.format.attribs[2].semantic.set( "TEX0" );
    scp.layout.format.attribs[2].offset = 24;
    scp.layout.format.attribs[2].format = FMT_FLOAT2;
    scp.layout.format.count = 3;
    scp.layout.format.stride = sizeof(Vertex);
    mVtxBuf = gs.createSurface( scp );
    if( 0 == mVtxBuf ) return false;
    mVtxBuf->download(
        0,
        0,
        vertices,
        sizeof(vertices),
        sizeof(vertices) );

    // create index buffer
    scp.bindings.clear();
    scp.bindings.append( SurfaceBindingParameter("D3D9_HLSL","IDXBUF") );
    scp.forcedAccessFlags = SURFACE_ACCESS_HOST_WRITE;
    scp.layout.dim = SURFACE_DIMENSION_1D;
    scp.layout.levels = 1;
    scp.layout.faces = 1;
    scp.layout.basemap.width = GN_ARRAY_COUNT(indices);
    scp.layout.basemap.height = 1;
    scp.layout.basemap.depth = 1;
    scp.layout.basemap.rowBytes = sizeof(indices);
    scp.layout.basemap.sliceBytes = scp.layout.basemap.rowBytes;
    scp.layout.format.attribs[0].semantic.set( "INDEX" );
    scp.layout.format.attribs[0].offset = 0;
    scp.layout.format.attribs[0].format = FMT_R_16_UINT;
    scp.layout.format.count = 1;
    scp.layout.format.stride = sizeof(short);
    mIdxBuf = gs.createSurface( scp );
    if( 0 == mIdxBuf ) return false;
    mIdxBuf->download(
        0,
        0,
        indices,
        sizeof(indices),
        sizeof(indices) );

    // create binding
    KernelBindingDesc bd;
    bd.bindings["VTXBUF0"].set( mVtxBuf, 0, 1, 0, 1 );
    bd.bindings["IDXBUF"].set( mIdxBuf, 0, 1, 0, 1 );
    mBinding = mKernel->createBinding( bd );
    if( 0 == mBinding ) return false;

    // success
    return true;
}

//
//
// -----------------------------------------------------------------------------
void TestD3D9Hlsl::quit( GraphicsSystem & )
{
    if( mBinding )
    {
        GN_ASSERT( mKernel );
        mKernel->deleteBinding( mBinding );
        mBinding = 0;
    }
    safeDelete( mVtxBuf );
    safeDelete( mIdxBuf );
    safeDelete( mTexture );
    safeDelete( mParam );
    mKernel = 0;
}

//
//
// -----------------------------------------------------------------------------
void TestD3D9Hlsl::draw( GraphicsSystem & )
{
    GN_ASSERT( mKernel );
    GN_ASSERT( mParam );

    Matrix44f world = mArcBall.getRotationMatrix44();
    Matrix44f pvw = mProjView * world;

    mParam->setParameter( "VSCF", pvw );

    mKernel->render( *mParam, mBinding );
}
