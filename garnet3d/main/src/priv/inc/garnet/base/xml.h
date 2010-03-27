#ifndef __GN_BASE_XML_H__
#define __GN_BASE_XML_H__
// *****************************************************************************
/// \file
/// \brief   XML processor. Wrap functionality of other external XML parsers. (working in progress)
/// \author  chenlee (2006.5.2)
// *****************************************************************************

namespace GN
{
    struct XmlCdata;
    struct XmlElement;
    struct XmlComment;
    class  XmlDocument;

    ///
    /// XML attribute class
    ///
    struct XmlAttrib
    {
        XmlDocument & doc;   ///< The document that this attribute belongs to.
        XmlElement  * node;  ///< pointer to the element that this attribute belongs to.
        XmlAttrib   * prev;  ///< pointer to previous attribute
        XmlAttrib   * next;  ///< pointer to next attribute
        StrA          name;  ///< attribute name
        StrA          value; ///< attribute value

        ///
        /// attach attribute to specific element
        ///
        void setOwner( XmlElement * element );

    protected:

        /// \name protected ctor/dtor to prevent user from creatiing/deleting this class.
        //@{
        XmlAttrib( XmlDocument & d ) : doc(d), node(0), prev(0), next(0) {}
        virtual ~XmlAttrib() {}
        //@}
    };

    ///
    /// XML node type
    ///
    enum XmlNodeType
    {
        XML_CDATA,   ///< cdata node
        XML_COMMENT, ///< comment node
        XML_ELEMENT, ///< element node
        NUM_XML_NODE_TYPES, ///< number of node types.
    };

    ///
    /// XML node class
    ///
    struct XmlNode
    {
        XmlDocument     & doc;     ///< reference to the owner document
        const XmlNodeType type;    ///< node type. can't be modified.
        XmlNode         * parent;  ///< pointer to parent node
        XmlNode         * prev;    ///< pointer to previous brother node
        XmlNode         * next;    ///< pointer to next brother node
        XmlNode         * child;   ///< pointer to first child

        /// \name method required by traversal class
        //@{
        XmlNode * GetParent() const { return parent; }
        XmlNode * GetPrevSibling() const { return prev; }
        XmlNode * GetNextSibling() const { return next; }
        XmlNode * GetFirstChild() const { return child; }
        //@}

        ///
        /// Change node parent
        ///
        void SetParent( XmlNode * parent, XmlNode * prev = NULL );

        ///
        /// return location of this node in the document (unimplemented)
        ///
        const char * GetLocation() const { return ""; }

        ///
        /// conver to cdata node
        ///
        XmlCdata * ToCdata() { return XML_CDATA == type ? (XmlCdata*)this : 0; }

        ///
        /// conver to comment node
        ///
        const XmlCdata * ToCdata() const { return XML_CDATA == type ? (const XmlCdata*)this : 0; }

        ///
        /// conver to comment node
        ///
        XmlComment * ToComment() { return XML_COMMENT == type ? (XmlComment*)this : 0; }

        ///
        /// conver to comment node
        ///
        const XmlComment * ToComment() const { return XML_COMMENT == type ? (const XmlComment*)this : 0; }

        ///
        /// Convert to element node
        ///
        XmlElement * ToElement() { return XML_ELEMENT == type ? (XmlElement*)this : 0; }

        ///
        /// Convert to element node
        ///
        const XmlElement * ToElement() const { return XML_ELEMENT == type ? (const XmlElement*)this : 0; }

        ///
        /// virtual dtor
        ///
        virtual ~XmlNode() {}

    protected:

        ///
        /// protected ctor to prevent user from creatiing this class.
        ///
        XmlNode( XmlDocument & d, XmlNodeType t )
            : doc(d)
            , type(t)
            , parent(0)
            , prev(0)
            , next(0)
            , child(0)
        {
            GN_ASSERT( 0 <= t && t < NUM_XML_NODE_TYPES );
        }
    };

    ///
    /// XML cdata node
    ///
    struct XmlCdata : public XmlNode
    {
        StrA text;  ///< text content.

    protected:

        ///
        /// protected ctor to prevent user from creatiing this class.
        ///
        XmlCdata( XmlDocument & d ) : XmlNode(d,XML_CDATA) {}
    };

    ///
    /// XML comment node
    ///
    struct XmlComment : public XmlNode
    {
        StrA text; ///< comment text

    protected:

        ///
        /// protected ctor to prevent user from creatiing this class.
        ///
        XmlComment( XmlDocument & d ) : XmlNode(d,XML_COMMENT) {}
    };

    ///
    /// XML element node
    ///
    /// \note
    ///     Here we assume that one element can have, at most, one text section.
    ///     If there were multiple text sections, they would be merged into one.
    ///
    struct XmlElement : public XmlNode
    {
        XmlAttrib * attrib;  ///< pointer to first attribute
        StrA        name;    ///< element name
        StrA        text;    ///< optional text section

        ///
        /// find specific attribute of element
        ///
        XmlAttrib * FindAttrib( const StrA & name, StringCompareCase sc = StringCompareCase::SENSITIVE ) const
        {
            for( XmlAttrib * a = attrib; a; a = a->next )
            {
                if( StringCompareCase::SENSITIVE == sc )
                {
                    if( name == a->name ) return a;
                }
                else
                {
                    if( 0 == StringCompareI( name.ToRawPtr(), a->name.ToRawPtr() ) ) return a;
                }
            }
            return NULL;
        }

        ///
        /// find specific child of element
        ///
        XmlElement * FindChildElement( const StrA & name, StringCompareCase sc = StringCompareCase::SENSITIVE ) const
        {
            for( XmlNode * n = child; n; n = n->next )
            {
                XmlElement * e = n->ToElement();
                if( !e ) continue;

                if( StringCompareCase::SENSITIVE == sc )
                {
                    if( name == e->name ) return e;
                }
                else
                {
                    if( 0 == StringCompareI( name.ToRawPtr(), e->name.ToRawPtr() ) ) return e;
                }
            }
            return NULL;
        }

    protected:

        ///
        /// protected ctor to prevent user from creatiing this class.
        ///
        XmlElement( XmlDocument & d )
            : XmlNode(d,XML_ELEMENT)
            , attrib(0)
        {
        }
    };

    ///
    /// XML parse result
    ///
    struct XmlParseResult
    {
        XmlNode * root;   ///< root node of the xml document
        size_t errLine;   ///< error position
        size_t errColumn; ///< error position
        StrA errInfo;     ///< error information
    };

    ///
    /// XML document
    ///
    class XmlDocument
    {
        // T must be one of XML node class
        template<class T> struct PooledNode : public T
        {
            PooledNode( XmlDocument & d ) : T(d) {}
        };
        struct PooledAttrib : public XmlAttrib
        {
            PooledAttrib( XmlDocument & d ) : XmlAttrib(d) {}
        };

        DynaArray<XmlNode*>      mNodes;
        DynaArray<PooledAttrib*> mAttribs;

    public:

        ///
        /// ctor
        ///
        XmlDocument() { mNodes.Reserve(256); mAttribs.Reserve(256); }

        ///
        /// dtor
        ///
        ~XmlDocument() { ReleaseAllNodesAndAttribs(); }

        ///
        /// parse xml string buffer
        ///
        bool Parse( XmlParseResult & result, const char * content, size_t length = 0 );

        ///
        /// parse xml file
        ///
        bool Parse( XmlParseResult &, File & );

        ///
        /// write xml document to file. If compact if false, the format output with newline and ident
        ///
        bool WriteToFile( File & file, const XmlNode & root, bool compact );

        ///
        /// Create new node. Nodes are created in pooled memory. No need
        /// to release them. They will be release automatically, when the
        /// XML document is deleted.
        ///
        XmlNode * CreateNode( XmlNodeType type, XmlNode * parent );

        ///
        /// Create element
        ///
        XmlElement * CreateElement( XmlNode * parent )
        {
            XmlNode * n = CreateNode( XML_ELEMENT, parent );
            return n ? n->ToElement() : NULL;
        }

        ///
        /// Create new attribute. Attributes are created in pooled memory also,
        /// just like XmlNode.
        ///
        XmlAttrib * CreateAttrib( XmlElement * owner );

    private:

        ///
        /// Release all attributes and nodes
        ///
        void ReleaseAllNodesAndAttribs();
    };

    ///
    /// load something from XML file
    ///
    template<class T>
    inline bool LoadFromXmlFile( T & t, File & fp, const StrA & basedir )
    {
        GN_GUARD;

        XmlDocument doc;
        XmlParseResult xpr;
        if( !doc.Parse( xpr, fp ) )
        {
            static Logger * sLogger = GetLogger( "GN.base.xml" );
            GN_ERROR(sLogger)(
                "Fail to parse XML file (%s):\n"
                "    line   : %d\n"
                "    column : %d\n"
                "    error  : %s",
                fp.Name(),
                xpr.errLine,
                xpr.errColumn,
                xpr.errInfo.ToRawPtr() );
            return false;
        }
        GN_ASSERT( xpr.root );
        return t.LoadFromXml( *xpr.root, basedir );

        GN_UNGUARD;
    }

    ///
    /// load something from XML file
    ///
    template<class T>
    inline bool LoadFromXmlFile( T & t, const StrA & filename )
    {
        GN_GUARD;

        static Logger * sLogger = GetLogger( "GN.base.xml" );
        GN_INFO(sLogger)( "Load '%s'", filename.ToRawPtr() );

        AutoObjPtr<File> fp( fs::OpenFile( filename, "rt" ) );
        if( !fp ) return false;

        StrA basedir = fs::DirName( filename );

        return LoadFromXmlFile( t, *fp, basedir );

        GN_UNGUARD;
    }
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_XML_H__
