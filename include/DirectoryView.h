///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryView.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryView.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __DirectoryView_h__
#define __DirectoryView_h__

#include "DirectoryEntryArray.h"
#include "SherpaEvent.h"

#define SETTING_DIRECTORYVIEW_SORTKEY _T("/DirectoryView/SortKey")
#define SETTING_DIRECTORYVIEW_TEXTSORTFLAGS _T("/DirectoryView/TextSortFlags")
#define SETTING_DIRECTORYVIEW_COLUMNWIDTH_NAME _T("/DirectoryView/Columns/NameWidth")
#define SETTING_DIRECTORYVIEW_COLUMNWIDTH_TYPE _T("/DirectoryView/Columns/TypeWidth")
#define SETTING_DIRECTORYVIEW_COLUMNWIDTH_PERMS _T("/DirectoryView/Columns/PermsWidth")

/*
We have to sub-class wxListCtrl, in order to use it as virtual
*/

class DirectoryView : public wxListCtrl, public DirectoryEntryArray
  {
	// friends, classes, enums and types
  typedef std::vector<int> ItemArray;

	// Constructors/destructors
  public:
    DirectoryView(wxWindow * wndParent);
    ~DirectoryView();

	// Operators

  // Slots
  protected:
		// Virtual list Implementation
		wxString			OnGetItemText(long item, long column) const;
		int						OnGetItemImage(long item) const;
		wxListItemAttr * OnGetItemAttr(long item) const;

		// Events
		void					OnIdle(wxIdleEvent& evt);

		// Keyboard events
    void          OnListKeyDown(wxListEvent& evt);

		// Mouse events
    void 					OnMouseMove(wxMouseEvent& evt);
    void 					OnLeftUp(wxMouseEvent& evt);
    void 					OnRightDown(wxMouseEvent& evt);
    void 					OnRightUp(wxMouseEvent& evt);
    void 					OnListItemRightClick(wxListEvent& evt);

    void					OnItemActivated(wxListEvent& evt);
    void					OnEndLabelEdit(wxListEvent& evt);
	
		// Drag'n'drop
    void					OnBeginDrag(wxListEvent& evt);

		// Popup menu events
    void					popToWastebin_Click(wxCommandEvent& evt);
    void					popNewDirectory_Click(wxCommandEvent& evt);
    void					popNewFile_Click(wxCommandEvent& evt);

		// Update events from frame
		void					OnSetPath(shSetPathEvent& evt);
    void					OnRename(shRenameEvent& evt);
    void					OnToWastebin(shToWastebinEvent& evt);
		void					OnRestore(shRestoreEvent& evt);
		void					OnDelete(shDeleteEvent& evt);
    void					OnCreate(shCreateEvent& evt);

	// Methods
  public:
    wxString			GetPath();
    void      		SetSortKey(DirectoryEntrySortKey des);
    void      		SetTextSortFlags(int nTextSortFlags);
    void      		SetShowHidden(bool bShowHidden);

  private:
    // Startup
    void      		CreateMenus();
    void      		AddColumns();
	
		// Settings
    void      		LoadSettings();
    void      		SaveSettings();
	
		// Threads
		/** 
			*  Tell wxWidgets event system if we want OnIdle() events
			*/
		void					SetWantIdleEvents(bool bWantIdleEvents);

		// List display
    void      		SetPath(const wxString& sNewPath);
    void          Refresh(bool bKeepSelection);
		void					ClearList();
    int	      		ItemIndexFromPoint(const wxPoint& pnt);

		// Selection
    ItemArray			GetSelectedItems();
    void      		ClearSelection();
    void      		SetSelected(int nItemIndex);
    void      		SelectItem(int nItemIndex);
    bool      		IsItemSelected(int nItemIndex);
  
    // Item Manipulation
    void      		ActivateItem(int nItemIndex);
    void          DeleteSelected(bool bDeleteFully);
    void      		RequestItemDeletion(int nItemIndex, bool bDeleteFully);

	// Variables
  private:
		// List display
		int								m_nNextUpdateIndex; // Used by OnIdle directory filling
    bool              m_bWantIdleEvents; // When we do a refresh, we say we want idle events
		// when we add/remove/etc a file, we say we don't

		// Menus
    wxMenu				    m_popItem;
    wxMenu				    m_popFreeSpace;
	
		// Directory and contents
    wxString          m_sPath;
    DirectoryEntrySortKey m_des;

		// Settings
    bool              m_bDirectoriesFirst;
	
		// Drag'n'drop
		bool							m_bDragging;
	
		// Sending events
		// Keep a copy of the parent, so sending events is more legible
		wxWindow * 				m_wndParent;

  DECLARE_NO_COPY_CLASS(DirectoryView)
	DECLARE_EVENT_TABLE()
  };

#endif // ndef __DirectoryView_h__
