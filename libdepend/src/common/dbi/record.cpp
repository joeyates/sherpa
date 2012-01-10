/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dbi/record.cpp
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include <wx/dbi/record.h>
#include <wx/dbi/types.h>

namespace wx
{

namespace DBI
{

////////////////////////////////////////////////////////////////////////////////////
// Record

#ifndef _MSC_VER

template <typename Type>
long Record<Type>::GetLong(Type typKey) const
	{
	wxVariant v;
	GetValue(typKey, v, TYPENAME_LONG);
	long n = v.GetLong();
	return n;
	}

template <typename Type>
wxString Record<Type>::GetString(Type typKey) const
	{
	wxVariant v;
	GetValue(typKey, v, TYPENAME_STRING);
	wxString s = v.GetString();
	return s;
	}

template <typename Type>
double Record<Type>::GetDouble(Type typKey) const
  {
	wxVariant v;
	GetValue(typKey, v, TYPENAME_DOUBLE);
	double d = v.GetDouble();
	return d;
  }

#endif // ndef _MSC_VER

#if WX_DBI_USE_BLOB
template <typename Type>
BLOB * Record<Type>::GetBLOB(Type typKey)  // Always requires a Release()
	{
	wxVariant v;
	GetValue(typKey, v, TYPENAME_WXOBJECT);

	wxObject * po = v.GetWxObjectPtr();
	BLOB * pblb = dynamic_cast<BLOB *>(po);
	
	pblb->AddRef();
	return pblb;
	}
#endif // WX_DBI_USE_BLOB

template <typename Type>
void Record<Type>::DumpField(wxVariant& v)
	{
  wxString sType = v.GetType();
  wxLogDebug(wxT("\tType: \"%s\""), sType.c_str());

  if(sType == TYPENAME_BOOLEAN)
    wxLogDebug(wxT("\t\t%u"), v.GetBool());
  else if(sType == TYPENAME_LONG)
    wxLogDebug(wxT("\t\t%u"), v.GetLong());
  else if(sType == TYPENAME_DOUBLE)
    {
    double d = v.GetDouble();
    wxLogDebug(wxT("\t\t%f"), d);
    }
  else if(sType == TYPENAME_STRING)
    {
    wxString s = v.GetString();
    wxLogDebug(wxT("\t\t\"%s\""), s.c_str());
    }
#if WX_DBI_USE_BLOB
  else if(sType == TYPENAME_WXOBJECT)
    {
    wxObject * po = v.GetWxObjectPtr();
    BLOB * pblb = dynamic_cast<BLOB *>(po);
    const void * pv = pblb->GetPtr();
    wxLogDebug(wxT("\t\tpv => %08x"), pv);
    }
#endif // WX_DBI_USE_BLOB
  else
    wxLogDebug(wxT("Unhandled type: '%s'"), sType.c_str());
	}

#if WX_DBI_USE_BLOB
template <typename Type>
void Record<Type>::DoCleanup(wxVariant& v)
	{
	wxString sType = v.GetType();
  if(sType == TYPENAME_WXOBJECT)
    {
    wxObject * po = v.GetWxObjectPtr();
    BLOB * pblb = dynamic_cast<BLOB *>(po);
    int nNewRefCount = pblb->Release();
    }
  }
#endif // WX_DBI_USE_BLOB

///////////////////////////////////////////////////////////////////////////
// ArrayRecord

ArrayRecord::~ArrayRecord()
	{
#if WX_DBI_USE_BLOB
	// Make sure we delete any wxObject pointers
	for(iterator it = begin(); it != end(); it++)
		{
    DoCleanup(*it);
		}
#endif // WX_DBI_USE_BLOB
	clear();
	}

void ArrayRecord::GetValue(int i, wxVariant& v, const wxString& sExpectedType) const
	{
	if(i < 0 || (unsigned int) i >= size())
    {
    wxString sError = wxString::Format
      (
      wxT("wx::DBI::ArrayRecord::GetValue(): field %u does not exist"),
      i
      );
		throw Exception(sError);
    }
	
	v = at(i);
	wxString sType = v.GetType();
	if(!sExpectedType.IsEmpty() && sType != sExpectedType)
    {
    wxString sThrowError = wxString::Format
      (
      wxT("wx::DBI::ArrayRecord::GetValue(): value for field %u is a '%s', not a '%s'"),
      i,
      sType.c_str(),
      sExpectedType.c_str()
      );
		throw Exception(sThrowError);
    }
	}

void ArrayRecord::Dump()
  {
  for(unsigned int i = 0; i < size(); i++)
    {
		wxLogDebug(wxT("[%u]"), i);
    wxVariant v = at(i);
    DumpField(v);
    }
  }

// explicit function template instantiation declarations for ArrayRecord (T = int):
// MSVC requires them as inline, so they're in the .h
#ifndef _MSC_VER
template wxString Record<int>::GetString(int nIndex) const;
template long     Record<int>::GetLong(int nIndex) const;
# if WX_DBI_USE_BLOB
template BLOB *   Record<int>::GetBLOB(int nIndex);
# endif // WX_DBI_USE_BLOB
template double   Record<int>::GetDouble(int nIndex) const;
#endif // ndef _MSC_VER

wxArrayString ArrayRecord::GetArrayString() const
  {
  wxArrayString as;
	for(const_iterator it = begin(); it != end(); it++)
		as.push_back(*it);

  return as;
  }

///////////////////////////////////////////////////////////////////////////
// HashRecord

HashRecord::~HashRecord()
	{
	/*
	// Make sure we delete any wxObject pointers
	for(iterator it = begin(); it != end(); it++)
		{
		wxVariant v = it->second;
    DoCleanup(v);
		}
	*/
	}

void HashRecord::GetValue(const wxString& sKey, wxVariant& v, const wxString& sExpectedType) const
	{
	const_iterator it = find(sKey);
	if(it == end())
    {
    wxString sError = wxString::Format
      (
      wxT("wx::DBI::HashRecord::GetValue(): value '%s' does not exist"),
      sKey.c_str()
      );
		throw Exception(sError);
    }
	
	v = wxVariant(it->second);
	wxString sType = v.GetType();
	if(!sExpectedType.IsEmpty() && sType != sExpectedType)
    {
    wxString sThrowError = wxString::Format
      (
      wxT("wx::DBI::HashRecord::GetValue(): value for key '%s' is a '%s', not a '%s'"),
      sKey.c_str(),
      sType.c_str(),
      sExpectedType.c_str()
      );
		throw Exception(sThrowError);
    }
	}

void HashRecord::Dump()
	{
	for(const_iterator it = begin(); it != end(); it++)
		{
		wxString sKey = it->first;
		wxVariant v = it->second;

		wxLogDebug(wxT("Field \"%s\""), sKey.c_str());
    DumpField(v);
		}
	}

// explicit function template instantiation declarations for HashRecord (T = wxString):
#ifndef _MSC_VER
template wxString Record<const wxString&>::GetString(const wxString& sKey) const;
template long     Record<const wxString&>::GetLong(const wxString& sKey) const;
# if WX_DBI_USE_BLOB
template BLOB *   Record<const wxString&>::GetBLOB(const wxString& sKey);
# endif // WX_DBI_USE_BLOB
template double   Record<const wxString&>::GetDouble(const wxString& sKey) const;
#endif // ndef _MSC_VER

/////////////////////////////////
// RecordArray

#ifdef _MSC_VER
template<ArrayRecord> void   RecordArray<ArrayRecord>::Dump();
template<HashRecord> void    RecordArray<HashRecord>::Dump();
#else // ifdef _MSC_VER
template<> void   RecordArray<ArrayRecord>::Dump();
template<> void   RecordArray<HashRecord>::Dump();
#endif // def _MSC_VER

#if WX_DBI_USE_BLOB
/////////////////////////////////
// BLOB

BLOB::BLOB(const void * pvc, int nLengthNew) :
	wxObject(),
	pv(0), nLength(nLengthNew), nRefCount(0)
	{
	if(pvc == 0)
		throw Exception(wxT("wxSQLite3::BLOB::BLOB(): 'pv' parameter is NULL"));

	if(nLengthNew == 0)
		throw Exception(wxT("wxSQLite3::BLOB::BLOB(): 'nLength' parameter is 0"));

	pv = malloc(nLength);
	memcpy(pv, pvc, nLength);
	AddRef();
	}

BLOB::~BLOB()
	{
	Release();
  if(nRefCount != 0)
    wxLogDebug(wxT("BLOB::~BLOB() Warning, nRefCount is non-zero: %u"), nRefCount);
	}

/////////////////////////////////
// Public methods

const void * BLOB::GetPtr() const
	{
	return (const void *) pv;
	}

int BLOB::GetLength() const
	{
	return nLength;
	}

int BLOB::AddRef()
	{
	nRefCount++;
	return nRefCount;
	}

int BLOB::Release()
	{
	nRefCount--;
	if(nRefCount == 0)
		{
		if(pv != 0)
			{
			free(pv);
			pv = 0;
      delete this;
			}
		}
	return nRefCount;
	}
#endif // WX_DBI_USE_BLOB

} // namespace DBI

} // namespace wx
