/////////////////////////////////////////////////////////////////////////////
// Name:        excel.h
// Author:      Joe Yates
// Purpose:     A wrapper for Excel Automation
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef __WXMSW__

#ifndef __msw_excel_h__
#define __msw_excel_h__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/msw/ole/automtn.h>

#include <wx/exception.h>

namespace wx
{

namespace Excel
{

enum XlFileFormat
  {
  xlExcel9795       = 0x0000002b,
  xlWorkbookNormal  = 0xffffefd1,
  };

class Worksheet : public wxAutomationObject
  {
  public:
    Worksheet();
    ~Worksheet()
      {}

  public:
    void PrintPreview();
    void PrintOut(long nFrom = 1, long nTo = -1, long nCopies = 1, bool bPreview = false);
  };
 
class Sheets : public wxAutomationObject
  {
  public:
    Sheets();
    ~Sheets()
      {}

  public:
    void GetItem(long nIndex, Worksheet& wsh);
    long GetCount();
  };

class Workbook : public wxAutomationObject
  {
  public:
    Workbook();
    ~Workbook()
      {}

  public:
    // Propery accessors
    Sheets&     GetWorksheets();
    wxString    GetFullName();
    bool        GetOpen()
      {
      return (GetDispatchPtr() != NULL)? true : false;
      }
    // Method accessors
    void        Close();
    void        PrintOut(long nFrom = 1, long nTo = -1, long nCopies = 1, bool bPreview = false);
    void        PrintPreview();
    void        SaveAs(const wxString& sFileName, XlFileFormat fft = xlWorkbookNormal);

  // Members
  private:
    Sheets    m_shs;
  };

class Application;
class Workbooks : public wxAutomationObject
  {
  public:
    Workbooks();
    ~Workbooks();

  public:
    void          Open(const wxString& sFileName, Workbook& wbk);
    void          Add(Workbook& wbk);
    void          Close();
    void          GetItem(long nIndex, Workbook& wbk);
    long          GetCount();
    Application * GetApplication();

  private:
    Application * m_app;
  };

class Application : public wxAutomationObject
  {
  // friends, classes, enums and types
  
  // Constructors/destructors
  public:
    Application(bool bUseExisting = false) :
      m_bUseExisting(bUseExisting)
      {}
    ~Application()
      {}

  // Operators
  
  // Slots
  private:
    
  // Methods
  public:
    // Propery accessors
    Workbooks&  GetWorkbooks();
    Sheets&     GetWorksheets();
    wxString    GetVersion();
    void        SetVisible(bool bVisible);
    // Method accessors

  private:
    void Create();

  // Members
  private:
    bool      m_bUseExisting;
    Workbooks m_wbs;
    Sheets    m_shs;
  };

} // namespace Excel
  
} // namespace wx

#endif // ndef __msw_excel_h__

#endif
	// def __WXMSW__
