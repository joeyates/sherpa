/////////////////////////////////////////////////////////////////////////////
// Name:        wx/exception.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_exception_h__
#define __wx_exception_h__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

namespace wx
{

#ifdef _MSC_VER
// See MSDN ID: Q119394
#define WorkaroundVa_start(ap,v) \
  { \
	int var= _INTSIZEOF(v); \
	__asm lea eax,v \
	__asm add eax,var \
	__asm mov ap,eax \
	}
#endif // def _MSC_VER

class Exception
  {
  // friends, classes, enums and types

  // Constructors/destructors
  public:
    Exception() :
			m_sError(wxT("")),
      m_szFile(NULL),
      m_nLine(-1)
      {
      }

    Exception(const Exception& e)
      {
			m_sError = e.m_sError;
      m_szFile = e.m_szFile;
      m_nLine = e.m_nLine;
      }

    Exception(const wxString& sError, ...) :
      m_szFile(NULL),
      m_nLine(-1)
      {
      va_list argptr;
#ifdef _MSC_VER
      WorkaroundVa_start(argptr, sError);
#else
      va_start(argptr, sError);
#endif // def _MSC_VER
      m_sError.PrintfV(sError, argptr);
      va_end(argptr);
      }

    Exception(const char * szFile, int nLine, const wxString& sError, ...) :
      m_szFile(szFile),
      m_nLine(nLine)
      {
      va_list argptr;
#ifdef _MSC_VER
      WorkaroundVa_start(argptr, sError);
#else
      va_start(argptr, sError);
#endif // def _MSC_VER
      m_sError.PrintfV(sError, argptr);
      va_end(argptr);
      }

    Exception(const Exception& e, const wxString& sError, ...)
      {
      va_list argptr;
#ifdef _MSC_VER
      WorkaroundVa_start(argptr, sError);
#else
      va_start(argptr, sError);
#endif // def _MSC_VER
      m_sError.PrintfV(sError, argptr);
      va_end(argptr);
			
			m_sError << wxT("\n") + e.GetString();
      }

    virtual Exception::~Exception()
      {}

		void PrintfV(const wxString& sError, va_list argptr)
			{
      m_sError.PrintfV(sError, argptr);
			}

  // Operators
	public:
    virtual operator wxString()
      { return GetString(); }
    virtual const char * GetFile()
      { return m_szFile; }
    virtual const int GetLine()
      { return m_nLine; }

  // Methods
	public:
		const wxString& GetString() const
      {
      return m_sError;
      }

  // Members
	protected:
		wxString      m_sError;
    const char *  m_szFile;
    int           m_nLine;
  };

} // namespace wx

#endif // ndef __wx_exception_h__
