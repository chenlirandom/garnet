#include "pch.h"

static GN::Logger * sLogger = GN::GetLogger("GN.gfx.util.SpriteRenderer");

static const char * glslvscode=
    "varying vec4 color; \n"
    "varying vec2 texcoords; \n"
    "void main() { \n"
    "   gl_Position.x = gl_Vertex.x * 2.0 - 1.0; \n"
    "   gl_Position.y = gl_Vertex.y * -2.0 + 1.0; \n"
    "   gl_Position.zw = gl_Vertex.zw; \n"
    "   color = gl_Color; \n"
    "   texcoords.xy = gl_MultiTexCoord0.xy; \n"
    "}";

static const char * glslpscode=
    "uniform sampler2D t0; \n"
    "varying vec4 color; \n"
    "varying vec2 texcoords; \n"
    "void main() { \n"
    "   gl_FragColor = color * texture2D( t0, texcoords ); \n"
    "}";

static const char * hlslvscode=
    "struct VSOUT { \n"
    "   float4 position  : POSITION0; \n"
    "   float4 color     : COLOR; \n"
    "   float2 texcoords : TEXCOORD; \n"
    "}; \n"
    "struct VSIN { \n"
    "   float4 position  : POSITION; \n"
    "   float4 color     : COLOR; \n"
    "   float2 texcoords : TEXCOORD; \n"
    "}; \n"
    "VSOUT main( VSIN i ) { \n"
    "   VSOUT o; \n"
    "   o.position.x  = i.position.x * 2.0 - 1.0; \n"
    "   o.position.y  = i.position.y * -2.0 + 1.0; \n"
    "   o.position.zw = i.position.zw; \n"
    "   o.color       = i.color; \n"
    "   o.texcoords   = i.texcoords; \n"
    "   return o; \n"
    "}";

static const char * hlslpscode=
    "sampler t0; \n"
    "struct VSOUT { \n"
    "   float4 position  : POSITION0; \n"
    "   float4 color     : COLOR; \n"
    "   float2 texcoords : TEXCOORD; \n"
    "}; \n"
    "float4 main( VSOUT i ) : COLOR0 { \n"
    "   return i.color * tex2D( t0, i.texcoords ); \n"
    "}";

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::SpriteRenderer::Init()
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::SpriteRenderer, () );

    enum
    {
        MAX_VERTICES = MAX_SPRITES * 4,
        MAX_INDICES  = MAX_SPRITES * 6,
        VTXBUF_SIZE  = MAX_VERTICES * sizeof(SpriteVertex)
    };

    // create a 2x2 pure white texture
    mPureWhiteTexture.Attach( mGpu.create2DTexture( 2, 2, 0, ColorFormat::RGBA32 ) );
    if( !mPureWhiteTexture ) return Failure();
    const UInt32 PURE_WHITE[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF } ;
    mPureWhiteTexture->UpdateMipmap( 0, 0, NULL, sizeof(UInt32)*2, sizeof(UInt32)*4, &PURE_WHITE );

    // create GPU program
    const GpuCaps & caps = mGpu.GetCaps();
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
        return Failure();
    }
    mGpuProgram.Attach( mGpu.createGpuProgram( gpd ) );
    if( !mGpuProgram ) return Failure();

    // create vertex format
    mVertexFormat.numElements = 3;
    mVertexFormat.elements[0].stream = 0;
    mVertexFormat.elements[0].offset = 0;
    mVertexFormat.elements[0].format = ColorFormat::FLOAT3;
    mVertexFormat.elements[0].bindTo( "position", 0 );
    mVertexFormat.elements[1].stream = 0;
    mVertexFormat.elements[1].offset = GN_FIELD_OFFSET( SpriteVertex, clr );
    mVertexFormat.elements[1].format = ColorFormat::RGBA32;
    mVertexFormat.elements[1].bindTo( "color", 0 );
    mVertexFormat.elements[2].stream = 0;
    mVertexFormat.elements[2].offset = GN_FIELD_OFFSET( SpriteVertex, tex );
    mVertexFormat.elements[2].format = ColorFormat::FLOAT2;
    mVertexFormat.elements[2].bindTo( "texcoord", 0 );

    // create vertex buffer
    mVertexBuffer.Attach( mGpu.createVtxBuf( VTXBUF_SIZE, true ) );
    if( !mVertexBuffer ) return Failure();

    // create index buffer
    mIndexBuffer.Attach( mGpu.createIdxBuf16( MAX_INDICES, false ) );
    if( !mIndexBuffer ) return Failure();
    DynaArray<UInt16> indices( MAX_INDICES );
    for( UInt16 i = 0; i < MAX_SPRITES; ++i )
    {
        indices[i*6+0] = i * 4 + 0;
        indices[i*6+1] = i * 4 + 1;
        indices[i*6+2] = i * 4 + 2;
        indices[i*6+3] = i * 4 + 0;
        indices[i*6+4] = i * 4 + 2;
        indices[i*6+5] = i * 4 + 3;
    }
    mIndexBuffer->Update( 0, MAX_INDICES, indices.ToRawPtr() );


    // create pending vertex buffer
    mSprites = (Sprite*)HeapMemory::Alloc( VTXBUF_SIZE );
    if( NULL == mSprites ) return Failure();

    // initialize other member variables
    mDrawBegun = false;
    mNextPendingSprite = mSprites;
    mNextFreeSprite = mSprites;

    // success
    return Success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SpriteRenderer::Quit()
{
    GN_GUARD;

    HeapMemory::Free( mSprites );
    mContext.Clear();
    mIndexBuffer.Clear();
    mVertexBuffer.Clear();
    mGpuProgram.Clear();
    mPureWhiteTexture.Clear();

    // standard Quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SpriteRenderer::drawBegin( Texture * texture, BitFields options )
{
    if( mDrawBegun )
    {
        GN_ERROR(sLogger)( "SpriteRenderer::drawBegin() can not be called consequtively w/o drawEnd() in between" );
        return;
    }

    // use pure white texture, if input texture is NULL
    if( NULL == texture ) texture = mPureWhiteTexture;

    // copy current renderer context
    mContext = mGpu.getContext();

    // setup parameters that are not affected by options
    mContext.textures[0].texture.Set( texture );
    mContext.vtxfmt            = mVertexFormat;
    mContext.vtxbufs[0].vtxbuf = mVertexBuffer;
    mContext.vtxbufs[0].stride = sizeof(SpriteVertex);
    mContext.idxbuf            = mIndexBuffer;

    // setup GPU program
    if( 0 == (options & USE_EXTERNAL_GPU_PROGRAM) )
    {
        mContext.gpuProgram = mGpuProgram;

        // default shader has no uniform
        mContext.uniforms.Clear();
    }
    if( 0 == (options & USE_EXTERNAL_TEXTURE_FILTERS) )
    {
        // use point sampling by default
        mContext.textures[0].sampler.filterMin = SamplerDesc::FILTER_POINT;
        mContext.textures[0].sampler.filterMip = SamplerDesc::FILTER_POINT;
        mContext.textures[0].sampler.filterMag = SamplerDesc::FILTER_POINT;
    }

    // setup alpha blending
    if( options & FORCE_ALPHA_BLENDING_ENABLED )
    {
        if( options & FORCE_ALPHA_BLENDING_DISABLED )
        {
            GN_WARN(sLogger)( "FORCE_ALPHA_BLENDING_ENABLED and FORCE_ALPHA_BLENDING_DISABLED should not be specifed together!" );
        }

        mContext.rs.alphaBlend[0].blendEnabled  = true;
        mContext.rs.alphaBlend[0].blendSrc      = GpuContext::BLEND_SRC_ALPHA;
        mContext.rs.alphaBlend[0].blendDst      = GpuContext::BLEND_INV_SRC_ALPHA;
        mContext.rs.alphaBlend[0].blendOp       = GpuContext::BLEND_OP_ADD;
        mContext.rs.alphaBlend[0].blendAlphaSrc = GpuContext::BLEND_SRC_ALPHA;
        mContext.rs.alphaBlend[0].blendAlphaDst = GpuContext::BLEND_INV_SRC_ALPHA;
        mContext.rs.alphaBlend[0].blendAlphaOp  = GpuContext::BLEND_OP_ADD;
    }
    else if( options & FORCE_ALPHA_BLENDING_DISABLED )
    {
        mContext.rs.alphaBlend[0].blendEnabled = false;
    }

    // setup depth test
    if( options & FORCE_DEPTH_TEST_ENABLED )
    {
        if( options & FORCE_DEPTH_TEST_DISABLED )
        {
            GN_WARN(sLogger)( "FORCE_DEPTH_TEST_ENABLED and FORCE_DEPTH_TEST_DISABLED should not be specifed together!" );
        }

        mContext.rs.depthTestEnabled = true;
    }
    else if( options & FORCE_DEPTH_TEST_DISABLED )
    {
        mContext.rs.depthTestEnabled = false;
    }

    // setup depth write
    if( options & FORCE_DEPTH_WRITE_ENABLED )
    {
        if( options & FORCE_DEPTH_WRITE_DISABLED )
        {
            GN_WARN(sLogger)( "FORCE_DEPTH_WRITE_ENABLED and FORCE_DEPTH_WRITE_DISABLED should not be specifed together!" );
        }

        mContext.rs.depthWriteEnabled = true;
    }
    else if( options & FORCE_DEPTH_WRITE_DISABLED )
    {
        mContext.rs.depthWriteEnabled = false;
    }

    // Note: D3D9 and Xenon needs -0.5f vertex shift
    if( mGpu.getOptions().api == GpuAPI::XENON )
    {
        mVertexShift = -0.5f;
    }
    else
    {
        mVertexShift = 0;
    }

    mDrawBegun = true;
    mOptions   = options;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SpriteRenderer::drawEnd()
{
    if( !mDrawBegun )
    {
        GN_ERROR(sLogger)( "SpriteRenderer::drawEnd() can not be called consequtively w/o drawBegin() in between" );
        return;
    }

    size_t numPendingSprites = mNextFreeSprite - mNextPendingSprite;
    if( numPendingSprites > 0 )
    {
        size_t firstPendingSpriteOffset = mNextPendingSprite - mSprites;

        mVertexBuffer->Update(
            firstPendingSpriteOffset * sizeof(Sprite),
            numPendingSprites * sizeof(Sprite),
            mNextPendingSprite,
            mSprites == mNextPendingSprite ? SurfaceUpdateFlag::DISCARD : SurfaceUpdateFlag::NO_OVERWRITE );

        mGpu.bindContext( mContext );

        mGpu.drawIndexed(
            PrimitiveType::TRIANGLE_LIST,
            numPendingSprites * 6,        // numidx
            firstPendingSpriteOffset * 4, // basevtx,
            0,                            // startvtx
            numPendingSprites * 4,        // numvtx
            0 );                          // startidx
    }

    mDrawBegun = false;
    if( mNextFreeSprite == mSprites + MAX_SPRITES ) mNextFreeSprite = mSprites; // rewind next-free pointer if needed.
    mNextPendingSprite = mNextFreeSprite;
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::SpriteRenderer::drawTextured(
    float x,
    float y,
    float w,
    float h,
    float u,
    float v,
    float tw,
    float th,
    float z )
{
    if( !mDrawBegun )
    {
        GN_ERROR(sLogger)( "Must be called between drawBegin() and drawEnd()." );
        return;
    }

    if( mNextFreeSprite == mSprites + MAX_SPRITES )
    {
        drawEnd();
        drawBegin( mContext.textures[0].texture.Get(), mOptions );
    }

    GN_ASSERT( mNextFreeSprite < mSprites + MAX_SPRITES );

    // get screen size based on current context
    UInt32 screenWidth, screenHeight;
    mGpu.getCurrentRenderTargetSize( &screenWidth, &screenHeight );

    float x1 = ( x + mVertexShift ) / screenWidth;
    float y1 = ( y + mVertexShift ) / screenHeight;
    float x2 = x1 + w / screenWidth;
    float y2 = y1 + h / screenHeight;
    float u2 = u + tw;
    float v2 = v + th;

    // fill vertex buffer
    mNextFreeSprite->v[0].pos.Set( x1, y1, z );
    mNextFreeSprite->v[0].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[0].tex.Set( u, v );

    mNextFreeSprite->v[1].pos.Set( x1, y2, z );
    mNextFreeSprite->v[1].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[1].tex.Set( u, v2 );

    mNextFreeSprite->v[2].pos.Set( x2, y2, z );
    mNextFreeSprite->v[2].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[2].tex.Set( u2, v2 );

    mNextFreeSprite->v[3].pos.Set( x2, y1, z );
    mNextFreeSprite->v[3].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[3].tex.Set( u2, v );

    // prepare for next sprite
    ++mNextFreeSprite;
}

//
//
// -----------------------------------------------------------------------------
void
GN::gfx::SpriteRenderer::drawSolid(
    UInt32 rgba,
    float  x,
    float  y,
    float  w,
    float  h,
    float  z )
{
    if( !mDrawBegun )
    {
        GN_ERROR(sLogger)( "Must be called between drawBegin() and drawEnd()." );
        return;
    }

    if( mNextFreeSprite == mSprites + MAX_SPRITES )
    {
        drawEnd();
        drawBegin( mContext.textures[0].texture.Get(), mOptions );
    }

    GN_ASSERT( mNextFreeSprite < mSprites + MAX_SPRITES );

    const DispDesc & dd = mGpu.getDispDesc();

    float x1 = x / dd.width;
    float y1 = y / dd.height;
    float x2 = (x + w) / dd.width;
    float y2 = (y + h) / dd.height;

    mNextFreeSprite->v[0].pos.Set( x1, y1, z );
    mNextFreeSprite->v[0].clr = rgba;
    mNextFreeSprite->v[0].tex.Set( 0, 0 );

    mNextFreeSprite->v[1].pos.Set( x1, y2, z );
    mNextFreeSprite->v[1].clr = rgba;
    mNextFreeSprite->v[1].tex.Set( 0, 0 );

    mNextFreeSprite->v[2].pos.Set( x2, y2, z );
    mNextFreeSprite->v[2].clr = rgba;
    mNextFreeSprite->v[2].tex.Set( 0, 0 );

    mNextFreeSprite->v[3].pos.Set( x2, y1, z );
    mNextFreeSprite->v[3].clr = rgba;
    mNextFreeSprite->v[3].tex.Set( 0, 0 );

    // prepare for next Sprite
    ++mNextFreeSprite;
}
