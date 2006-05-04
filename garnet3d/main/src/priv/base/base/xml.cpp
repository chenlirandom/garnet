#include "pch.h"
#include "garnet/base/xml.h"
#include <expat.h>

// *****************************************************************************
// local functions
// *****************************************************************************

struct AutoFree
{
    XML_Parser ptr;
    AutoFree( XML_Parser p ) : ptr(p) { GN_ASSERT(p); }
    ~AutoFree() { XML_ParserFree( ptr ); }
};

// This is used to trace where we are, during parsing the XML documents.
struct ParseTracer
{
    XML_Parser parser;
    GN::XmlDocument * doc;
    GN::XmlParseResult * result;
    GN::XmlNode * parent;
    GN::XmlNode * prev;
};

static void * sXmlMalloc( size_t sz ) { return GN::memAlloc( sz ); }
static void * sXmlRealloc( void * p, size_t sz ) { return GN::memReAlloc( p, sz ); }
static void sXmlFree( void * p ) { return GN::memFree( p ); }

static void sIdent( GN::File & fp, int ident )
{
    for( int i = 0; i < ident; ++i ) fp <<"\t";
}

static void sFormatAttributes( GN::File & fp, const GN::XmlAttrib * att, int ident )
{
    for( ; 0 != att; att = att->next )
    {
        sIdent( fp, ident );
        fp << att->name << "=\"" << att->value << "\"\n";
    }
}

static bool sFormatNodes( GN::File & fp, const GN::XmlNode * root, int ident )
{
    GN_ASSERT( root );

    switch( root->type )
    {
        case GN::XML_CDATA:
        {
            const GN::XmlCdata * c = root->toCdata();
            GN_ASSERT( c && !c->child ); // cdata node should have no child.
            sIdent( fp, ident );
            fp << "<![CDATA[" << c->text << "]]>\n";
            break;
        }

        case GN::XML_COMMENT:
        {
            break;
        }

        case GN::XML_ELEMENT:
        {
            const GN::XmlElement * e = root->toElement();
            GN_ASSERT( e );

            sIdent( fp, ident );
            fp << "<" << e->name;

            if( e->attrib )
            {
                fp << "\n";
                sFormatAttributes( fp, e->attrib, ident + 1 );
                sIdent( fp, ident );
            }

            if( e->child )
            {
                fp << ">\n";
                if( e->child ) sFormatNodes( fp, e->child, ident + 1 );
                sIdent( fp, ident );
                fp << "</" << e->name << ">\n";
            }
            else
            {
                fp << "/>\n";
            }
            break;
        }

        case GN::XML_TEXT:
        {
            const GN::XmlText * t = root->toText();
            GN_ASSERT( t && !t->child ); // text node should have no child.
            sIdent( fp, ident );
            fp << t->text << "\n";
            break;
        }

        default:
            GN_UNEXPECTED(); // program should not reach here
            GN_ERROR( "Unknown node type!" );
            return false;
    };

    // print brothers
    return root->sibling ? sFormatNodes( fp, root->sibling, ident ) : true;
}

static void sCompactAttributes( GN::File & fp, const GN::XmlAttrib * att )
{
    for( ; 0 != att; att = att->next )
    {
        fp << " " << att->name << "=\"" << att->value << "\"";
    }
}

static bool sCompactNodes( GN::File & fp, const GN::XmlNode * root )
{
    GN_ASSERT( root );

    switch( root->type )
    {
        case GN::XML_CDATA:
        {
            const GN::XmlCdata * c = root->toCdata();
            GN_ASSERT( !c->child ); // cdata node should have no child.
            fp << "<![CDATA[" << c->text << "]]>";
            break;
        }

        case GN::XML_COMMENT:
        {
            break;
        }

        case GN::XML_ELEMENT:
        {
            const GN::XmlElement * e = root->toElement();
            GN_ASSERT( e );
            fp << "<" << e->name;
            if( e->attrib )
            {
                sCompactAttributes( fp, e->attrib );
            }
            if( e->child )
            {
                fp << ">\n";
                if( e->child ) sCompactNodes( fp, e->child );
                fp << "</" << e->name << ">";
            }
            else
            {
                fp << "/>";
            }
            break;
        }

        case GN::XML_TEXT:
        {
            const GN::XmlText * t = root->toText();
            GN_ASSERT( !t->child ); // text node should have no child.
            // TODO: trim text before print
            fp << t->text;
            break;
        }

        default:
            GN_UNEXPECTED(); // program should not reach here
            GN_ERROR( "Unknown node type!" );
            return false;
    };

    // print brothers
    return root->sibling ? sCompactNodes( fp, root->sibling ) : true;
}


static void sParseFail( ParseTracer * tracer, const char * errInfo )
{
    XML_StopParser( tracer->parser, XML_FALSE );
    tracer->result->errInfo = errInfo;
    tracer->result->errLine = XML_GetCurrentLineNumber( tracer->parser );
    tracer->result->errColumn = XML_GetCurrentColumnNumber( tracer->parser );
}

static GN::XmlNode * sNewNode( ParseTracer * tracer, GN::XmlNodeType type )
{
    GN::XmlNode * n = tracer->doc->createNode( type );
    if( 0 == n )
    {
        sParseFail(
            tracer,
            GN::strFormat( "Fail to create node with type of '%d'", type ).cptr() );
        return NULL;
    }
    n->parent = tracer->parent;
    n->sibling = NULL;
    n->child = NULL;

    // update tracer
    // update tracer
    if( tracer->prev )
    {
        // this is not the first node in this level. Let its previous sibling points to this.
        tracer->prev->sibling = n;
    }
    else if( tracer->parent )
    {
        // this is the first node in this level. Let the parent node points to this.
        tracer->parent->child = n;
    }
    tracer->parent = n;
    tracer->prev = NULL;

    return n;
}

// *****************************************************************************
// Expat handlers
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void XMLCALL sStartElementHandler(
    void * userData,
    const XML_Char * name,
    const XML_Char ** atts )
{
    GN_ASSERT( userData && name );
    
    ParseTracer * tracer = (ParseTracer*)userData;

    // create new node
    GN::XmlNode * n = sNewNode( tracer, GN::XML_ELEMENT );
    if( 0 == n ) return;
    GN::XmlElement * e = n->toElement();
    GN_ASSERT( e );

    e->attrib = NULL;
    e->name = name;

    // create attribute list
    GN::XmlAttrib * lastAttrib = NULL;
    while( *atts )
    {
        GN::XmlAttrib * a = tracer->doc->createAttrib();
        if( 0 == a )
        {
            sParseFail( tracer, "Fail to create attribute." );
            return;
        }

        a->node = e;
        a->next = NULL;
        a->name = atts[0];
        a->value = atts[1];

        if( lastAttrib )
        {
            lastAttrib->next = a;
        }
        else
        {
            e->attrib = a;
        }
        lastAttrib = a;

        atts += 2;
    }
}

//
//
// -----------------------------------------------------------------------------
void XMLCALL sEndElementHandler( void * userData, const XML_Char * )
{
    ParseTracer * tracer = (ParseTracer*)userData;

    // update tracer
    GN_ASSERT( tracer->parent );
    tracer->prev = tracer->parent;
    tracer->parent = tracer->parent->parent;
}

//
//
// -----------------------------------------------------------------------------
static void XMLCALL sStartCdataSectionHandler( void * userData )
{
    ParseTracer * tracer = (ParseTracer*)userData;

    // create new node
    GN::XmlNode * n = sNewNode( tracer, GN::XML_CDATA );
    if( 0 == n ) return;
}

//
//
// -----------------------------------------------------------------------------
static void XMLCALL sEndCdataSectionHandler( void * userData )
{
    ParseTracer * tracer = (ParseTracer*)userData;

    GN_ASSERT( tracer->parent && tracer->parent->type == GN::XML_CDATA );

    // update tracer
    tracer->prev = tracer->parent;
    tracer->parent = tracer->parent->parent;
}

//
//
// -----------------------------------------------------------------------------
static void XMLCALL sCharacterDataHandler(
    void * userData, const XML_Char * s, int len )
{
    ParseTracer * tracer = (ParseTracer*)userData;

    GN_ASSERT( tracer->parent );

    if( tracer->parent->type == GN::XML_CDATA )
    {
        GN::XmlCdata * t = tracer->parent->toCdata();
        t->text.append( s, len );
    }
    else
    {
        // skip leading spaces
        while( len > 0 && (' '==*s || '\t'==*s || '\n'==*s) )
        {
            ++s; --len;
        }
        if( 0 == len ) return;

        if( tracer->parent->type == GN::XML_ELEMENT )
        {
            if( tracer->prev && tracer->prev->type == GN::XML_TEXT )
            {
            }
        }
    }
}

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::XmlDocument::parseBuffer(
    XmlParseResult & result, const char * content, size_t length )
{
    GN_GUARD;

    result.errInfo.clear();
    result.errLine = 0;
    result.errColumn = 0;

    // create parser
    XML_Memory_Handling_Suite mm = { &sXmlMalloc, &sXmlRealloc, &sXmlFree };
    XML_Parser parser = XML_ParserCreate_MM( 0, &mm, 0 );
    if( 0 == parser )
    {
        result.errInfo = "Fail to create parser.";
        return false;
    }
    AutoFree af(parser); // free the parser automatically when go out of this function.

    // setup user data
    ParseTracer userData = { parser, this, &result, NULL, NULL };
    XML_SetUserData( parser, &userData );

    // setup handlers
    XML_SetElementHandler( parser, &sStartElementHandler, &sEndElementHandler );
    XML_SetCdataSectionHandler( parser, &sStartCdataSectionHandler, &sEndCdataSectionHandler );
    XML_SetCharacterDataHandler( parser, &sCharacterDataHandler );

    // start parse
    XML_Status status = XML_Parse( parser, content, (int)length, XML_TRUE );
    if( XML_STATUS_OK != status )
    {
        if( result.errInfo.empty() )
        {
            result.errInfo = "XML_Parse() failed.";
            result.errLine = XML_GetCurrentLineNumber( parser );
            result.errColumn = XML_GetCurrentColumnNumber( parser );
        }
        return false;
    }

    GN_ASSERT( userData.prev );
    result.root = userData.prev;

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::XmlDocument::writeToFile( File & file, const XmlNode & root, bool compact )
{
    GN_GUARD;
    if( compact )
        return sCompactNodes( file, &root );
    else
        return sFormatNodes( file, &root, 0 );
    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::XmlNode * GN::XmlDocument::createNode( XmlNodeType type )
{
    XmlNode * p;
    switch( type )
    {
        case XML_CDATA   : p = new PooledNode<XmlCdata>; break;
        case XML_COMMENT : p = new PooledNode<XmlComment>; break;
        case XML_ELEMENT : p = new PooledNode<XmlElement>; break;
        case XML_TEXT    : p = new PooledNode<XmlText>; break;
        default          : GN_ERROR( "invalid node type : %d", type ); return NULL;
    }
    mNodes.push_back( p );
    return p;
}

//
//
// -----------------------------------------------------------------------------
GN::XmlAttrib * GN::XmlDocument::createAttrib()
{
    PooledAttrib * p = new PooledAttrib;
    mAttribs.push_back( p );
    return p;
}

//
//
// -----------------------------------------------------------------------------
void GN::XmlDocument::releaseAllNodesAndAttribs()
{
    GN_GUARD;

    for( size_t i = 0; i < mNodes.size(); ++i ) delete mNodes[i];
    mNodes.clear();

    for( size_t i = 0; i < mAttribs.size(); ++i ) delete mAttribs[i];
    mAttribs.clear();

    GN_UNGUARD;
}
