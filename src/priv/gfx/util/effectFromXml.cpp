#include "pch.h"
#include "garnet/gfx/effect.h"
#include "garnet/base/xml.h"

// *****************************************************************************
// local functions
// *****************************************************************************

using namespace GN;
using namespace GN::gfx;

static GN::Logger * sLogger = GN::getLogger("GN.gfx.base.Effect");

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
// get value of specific attribute
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
static void sParseTexture( EffectDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "texture" == node.name );

    const char * n = sGetItemName( node, "texture" );
    if( !n ) return;

    desc.textures[n];
}

//
//
// -----------------------------------------------------------------------------
static void sParseUniform( EffectDesc & desc, const XmlElement & node )
{
    const char * n = sGetItemName( node, "uniform" );
    if( !n ) return;

    EffectDesc::UniformDesc & ud = desc.uniforms[n];
    ud.hasDefaultValue = ud.defaultValue.loadFromXmlNode( node );
}

//
//
// -----------------------------------------------------------------------------
static void sParseParameters( EffectDesc & desc, const XmlNode & root )
{
    for( const XmlNode * n = root.child; n; n = n->sibling )
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
static void sParseTexref( EffectDesc & desc, EffectDesc::ShaderDesc & sd, const XmlElement & node )
{
    const char * ref = sGetAttrib( node, "ref" );
    if( !ref ) return sPostError( node, "attribute 'ref' not found." );

    UInt32 stage = sGetIntAttrib( node, "stage", (UInt32)-1 );
    if( (UInt32)-1 == stage ) return sPostError( node, "attribute 'stage' not found." );

    const EffectDesc::TextureDesc * td = desc.findTexture( ref );
    if( !td ) return sPostError( node, GN::strFormat( "Invalid texture reference to '%s'", ref ) );

    sd.textures[stage] = ref;
}

//
//
// -----------------------------------------------------------------------------
static void sParseUniref( EffectDesc & desc, EffectDesc::ShaderDesc & sd, const XmlElement & node )
{
    const char * ref = sGetAttrib( node, "ref" );
    if( !ref ) return sPostError( node, "attribute 'ref' not found" );

    const char * binding = sGetAttrib( node, "binding" );
    if( !binding ) return sPostError( node, "attribute 'binding' not found" );

    const EffectDesc::UniformDesc * ud = desc.findUniform( ref );
    if( !ud ) return sPostError( node, GN::strFormat("Invalid uniform reference to '%s'", ref ) );

    sd.uniforms[binding] = ref;
}

//
//
// -----------------------------------------------------------------------------
static bool sParseConditionToken( EffectDesc::ShaderDesc & sd, const XmlElement & node )
{
    if( "token" != node.name )
    {
        sPostError( node, "node name must be \"token\" here.!" );
        return false;
    }

    // parse the token
    EffectDesc::Token t;
    StrA type = sGetAttrib( node, "type" );
    StrA value = sGetAttrib( node, "value" );
    if( "opcode" == type )
    {
        t.type = EffectDesc::OPCODE;
        t.opcode = EffectDesc::sStr2OpCode( value );
        if( EffectDesc::OPCODE_INVALID == t.opcode )
        {
            sPostError( node, strFormat( "invalid opcode: %s", value.cptr() ) );
            return false;
        }
    }
    else if( "values" == type )
    {
        t.setS( value );
    }
    else if( "valuei" == type )
    {
        t.type = EffectDesc::VALUEI;
        if( !str2SInt32( t.valueI, value.cptr() ) )
        {
            sPostError( node, strFormat( "invalid integer: %s", value.cptr() ) );
            return false;
        }
    }
    else
    {
        sPostError( node, strFormat("invalid t type: %s",type.cptr()) );
        return false;
    }

    // append to token array
    sd.prerequisites.tokens.push_back( t );

    // parse child tokens
    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;
        if( !sParseConditionToken( sd, *e ) ) return false;
    }

    // success
    return true;
}

//
//
// -----------------------------------------------------------------------------
static void sParsePrerequisites( EffectDesc::ShaderDesc & sd, const XmlElement & node )
{
    GN_ASSERT( "prerequisites" == node.name );

    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;
        if( !sParseConditionToken( sd, *e ) )
        {
            sd.prerequisites.tokens.clear();
            break;
        }
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseCode( EffectDesc::ShaderDesc & sd, const XmlElement & node )
{
    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlCdata * c = n->toCdata();
        if( c )
        {
            // get shader code
            sd.code = c->text;

            if( !sd.code.empty() )
            {
                // get hints
                sd.hints = sGetAttrib( node, "hints", "" );

                // get shading language
                const char * lang = sGetAttrib( node, "lang" );
                if( !str2ShadingLanguage( sd.lang, lang ) )
                {
                    sPostError( node, strFormat("invalid shading language: %s",lang?lang:"") );
                }
            }

            return;
        }
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseShader( EffectDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "shader" == node.name );

    // get shader name
    const char * name = sGetItemName( node, "shader" );
    if( !name ) return;

    EffectDesc::ShaderDesc sd;

    // get shader type
    const char * type = sGetAttrib( node, "type" );
    if( !str2ShaderType( sd.type, type ) )
    {
        sPostError( node, strFormat("invalid shader type: %s",type?type:"") );
        return;
    }

    // parse children
    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "texref" == e->name ) sParseTexref( desc, sd, *e );
        else if( "uniref" == e->name ) sParseUniref( desc, sd, *e );
        else if( "prerequisites" == e->name ) sParsePrerequisites( sd, *e );
        else if( "code" == e->name ) sParseCode( sd, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }

    // success
    desc.shaders[name] = sd;
}

//
//
// -----------------------------------------------------------------------------
static void sParseShaders( EffectDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "shaders" == node.name );

    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "shader" == e->name ) sParseShader( desc, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseRsb( RenderStateBlockDesc & rsb, const XmlElement & node )
{
    RenderState rs;
    int rsv;
    for( const XmlAttrib * a = node.attrib; a; a = a->next )
    {
        if( !str2RenderState( rs, a->name.cptr() ) )
        {
            sPostError( node, strFormat( "invalid render state: %s", a->name.cptr() ) );
            continue;
        }

        const RenderStateDesc & rsdesc = getRenderStateDesc( rs );

        switch( rsdesc.valueType )
        {
            case RenderStateDesc::VT_ENUM:
                if( !str2RenderStateValue( (RenderStateValue&)rsv, a->value.cptr() ) )
                {
                    sPostError( node, strFormat(
                        "invalid render state value: %s (for render state %s)",
                        a->value.cptr(), a->name.cptr() ) );
                    continue;
                }
                rsb.set( rs, rsv );
                break;

            case RenderStateDesc::VT_INT :
                if( !str2Int( rsv, a->value.cptr() ) )
                {
                    sPostError( node, strFormat(
                        "invalid render state value: %s (for render state %s)",
                        a->value.cptr(), a->name.cptr() ) );
                    continue;
                }
                rsb.set( rs, rsv );
                break;

            default: GN_UNEXPECTED();
        }
    }
}

//
//
// -----------------------------------------------------------------------------
static const char * sGetShaderRef(
    EffectDesc & desc,
    const XmlElement & node,
    const char * attribName,
    ShaderType type )
{
    const char * ref = sGetAttrib( node, attribName );
    if( !ref )
    {
        sPostError( node, strFormat("attrib %s not found",attribName) );
        return 0;
    }

    const EffectDesc::ShaderDesc * sd = desc.findShader( ref );
    if( !sd ) { sPostError( node, "Invalid shader reference" ); return 0; }

    if( type != sd->type ) { sPostError( node, "Incompatible shader type." ); return 0; }

    return ref;
}

//
//
// -----------------------------------------------------------------------------
static void sParsePass( EffectDesc & desc, EffectDesc::TechniqueDesc & td, const XmlElement & node )
{
    const char * vs = sGetShaderRef( desc, node, "vs", SHADER_VS );
    const char * ps = sGetShaderRef( desc, node, "ps", SHADER_PS );
    const char * gs = sGetShaderRef( desc, node, "gs", SHADER_GS );
    if( !vs || !ps || !gs ) return;

    td.passes.resize( td.passes.size() + 1 );
    EffectDesc::PassDesc & pd = td.passes.back();

    pd.shaders[SHADER_VS] = vs;
    pd.shaders[SHADER_PS] = ps;
    pd.shaders[SHADER_GS] = gs;

    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "rsb" == e->name ) sParseRsb( pd.rsb, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }
}

//
//
// -----------------------------------------------------------------------------
static void sParseTechnique( EffectDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "technique" == node.name );

    // get technique name
    const char * name = sGetItemName( node, "technique" );
    if( !name ) return;

    EffectDesc::TechniqueDesc td;
    td.name = name;

    // parse children
    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "rsb" == e->name ) sParseRsb( td.rsb, *e );
        else if( "pass" == e->name ) sParsePass( desc, td, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }

    desc.techniques.push_back( td );
}

//
//
// -----------------------------------------------------------------------------
static void sParseTechniques( EffectDesc & desc, const XmlElement & node )
{
    GN_ASSERT( "techniques" == node.name );

    for( const XmlNode * n = node.child; n; n = n->sibling )
    {
        const XmlElement * e = n->toElement();
        if( !e ) continue;

        if( "rsb" == e->name ) sParseRsb( desc.rsb, *e );
        else if( "technique" == e->name ) sParseTechnique( desc, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::EffectDesc::loadFromXmlNode( const XmlNode & root, const StrA & )
{
    GN_GUARD;

    clear();

    const XmlElement * e = root.toElement();

    if( 0 == e ||e->name != "effect" )
    {
        GN_ERROR(sLogger)( "root node must be \"<effect>\"." );
        return false;
    }

    for( const XmlNode * n = e->child; n; n = n->sibling )
    {
        e = n->toElement();
        if( !e ) continue;

        if( "parameters" == e->name ) sParseParameters( *this, *e );
        else if( "shaders" == e->name ) sParseShaders( *this, *e );
        else if( "techniques" == e->name ) sParseTechniques( *this, *e );
        else sPostError( *e, "Unknown node. Ignored" );
    }

    return true;

    GN_UNGUARD;
}
