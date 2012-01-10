/////////////////////////////////////////////////////////////////////////////
// Name:        .cpp
// Author:      Joe Yates
// Purpose: 		
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/xmldom.h"

#include <pathan/internal/DOMutils/XStr.hpp> // for X()

#include <pathan/PathanPlatformUtils.hpp> // for PathanPlatformUtils::initialize() and PathanPlatformUtils::terminate()
#include <pathan/XPath2Result.hpp>

#include <string>

using namespace XERCES_CPP_NAMESPACE;

namespace wx
{

typedef std::basic_string<unsigned short> String16;

///////////////////////////////////////////////////////////////////////////////////////////////////
// String conversion functions (16 bit <-> 32 bit)

wxString String16TowxString(const String16 s16)
	{
	int nLength = s16.size();
	wchar_t * wsz = new wchar_t[nLength + 1];
	for(int i = 0; i < nLength; i++)
		wsz[i] = s16[i];
	wsz[nLength] = 0;
	wxString s = wsz;
	delete [] wsz;
	return s;
	}

String16 wxStringToString16(const wxString& s)
	{
	int nLength = s.Length();
	const wchar_t * wsz = (const wchar_t *) s.c_str();
	String16 s16;
	s16.reserve(nLength + 1);
	for(int i = 0; i < nLength; i++)
		s16[i] = wsz[i];
	s16[nLength] = 0;
	return s16;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
// XmlNode

/////////////////////////////////
// Constructors/destructors

XmlNode::XmlNode(const XmlNode& nodOther) :
	Attributes(nodOther.m_elm)
	{
	m_elm = nodOther.m_elm;
	}

XmlNode::~XmlNode()
	{
	}

XmlNode::XmlNode(DOMElement * elm) :
	Attributes(elm)
	{
	SetNode(elm);
	}

/////////////////////////////////
// Operators

/////////////////////////////////
// Public Methods

// Run an XPath query
XmlNodeArray XmlNode::GetNodes(const wxString& sXPathQuery)
	{
	String16 s16Query = wxStringToString16(sXPathQuery);
	DOMDocument * doc = m_elm->getOwnerDocument();

  const XPath2Result * xpr = (const XPath2Result *) doc->evaluate
		(
		s16Query.c_str(),
		(DOMNode *) m_elm,
		NULL,
		XPath2Result::SNAPSHOT_RESULT,
		0
		); 

	// TBI: handle Text nodes, etc
	XmlNodeArray anod;
	for(int i = 0; i < xpr->getSnapshotLength(); i++)
	  {
		xpr->snapshotItem(i);
    if(xpr->isNode())
			{
			const DOMNode * pnod = xpr->asNode();
			XmlNode nod((DOMElement *) pnod);
			anod.Push(nod);
			}
		}

	return anod;
	}

// Run an XPath query
XmlNode XmlNode::GetNode(const wxString& sXPathQuery)
	{
	String16 s16Query = wxStringToString16(sXPathQuery);
	DOMDocument * doc = m_elm->getOwnerDocument();

  const XPath2Result * xpr = (const XPath2Result *) doc->evaluate
		(
		s16Query.c_str(),
		(DOMNode *) m_elm,
		NULL,
		XPath2Result::SNAPSHOT_RESULT,
		0
		);

	if(xpr->getSnapshotLength() == 0)
		throw wxString(_T("No node found"));

	xpr->snapshotItem(0);
	if(!xpr->isNode())
		throw wxString(_T("No node found"));

	const DOMNode * pnod = xpr->asNode();
	return XmlNode((DOMElement *) pnod);
	}

wxString XmlNode::ToString()
	{
	DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
  DOMWriter * wrt = impl->createDOMWriter();

  const XMLCh * xmsDocument = wrt->writeToString(*m_elm); // TBI: needs release
	wxString s = String16TowxString(xmsDocument);
  wrt->release();

	return s;
	}

wxString XmlNode::GetText()
	{
	DOMNode * nod = m_elm->getFirstChild();
	wxString s = _T("");
	while(nod != NULL)
		{
		DOMNode::NodeType ntp = (DOMNode::NodeType) nod->getNodeType();
		if(ntp == DOMNode::TEXT_NODE)
			{
			DOMText * txt = (DOMText *) nod;
			const XMLCh * xmsText = txt->getData();
			wxString sPart = String16TowxString(xmsText);
			s += sPart;
			
			nod = nod->getNextSibling();
			}
		}

	return s;
	}

/////////////////////////////////
// Private Methods

void XmlNode::SetNode(DOMElement * elm)
	{
	m_elm = elm;
	Attributes.SetNode(m_elm);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
// XmlAttributes

/////////////////////////////////
// Constructors/destructors

XmlAttributes::XmlAttributes(DOMElement * elmParent) :
	m_elmParent(elmParent)
	{
	}

/////////////////////////////////
// Operators

wxString XmlAttributes::operator[](const wxString& sAttribute) const
	{
	String16 s16Attribute = wxStringToString16(sAttribute);
	const XMLCh * xmsValue = m_elmParent->getAttribute(s16Attribute.c_str());
	wxString sValue = String16TowxString(xmsValue);
	return sValue;
	}

/////////////////////////////////
// Public Methods

/////////////////////////////////
// Private Methods

void XmlAttributes::SetNode(DOMElement * elmParent)
	{
	m_elmParent = elmParent;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
// XmlNodeArray

/////////////////////////////////
// Constructors/destructors

int XmlNodeArray::GetLength()
	{
	return size();
	}

/////////////////////////////////
// Operators

const XmlNode& XmlNodeArray::operator[](int i)
	{
	if(i < 0 || i >= size())
		throw wxString(_T("XmlNodeArray::GetItem() Array index out of bounds"));
	
	return at(i);
	}

/////////////////////////////////
// Public Methods

/////////////////////////////////
// Private Methods

void XmlNodeArray::Push(const XmlNode& nod)
	{
	push_back(nod);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
// XmlDocument

/////////////////////////////////
// Constructors/destructors

XmlDocument::XmlDocument()
  {
  try
		{
    PathanPlatformUtils::initialize();
	  }
	catch (const XMLException& eXerces)
		{
		wxString sError((wchar_t *) (XMLCh*) eXerces.getMessage());
    throw sError;
  	}
  }

XmlDocument::~XmlDocument()
  {
  PathanPlatformUtils::terminate();
	}

/////////////////////////////////
// Operators

/////////////////////////////////
// Slots

/////////////////////////////////
// Public Methods

void XmlDocument::Load(const wxString& sPathName)
	{
	DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
  DOMBuilder * prs = impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
//   prs->setFeature(X("namespaces"), true);
//   prs->setFeature(X("http://apache.org/xml/features/validation/schema"), true);
//   prs->setFeature(X("validation"), true);
	String16 s16PathName = wxStringToString16(sPathName);
  doc = prs->parseURI(s16PathName.c_str());
	if(doc == NULL)
		throw wxString(_T("Can't load"));

	DOMElement * elmDocument = doc->getDocumentElement();
	XmlNode::SetNode(elmDocument);
	}

void XmlDocument::Save()
	{
	}

void XmlDocument::SaveAs(const wxString& sPathName)
	{
	}

/////////////////////////////////
// Private Methods

} // namespace wx
