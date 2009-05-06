#include "pch.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.util.LineRenderer");

static const char * glslvscode=
    "varying vec4 color; \n"
    "void main() { \n"
    "   // NOTE: GLSL matrix is colomn major \n"
    "   mat4 pvw = mat4(gl_MultiTexCoord0, gl_MultiTexCoord1, gl_MultiTexCoord2, gl_MultiTexCoord3); \n"
    "   gl_Position = gl_Vertex * pvw; \n"
    "   color       = gl_Color; \n"
    "}";

static const char * glslpscode=
    "varying vec4 color; \n"
    "void main() { \n"
    "   gl_FragColor = color; \n"
    "}";

static const char * hlslvscode=
    "struct VSIN { \n"
    "   float4 pos    : POSITION0; \n"
    "   float4 clr    : COLOR; \n"
    "   float4 pvw_r0 : TEXCOORD0; \n"
    "   float4 pvw_r1 : TEXCOORD1; \n"
    "   float4 pvw_r2 : TEXCOORD2; \n"
    "   float4 pvw_r3 : TEXCOORD3; \n"
    "}; \n"
    "struct VSOUT { \n"
    "   float4 pos    : POSITION; \n"
    "   float4 clr    : COLOR; \n"
    "}; \n"
    "VSOUT main( in VSIN i ) { \n"
    "   float4x4 pvw = { i.pvw_r0, i.pvw_r1, i.pvw_r2, i.pvw_r3 }; \n"
    "   VSOUT o; \n"
    "   o.pos = mul( pvw, i.pos ); \n"
    "   o.clr = i.clr; \n"
    "   return o; \n"
    "}";

static const char * hlslpscode=
    "struct VSIO { \n"
    "   float4 pos : POSITION; \n"
    "   float4 clr : COLOR; \n"
    "}; \n"
    "float4 main( in VSIO i ) : COLOR0 { \n"
    "   return i.clr; \n"
    "}";

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::LineRenderer::init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::LineRenderer, () );

    // create GPU program
    const RendererCaps & caps = mRenderer.getCaps();
    GpuProgramDesc gpd;
    if( caps.vsLanguages & GpuProgramLanguage::GLSL &&
        caps.psLanguages & GpuProgramLanguage::GLSL )
    {
        gpd.lang = GpuProgramLanguage::GLSL;
        gpd.vs.source = glslvscode;
        gpd.ps.source = glslpscode;
    }
    else if( caps.vsLanguages & GpuProgramLanguage::HLSL9 &&
             caps.psLanguages & GpuProgramLanguage::HLSL9 )
    {
        gpd.lang = GpuProgramLanguage::HLSL9;
        gpd.vs.source = hlslvscode;
        gpd.vs.entry = "main";
        gpd.ps.source = hlslpscode;
        gpd.ps.entry = "main";
    }
    else
    {
        GN_ERROR(sLogger)( "Sprite renderer requires either GLSL or HLSL support from graphics hardware." );
        return failure();
    }
    mContext.gpuProgram.attach( mRenderer.createGpuProgram( gpd ) );
    if( !mContext.gpuProgram ) return failure();

    // create vertex format
    mContext.vtxfmt.numElements = 6;
    mContext.vtxfmt.elements[0].stream = 0;
    mContext.vtxfmt.elements[0].offset = GN_FIELD_OFFSET( LineVertex, pos );
    mContext.vtxfmt.elements[0].format = ColorFormat::FLOAT3;
    mContext.vtxfmt.elements[0].bindTo( "position", 0 );
    mContext.vtxfmt.elements[1].stream = 0;
    mContext.vtxfmt.elements[1].offset = GN_FIELD_OFFSET( LineVertex, colorInRGBA );
    mContext.vtxfmt.elements[1].format = ColorFormat::RGBA32;
    mContext.vtxfmt.elements[1].bindTo( "color", 0 );
    mContext.vtxfmt.elements[2].stream = 0;
    mContext.vtxfmt.elements[2].offset = GN_FIELD_OFFSET( LineVertex, pvw );
    mContext.vtxfmt.elements[2].format = ColorFormat::FLOAT4;
    mContext.vtxfmt.elements[2].bindTo( "texcoord", 0 );
    mContext.vtxfmt.elements[3].stream = 0;
    mContext.vtxfmt.elements[3].offset = GN_FIELD_OFFSET( LineVertex, pvw ) + sizeof(Vector4f);
    mContext.vtxfmt.elements[3].format = ColorFormat::FLOAT4;
    mContext.vtxfmt.elements[3].bindTo( "texcoord", 1 );
    mContext.vtxfmt.elements[4].stream = 0;
    mContext.vtxfmt.elements[4].offset = GN_FIELD_OFFSET( LineVertex, pvw ) + sizeof(Vector4f) * 2;
    mContext.vtxfmt.elements[4].format = ColorFormat::FLOAT4;
    mContext.vtxfmt.elements[4].bindTo( "texcoord", 2 );
    mContext.vtxfmt.elements[5].stream = 0;
    mContext.vtxfmt.elements[5].offset = GN_FIELD_OFFSET( LineVertex, pvw ) + sizeof(Vector4f) * 3;
    mContext.vtxfmt.elements[5].format = ColorFormat::FLOAT4;
    mContext.vtxfmt.elements[5].bindTo( "texcoord", 3 );

    // create vertex buffer
    mContext.vtxbufs[0].attach( mRenderer.createVtxBuf( MAX_LINES * sizeof(Line), true ) );
    if( !mContext.vtxbufs[0] ) return failure();
    mContext.strides[0] = sizeof(LineVertex);

    // create line buffer
    mLines = (Line*)heapAlloc( MAX_LINES * sizeof(Line) );
    if( NULL == mLines ) return failure();
    mNextPendingLine = mLines;
    mNextFreeLine = mLines;

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::LineRenderer::quit()
{
    GN_GUARD;

    heapFree( mLines ); mLines = NULL;
    mContext.clear();

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::LineRenderer::drawLines(
    const void *      positions,
    size_t            stride,
    size_t            numpoints,
    UInt32            colorInRgba,
    const Matrix44f & projViewWorld )
{
    size_t numNewLines = numpoints / 2;

    if( numNewLines + mNextFreeLine > mLines + MAX_LINES )
    {
        // there's no enough space to hold incoming lines, flush the pending buffer.
        flush();
    }

    const UInt8 * positionsU8 = (const UInt8*)positions;

    // handle line buffer longer than maxinum length
    for( size_t i = 0; i < numNewLines / MAX_LINES; ++i )
    {
        drawLines(
            positionsU8,
            stride,
            MAX_LINES * 2,
            colorInRgba,
            projViewWorld );

        positionsU8 += MAX_LINES * 2 * stride;
    }

    size_t remainingNewLines = numNewLines % MAX_LINES;
    GN_ASSERT( remainingNewLines + mNextFreeLine <= mLines + MAX_LINES );
    for( size_t i = 0; i < remainingNewLines; ++i )
    {
        mNextFreeLine->v0.pos = *(const Vector3f*)positionsU8;
        mNextFreeLine->v0.colorInRGBA = colorInRgba;
        mNextFreeLine->v0.pvw = projViewWorld;

        mNextFreeLine->v1.pos = *(const Vector3f*)(positionsU8 + stride);
        mNextFreeLine->v1.colorInRGBA = colorInRgba;
        mNextFreeLine->v1.pvw = projViewWorld;

        // next line segement
        positionsU8 += stride * 2;
        ++mNextFreeLine;
    }

    if( !mBatchingModeEnabled ) flush();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::LineRenderer::flush()
{
    size_t numPendingLines = mNextFreeLine - mNextPendingLine;
    if( 0 == numPendingLines ) return;

    size_t firstPendingLineOffset = mNextPendingLine - mLines;

    mContext.vtxbufs[0]->update(
        firstPendingLineOffset * sizeof(Line),
        numPendingLines * sizeof(Line),
        mNextPendingLine,
        mLines == mNextPendingLine ? SurfaceUpdateFlag::DISCARD : SurfaceUpdateFlag::NO_OVERWRITE );

    mRenderer.bindContext( mContext );

    mRenderer.draw(
        PrimitiveType::LINE_LIST,
        numPendingLines * 2,       // numvtx
        firstPendingLineOffset * 2 // startvtx,
        );

    if( mNextFreeLine == mLines + MAX_LINES ) mNextFreeLine = mLines; // rewind next-free pointer if needed.
    mNextPendingLine = mNextFreeLine;
}
