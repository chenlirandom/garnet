#include "pch.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.util.SpriteRenderer");

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
bool GN::gfx::SpriteRenderer::init()
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
    mPureWhiteTexture.attach( mRenderer.create2DTexture( 2, 2, 0, ColorFormat::RGBA32 ) );
    if( !mPureWhiteTexture ) return failure();
    const UInt32 PURE_WHITE[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF } ;
    mPureWhiteTexture->updateMipmap( 0, 0, NULL, sizeof(UInt32)*2, sizeof(UInt32)*4, &PURE_WHITE );

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
    mPrivateContext.gpuProgram.attach( mRenderer.createGpuProgram( gpd ) );
    if( !mPrivateContext.gpuProgram ) return failure();

    // create vertex format
    mPrivateContext.vtxfmt.numElements = 3;
    mPrivateContext.vtxfmt.elements[0].stream = 0;
    mPrivateContext.vtxfmt.elements[0].offset = 0;
    mPrivateContext.vtxfmt.elements[0].format = ColorFormat::FLOAT3;
    mPrivateContext.vtxfmt.elements[0].bindTo( "position", 0 );
    mPrivateContext.vtxfmt.elements[1].stream = 0;
    mPrivateContext.vtxfmt.elements[1].offset = GN_FIELD_OFFSET( SpriteVertex, clr );
    mPrivateContext.vtxfmt.elements[1].format = ColorFormat::RGBA32;
    mPrivateContext.vtxfmt.elements[1].bindTo( "color", 0 );
    mPrivateContext.vtxfmt.elements[2].stream = 0;
    mPrivateContext.vtxfmt.elements[2].offset = GN_FIELD_OFFSET( SpriteVertex, tex );
    mPrivateContext.vtxfmt.elements[2].format = ColorFormat::FLOAT2;
    mPrivateContext.vtxfmt.elements[2].bindTo( "texcoord", 0 );

    // create vertex buffer
    mPrivateContext.vtxbufs[0].attach( mRenderer.createVtxBuf( VTXBUF_SIZE, true ) );
    if( !mPrivateContext.vtxbufs[0] ) return failure();
    mPrivateContext.strides[0] = sizeof(SpriteVertex);

    // create index buffer
    mPrivateContext.idxbuf.attach( mRenderer.createIdxBuf16( MAX_INDICES, false ) );
    if( !mPrivateContext.idxbuf ) return failure();
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
    mPrivateContext.idxbuf->update( 0, MAX_INDICES, indices.cptr() );

    // setup sampler (point sampling)
    mPrivateContext.samplers[0].filterMin = TextureSampler::FILTER_POINT;
    mPrivateContext.samplers[0].filterMip = TextureSampler::FILTER_POINT;
    mPrivateContext.samplers[0].filterMag = TextureSampler::FILTER_POINT;

    // setup texture binding
    mPrivateContext.textures[0].bindTo( "t0" );

    // create pending vertex buffer
    mSprites = (Sprite*)heapAlloc( VTXBUF_SIZE );
    if( NULL == mSprites ) return failure();

    // initialize other member variables
    mDrawBegun = false;
    mNextPendingSprite = mSprites;
    mNextFreeSprite = mSprites;

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::SpriteRenderer::quit()
{
    GN_GUARD;

    heapFree( mSprites );
    mPrivateContext.clear();
    mEnvironmentContext.clear();
    mPureWhiteTexture.clear();

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
void GN::gfx::SpriteRenderer::drawBegin( Texture * texture, BitFields options )
{
    if( mDrawBegun )
    {
        GN_ERROR(sLogger)( "SpriteRenderer::drawBegin() can not be called consequtively w/o drawEnd() in between" );
        return;
    }

    // use pure white texture, if input texture is NULL
    if( NULL == texture ) texture = mPureWhiteTexture;

    if( options & USE_COSTOM_CONTEXT )
    {
        mEnvironmentContext = mRenderer.getContext();
        mEnvironmentContext.vtxfmt = mPrivateContext.vtxfmt;
        mEnvironmentContext.vtxbufs[0] = mPrivateContext.vtxbufs[0];
        mEnvironmentContext.idxbuf = mPrivateContext.idxbuf;
        mEnvironmentContext.textures[0].texture.set( texture );
        mEffectiveContext = &mEnvironmentContext;
    }
    else
    {
        mPrivateContext.textures[0].texture.set( texture );
        mPrivateContext.blendEnabled = !(options & OPAQUE_SPRITE);
        mPrivateContext.depthTest = options & ENABLE_DEPTH_TEST;
        mPrivateContext.depthWrite = options & ENABLE_DEPTH_WRITE;
        mEffectiveContext = &mPrivateContext;
    }

    if( RendererAPI::D3D9 == mRenderer.getOptions().api )
    {
        // Shift vertex a little bit on D3D9 platform
        mVertexShift = -0.5f;
    }
    else
    {
        // no need to shift pixel on platform other than D3D9
        mVertexShift = 0.0f;
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

        mPrivateContext.vtxbufs[0]->update(
            firstPendingSpriteOffset * sizeof(Sprite),
            numPendingSprites * sizeof(Sprite),
            mNextPendingSprite,
            mSprites == mNextPendingSprite ? SurfaceUpdateFlag::DISCARD : SurfaceUpdateFlag::NO_OVERWRITE );

        mRenderer.bindContext( *mEffectiveContext );

        mRenderer.drawIndexed(
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
        drawBegin( mEffectiveContext->textures[0].texture.get(), mOptions );
    }

    GN_ASSERT( mNextFreeSprite < mSprites + MAX_SPRITES );

    const DispDesc & dd = mRenderer.getDispDesc();

    float x1 = ( x + mVertexShift ) / dd.width;
    float y1 = ( y + mVertexShift ) / dd.height;
    float x2 = x1 + w / dd.width;
    float y2 = y1 + h / dd.height;
    float u2 = u + tw;
    float v2 = v + th;

    // fill vertex buffer
    mNextFreeSprite->v[0].pos.set( x1, y1, z );
    mNextFreeSprite->v[0].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[0].tex.set( u, v );

    mNextFreeSprite->v[1].pos.set( x1, y2, z );
    mNextFreeSprite->v[1].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[1].tex.set( u, v2 );

    mNextFreeSprite->v[2].pos.set( x2, y2, z );
    mNextFreeSprite->v[2].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[2].tex.set( u2, v2 );

    mNextFreeSprite->v[3].pos.set( x2, y1, z );
    mNextFreeSprite->v[3].clr = 0xFFFFFFFF;
    mNextFreeSprite->v[3].tex.set( u2, v );

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
        drawBegin( mEffectiveContext->textures[0].texture.get(), mOptions );
    }

    GN_ASSERT( mNextFreeSprite < mSprites + MAX_SPRITES );

    const DispDesc & dd = mRenderer.getDispDesc();

    float x1 = x / dd.width;
    float y1 = y / dd.height;
    float x2 = (x + w) / dd.width;
    float y2 = (y + h) / dd.height;

    mNextFreeSprite->v[0].pos.set( x1, y1, z );
    mNextFreeSprite->v[0].clr = rgba;
    mNextFreeSprite->v[0].tex.set( 0, 0 );

    mNextFreeSprite->v[1].pos.set( x1, y2, z );
    mNextFreeSprite->v[1].clr = rgba;
    mNextFreeSprite->v[1].tex.set( 0, 0 );

    mNextFreeSprite->v[2].pos.set( x2, y2, z );
    mNextFreeSprite->v[2].clr = rgba;
    mNextFreeSprite->v[2].tex.set( 0, 0 );

    mNextFreeSprite->v[3].pos.set( x2, y1, z );
    mNextFreeSprite->v[3].clr = rgba;
    mNextFreeSprite->v[3].tex.set( 0, 0 );

    // prepare for next Sprite
    ++mNextFreeSprite;
}
