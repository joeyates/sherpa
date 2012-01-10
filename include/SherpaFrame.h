///////////////////////////////////////////////////////////////////////////////
// Name:        SherpaFrame.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: SherpaFrame.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __SherpaFrame_h__
#define __SherpaFrame_h__

#include "DirectoryTree.h"
#include "DirectoryView.h"
#include "Wastebin.h"
#include "UndoStack.h"
#include "SherpaEvent.h"

#define SETTING_INITIAL_PATH						wxT("/SherpaFrame/InitialPath")
#define SETTING_SHERPAFRAME_WIDTH				wxT("/SherpaFrame/Width")
#define SETTING_SHERPAFRAME_HEIGHT			wxT("/SherpaFrame/Height")
#define SETTING_SHERPAFRAME_LEFT				wxT("/SherpaFrame/Left")
#define SETTING_SHERPAFRAME_TOP					wxT("/SherpaFrame/Top")
#define SETTING_SHERPAFRAME_SHOWHIDDEN	wxT("/SherpaFrame/ShowHidden")

class SherpaFrame: public wxFrame
  {
	// friends, classes, enums and types

	// Constructors/destructors
  public:
    SherpaFrame(const wxString& sTitle);
    ~SherpaFrame();

	// Operators

	// Slots
  protected:
    void OnKeyDown(wxKeyEvent& evt);
    void OnExit(wxCommandEvent& evt);

    void mnuFileEmptyWastebin_Click(wxCommandEvent& evt);
    void mnuEditUndo_Click(wxCommandEvent& evt);
    void mnuEditRedo_Click(wxCommandEvent& evt);
    void mnuEditPreferences_Click(wxCommandEvent& evt);
    void mnuViewShowHidden_Click(wxCommandEvent& evt);
    void mnuDebugWastebin_Click(wxCommandEvent& evt);
    void mnuDebugUndo_Click(wxCommandEvent& evt);

		void txtPath_Change(wxCommandEvent& evt);
		void txtPath_Enter(wxCommandEvent& evt);

    void OnSetPath(shSetPathEvent& evt);
    void OnDelete(shDeleteEvent& evt);
    void OnToWastebin(shToWastebinEvent& evt);
    void OnCreate(shCreateEvent& evt);
    void OnRename(shRenameEvent& evt);
	
	// Methods
  private:
		// The window
    void			CreateMenus();
    void			CreateWidgets();
    //void			DoSize();
    void			SetShowHidden(bool bShowHidden, bool bUpdate);
		void			SetCaption(const wxString& sPath);
	
		// State
		void			SetPath(const wxString& sPath);
	
		// Undo
		int				SendToWastebin(const wxString& sPathName);
		void			Undo();
		void			Redo();
		void			SetUndoEnabled();

		// Settings
    void			LoadSettings();
    void			SaveSettings();

	// Variables
  protected:
  private:
		wxString						m_sTitle;
		wxTextCtrl * 				m_txtPath;
		wxSplitterWindow *  m_spl;
		DirectoryTree *     m_tre;
		DirectoryView *     m_drv;
		Waste::Bin *				m_bin;
		Undo::UndoStack			m_undo;
  
    bool                m_bShowHidden;
    bool                m_bUpdating;
    
  DECLARE_NO_COPY_CLASS(SherpaFrame)
  DECLARE_EVENT_TABLE()
  };

#endif // ndef __SherpaFrame_h__
