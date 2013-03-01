#include "pch.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.util.LineRenderer");

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

    // create vertex format
    mContext.vtxbind.resize( 6 );
    mContext.vtxbind[0].stream = 0;
    mContext.vtxbind[0].offset = GN_FIELD_OFFSET( LineVertex, pos );
    mContext.vtxbind[0].format = ColorFormat::FLOAT3;
    mContext.vtxbind[1].stream = 0;
    mContext.vtxbind[1].offset = GN_FIELD_OFFSET( LineVertex, colorInRGBA );
    mContext.vtxbind[1].format = ColorFormat::RGBA32;
    mContext.vtxbind[2].stream = 0;
    mContext.vtxbind[2].offset = GN_FIELD_OFFSET( LineVertex, transform );
    mContext.vtxbind[2].format = ColorFormat::FLOAT4;
    mContext.vtxbind[3].stream = 0;
    mContext.vtxbind[3].offset = GN_FIELD_OFFSET( LineVertex, transform ) + sizeof(Vector4f);
    mContext.vtxbind[3].format = ColorFormat::FLOAT4;
    mContext.vtxbind[4].stream = 0;
    mContext.vtxbind[4].offset = GN_FIELD_OFFSET( LineVertex, transform ) + sizeof(Vector4f) * 2;
    mContext.vtxbind[4].format = ColorFormat::FLOAT4;
    mContext.vtxbind[5].stream = 0;
    mContext.vtxbind[5].offset = GN_FIELD_OFFSET( LineVertex, transform ) + sizeof(Vector4f) * 3;
    mContext.vtxbind[5].format = ColorFormat::FLOAT4;

    // create GPU program
    const GpuCaps & caps = mGpu.caps();
    GpuProgramDesc gpd;
    if( caps.shaderModels & ShaderModel::GLSL_1_00 )
    {
        gpd.lang = GpuProgramLanguage::GLSL;
        gpd.shaderModels = ShaderModel::GLSL_1_00;
        gpd.vs.source = glslvscode;
        gpd.ps.source = glslpscode;

        mContext.gpuProgram.attach( mGpu.createGpuProgram( gpd ) );
        if( !mContext.gpuProgram ) return failure();

        const GpuProgramParameterDesc & gppd = mContext.gpuProgram->getParameterDesc();

        mContext.vtxbind[0].attribute = gppd.attributes["gl_Vertex"];
        mContext.vtxbind[1].attribute = gppd.attributes["gl_Color"];
        mContext.vtxbind[2].attribute = gppd.attributes["gl_MultiTexCoord0"];
        mContext.vtxbind[3].attribute = gppd.attributes["gl_MultiTexCoord1"];
        mContext.vtxbind[4].attribute = gppd.attributes["gl_MultiTexCoord2"];
        mContext.vtxbind[5].attribute = gppd.attributes["gl_MultiTexCoord3"];
    }
    else if( caps.shaderModels & ShaderModel::SM_2_0 )
    {
        gpd.lang = GpuProgramLanguage::HLSL9;
        gpd.shaderModels = ShaderModel::SM_2_0;
        gpd.vs.source = hlslvscode;
        gpd.vs.entry = "main";
        gpd.ps.source = hlslpscode;
        gpd.ps.entry = "main";

        mContext.gpuProgram.attach( mGpu.createGpuProgram( gpd ) );
        if( !mContext.gpuProgram ) return failure();

        const GpuProgramParameterDesc & gppd = mContext.gpuProgram->getParameterDesc();

        mContext.vtxbind[0].attribute = gppd.attributes["POSITION0"];
        mContext.vtxbind[1].attribute = gppd.attributes["COLOR0"];
        mContext.vtxbind[2].attribute = gppd.attributes["TEXCOORD0"];
        mContext.vtxbind[3].attribute = gppd.attributes["TEXCOORD1"];
        mContext.vtxbind[4].attribute = gppd.attributes["TEXCOORD2"];
        mContext.vtxbind[5].attribute = gppd.attributes["TEXCOORD3"];
    }
    else
    {
        GN_ERROR(sLogger)( "Sprite renderer requires either GLSL or HLSL support from graphics hardware." );
        return failure();
    }

    // create vertex buffer
    mContext.vtxbufs[0].vtxbuf.attach( mGpu.createVtxBuf( MAX_LINES * sizeof(Line), true ) );
    if( !mContext.vtxbufs[0].vtxbuf ) return failure();
    mContext.vtxbufs[0].stride = sizeof(LineVertex);

    // create line buffer
    mLines = (Line*)HeapMemory::alloc( MAX_LINES * sizeof(Line) );
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

    HeapMemory::dealloc( mLines ); mLines = NULL;
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
    uint32            colorInRgba,
    const Matrix44f & transform )
{
    if( 0 == stride ) stride = sizeof(float)*3;

    size_t numNewLines = numpoints / 2;

    const uint8 * positionsU8 = (const uint8*)positions;

    if( numNewLines > MAX_LINES )
    {
        // handle line buffer longer than maxinum length.
        for( size_t i = 0; i < numNewLines / MAX_LINES; ++i )
        {
            drawLines(
                positionsU8,
                stride,
                MAX_LINES * 2,
                colorInRgba,
                transform );

            positionsU8 += MAX_LINES * 2 * stride;
        }
        numNewLines %= MAX_LINES;
    }

    GN_ASSERT( numNewLines <= MAX_LINES );

    if( numNewLines + mNextFreeLine > mLines + MAX_LINES )
    {
        // there's no enough space to hold all incoming lines. So flush first.
        flush();
    }

    GN_ASSERT( numNewLines + mNextFreeLine <= mLines + MAX_LINES );

    for( size_t i = 0; i < numNewLines; ++i )
    {
        GN_ASSERT( mLines <= mNextFreeLine && mNextFreeLine < (mLines + MAX_LINES) );
        mNextFreeLine->v0.pos = *(const Vector3f*)positionsU8;
        mNextFreeLine->v0.colorInRGBA = colorInRgba;
        mNextFreeLine->v0.transform = transform;

        mNextFreeLine->v1.pos = *(const Vector3f*)(positionsU8 + stride);
        mNextFreeLine->v1.colorInRGBA = colorInRgba;
        mNextFreeLine->v1.transform = transform;

        // next line segement
        positionsU8 += stride * 2;
        ++mNextFreeLine;
    }

    if( !mBatchingModeEnabled ) flush();
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::LineRenderer::drawBox(
    const Boxf      & box,
    uint32            colorInRgba,
    const Matrix44f & transform )
{
    Vector3f points[] =
    {
        box.corner( 0 ), box.corner( 1 ),
        box.corner( 1 ), box.corner( 2 ),
        box.corner( 2 ), box.corner( 3 ),
        box.corner( 3 ), box.corner( 0 ),

        box.corner( 0 ), box.corner( 4 ),
        box.corner( 1 ), box.corner( 5 ),
        box.corner( 2 ), box.corner( 6 ),
        box.corner( 3 ), box.corner( 7 ),

        box.corner( 4 ), box.corner( 5 ),
        box.corner( 5 ), box.corner( 6 ),
        box.corner( 6 ), box.corner( 7 ),
        box.corner( 7 ), box.corner( 4 ),
    };

    drawLines( points, sizeof(Vector3f), GN_ARRAY_COUNT(points), colorInRgba, transform );
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::LineRenderer::flush()
{
    size_t numPendingLines = mNextFreeLine - mNextPendingLine;
    if( 0 == numPendingLines ) return;

    size_t firstPendingLineOffset = mNextPendingLine - mLines;

    GN_ASSERT( firstPendingLineOffset + numPendingLines <= MAX_LINES );

    // copy render targets from current context
    mContext.colortargets = mGpu.getContext().colortargets;
    mContext.depthstencil = mGpu.getContext().depthstencil;

    mContext.vtxbufs[0].vtxbuf->update(
        (uint32)(firstPendingLineOffset * sizeof(Line)),
        (uint32)(numPendingLines * sizeof(Line)),
        mNextPendingLine,
        mLines == mNextPendingLine ? SurfaceUpdateFlag::DISCARD : SurfaceUpdateFlag::NO_OVERWRITE );

    mGpu.bindContext( mContext );

    mGpu.draw(
        PrimitiveType::LINE_LIST,
        (uint32)(numPendingLines * 2),       // numvtx
        (uint32)(firstPendingLineOffset * 2) // startvtx,
        );

    // rewind all pointers
    mNextFreeLine = mLines;
    mNextPendingLine = mLines;
}

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::ThickLineRenderer::ThickLineRenderer()
{
    clear();
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ThickLineRenderer::init( Gpu & g )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::ThickLineRenderer, () );

    // initialize shaders
    const GpuCaps & caps = g.caps();
    GpuProgramDesc gpd;
    if( caps.shaderModels & ShaderModel::SM_2_0 )
    {
        const char * hlslvscode =
            "struct VSIO { \n"
            "   float4 pos : POSITION0; \n"
            "   float2 tex : TEXCOORD0; \n"
            "   float4 clr : COLOR0;    \n"
            "}; \n"
            "VSIO main( VSIO i ) { \n"
            "   VSIO o = i; \n"
            "   return o; \n"
            "}";

        const char * hlslpscode =
            "struct VSIO { \n"
            "   float4 pos : POSITION0; \n"
            "   float2 tex : TEXCOORD0; \n"
            "   float4 clr : COLOR0;    \n"
            "}; \n"
            "float4 main( VSIO i ) : COLOR0 { \n"
            "   return i.clr; \n"
            "}";

        gpd.lang = GpuProgramLanguage::HLSL9;
        gpd.shaderModels = ShaderModel::SM_2_0;
        gpd.vs.source = hlslvscode;
        gpd.vs.entry = "main";
        gpd.ps.source = hlslpscode;
        gpd.ps.entry = "main";

        mContext.gpuProgram.attach( g.createGpuProgram( gpd ) );
        if( !mContext.gpuProgram ) return failure();

        const GpuProgramParameterDesc & gppd = mContext.gpuProgram->getParameterDesc();

        mContext.vtxbind[0].attribute = gppd.attributes["POSITION0"];
        mContext.vtxbind[1].attribute = gppd.attributes["COLOR0"];
        mContext.vtxbind[2].attribute = gppd.attributes["TEXCOORD0"];
    }
    else if( caps.shaderModels & ShaderModel::GLSL_1_00 )
    {
        static const char * glslvscode=
            "varying vec4 color; \n"
            "varying vec2 texcoords; \n"
            "void main() { \n"
            "   gl_Position = gl_Vertex; \n"
            "   color       = gl_Color; \n"
            "   texcoords   = gl_MultiTexCoord0.xy; \n"
            "}";

        static const char * glslpscode=
            "varying vec4 color; \n"
            "varying vec2 texcoords; \n"
            "void main() { \n"
            "   gl_FragColor = color; \n"
            "}";

        gpd.lang = GpuProgramLanguage::GLSL;
        gpd.shaderModels = ShaderModel::GLSL_1_00;
        gpd.vs.source = glslvscode;
        gpd.ps.source = glslpscode;

        mContext.gpuProgram.attach( g.createGpuProgram( gpd ) );
        if( !mContext.gpuProgram ) return failure();

        const GpuProgramParameterDesc & gppd = mContext.gpuProgram->getParameterDesc();

        mContext.vtxbind[0].attribute = gppd.attributes["gl_Vertex"];
        mContext.vtxbind[1].attribute = gppd.attributes["gl_Color"];
        mContext.vtxbind[2].attribute = gppd.attributes["gl_MultiTexCoord0"];
    }
    else
    {
        GN_ERROR(sLogger)( "Sprite renderer requires either GLSL or HLSL support from graphics hardware." );
        return failure();
    }

    // create vertex format
    mContext.vtxbind.resize( 3 );
    mContext.vtxbind[0].stream = 0;
    mContext.vtxbind[0].offset = 0;
    mContext.vtxbind[0].format = ColorFormat::FLOAT4;
    mContext.vtxbind[1].stream = 0;
    mContext.vtxbind[1].offset = 16;
    mContext.vtxbind[1].format = ColorFormat::RGBA32;
    mContext.vtxbind[2].stream = 0;
    mContext.vtxbind[2].offset = 20;
    mContext.vtxbind[2].format = ColorFormat::FLOAT2;

    // initialize index buffer
    const uint16 numpoly = MAX_VERTICES / 6;
    const uint16 numidx  = numpoly * 12; // 12 indices (4 triangles) per 6 verices
    uint16 indices[numidx];
    for( uint16 i = 0; i < numpoly; ++i )
    {
        uint16 * p = indices + i * 12;

        uint16 v = i * 6;

        // v1-v2
        // |   \
        // v0   \
        //  \    \
        //   \    \
        //    \    \
        //     \    v3
        //      \   |
        //       v5-v4

        *p++ = v + 0;
        *p++ = v + 1;
        *p++ = v + 2;

        *p++ = v + 0;
        *p++ = v + 2;
        *p++ = v + 3;

        *p++ = v + 0;
        *p++ = v + 3;
        *p++ = v + 5;

        *p++ = v + 3;
        *p++ = v + 4;
        *p++ = v + 5;
    }
    mContext.idxbuf.attach(g.createIdxBuf16(numidx));
    if( !mContext.idxbuf ) return failure();
    mContext.idxbuf->update(0, numidx, indices);

    // create vertex buffer
    mContext.vtxbufs[0].vtxbuf.attach( g.createVtxBuf( sizeof(m_Vertices), true ) );
    if( !mContext.vtxbufs[0].vtxbuf ) return failure();
    mContext.vtxbufs[0].stride = sizeof(PrivateVertex);

    // done
    mGpu = &g;
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::quit()
{
    GN_GUARD;

    mContext.clear();

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::ThickLineRenderer::drawBegin( const ThickLineParameters & parameters )
{
    if( m_Drawing )
    {
        GN_ERROR(sLogger)( "Unpaired drawBegin()" );
        return false;
    }

    const GpuContext & gc = mGpu->getContext();

    // copy current render targets
    mContext.colortargets = mGpu->getContext().colortargets;
    mContext.depthstencil = mGpu->getContext().depthstencil;

    // setup line parameters
    (ThickLineParameters&)m_Parameters = parameters;
    uint32 rtw = 0, rth = 0;
    if (gc.rs.viewport.w == 0 || gc.rs.viewport.h == 0)
    {
        mGpu->getCurrentRenderTargetSize(&rtw, &rth);
    }
    float vpw = (0 == gc.rs.viewport.w) ? (float)rtw : (float)gc.rs.viewport.w;
    float vph = (0 == gc.rs.viewport.h) ? (float)rth : (float)gc.rs.viewport.h;

    m_Parameters.wvp = parameters.proj * parameters.worldview;
    m_Parameters.screenWidth = vpw;
    m_Parameters.screenHeight = vph;
    m_Parameters.endPointHalfWidth = (float)m_Parameters.width / vpw;
    m_Parameters.endPointHalfHeight = (float)m_Parameters.width / vph;

    m_Drawing = true;
    m_NumVertices = 0;
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::drawEnd()
{
    if( !m_Drawing )
    {
        GN_ERROR(sLogger)( "Unpaired drawEnd()" );
        return;
    }

    flush();
    m_Drawing = false;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::line( const ThickLineVertex & v0, const ThickLineVertex & v1 )
{
    if( !m_Drawing )
    {
        GN_ERROR(sLogger)( "ThickLineRenderer::line() must be called in between ThickLineRenderer::drawBegin() and ThickLineRenderer::drawEnd()." );
        return;
    }

    EndPoint e0, e1;
    calcEndPoint( e0, v0 );
    calcEndPoint( e1, v1 );

    PrivateVertex * v = newPolygon6();

    if( e0.post * e1.posw < e1.post * e0.posw )
    {
        if( e0.posl * e1.posw < e1.posl * e0.posw )
        {
            // e1 in quadrant 1

            //     ----
            //    / e1|
            //   /    /
            //  /    /
            // /    /
            // |e0 /
            // ----

            e0.bottomRight( v++ );
            e0.bottomLeft( v++ );
            e0.topLeft( v++ );
            e1.topLeft( v++ );
            e1.topRight( v++ );
            e1.bottomRight( v++ );
        }
        else
        {
            // e1 in quadrant 2

            // ----
            // |e1 \
            // \    \
            //  \    \
            //   \    \
            //    \ e0|
            //     ----

            e0.topRight( v++ );
            e0.bottomRight( v++ );
            e0.bottomLeft( v++ );
            e1.bottomLeft( v++ );
            e1.topLeft( v++ );
            e1.topRight( v++ );
        }
    }
    else
    {
        if( e0.posl * e1.posw < e1.posl * e0.posw )
        {
            // e1 in quadrant 4

            // ----
            // |e0 \
            // \    \
            //  \    \
            //   \    \
            //    \ e1|
            //     ----

            e0.bottomLeft( v++ );
            e0.topLeft( v++ );
            e0.topRight( v++ );
            e1.topRight( v++ );
            e1.bottomRight( v++ );
            e1.bottomLeft( v++ );
        }
        else
        {
            // e1 in quadrant 3

            //     ----
            //    / e0|
            //   /    /
            //  /    /
            // /    /
            // |e1 /
            // ----

            e0.topLeft( v++ );
            e0.topRight( v++ );
            e0.bottomRight( v++ );
            e1.bottomRight( v++ );
            e1.bottomLeft( v++ );
            e1.topLeft( v++ );
        }
    }
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::line( float x1, float y1, float z1, float x2, float y2, float z2, uint32 color )
{
    ThickLineVertex v[2] =
    {
        { x1, y1, z1, 0, 0, color },
        { x2, y2, z2, 1, 1, color },
    };
    lineList( v, 2 );
}


//
//
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::lineList( const ThickLineVertex * vertices, size_t numverts )
{
    size_t numLines = numverts / 2;
    for( size_t i = 0; i < numLines; ++i )
    {
        line( vertices[i*2+0], vertices[i*2+1] );
    }
}

// *****************************************************************************
// private methods
// *****************************************************************************

//
// Expand thick line vertex to a quad
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::calcEndPoint(
    EndPoint              & endpoint,
    const ThickLineVertex & vertex )
{
    // determine center position and end point size
    Vector4f center;
    float half_w;
    float half_h;
    if( m_Parameters.widthInScreenSpace )
    {
        // translate vertex into clip space
        center.set( vertex.x, vertex.y, vertex.z, 1.0f );
        center = m_Parameters.wvp * center;

        // get end point positions in clip space
        half_w = m_Parameters.endPointHalfWidth * center.w;
        half_h = m_Parameters.endPointHalfHeight * center.w;
    }
    else
    {
        // translate vertex into view space
        center.set( vertex.x, vertex.y, vertex.z, 1.0f );
        center = m_Parameters.worldview * center;

        // get position of left-top corner in view space
        float half_size = m_Parameters.width / 2.0f * center.w;
        Vector4f topleft = center + Vector4f( -half_size, half_size, 0.0f, 0.0f );

        // translate both center and left top into clip space
        center = m_Parameters.proj * center;
        topleft = m_Parameters.proj * topleft;

        half_w = fabs( topleft.x - center.x );
        half_h = fabs( topleft.y - center.y );

        float w = center.w;
        float min_half_w = 1.0f / m_Parameters.screenWidth * w;
        float min_half_h = 1.0f / m_Parameters.screenHeight * w;
        if( half_w < min_half_w ) half_w = min_half_w;
        if( half_h < min_half_h ) half_h = min_half_h;
    }

    endpoint.posl = center.x - half_w;
    endpoint.posr = center.x + half_w;
    endpoint.post = center.y + half_h;
    endpoint.posb = center.y - half_h;
    endpoint.posz = center.z;
    endpoint.posw = center.w;

    // TODO: interpolate end point texcoord for each corner
    endpoint.texl = vertex.u;
    endpoint.texr = vertex.u;
    endpoint.text = vertex.v;
    endpoint.texb = vertex.v;

    // TODO: interpolate end point color for each corner
    endpoint.color = vertex.color;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ThickLineRenderer::PrivateVertex *
GN::gfx::ThickLineRenderer::newPolygon6()
{
    if( m_NumVertices + 6 > MAX_VERTICES )
    {
        flush();
    }

    PrivateVertex * nextVertex = &m_Vertices[m_NumVertices];
    m_NumVertices += 6;

    return nextVertex;
}

//
// push pending vertices into GPU pipeline
// -----------------------------------------------------------------------------
void GN::gfx::ThickLineRenderer::flush()
{
    if( m_NumVertices > 0 )
    {
        // update vertex buffer
        uint32 vbsize = (uint32)sizeof(PrivateVertex) * m_NumVertices;
        mContext.vtxbufs[0].vtxbuf->update(0, vbsize, m_Vertices);

        // do rendering
        mGpu->bindContext(mContext);
        mGpu->drawIndexed(
            PrimitiveType::TRIANGLE_LIST,
            m_NumVertices / 6 * 12, // num indices
            0, // base vertex index
            0, // min index
            m_NumVertices, // num vertices
            0); // start index
    }
    m_NumVertices = 0;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ThickLineRenderer::PrivateVertex *
GN::gfx::ThickLineRenderer::EndPoint::topLeft( PrivateVertex * v )
{
    v->position.set( posl, post, posz, posw );
    v->u = texl;
    v->v = text;
    v->color = color;
    return v;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ThickLineRenderer::PrivateVertex *
GN::gfx::ThickLineRenderer::EndPoint::topRight( PrivateVertex * v )
{
    v->position.set( posr, post, posz, posw );
    v->u = texr;
    v->v = text;
    v->color = color;
    return v;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ThickLineRenderer::PrivateVertex *
GN::gfx::ThickLineRenderer::EndPoint::bottomLeft( PrivateVertex * v )
{
    v->position.set( posl, posb, posz, posw );
    v->u = texl;
    v->v = texb;
    v->color = color;
    return v;
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::ThickLineRenderer::PrivateVertex *
GN::gfx::ThickLineRenderer::EndPoint::bottomRight( PrivateVertex * v )
{
    v->position.set( posr, posb, posz, posw );
    v->u = texr;
    v->v = texb;
    v->color = color;
    return v;
}

