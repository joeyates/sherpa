/////////////////////////////////////////////////////////////////////////////
// Name:        
// Author:      Joe Yates
// Purpose:     
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef __WXMSW__

#include "wx/msw/excel.h"

namespace wx
{

namespace Excel
{
  
void Application::SetVisible(bool bVisible)
  {
  Create();

  wxVariant avProperty[1];
  avProperty[0] = wxVariant(bVisible);
  PutProperty(wxT("Visible"), 1, avProperty);
  }

wxString Application::GetVersion()
  {
  Create();

  wxVariant vsVersion = GetProperty(wxT("Version"));
  return vsVersion.GetString();
  }

Workbooks& Application::GetWorkbooks()
  {
  Create();

  if(m_wbs.GetDispatchPtr() == NULL)
    {
    bool bGotWorkbooks = GetObject(m_wbs, wxT("Workbooks"));
    if(!bGotWorkbooks)
      throw Exception(wxT("Could not get Workbooks object reference."));
    }
  return m_wbs;
  }

Sheets& Application::GetWorksheets()
  {
  Create();

  if(m_shs.GetDispatchPtr() == NULL)
    {
    bool bGotSheets = GetObject(m_shs, wxT("Worksheets"));
    if(!bGotSheets)
      throw Exception(wxT("Could not get Worksheets collection object reference."));
    }
  return m_shs;
  }

void Application::Create()
  {
  if(GetDispatchPtr() != NULL)
    return;

  if(m_bUseExisting)
    {
    // Try to attach to an existing instance
    GetInstance(wxT("Excel.Application"));

    if(GetDispatchPtr() != NULL)
      return;
    }

  bool bCreated = CreateInstance(wxT("Excel.Application"));
  if(!bCreated)
    throw Exception(wxT("Could not create Excel object."));
  }

Workbooks::Workbooks() :
  wxAutomationObject(),
  m_app(NULL)
  {
  }

Workbooks::~Workbooks()
  {
  delete m_app;
  }

void Workbooks::Open(const wxString& sFileName, Workbook& wbk)
  {
  wxVariant av[1];
  av[0] = wxVariant(sFileName, wxT("Filename"));
  CallMethod(wxT("Open"), 1, av);

  GetApplication()->GetObject(wbk, wxT("ActiveWorkbook"));
  }

Application * Workbooks::GetApplication()
  {
  if(m_app == NULL)
    {
    m_app = new Application;

    bool bGotApp = GetObject(*m_app, wxT("Application"));
    if(!bGotApp)
      throw Exception(wxT("Could not get Application object reference."));
    }
  
  return m_app;
  }

void Workbooks::Add(Workbook& wbk)
  {
  GetObject(wbk, wxT("Add"));
  }

void Workbooks::Close()
  {
  CallMethod(wxT("Close"));
  }

// Collection is 1-based
void Workbooks::GetItem(long nIndex, Workbook& wbk)
  {
  wxVariant avWorkbooksItem[1];
  avWorkbooksItem[0] = wxVariant(nIndex);
  bool bGotWorkbook = GetObject(wbk, wxT("Item"), 1, avWorkbooksItem);
  if(!bGotWorkbook)
    throw Exception(wxT("Workbooks::GetItem() Cannot get Workbook"));
  }

long Workbooks::GetCount()
  {
  wxVariant vCount = GetProperty(wxT("Count"));
  if(!vCount.IsType(wxT("long")))
    throw Exception(wxString(wxT("Workbooks::GetCount() Incorrect type: ")) + vCount.GetType());

  return vCount.GetLong();
  }

Workbook::Workbook() :
  wxAutomationObject()
  {
  }

Sheets& Workbook::GetWorksheets()
  {
  if(m_shs.GetDispatchPtr() == NULL)
    {
    bool bGotSheets = GetObject(m_shs, wxT("Worksheets"));
    if(!bGotSheets)
      throw Exception(wxT("Could not get Worksheets collection object reference."));
    }
  return m_shs;
  }

wxString Workbook::GetFullName()
  {
  wxVariant vFullName = GetProperty(wxT("FullName"));
  if(!vFullName.IsType(wxT("string")))
    throw Exception(wxString(wxT("Workbook::GetFullName() Incorrect type: ")) + vFullName.GetType());

  return vFullName.GetString();
  }

void Workbook::Close()
  {
  wxVariant avClose[1];
  avClose[0] = wxVariant(false, wxT("SaveChanges"));
  CallMethod(wxT("Close"), 1, avClose);
  }

void Workbook::SaveAs(const wxString& sFileName, XlFileFormat fft)
  {
  wxVariant avSaveAs[2];
  avSaveAs[0] = wxVariant(sFileName, wxT("Filename"));
  avSaveAs[1] = wxVariant((long) fft, wxT("FileFormat"));
  CallMethod(wxT("SaveAs"), 2, avSaveAs); 
  }

void Workbook::PrintPreview()
  {
  CallMethod(wxT("PrintPreview"));
  }

void Workbook::PrintOut(long nFrom, long nTo, long nCopies, bool bPreview)
  {
  long nParameterCount = 3;
  wxVariant av[4];
  av[0] = wxVariant(nFrom, wxT("From"));
  av[1] = wxVariant(nCopies, wxT("Copies"));
  av[2] = wxVariant(bPreview, wxT("Preview"));
  if(nTo != -1)
    {
    av[3] = wxVariant(nTo, wxT("To"));
    nParameterCount++;
    }
  CallMethod(wxT("PrintOut"), nParameterCount, av);
  }

Sheets::Sheets() :
  wxAutomationObject()
  {
  }

void Sheets::GetItem(long nIndex, Worksheet& wsh)
  {
  wxVariant avSheetsItem[1];
  avSheetsItem[0] = wxVariant(nIndex);
  bool bGotWorksheet = GetObject(wsh, wxT("Item"), 1, avSheetsItem);
  if(!bGotWorksheet)
    throw Exception(wxT("Could not get Worksheet object reference."));
  }

long Sheets::GetCount()
  {
  wxVariant vCount = GetProperty(wxT("Count"));
  if(!vCount.IsType(wxT("long")))
    throw Exception(wxString(wxT("Sheets::GetCount() Incorrect type: ")) + vCount.GetType());

  return vCount.GetLong();
  }

Worksheet::Worksheet() :
  wxAutomationObject()
  {
  }

void Worksheet::PrintPreview()
  {
  CallMethod(wxT("PrintPreview"));
  }

void Worksheet::PrintOut(long nFrom, long nTo, long nCopies, bool bPreview)
  {
  long nParameterCount = 3;
  wxVariant av[4];
  av[0] = wxVariant(nFrom, wxT("From"));
  av[1] = wxVariant(nFrom, wxT("Copies"));
  av[2] = wxVariant(bPreview, wxT("Preview"));
  if(nTo != -1)
    {
    av[3] = wxVariant(nTo, wxT("To"));
    nParameterCount++;
    }
  CallMethod(wxT("PrintOut"), nParameterCount, av);
  }

} // namespace Excel
  
} // namespace wx

#endif
	// def __WXMSW__
