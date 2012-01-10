///////////////////////////////////////////////////////////////////////////////
// Name:        OptionsDialog.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: OptionsDialog.cpp,v 1.1.1.1 2006/01/24 22:13:09 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "OptionsDialog.h"

# if _MSC_VER > 1000
# pragma warning (disable : 4355) // 'this' : used in base member initializer list
#endif // _MSC_VER > 1000

/////////////////////////////////
// Constructors/destructors

OptionsDialog::OptionsDialog(wxWindow * wndParent) :
  wxDialog(wndParent, wxID_ANY, wxString(_T("Sherpa Preferences"))),
  nbk(this, -1)
  {
	AddPanels();
  }

/////////////////////////////////
// Operators

/////////////////////////////////
// Slots

BEGIN_EVENT_TABLE(OptionsDialog, wxDialog)
  EVT_CHECKBOX(ID_OPTIONS_CHKSHOWSHERPAFRAMEEVENTS, OptionsDialog::OnTraceMaskCheck)
  EVT_CHECKBOX(ID_OPTIONS_CHKSHOWDIRECTORYTREEVENTS, OptionsDialog::OnTraceMaskCheck)
  EVT_CHECKBOX(ID_OPTIONS_CHKSHOWDIRECTORYVIEWEVENTS, OptionsDialog::OnTraceMaskCheck)
END_EVENT_TABLE()

void OptionsDialog::OnTraceMaskCheck(wxCommandEvent& evt)
  {
  //wxLogDebug(_T("OptionsDialog::OnTraceMaskCheck()"));
  bool bShow = evt.IsChecked();
  wxString sMaskName;
  switch(evt.GetId())
    {
    case ID_OPTIONS_CHKSHOWSHERPAFRAMEEVENTS:
      sMaskName = SHERPAFRAME_EVENTS;
      break;

    case ID_OPTIONS_CHKSHOWDIRECTORYTREEVENTS:
      sMaskName = DIRECTORYTREE_EVENTS;
      break;

    case ID_OPTIONS_CHKSHOWDIRECTORYVIEWEVENTS:
      sMaskName = DIRECTORYVIEW_EVENTS;
      break;

    default:
      return;
    }
  //wxLogDebug(_T("\tMask name: '%s'"), sMaskName.c_str());
  //wxLogDebug(_T("\tNew value: %u"), bShow);
  if(bShow)
    wxLog::AddTraceMask(sMaskName);
  else
    wxLog::RemoveTraceMask(sMaskName);
  }

/////////////////////////////////
// Public Methods

/////////////////////////////////
// Private Methods

void OptionsDialog::AddPanels()
	{
  wxPanel * panSkin = new wxPanel(&nbk);
  nbk.AddPage(panSkin, _T("Skin"), true);
	
	wxArrayString asThemes;
	asThemes.Add(_T("Default"));
	int nStyle = wxCB_DROPDOWN | wxCB_READONLY;
  wxComboBox * cmbTheme = new wxComboBox
    (
    panSkin,
    ID_OPTIONS_CMBTHEME,
    wxT(""),
    wxPoint(5, 5),
    wxDefaultSize,
    asThemes,
    nStyle
    );

  wxPanel * panDebug = new wxPanel(&nbk);
  nbk.AddPage(panDebug, _T("Debug"), true);

  wxCheckBox * chkShowSherpaFrameEvents = new wxCheckBox(panDebug, ID_OPTIONS_CHKSHOWSHERPAFRAMEEVENTS, _T("Show 'SherpaFrame' events"), wxPoint(5, 5));
  wxCheckBox * chkShowDirectoryTreeEvents = new wxCheckBox(panDebug, ID_OPTIONS_CHKSHOWDIRECTORYTREEVENTS, _T("Show 'DirectoryTree' events"), wxPoint(5, 35));
  wxCheckBox * chkShowDirectoryViewEvents = new wxCheckBox(panDebug, ID_OPTIONS_CHKSHOWDIRECTORYVIEWEVENTS, _T("Show 'DirectoryView' events"), wxPoint(5, 65));

  bool bShowSherpaFrameEvents = wxLog::IsAllowedTraceMask(SHERPAFRAME_EVENTS);
  bool bShowDirectoryTreeEvents = wxLog::IsAllowedTraceMask(DIRECTORYTREE_EVENTS);
  bool bShowDirectoryViewEvents = wxLog::IsAllowedTraceMask(DIRECTORYVIEW_EVENTS);

  chkShowSherpaFrameEvents->SetValue(bShowSherpaFrameEvents);
  chkShowDirectoryTreeEvents->SetValue(bShowDirectoryTreeEvents);
  chkShowDirectoryViewEvents->SetValue(bShowDirectoryViewEvents);
	}
