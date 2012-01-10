/////////////////////////////////////////////////////////////////////////////
// Name:        xmldom.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef wxXMLDOM_XERCES

#ifndef __wx_xmldom_h__
#define __wx_xmldom_h__

#include "wx/defs.h"

#include <xercesc/dom/DOM.hpp>

#include <vector>

using namespace XERCES_CPP_NAMESPACE;

namespace wx
{

class XmlNode;

/**
 *	The attributes of an XML node.
 *  This class is a container for the attributes of an XML node.
 */
class XmlAttributes
  {
  // friends, classes, enums and types
	friend class XmlNode;

  // Constructors/destructors
  private:
		/** Create a ready-to-use XmlAttributes instance.
		 *  This ctor is used to create instances
		 *	when the XML document has already been loaded.
		 *  \param 			elmParent The parent XmlNode's DOMElement
		 *  \exception 	None
		 */ 
    XmlAttributes(DOMElement * elmParent);
	
		/** Create an empty XmlAttributes.
		 *  This ctor is used by XmlNode.
		 *  \exception 		None
		 */ 
    XmlAttributes() {};

  // Operators
	public:
		/**
		 *	Get an attribute value.
		 *  Returns the attribute, or throws an exception if the 
	   *  attribute does not exist.
	   *  \param[in] sName  	The name of the attribute.
		 *  \exception wxString The attribute does not exist.
		 *  \return    					The value as a wxString
		 */ 
		wxString operator[](const wxString& sName) const;

  // Slots
  // Methods
	private:
		/**
		 *	Supply the element whose attributes we represent.
		 *  Allows an XmlNode to set its attributes' element after
		 *  creation.
		 */ 
		void SetNode(DOMElement * elmParent);

  // Members
	private:
		DOMElement *	m_elmParent;
  };

class XmlDocument;
class XmlNodeArray;

class XmlNode
  {
  // friends, classes, enums and types
	friend class XmlDocument; // It uses XmlNode() and SetNode()

  // Constructors/destructors
  public:
    XmlNode(const XmlNode& nodOther);
    ~XmlNode();
	private:
    XmlNode() {}; // Declared for XmlDocument()
    XmlNode(DOMElement * elm);

  // Operators

  // Methods
  public:
		/**
		 *	Perform an XPath query.
     *  \param 			sXPathQuery An XPath query
     *  \exception 	None
     *  \return 			An XmlNodeArray containing matching nodes
     */ 
		XmlNodeArray	GetNodes(const wxString& sXPathQuery);
		XmlNode				GetNode(const wxString& sXPathQuery);
		wxString			ToString();
		wxString			GetText();

	private:
		void					SetNode(DOMElement * elm);

  // Members
	public:
		XmlAttributes	Attributes;
	private:
		DOMElement *	m_elm;
  };

class XmlDocument : public XmlNode
  {
  // friends, classes, enums and types
  // Constructors/destructors
  public:
    XmlDocument();
    ~XmlDocument();

  // Operators
  // Methods
	public:
		/**
		 *	Load an XML document.
		 *  \param sPathName The full path name of the XML file.
		 *  \exception None.
		 *  \return void
		 */
		void			Load(const wxString& sPathName);
		void			Save();
		void			SaveAs(const wxString& sPathName);

  // Members
	private:
		DOMDocument *	doc;
		wxString	m_sPathName;
  };

class XmlNodeArray : private std::vector<XmlNode>
	{
  // friends, classes, enums and types
	friend class XmlNode; // It uses Push()

  // Constructors/destructors
  public:

  // Operators
  public:
		const XmlNode&	operator[](int i);

  // Methods
  public:
		int							GetLength();

	private:
		void Push(const XmlNode& nod);

  // Members
	};

} // namespace wx

#endif // ndef __wx_xmldom_h__

#endif
	// def wxXMLDOM_XERCES
