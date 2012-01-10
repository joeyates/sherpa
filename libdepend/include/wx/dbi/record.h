/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dbi/record.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_dbi_record_h__
#define __wx_dbi_record_h__

// Set 'WX_DBI_USE_BLOB' to 1 to include ref-counted handling of BLOBs
#define WX_DBI_USE_BLOB 0

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wx/wx.h"

#ifdef _MSC_VER
#pragma warning (disable : 4786) // identifier was truncated to '255' characters in the browser information
#endif // def _MSC_VER

#include <map>
#include <vector>

#include <wx/variant.h>

#include <wx/exception.h>

namespace wx
{

namespace DBI
{

#if WX_DBI_USE_BLOB
class BLOB;
#endif // WX_DBI_USE_BLOB

/**
 *  The virtual base class of the two Record containers: HashRecord and ArrayRecord.
 *  Provides keyed access to database records.
 */
template<typename Type>
class Record
  {
	public:
		virtual ~Record()
			{}

  // Methods
	public:
#ifdef _MSC_VER
    // Inlined due to MS bug: Q128789
    long GetLong(Type typKey) const
	    {
	    wxVariant v;
	    GetValue(typKey, v, TYPENAME_LONG);
	    long n = v.GetLong();
	    return n;
	    }

    wxString GetString(Type typKey) const
	    {
	    wxVariant v;
	    GetValue(typKey, v, TYPENAME_STRING);
	    wxString s = v.GetString();
	    return s;
	    }

    double GetDouble(Type typKey) const
      {
	    wxVariant v;
	    GetValue(typKey, v, TYPENAME_DOUBLE);
	    double d = v.GetDouble();
	    return d;
      }
#else
		long			    GetLong(Type typKey) const;
		wxString	    GetString(Type typKey) const;
# if WX_DBI_USE_BLOB
		BLOB *		    GetBLOB(Type typKey); // Always requires a Release()
# endif // WX_DBI_USE_BLOB
		double 		    GetDouble(Type typKey) const;
#endif // def _MSC_VER

    /**
     *  Return the field indexed by 'typKey'.
     *  \param       typKey A key to a field.
     *     This is an int for ArrayRecord, and a wxString for HashRecord.
     *  \param       v      Receives the value.
     *  \param       sExpectedType  Indicates the type of field: long, string, etc
     *  \exception   Throws a wx::Exception of there is no value for the
     *     given key.
     *  \return      void
     */
    virtual void  GetValue
      (
      Type typKey,
      wxVariant& v,
      const wxString& sExpectedType
      ) const                                           = 0;
		virtual void	Dump()                                = 0;

  protected:
		void          DumpField(wxVariant& v);
  };

/**
 *  Record container with int keys.
 */
class ArrayRecord :
  public Record<int>,
  public std::vector<wxVariant>
	{
  // Constructors/destructors
	public:
		~ArrayRecord();

  // Operators
	public:

    /**
     *  Convert the ArrayRecord to a wxArrayString.
     *  \return  Converts all values to wxString and returns them in a wxArrayString.
     */
    wxArrayString GetArrayString() const;
    //operator wxArrayString() const;

  // Methods

  // 'Record<>' implementation
	public:
		/** Return the ith element of the record
		  */
		void GetValue
      (
      int i,
      wxVariant& v,
      const wxString& sExpectedType = wxT("")
      ) const;
		void			  Dump();
  };

class HashRecord :
	public Record<const wxString&>,
	public std::map<wxString, wxVariant>
	{
  // friends, classes, enums and types

  // Constructors/destructors
	public:
		~HashRecord();

  // Methods
	
	public:
    /**
     *  Get a named field.
     *  \param sKey						The key to the field.
     *  \param v     					Receives the value.
     *  \param sExpectedType  Indicates the type of field: long, string, etc
     */
		void GetValue
			(
			const wxString& sKey,
			wxVariant& v,
			const wxString& sExpectedType = wxT("")
			) const;
		void			  Dump();
	};

template<class Type>
class RecordArray : public std::vector<Type>
  {
  // Methods
	public:
    // N.B. this fn is inlined as the MS compiler gives
    // an unresolved external error if it the implementation
    // is in a separate source file
		void Dump()
      {
      for(int i = 0; i < this->size(); i++)
        {
		    wxLogDebug(wxT("Record %u"), i);
        this->at(i).Dump();
        }
      }
  };

class ArrayRecordArray : public RecordArray<ArrayRecord>
  {
  };

class HashRecordArray : public RecordArray<HashRecord>
  {
  };

#if WX_DBI_USE_BLOB
/*
 *  Holds ref-counted data object and data length.
 *  Any object which returns a pointer should AddRef.
 *  The pointer should then be Release()-ed.
 */
class BLOB : public wxObject
	{
	public:
		BLOB(const void * pvc, int nLengthNew);
		~BLOB();

	public:
		const void *	GetPtr() const;
		int						GetLength() const;
		int						AddRef();
		int						Release();

	private:
		wxObject			poData; // This object holds the ref-counted data pointer
		int						nLength;
		// Holds void * to data
		// The data pointer is owned by the object, which frees it on destruction
		void *				pv; // malloc-ed by ctor, free-d when nRefCount goes to 0 in Release()
		int						nRefCount;
	};
#endif // WX_DBI_USE_BLOB

} //namespace DBI

} //namespace wx

#endif // ndef __wx_dbi_record_h__
