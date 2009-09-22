#include "pch.h"

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.gpures");

typedef GN::gfx::EffectResourceDesc::ShaderPrerequisites ShaderPrerequisites;
typedef GN::gfx::EffectResourceDesc::EffectUniformDesc EffectUniformDesc;
typedef GN::gfx::EffectResourceDesc::EffectTextureDesc EffectTextureDesc;
typedef GN::gfx::EffectResourceDesc::EffectGpuProgramDesc EffectGpuProgramDesc;
typedef GN::gfx::EffectResourceDesc::EffectRenderStateDesc EffectRenderStateDesc;
typedef GN::gfx::EffectResourceDesc::EffectPassDesc EffectPassDesc;
typedef GN::gfx::EffectResourceDesc::EffectTechniqueDesc EffectTechniqueDesc;

// *****************************************************************************
// Local stuff
// *****************************************************************************

//
// post error message
// -----------------------------------------------------------------------------
static void sPostError( const XmlNode & node, const StrA & msg )
{
    GN_UNUSED_PARAM( node );
    GN_ERROR(sLogger)( "%s", msg.cptr() );
}

//
// get value of specific attribute
// -----------------------------------------------------------------------------
static const char * sGetAttrib(
    const XmlElement & node,
    const char * attribName,
    const char * defaultValue = NULL )
{
    const XmlAttrib * a = node.findAttrib( attribName );
    return a ? a->value.cptr() : defaultValue;
}

//
// get value of integer attribute
// -----------------------------------------------------------------------------
template<typename T>
static T sGetIntAttrib( const XmlElement & node, const char * attribName, T defaultValue )
{
    const XmlAttrib * a = node.findAttrib( attribName );

    T result;

    if( !a || !str2Int<T>( result, a->value.cptr() ) )
        return defaultValue;
    else
        return result;
}

//
// get value of boolean attribute
// -----------------------------------------------------------------------------
static bool sGetBoolAttrib( const XmlElement & node, const char * attribName, bool defaultValue )
{
    const XmlAttrib * a = node.findAttrib( attribName );
    if( !a ) return defaultValue;

    if( 0 == strCmpI( "1", a->value.cptr() ) ||
        0 == strCmpI( "true", a->value.cptr() ) )
    {
        return true;
    }
    else if( 0 == strCmpI( "0", a->value.cptr() ) ||
             0 == strCmpI( "false", a->value.cptr() ) )
    {
        return false;
    }
    else
    {
        return defaultValue;
    }
}

//
// get value of name attribute
// -----------------------------------------------------------------------------
static const char * sGetItemName( const XmlElement & node, const char * nodeType )
{
    XmlAttrib * a = node.findAttrib( "name" );
    if( !a )
    {
        sPostError( node, strFormat("Unnamed %s node. Ignored.", nodeType) );
        return 0;
    }
    return a->value.cptr();
}

//
//
// -----------------------------------------------------------------------------
static void sParseTexture( EffectResourceDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "texture" == node.name );

    const char * name = sGetItemName( node, "texture" );
    if( !name ) return;

    desc.textures[name];

    GN_TODO( "load samplers." );
}

//
//
// -----------------------------------------------------------------------------
static void sParseUniform( EffectResourceDesc & desc, const XmlElement & node )
{
    const char * name = sGetItemName( node, "uniform" );
    if( !name ) return;

    EffectResourceDesc::EffectUniformDesc & ud = desc.uniforms[name];

    const char * type = sGetAttrib( node, "type" );
    if( NULL == type )
    {
        ud.size = sGetIntAttrib<size_t>( node, "size", 0 );
    }
    else
    {
        if( 0 == strCmpI( "matrix", type ) ||
            0 == strCmpI( "matrix4x4", type ) ||
            0 == strCmpI( "matrix44", type ) ||
            0 == strCmpI( "matrix4", type ) ||
            0 == strCmpI( "mat4", type ) ||
            0 == strCmpI( "float4x4", type ) )
        {
            ud.size = sizeof(Matrix44f);
        }
        else if(
            0 == strCmpI( "vector", type ) ||
            0 == strCmpI( "vec4", type ) ||
            0 == strCmpI( "vector4", type ) ||
            0 == strCmpI( "float4", type ) )
        {
            ud.size = sizeof(float)*4;
        }
        else
        {
            sPostError( node, strFormat( "Unrecognized uniform type: %s", type ) );
            ud.size = 0;
        }
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseParameters( EffectResourceDesc & desc, const XmlNode & root )
{
    for( const XmlNode * n = root.child; n; n = n->next )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "texture" == e->name ) sParseTexture( desc, *e );
        else if( "uniform" == e->name ) sParseUniform( desc, *e );
        else sPostError( *e, strFormat( "Unknown parameter '%s'. Ignored", e->name.cptr() ) );
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseTexref( EffectResourceDesc::EffectGpuProgramDesc & sd, const XmlElement & node )
{
    const char * texture = sGetAttrib( node, "texture" );
    if( !texture ) return sPostError( node, "Attribute 'texture' is missing." );

    const char * shaderParameter = sGetAttrib( node, "shaderParameter", NULL );
    if( !shaderParameter ) return sPostError( node, "Attribute 'shaderParameter' is missing." );

    sd.textures[shaderParameter] = texture;
}

//
//
// -----------------------------------------------------------------------------
static void sParseUniref( EffectResourceDesc::EffectGpuProgramDesc & sd, const XmlElement & node )
{
    const char * uniform = sGetAttrib( node, "uniform" );
    if( !uniform ) return sPostError( node, "Attribute 'uniform' not found" );

    const char * shaderParameter = sGetAttrib( node, "shaderParameter" );
    if( !shaderParameter ) return sPostError( node, "Attribute 'shaderParameter' not found" );

    sd.uniforms[shaderParameter] = uniform;
}

//
//
// -----------------------------------------------------------------------------
static void sParsePrerequisites( EffectResourceDesc::ShaderPrerequisites& sp, const XmlElement & node )
{
    GN_ASSERT( "prerequisites" == node.name );
    GN_UNUSED_PARAM( sp );
    GN_UNUSED_PARAM( node );
    GN_UNIMPL_WARNING();
}

//
//
// -----------------------------------------------------------------------------
static void sParseCode( EffectGpuProgramDesc & sd, ShaderCode & code, const XmlElement & node )
{
    // get shader code
    for( const XmlNode * n = node.child; n; n = n->next )
    {
        const XmlCdata * c = n->toCdata();
        if( c )
        {
            size_t offset = sd.shaderSourceBuffer.size();
            sd.shaderSourceBuffer.append( c->text.cptr(), c->text.size() + 1 );
            code.source = (const char*)offset;
            break;
        }
    }

    // get shader entry
    const char * entry = sGetAttrib( node, "entry", NULL );
    if( entry )
    {
        size_t offset = sd.shaderSourceBuffer.size();
        sd.shaderSourceBuffer.append( entry, strLen(entry) + 1 );
        code.entry = (const char *)offset;
    }
    else
    {
        code.entry = NULL;
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseGpuProgram( EffectResourceDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "gpuprogram" == node.name );

    // get shader name
    const char * name = sGetItemName( node, "gpuprogram" );
    if( !name ) return;

    EffectResourceDesc::EffectGpuProgramDesc sd;

    // Add some dummy data into shader source buffer,
    // to ensure that any valid shader source offset won't be zero.
    sd.shaderSourceBuffer.append( "ABCD", 4 );

    // get shading language
    const char * lang = sGetAttrib( node, "lang" );
    sd.gpd.lang = GpuProgramLanguage::sFromString( lang );
    if( GpuProgramLanguage::INVALID == sd.gpd.lang )
    {
        sPostError( node, strFormat("invalid shading language: %s",lang?lang:"<NULL>") );
        return;
    }

    sd.gpd.optimize = sGetBoolAttrib( node, "optimize", true );
    sd.gpd.debug    = sGetBoolAttrib( node, "debug", true );

    // parse children
    for( const XmlNode * n = node.child; n; n = n->next )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "texref" == e->name ) sParseTexref( sd, *e );
        else if( "uniref" == e->name ) sParseUniref( sd, *e );
        else if( "prerequisites" == e->name ) sParsePrerequisites( sd.prerequisites, *e );
        else if( "vs" == e->name ) sParseCode( sd, sd.gpd.vs, *e );
        else if( "gs" == e->name ) sParseCode( sd, sd.gpd.gs, *e );
        else if( "ps" == e->name ) sParseCode( sd, sd.gpd.ps, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }

    // convert all shader source offsets to pointers
    const char * start = sd.shaderSourceBuffer.cptr();
    if( sd.gpd.vs.source ) sd.gpd.vs.source += (size_t)start;
    if( sd.gpd.vs.entry  ) sd.gpd.vs.entry  += (size_t)start;
    if( sd.gpd.gs.source ) sd.gpd.gs.source += (size_t)start;
    if( sd.gpd.gs.entry  ) sd.gpd.gs.entry  += (size_t)start;
    if( sd.gpd.ps.source ) sd.gpd.ps.source += (size_t)start;
    if( sd.gpd.ps.entry  ) sd.gpd.ps.entry  += (size_t)start;

    // success
    desc.gpuprograms[name] = sd;
}

//
//
// -----------------------------------------------------------------------------
static void sParseGpuPrograms( EffectResourceDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "gpuprograms" == node.name );

    for( const XmlNode * n = node.child; n; n = n->next )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "gpuprogram" == e->name ) sParseGpuProgram( desc, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseRenderStates( EffectResourceDesc::EffectRenderStateDesc & rsdesc, const XmlElement & node )
{
    GN_UNUSED_PARAM( rsdesc );
    GN_UNIMPL_WARNING();
    for( const XmlAttrib * a = node.attrib; a; a = a->next )
    {
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParsePass( EffectResourceDesc::EffectTechniqueDesc & td, const XmlElement & node )
{
    const char * gpname = sGetAttrib( node, "gpuprogram" );
    if( !gpname )
    {
        sPostError( node, "required attribute 'gpuprogram' is missing." );
        return;
    }

    td.passes.resize( td.passes.size() + 1 );

    EffectResourceDesc::EffectPassDesc & pd = td.passes.back();

    pd.gpuprogram = gpname;

    for( const XmlNode * n = node.child; n; n = n->next )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "renderstates" == e->name ) sParseRenderStates( pd.renderstates, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseTechnique( EffectResourceDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "technique" == node.name );

    // get techique name
    const char * techName = sGetItemName( node, "technique" );
    if( NULL == techName ) return;

    EffectResourceDesc::EffectTechniqueDesc td;

    // get technique quality, default is 100
    td.quality = sGetIntAttrib( node, "quality", 100 );

    // parse children
    for( const XmlNode * n = node.child; n; n = n->next )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "renderstates" == e->name ) sParseRenderStates( td.renderstates, *e );
        else if( "pass" == e->name ) sParsePass( td, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }

    desc.techniques[techName] = td;
}

//
//
// -----------------------------------------------------------------------------
static void sParseTechniques( EffectResourceDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "techniques" == node.name );

    for( const XmlNode * n = node.child; n; n = n->next )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "renderstates" == e->name ) sParseRenderStates( desc.renderstates, *e );
        else if( "technique" == e->name ) sParseTechnique( desc, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }
}

//
//
// -----------------------------------------------------------------------------
static void sCopyShaderSourcePtr(
    const char          * & to,
    const DynaArray<char> & tobuf,
    const char            * from,
    const DynaArray<char> & frombuf )
{
    GN_ASSERT( tobuf.size() == frombuf.size() );

    const char * s = frombuf.cptr();
    const char * e = s + frombuf.size();

    if( s <= from && from < e )
    {
        to = tobuf.cptr() + ( from - s );
    }
    else
    {
        to = from;
    }
}

//
//
// -----------------------------------------------------------------------------
static void sCopyShaderDesc( EffectGpuProgramDesc & to, const EffectGpuProgramDesc & from )
{
    to.shaderSourceBuffer = from.shaderSourceBuffer;

    #define COPY_SHADER_PTR( x ) sCopyShaderSourcePtr( to.gpd.x, to.shaderSourceBuffer, from.gpd.x, from.shaderSourceBuffer );

    COPY_SHADER_PTR( vs.source );
    COPY_SHADER_PTR( vs.entry );

    COPY_SHADER_PTR( gs.source );
    COPY_SHADER_PTR( gs.entry );

    COPY_SHADER_PTR( ps.source );
    COPY_SHADER_PTR( ps.entry );

    #undef COPY_SHADER_PTR
}

// *****************************************************************************
// Public methods
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::gfx::EffectResourceDesc::EffectGpuProgramDesc::EffectGpuProgramDesc(
    const EffectGpuProgramDesc & rhs )
{
    prerequisites = rhs.prerequisites;
    gpd = rhs.gpd;
    textures = rhs.textures;
    uniforms = rhs.uniforms;
    sCopyShaderDesc( *this, rhs );
}

//
//
// -----------------------------------------------------------------------------
GN::gfx::EffectResourceDesc::EffectGpuProgramDesc &
GN::gfx::EffectResourceDesc::EffectGpuProgramDesc::operator=(
    const EffectGpuProgramDesc & rhs )
{
    prerequisites = rhs.prerequisites;
    gpd = rhs.gpd;
    textures = rhs.textures;
    uniforms = rhs.uniforms;
    sCopyShaderDesc( *this, rhs );
    return *this;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::EffectResourceDesc::clear()
{
    textures.clear();
    uniforms.clear();
    gpuprograms.clear();
    techniques.clear();

    GpuContext::RenderStates rs;
    rs.clear();

    memset( &renderstates, 0, sizeof(renderstates) );

    // copy default render state values from GPU context
    renderstates.depthTestEnabled  = !!rs.depthTestEnabled;
    renderstates.depthWriteEnabled = !!rs.depthWriteEnabled;
    renderstates.depthFunc         = rs.depthFunc;

    renderstates.stencilEnabled    = !!rs.stencilEnabled;
    renderstates.stencilPassOp     = rs.stencilPassOp;
    renderstates.stencilFailOp     = rs.stencilFailOp;
    renderstates.stencilZFailOp    = rs.stencilZFailOp;

    renderstates.blendEnabled      = !!rs.blendEnabled;
    renderstates.blendSrc          = rs.blendSrc;
    renderstates.blendDst          = rs.blendDst;
    renderstates.blendOp           = rs.blendOp;
    renderstates.blendAlphaSrc     = rs.blendAlphaSrc;
    renderstates.blendAlphaDst     = rs.blendAlphaDst;
    renderstates.blendAlphaOp      = rs.blendAlphaOp;

    renderstates.fillMode          = rs.fillMode;
    renderstates.cullMode          = rs.cullMode;
    renderstates.frontFace         = rs.frontFace;
    renderstates.msaaEnabled       = !!rs.msaaEnabled;

    renderstates.blendFactors      = rs.blendFactors;

    // exept these:
    //renderstates.colorWriteMask    = rs.colorWriteMask;
    //renderstates.viewport          = rs.viewport;
    //renderstates.scissorRect       = rs.scissorRect;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::EffectResourceDesc::loadFromXmlNode( const XmlNode & root )
{
    clear();

    const XmlElement * e = root.toElement();

    if( 0 == e ||e->name != "effect" )
    {
        GN_ERROR(sLogger)( "root node must be \"<effect>\"." );
        return false;
    }

    for( const XmlNode * n = e->child; n; n = n->next )
    {
        e = n->toElement();
        if( !e ) continue;

        if( "parameters" == e->name ) sParseParameters( *this, *e );
        else if( "gpuprograms" == e->name ) sParseGpuPrograms( *this, *e );
        else if( "techniques" == e->name ) sParseTechniques( *this, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }

    return true;
}

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::EffectResourceDesc::saveToXmlNode( XmlNode & root ) const
{
    GN_UNUSED_PARAM( root );
    GN_UNIMPL();
    return false;
}
