///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryTree.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryTree.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __DirectoryTree_h__
#define __DirectoryTree_h__

#include "DirectoryEntryArray.h"
#include "SherpaEvent.h"

class DirectoryTree : public wxTreeCtrl
  {
	// friends, classes, enums and types
  class DirectoryTreeItem : public wxTreeItemData
    {
    // Constructors/destructors
    public:
      DirectoryTreeItem(const wxString& sNewPath);
      ~DirectoryTreeItem()
        {}

		// Methods
    public:
			wxString GetPath() const
				{ return m_sPath; }
			bool IsPopulated() const
				{ return m_bPopulated; }
			void SetIsPopulated()
				{ m_bPopulated = true; }

		private:
			wxString	m_sPath;
			bool			m_bPopulated;
    };

	// Constructors/destructors
  public:
    DirectoryTree
			(
			wxWindow * wndParent,
			const wxWindowID id = wxID_ANY,
			const wxPoint& pntPosition = wxDefaultPosition,
			const wxSize& siz = wxDefaultSize,
			long nStyle = wxNO_BORDER | wxTR_HAS_BUTTONS
			);

	// Operators

	// Slots
  public:
    void					OnLeftMouseDown(wxMouseEvent& evt);
    void					OnItemExpanded(wxTreeEvent& evt);
		void					OnSetPath(shSetPathEvent& evt);
    void          OnToWastebin(shToWastebinEvent& evt);
		void					OnDelete(shDeleteEvent& evt);
    void          OnCreate(shCreateEvent& evt);

	// Methods
  public:
    wxString      GetPath();
    void          SetShowHidden(bool bNewShowHidden, bool bUpdate);

  private:
		// Tree modification
    void          InitTree();
    wxTreeItemId  PopulatePath(const wxString& sPath);
    void		      AddChildren(const wxTreeItemId& tid);
    wxTreeItemId  AddChild(const wxTreeItemId& tidParent, const wxString& sPath);
		void					RemoveItem(const wxString& sPath);

		// Selection
    void          SetPath(const wxString& sNewPath);
    void          DoSelection(wxTreeItemId& tid);

		// Accessors
    wxTreeItemId  GetItemFromPath(const wxString& sPath, bool bReturnBestFit = false);
    wxTreeItemId  GetChildByName(wxTreeItemId& tidParent, const wxString& sName);
    bool          GetItemPath(wxTreeItemId tid, wxString& sPath);

    bool          IsAncestorOf(wxTreeItemId tidPutativeAncestor, wxTreeItemId tid);
    wxTreeItemId  GetRootItem(const wxString& sPath);

	// Members
  private:
		wxImageList 	m_imlButtons;
		wxImageList		m_iml;
    bool        	m_bShowHidden;
	
		// Sending events
		// Keep a copy of the frame, so sending events is more legible
		wxWindow * 		m_wndParent;

	DECLARE_EVENT_TABLE()
  };

#endif // ndef __DirectoryTree_h__
