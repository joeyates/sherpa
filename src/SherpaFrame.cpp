///////////////////////////////////////////////////////////////////////////////
// Name:        SherpaEvent.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: SherpaFrame.cpp,v 1.1.1.1 2006/01/24 22:13:11 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"

#include "SherpaFrame.h"
#include "OptionsDialog.h"
#include "ResourceManager.h"

#define MENU_FILE             		_T("&File")
#define MENU_FILE_EMPTY_WASTEBIN 	_T("Empty wastebin")
#define MENU_FILE_EXIT          	_T("E&xit\tAlt-X")
#define MENU_EDIT             		_T("&Edit")
#define MENU_EDIT_UNDO				   	_T("Undo")
#define MENU_EDIT_REDO				   	_T("Redo")
#define MENU_EDIT_PREFERENCES   	_T("Preferences...")
#define MENU_VIEW             		_T("&View")
#define MENU_VIEW_SHOWHIDDEN    	_T("Show Hidden")
#define MENU_FAVOURITES       		_T("Favourites")
#define MENU_DEBUG					   		_T("Debug")
#define MENU_DEBUG_WASTEBIN    		_T("Dump Wastebin")
#define MENU_DEBUG_UNDO    				_T("Dump Undo Stack")

#if _MSC_VER > 1000
# pragma warning (disable : 4355) // 'this' : used in base member initializer list
#endif // _MSC_VER > 1000

extern Persona						psn;
extern ResourceManager *	res;

/////////////////////////////////
// Constructors/destructors

SherpaFrame::SherpaFrame(const wxString& sTitle) :
  wxFrame((wxWindow *) NULL, -1, sTitle),
	m_sTitle(sTitle),
	m_bShowHidden(false),
	m_bUpdating(false)
  {
	// Set the icon
  wxBitmap bmpApp = res->GetImage(resApplication);
	if(bmpApp.Ok())
    {
    wxIcon icoApp;
		icoApp.CopyFromBitmap(bmpApp);
		SetIcon(icoApp);
    }
	else
		wxLogDebug(wxT("Failed to load Sherpa icon."));

  CreateMenus();
  CreateWidgets();

  wxString sSeparator = wxFileName::GetPathSeparator();

	wxString sHome = wxGetHomeDir();
  wxString sWastebinPath = wxString::Format
    (
    wxT("%s%s%s"),
    sHome.c_str(),
    sSeparator.c_str(),
    SETTINGS_DIRECTORY
    );
	m_bin = new Waste::Bin(sWastebinPath);

  LoadSettings();
  }

SherpaFrame::~SherpaFrame()
  {
  SaveSettings();
	delete m_bin;
  }

/////////////////////////////////
// Operators

/////////////////////////////////
// Slots

BEGIN_EVENT_TABLE(SherpaFrame,wxFrame)
  EVT_KEY_DOWN(                               SherpaFrame::OnKeyDown)
  EVT_MENU(shMENUID_FILE_EXIT,								SherpaFrame::OnExit)
  EVT_MENU(shMENUID_FILE_EMPTY_WASTEBIN,			SherpaFrame::mnuFileEmptyWastebin_Click)
  EVT_MENU(shMENUID_EDIT_UNDO,								SherpaFrame::mnuEditUndo_Click)
  EVT_MENU(shMENUID_EDIT_REDO,								SherpaFrame::mnuEditRedo_Click)
  EVT_MENU(shMENUID_EDIT_PREFERENCES,					SherpaFrame::mnuEditPreferences_Click)
  EVT_MENU(shMENUID_VIEW_SHOWHIDDEN,					SherpaFrame::mnuViewShowHidden_Click)
  EVT_MENU(shMENUID_DEBUG_WASTEBIN,						SherpaFrame::mnuDebugWastebin_Click)
  EVT_MENU(shMENUID_DEBUG_UNDO,								SherpaFrame::mnuDebugUndo_Click)

  EVT_TEXT(wxID_ANY,													SherpaFrame::txtPath_Change)
  EVT_TEXT_ENTER(wxID_ANY,										SherpaFrame::txtPath_Enter)

  EVT_SET_PATH(																SherpaFrame::OnSetPath)
  EVT_RENAME(																	SherpaFrame::OnRename)
  EVT_DELETE(																	SherpaFrame::OnDelete)
  EVT_TOWASTEBIN(															SherpaFrame::OnToWastebin)
  EVT_CREATE(																	SherpaFrame::OnCreate)
END_EVENT_TABLE()

void SherpaFrame::OnKeyDown(wxKeyEvent& evt)
  {
  int nKeyCode = evt.GetKeyCode();
	bool bShiftDown = evt.ShiftDown();
	bool bCtrlDown = evt.ControlDown();

	switch(nKeyCode)
		{
		case 'Y':
			if(bCtrlDown)
				{
				wxLogDebug(wxT("Ctrl+Y pressed"));
				Redo();
				}
			break;
		case 'Z':
			if(bCtrlDown)
				{
				wxLogDebug(wxT("SherpaFrame::OnKeyDown() Ctrl+Z pressed"));
				Undo();
				}
			break;
		}
  }

void SherpaFrame::OnExit(wxCommandEvent& evt)
  {
  Close(true);
  }

void SherpaFrame::mnuFileEmptyWastebin_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuFileEmptyWastebin_Click()"));
  m_bin->Empty();
  }

void SherpaFrame::mnuEditPreferences_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuEditPreferences_Click()"));
  OptionsDialog opt(this);
  opt.ShowModal();
  }

void SherpaFrame::mnuEditUndo_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuEditUndo_Click()"));
	Undo();
  }

void SherpaFrame::mnuEditRedo_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuEditRedo_Click()"));
	Redo();
  }

void SherpaFrame::mnuViewShowHidden_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuViewShowHidden_Click()"));
  SetShowHidden(evt.IsChecked(), true);
  }
  
void SherpaFrame::mnuDebugWastebin_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuDebugWastebin_Click()"));
	wxLogDebug(wxT("\nWastebin contents:\n"));
	m_bin->Dump();
  }
  
void SherpaFrame::mnuDebugUndo_Click(wxCommandEvent& evt)
  {
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::mnuDebugUndo_Click()"));
	wxLogDebug(wxT("\nUndo contents:\n"));
	wxString sDump = m_undo.Dump();
	wxLogDebug(sDump);
  }

void SherpaFrame::txtPath_Change(wxCommandEvent& evt)
	{
	if(m_bUpdating)
		return;
	}

void SherpaFrame::txtPath_Enter(wxCommandEvent& evt)
	{
	wxString sPath = m_txtPath->GetValue();
	if(!wxDirExists(sPath))
		return;
	
	SetPath(sPath);
	}

void SherpaFrame::OnSetPath(shSetPathEvent& evt)
	{
	wxString sPath = evt.GetPath();
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::OnSetPath('%s')"), sPath.c_str());

  // Check we can open the directory
	bool bCanExecute = psn.CanExecute(sPath);
	if(!bCanExecute)
		{
		// TBI: Proper message
		wxMessageBox(wxT("File Permissions"), wxT("File Permissions"), wxOK | wxICON_INFORMATION);
		return;
		}

  SetPath(sPath);
	}

void SherpaFrame::OnDelete(shDeleteEvent& evt)
	{
	wxString sPathName = evt.GetPathName();

	// TBI: Check permissions

	if(wxDirExists(sPathName))
		DeletePath(sPathName);
	else
		wxRemoveFile(sPathName);

	shDeleteEvent dleTree(sPathName);
	dleTree.StopPropagation();
  m_tre->GetEventHandler()->ProcessEvent(dleTree);

	shDeleteEvent dleView(sPathName);
	dleView.StopPropagation();
  m_drv->GetEventHandler()->ProcessEvent(dleView);
	}

void SherpaFrame::OnToWastebin(shToWastebinEvent& evt)
	{
	wxString sPathName = evt.GetPathName();
	int nWastebinId = SendToWastebin(sPathName);

	// Create Undo step
  Undo::FileToWastebin * fdl = new Undo::FileToWastebin(sPathName, nWastebinId);
 	m_undo.Add(fdl);
	SetUndoEnabled();

	// Tell the frames
	shToWastebinEvent tweTree(sPathName);
	tweTree.StopPropagation();
  m_tre->GetEventHandler()->ProcessEvent(tweTree);

	shToWastebinEvent tweView(sPathName);
	tweView.StopPropagation();
  m_drv->GetEventHandler()->ProcessEvent(tweView);
	}

void SherpaFrame::OnCreate(shCreateEvent& evt)
	{
	wxLogTrace(SHERPAFRAME_EVENTS, wxT("SherpaFrame::OnCreate()"));
	wxString sPath = evt.GetPath();
	wxString sFileType = evt.GetFileType();
	
	wxString sSeparator = wxFileName::GetPathSeparator();

	wxString sNew;
	if(sFileType == MIME_DIRECTORY)
		{
		sNew = GetUniquePathName(sPath + sSeparator + wxT("New Directory"));
		CreatePath(sNew);
		}
	else
		{
		// TODO: handle templating, typical extensions, etc
		sNew = GetUniquePathName(sPath + sSeparator + wxT("New File"));
		wxFile fil;
		fil.Create(sNew, true, wxS_DEFAULT); // FIXME: check permissions on UNIX
		}
		
	Undo::FileCreate * fct = new Undo::FileCreate(sNew);
	m_undo.Add(fct);
	SetUndoEnabled();

	shCreateEvent cteTree(sNew, wxT(""));
	cteTree.StopPropagation();
  m_tre->GetEventHandler()->ProcessEvent(cteTree);

	shCreateEvent cteView(sNew, wxT(""));
	cteView.StopPropagation();
  m_drv->GetEventHandler()->ProcessEvent(cteView);
	}

void SherpaFrame::OnRename(shRenameEvent& evt)
	{
  wxLogTrace(SHERPAFRAME_EVENTS, wxT("OnRename() event"));
	
	wxString sOldPathName = evt.GetOldPathName();
	wxString sNewPathName = evt.GetNewPathName();
	
	// TBI: Refuse if:
	//	1. Filename is illegal (OS-specific)
	//	2. Filename esists
	
	Undo::FileMove * flm = new Undo::FileMove(sOldPathName, sNewPathName);
	m_undo.Add(flm);
	SetUndoEnabled();

	wxRename(sOldPathName, sNewPathName);

	shRenameEvent rne(sOldPathName, sNewPathName);
  m_drv->GetEventHandler()->ProcessEvent(rne);
	}

//////////////////////////////////////////////////////////////////
// Public Methods

//////////////////////////////////////////////////////////////////
// Private Methods

//////////////////
// The window

void SherpaFrame::CreateMenus()
  {
  wxMenu * mnuFile = new wxMenu;
  mnuFile->Append(shMENUID_FILE_EMPTY_WASTEBIN, MENU_FILE_EMPTY_WASTEBIN);
  mnuFile->Append(shMENUID_FILE_EXIT, MENU_FILE_EXIT);

  wxMenu * mnuEdit = new wxMenu;
  mnuEdit->Append(shMENUID_EDIT_UNDO, MENU_EDIT_UNDO);
  mnuEdit->Append(shMENUID_EDIT_REDO, MENU_EDIT_REDO);
  mnuEdit->Append(shMENUID_EDIT_PREFERENCES, MENU_EDIT_PREFERENCES);

  wxMenu * mnuView = new wxMenu;
  mnuView->AppendCheckItem(shMENUID_VIEW_SHOWHIDDEN, MENU_VIEW_SHOWHIDDEN);

  wxMenu * mnuFavourites = new wxMenu;

  wxMenu * mnuDebug = new wxMenu;
  mnuDebug->Append(shMENUID_DEBUG_WASTEBIN, MENU_DEBUG_WASTEBIN);
  mnuDebug->Append(shMENUID_DEBUG_UNDO, MENU_DEBUG_UNDO);

  wxMenuBar * mnb = new wxMenuBar;
  mnb->Append(mnuFile, MENU_FILE);
  mnb->Append(mnuEdit, MENU_EDIT);
  mnb->Append(mnuView, MENU_VIEW);
  mnb->Append(mnuFavourites, MENU_FAVOURITES);
  mnb->Append(mnuDebug, MENU_DEBUG);
  SetMenuBar(mnb);
	
	SetUndoEnabled();
  }

void SherpaFrame::CreateWidgets()
	{
  wxStaticText * staPath = new wxStaticText
    (
    this,
    wxID_ANY,
    _T("Path:"),
    wxDefaultPosition,
    wxDefaultSize
    );
  m_txtPath = new wxTextCtrl
    (
    this,
    wxID_ANY,
    wxT(""),
    wxDefaultPosition,
    wxSize(-1, 22),
		wxTE_PROCESS_ENTER
    );

  wxBoxSizer * szrPathEdit = new wxBoxSizer(wxHORIZONTAL);
  szrPathEdit->Add
    (
    staPath,
    wxSizerFlags().Border(wxALL, 5)
    );
  szrPathEdit->Add
    (
    m_txtPath,
    1, // Allow expansion on axis of parent sizer (= horizontal)
    wxEXPAND | wxHORIZONTAL,
    5
    );

  m_spl = new wxSplitterWindow(this, -1);
  m_tre = new DirectoryTree(m_spl);
  m_drv = new DirectoryView(m_spl);

	m_spl->SetMinimumPaneSize(60);
	m_spl->SplitVertically(m_tre, m_drv, 300);

  wxBoxSizer * szrMain = new wxBoxSizer(wxVERTICAL);
  szrMain->Add
    (
    szrPathEdit,
    0,
    wxEXPAND | wxHORIZONTAL,
    5
    );
  szrMain->Add
    (
    m_spl,
    1,
    wxEXPAND | wxALL,
    5
    );

  SetAutoLayout(true);
  SetSizer(szrMain);
	}

/*
void SherpaFrame::DoSize()
  {
	wxSize sizClient = GetClientSize();
	m_spl->SetSize(sizClient);
  }
*/

void SherpaFrame::SetShowHidden(bool bShowHidden, bool bUpdate)
	{
  m_bShowHidden = bShowHidden;
  wxMenuBar * mnb = GetMenuBar();
  wxMenuItem * mnuShowHidden = mnb->FindItem(shMENUID_VIEW_SHOWHIDDEN, NULL);
  mnuShowHidden->Check(m_bShowHidden);
  m_tre->SetShowHidden(m_bShowHidden, bUpdate);
  m_drv->SetShowHidden(m_bShowHidden);
  }

void SherpaFrame::SetCaption(const wxString& sPath)
	{
	wxString sCaption = sPath;
	SetTitle(sCaption);
	m_bUpdating = true;
	m_txtPath->SetValue(sPath);
	m_bUpdating = false;
	}

//////////////////
// State

void SherpaFrame::SetPath(const wxString& sPath)
	{
	shSetPathEvent speTreeView(sPath);
  m_tre->GetEventHandler()->ProcessEvent(speTreeView);

	shSetPathEvent speDirectoryView(sPath);
  m_drv->GetEventHandler()->ProcessEvent(speDirectoryView);
	
	SetCaption(sPath);
	}

int SherpaFrame::SendToWastebin(const wxString& sPathName)
	{
	// TBI: Check permissions

	// Move to wastebin
  wxString sFileType;
	if(wxDirExists(sPathName))
		sFileType = MIME_DIRECTORY;
	else
		sFileType = res->GetFileType(sPathName);

	// Move to the Wastebin
 	Waste::File fil(sPathName, sFileType);
 	int nWastebinId = m_bin->Throw(fil);
	
	return nWastebinId;
	}

// Undo/Redo
void SherpaFrame::Undo()
	{
	bool bCanUndo = m_undo.CanUndo();
	if(!bCanUndo)
		return;
	
	Undo::FileChange * chg = m_undo.Undo();
	SetUndoEnabled();

	wxString sType = chg->GetType();
	wxString sPathName = chg->GetPathName();
	wxString sChange = chg->ToString();
	
	if(sType == shFILECHANGE_MOVE)
		{
		Undo::FileMove * fmv = dynamic_cast<Undo::FileMove *>(chg);

		wxString sOldPathName = fmv->GetPathName();
		wxString sNewPathName = fmv->GetNewPathName();
		if(!PathNameExists(sNewPathName))
			return;
		if(PathNameExists(sOldPathName))
			return;
		
		// Check destination path exists, create otherwise
		wxRename(sNewPathName, sOldPathName);
		}
	else if(sType == shFILECHANGE_TOWASTEBIN)
		{
		Undo::FileToWastebin * ftw = dynamic_cast<Undo::FileToWastebin *>(chg);

		int nWastebinId = ftw->GetWastebinId();
		wxString sPathName = wxT("");
		try
			{
			bool bRestored = m_bin->Restore(nWastebinId, sPathName);
			if(!bRestored)
				return;
			}
		catch(const wx::Exception& e)
			{
			wxString sError = e.GetString();
			wxLogDebug(wxT("SherpaFrame::Undo() Exception: '%s'"), sError.c_str());
			return;
			}

		ftw->SetPathName(sPathName);

		shRestoreEvent rseTreeView(sPathName);
		m_tre->GetEventHandler()->ProcessEvent(rseTreeView);

		shRestoreEvent rseDirectoryView(sPathName);
		m_drv->GetEventHandler()->ProcessEvent(rseDirectoryView);
		}
	else if(sType == shFILECHANGE_RESTORE || sType == shFILECHANGE_CREATE)
		{
		// Here we have to juggle:
		// Get the details of the FileCreate item
		// Create an equivalent ToWastebin item
		// Throw the Created file away
		// Replace the OnCreate item in the UndoStack (which takes care of deleting the OnCreate item)
		wxString sPathName = chg->GetPathName();

		int nWastebinId = SendToWastebin(sPathName);

	  Undo::FileToWastebin * fdl = new Undo::FileToWastebin(sPathName, nWastebinId);
		m_undo.Replace(chg, fdl);

		shToWastebinEvent tweTree(sPathName);
		tweTree.StopPropagation();
		m_tre->GetEventHandler()->ProcessEvent(tweTree);
	
		shToWastebinEvent tweView(sPathName);
		tweView.StopPropagation();
		m_drv->GetEventHandler()->ProcessEvent(tweView);
		}
	else
		{
    throw wx::Exception(wxT("SherpaFrame::Undo() Unknown change type: '%s'."), sType.c_str());
		}
	}

void SherpaFrame::Redo()
	{
	bool bCanRedo = m_undo.CanRedo();
	if(!bCanRedo)
		return;
	
	Undo::FileChange * chg = m_undo.Redo();
	SetUndoEnabled();

	wxString sType = chg->GetType();
	wxString sPathName = chg->GetPathName();
	wxString sChange = chg->ToString();
	
	if(sType == shFILECHANGE_MOVE)
		{
		Undo::FileMove * fmv = dynamic_cast<Undo::FileMove *>(chg);

		wxString sOldPathName = fmv->GetPathName();
		wxString sNewPathName = fmv->GetNewPathName();
		if(PathNameExists(sNewPathName))
			return;
		if(!PathNameExists(sOldPathName))
			return;
		
		// Check destination path exists, create otherwise
		wxRename(sOldPathName, sNewPathName);
		}
	else if(sType == shFILECHANGE_TOWASTEBIN)
		{
		Undo::FileToWastebin * ftw = dynamic_cast<Undo::FileToWastebin *>(chg);

		// N.B. The WastebinId changes
		int nWastebinId = SendToWastebin(sPathName);
		ftw->SetWastebinId(nWastebinId);

		// Tell the frames
		shToWastebinEvent tweTree(sPathName);
		tweTree.StopPropagation();
		m_tre->GetEventHandler()->ProcessEvent(tweTree);
	
		shToWastebinEvent tweView(sPathName);
		tweView.StopPropagation();
		m_drv->GetEventHandler()->ProcessEvent(tweView);
		}
	else if(sType == shFILECHANGE_RESTORE)
		{
    throw wx::Exception(wxT("SherpaFrame::Redo() Can't redo a file restore."));
		}
	else if(sType == shFILECHANGE_CREATE)
		{
    throw wx::Exception(wxT("SherpaFrame::Redo() Can't redo a file create."));
		}
	else
		{
    throw wx::Exception(wxT("SherpaFrame::Redo() Unknown change type: '%s'."), sType.c_str());
		}
	}

void SherpaFrame::SetUndoEnabled()
	{
	wxMenuBar * mnb = GetMenuBar();
	if(mnb == NULL)
		return;
	bool bCanUndo = m_undo.CanUndo();
	bool bCanRedo = m_undo.CanRedo();
	mnb->Enable(shMENUID_EDIT_UNDO, bCanUndo);
	mnb->Enable(shMENUID_EDIT_REDO, bCanRedo);
	}

// Settings
void SherpaFrame::LoadSettings()
  {
  wxConfig * cfg = (wxConfig *) wxConfig::Get();

  int nWidth = cfg->Read(SETTING_SHERPAFRAME_WIDTH, 500);
  int nHeight = cfg->Read(SETTING_SHERPAFRAME_HEIGHT, 400);
  SetSize(nWidth, nHeight);
  int nLeft = cfg->Read(SETTING_SHERPAFRAME_LEFT, 200); // TBI: Check value
  int nTop = cfg->Read(SETTING_SHERPAFRAME_TOP, 20); // TBI: Check value
  Move(nLeft, nTop);
  
  // Show hidden
  bool bShowHidden;
  cfg->Read(SETTING_SHERPAFRAME_SHOWHIDDEN, &bShowHidden, true);
  SetShowHidden(bShowHidden, false);

  // Show the initial path
  wxString sHome = psn.GetHome();
  wxString sInitialPath = cfg->Read(SETTING_INITIAL_PATH, sHome);
  if(sInitialPath.IsEmpty())
    sInitialPath = sHome;

  SetPath(sInitialPath);
  }

void SherpaFrame::SaveSettings()
  {
  wxConfig * cfg = (wxConfig *) wxConfig::Get();

  wxSize siz = GetSize();
  cfg->Write(SETTING_SHERPAFRAME_WIDTH, siz.GetWidth());
  cfg->Write(SETTING_SHERPAFRAME_HEIGHT, siz.GetHeight());
  wxPoint pt = GetPosition();
  cfg->Write(SETTING_SHERPAFRAME_LEFT, pt.x); // TBI: Check value
  cfg->Write(SETTING_SHERPAFRAME_TOP, pt.y); // TBI: Check value

  cfg->Write(SETTING_SHERPAFRAME_SHOWHIDDEN, m_bShowHidden);

	wxString sPath = m_tre->GetPath();
  cfg->Write(SETTING_INITIAL_PATH, sPath);
  }
