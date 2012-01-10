///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryView.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryView.cpp,v 1.1.1.1 2006/01/24 22:13:12 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "DirectoryView.h"
#include "ResourceManager.h"

extern ResourceManager * res;

/////////////////////////////////
// Constructors/destructors

DirectoryView::DirectoryView(wxWindow * wndParent) :
	wxListCtrl
    (
    wndParent,
    wxID_ANY,
    wxDefaultPosition,
    wxDefaultSize,
    wxLC_REPORT | wxLC_VIRTUAL | wxSUNKEN_BORDER | wxLC_EDIT_LABELS
    ),
	m_wndParent(wndParent),
	m_bWantIdleEvents(false),
	m_nNextUpdateIndex(0),
	m_bDirectoriesFirst(true),
	m_bDragging(false)
  {
  wxASSERT_MSG(wndParent != 0, wxT("DirectoryView::DirectoryView() 'wndParent' == 0."));

  SetImageList(res, wxIMAGE_LIST_SMALL);
  
  CreateMenus();
  AddColumns();
  LoadSettings();
  }

DirectoryView::~DirectoryView()
  {
  SaveSettings();
  }

/////////////////////////////////
// Operators

/////////////////////////////////
// Slots

BEGIN_EVENT_TABLE(DirectoryView, wxListCtrl)
	// System events
  EVT_IDLE(														DirectoryView::OnIdle)

  // Keyboard events
  EVT_LIST_KEY_DOWN(wxID_ANY,         DirectoryView::OnListKeyDown)

	// Mouse events
  EVT_MOTION(													DirectoryView::OnMouseMove)
  EVT_LEFT_UP(												DirectoryView::OnLeftUp)
  EVT_RIGHT_DOWN(											DirectoryView::OnRightDown)
  EVT_RIGHT_UP(												DirectoryView::OnRightUp)
  EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, DirectoryView::OnListItemRightClick)

	// List events
	EVT_LIST_ITEM_ACTIVATED(-1, 				DirectoryView::OnItemActivated)
	EVT_LIST_END_LABEL_EDIT(-1,					DirectoryView::OnEndLabelEdit)
	
	// Drag'n'drop
	EVT_LIST_BEGIN_DRAG(-1,							DirectoryView::OnBeginDrag)

	// Events from frame
  EVT_SET_PATH(												DirectoryView::OnSetPath)
  EVT_RENAME(													DirectoryView::OnRename)
  EVT_TOWASTEBIN(											DirectoryView::OnToWastebin)
  EVT_RESTORE(												DirectoryView::OnRestore)
  EVT_DELETE(													DirectoryView::OnDelete)
  EVT_CREATE(													DirectoryView::OnCreate)

	// Menu events
  EVT_MENU(shMENUID_TOWASTEBIN,					DirectoryView::popToWastebin_Click)
  EVT_MENU(shMENUID_NEW_DIRECTORY,			DirectoryView::popNewDirectory_Click)
  EVT_MENU(shMENUID_NEW_FILE,						DirectoryView::popNewFile_Click)
END_EVENT_TABLE()

wxString DirectoryView::OnGetItemText(long item, long column) const
	{
	int nEntryCount = DirectoryEntryArray::GetCount();
	if(item < 0 || item >= nEntryCount)
		{
		wxLogDebug(wxT("DirectoryView::OnGetItemText(%i, %i): item index %i is invalid, acceptable range is 0 to %i"), item, column, item, nEntryCount - 1);
		return wxT("");
		}

  const DirectoryEntry * dre = DirectoryEntryArray::GetItem(item);
	if(dre == NULL)
		return wxT("");

	switch(column)
		{
		case 0:
			return dre->GetName();

		case 1:
			return dre->GetDescription();

		case 2:
			{
			int nPermissions = dre->GetPermissions();
      wxString sPerms = wxString::Format(wxT("0%o"), nPermissions);
			return sPerms;
			}

		default:
			return wxT("");
		}
	}

int DirectoryView::OnGetItemImage(long item) const
	{
	int nEntryCount = DirectoryEntryArray::GetCount();
	if(item < 0 || item >= nEntryCount)
		{
		wxLogDebug(wxT("DirectoryView::OnGetItemImage(%i): invalid item index, acceptable range is 0 to %i"), item, nEntryCount - 1);
		return -1;
		}

  const DirectoryEntry * dre = DirectoryEntryArray::GetItem(item);
	if(dre == NULL)
		{
		wxLogDebug(wxT("DirectoryView::OnGetItemImage(): DirectoryEntryArray::GetItem() returned NULL"), item);
		return -1;
		}
  int nImageIndex = dre->GetImageIndex();

	return nImageIndex;
	}

/*
Override this fn to avoid ASSERTs in the base class version which
occur when we are changing the number of items in another thread
*/
wxListItemAttr * DirectoryView::OnGetItemAttr(long item) const
	{
	return NULL;
	}

/*

The list is virtual
The initial values supplied for description and image are default
Each time OnIdle() is called, it updates a single item,
getting the true file type description and the correct imaeg

*/

void DirectoryView::OnIdle(wxIdleEvent& evt)
	{
	int nEntryCount = DirectoryEntryArray::GetCount();
	bool bUpdated = false;
	while(m_nNextUpdateIndex < nEntryCount && !bUpdated)
		{
		DirectoryEntry * dre = const_cast<DirectoryEntry *>(DirectoryEntryArray::GetItem(m_nNextUpdateIndex));
    if(dre != NULL)
      {
		  bUpdated = dre->CacheValues();
		  if(bUpdated)
			  RefreshItem(m_nNextUpdateIndex);
      }
	
		m_nNextUpdateIndex++;
		}
	
	// We get an OnIdle event after each GUI modification
	// If we call RefreshItem() on a visible item, a change happens, and we get another OnIdle()
	// But if the item is not visible, we need to indicate we still have work to do
	if(m_nNextUpdateIndex < nEntryCount)
		evt.RequestMore(true);
	}

void DirectoryView::OnListKeyDown(wxListEvent& evt)
  {
  int nKeyCode = evt.GetKeyCode();
	wxLogDebug(wxT("DirectoryView::OnListKeyDown(): nKeyCode: %i"), nKeyCode);
  switch(nKeyCode)
    {
    case WXK_DELETE:
			{
			bool bDeleteFully = wxGetKeyState(WXK_SHIFT);
      DeleteSelected(bDeleteFully);
      break;
			}

    case WXK_F2:
      {
			ItemArray tia = GetSelectedItems();
			int nCount = tia.size();
			if(nCount == 1)
				{
				int nFirstIndex = tia.at(0);
				EditLabel(nFirstIndex);
				}
      break;
			}

    case WXK_F5:
      Refresh(true);
      break;

    default:
			evt.Skip();
      break;
    }
  }

void DirectoryView::OnMouseMove(wxMouseEvent& evt)
  {
	if(!m_bDragging)
		{
		if(evt.m_leftDown)
			{
			ItemArray tia = GetSelectedItems();
	
			int nCount = tia.size();
			if(nCount > 0)
				{
				wxFileDataObject fdo;

				for(ItemArray::const_iterator it = tia.begin(); it != tia.end(); it++)
					{
					int nItemIndex = *it;
					wxLogDebug(wxT("nItemIndex: %i"), nItemIndex);
					const DirectoryEntry * dre = DirectoryEntryArray::GetItem(*it);
					if(dre != NULL)
						{
						wxString sSelectedPathName = dre->GetPathName();
						wxLogDebug(wxT("sSelectedPathName: '%s'"), sSelectedPathName.c_str());
						fdo.AddFile(sSelectedPathName);
						}
					}

				m_bDragging = true;
				}
			}
		}

  evt.Skip();
	}

void DirectoryView::OnLeftUp(wxMouseEvent& evt)
  {
	m_bDragging = false;

	int nItemIndex = ItemIndexFromPoint(evt.GetPosition());

	if(evt.ShiftDown())
		{
		if(nItemIndex == wxNOT_FOUND)
			{
			// If there are selected items,
			// the selection is made to run from the first selected item down to the end
			int nEntryCount = DirectoryEntryArray::GetCount();
			nItemIndex = nEntryCount - 1;
			ItemArray tia = GetSelectedItems();
			ClearSelection();
	
			int nCount = tia.size();
			if(nCount > 0)
				{
				int nFirstIndex = tia.at(0);
				for(int i = nFirstIndex; i <= nItemIndex; i++)
					SelectItem(i);
				}
			}
		// ...else we want to select to a particular item - the list ctrl handles this type of selection
		}
	else
		{
		// Clear the selection if click is on white space
		if(nItemIndex == wxNOT_FOUND)
			ClearSelection();
		else
			{
			/* Bugfix: while we clear the selection, we remove focus
			// unfortunately, we no longer get ItemSelected events
			ItemArray tia = GetSelectedItems();
			int nCount = tia.size();
			if(nCount == 0)
				{
				wxLogDebug(wxT("\tselecting item"));
				SelectItem(nItemIndex);
				}
			*/
			}
		}

  evt.Skip();
	}

// This slot has been implemented as without it we don't get
// the right up event (except for right double click)
void DirectoryView::OnRightDown(wxMouseEvent& evt)
  {
  SetFocus();

  //evt.Skip();
  }

/*

We distinguish 2 places where a user can click:
1. On a directory or file
2. Elsewhere

*/

void DirectoryView::OnRightUp(wxMouseEvent& evt)
  {
	int nItemIndex = ItemIndexFromPoint(evt.GetPosition());

  if(nItemIndex == wxNOT_FOUND)
    PopupMenu(&m_popFreeSpace);
  else
    {
    // The user has clicked on a file or directory
	  wxString sText = GetItemText(nItemIndex);
	  wxLogTrace(DIRECTORYVIEW_EVENTS, wxT("OnListItemRightClick(): item '%s'"), sText.c_str());
    // If this item is already selected, do not change selection.
    // So we can preserve multiple selection
    bool bSelected = IsItemSelected(nItemIndex);
    wxLogTrace(DIRECTORYVIEW_EVENTS, wxT("OnListItemRightClick: IsItemSelected() returned %u"), bSelected);
    if(!bSelected)
      SetSelected(nItemIndex);

    PopupMenu(&m_popItem);
    }
  }

void DirectoryView::OnListItemRightClick(wxListEvent& evt)
  {
  //wxLogDebug(wxT("OnListItemRightClick()"));
  }

/*

The user has double-clicked an item, or pressed return while one (or more) items are selected.

If there is more than one folder selected, open the first.
If one folder is selected - tell the frame to open it.
If one or more files are selected - open it/them.

*/

void DirectoryView::OnItemActivated(wxListEvent& evt)
  {
	int bDirectorySelected = false;

  ItemArray tia = GetSelectedItems();
  int nCount = tia.size();
  for(int i = 0; i < nCount; i++)
    {
    int nItemIndex = tia.at(i);
		const DirectoryEntry * dre = DirectoryEntryArray::GetItem(nItemIndex);
	  DirectoryEntryType det = dre->GetType();
		if(det == detDirectory)
			{
			// Only activate the first
			if(!bDirectorySelected)
    		ActivateItem(nItemIndex);

			bDirectorySelected = true;
			}
		else
  		ActivateItem(nItemIndex);
    }
  }

void DirectoryView::OnEndLabelEdit(wxListEvent& evt)
  {
	int nItemIndex = evt.GetIndex();
	wxString sNewName = evt.GetText();
	if(sNewName.IsEmpty())
		return;

  const DirectoryEntry * dre = DirectoryEntryArray::GetItem(nItemIndex);
  if(dre == NULL)
    {
    wxLogDebug(wxT("DirectoryEntryArray::OnEndLabelEdit(), DirectoryEntryArray::GetItem(%u) returned NULL"), nItemIndex);
    return;
    }
	wxString sPathName = dre->GetPathName();
	wxString sNewPathName = m_sPath + _T("/") + sNewName; // TBI: OS

	wxLogDebug(wxT("DirectoryView::OnEndLabelEdit(): Firing wxRenameEvent"));
	// Tell frame, so it can tell the tree (and record for undo)
	shRenameEvent rne(sPathName, sNewPathName);
  m_wndParent->GetEventHandler()->ProcessEvent(rne);
	}

void DirectoryView::OnBeginDrag(wxListEvent& evt)
  {
	}

void DirectoryView::OnSetPath(shSetPathEvent& evt)
	{
	wxString sPath = evt.GetPath();
	SetPath(sPath);
	}

void DirectoryView::popNewDirectory_Click(wxCommandEvent& evt)
  {
  wxLogTrace(DIRECTORYVIEW_EVENTS, wxT("popAddDirectory_Click"));

	SetWantIdleEvents(false);

	shCreateEvent nde(m_sPath, MIME_DIRECTORY);
  m_wndParent->GetEventHandler()->ProcessEvent(nde);
  }

void DirectoryView::popNewFile_Click(wxCommandEvent& evt)
  {
  wxLogTrace(DIRECTORYVIEW_EVENTS, wxT("popAddFile_Click"));

	SetWantIdleEvents(false);

	shCreateEvent nde(m_sPath, wxT("")); // TBI: put in file type
  m_wndParent->GetEventHandler()->ProcessEvent(nde);
	}

void DirectoryView::popToWastebin_Click(wxCommandEvent& evt)
  {
  wxLogTrace(DIRECTORYVIEW_EVENTS, wxT("popToWastebin_Click()"));

  DeleteSelected(false); // Sets want idle
  }

void DirectoryView::OnRename(shRenameEvent& evt)
	{
	wxString sOldPathName = evt.GetOldPathName();
	wxString sNewPathName = evt.GetNewPathName();
	
	// Ensure selection stays on the new item, if it is selected
	bool bSelectItem = false;
	int nOldIndex = GetIndexFromPathName(sOldPathName);
	if(nOldIndex != -1)
		bSelectItem = IsItemSelected(nOldIndex);

	Refresh(true);

	if(bSelectItem)
		{
		int nNewIndex = GetIndexFromPathName(sNewPathName);
		if(nNewIndex != -1)
			SelectItem(nNewIndex);
		}
	}

void DirectoryView::OnToWastebin(shToWastebinEvent& evt)
	{
  Refresh(true);
	}

void DirectoryView::OnRestore(shRestoreEvent& evt)
	{
  Refresh(true);
	}

void DirectoryView::OnDelete(shDeleteEvent& evt)
	{
  Refresh(true);
	}

void DirectoryView::OnCreate(shCreateEvent& evt)
	{
  Refresh(true);
	
	wxString sNewPath = evt.GetPath();
	int nNewIndex = GetIndexFromPathName(sNewPath);
	if(nNewIndex == -1)
		return;

 	SetSelected(nNewIndex);
  EnsureVisible(nNewIndex);
  EditLabel(nNewIndex);
	}

/////////////////////////////////
// Public Methods

wxString DirectoryView::GetPath()
	{
	return m_sPath;
	}

void DirectoryView::SetSortKey(DirectoryEntrySortKey desNew)
  {
  des = desNew;
  Refresh(true);
  }

void DirectoryView::SetTextSortFlags(int nNewTextSortFlags)
  {
  nTextSortFlags = nNewTextSortFlags;
  Refresh(true);
  }

void DirectoryView::SetShowHidden(bool bShowHidden)
  {
  bShowHiddenFiles = bShowHidden;
  Refresh(true);
  }

//////////////////////////////////////////////////////////////////
// Private Methods

///////////////////////
// Startup

void DirectoryView::CreateMenus()
  {
  m_popItem.Append(shMENUID_TOWASTEBIN, _T("To wastebin"));

	wxMenu * mnuFreeSpaceNewItems = new wxMenu();
	mnuFreeSpaceNewItems->Append(shMENUID_NEW_DIRECTORY, _T("Directory"));
	mnuFreeSpaceNewItems->Append(shMENUID_NEW_FILE, _T("File"));
	m_popFreeSpace.Append(-1, _T("New"), mnuFreeSpaceNewItems);
  }

void DirectoryView::AddColumns()
  {
  wxListItem lsiName;
  lsiName.SetText(_T("Name"));
  lsiName.SetImage(-1);
  InsertColumn(0, lsiName);

  wxListItem lsiType;
  lsiType.SetText(_T("Type"));
  lsiType.SetAlign(wxLIST_FORMAT_LEFT);
  InsertColumn(1, lsiType);

  wxListItem lsiPerms;
  lsiPerms.SetText(_T("Permissions"));
  lsiPerms.SetAlign(wxLIST_FORMAT_RIGHT);
  InsertColumn(2, lsiPerms);
  }

///////////////////////
// Settings

void DirectoryView::LoadSettings()
  {
  wxConfig * cfg = (wxConfig *) wxConfig::Get();
  
  // Sorting
  des = (DirectoryEntrySortKey) cfg->Read(SETTING_DIRECTORYVIEW_SORTKEY, desFILENAME);
  int nTextSortFlags = cfg->Read(SETTING_DIRECTORYVIEW_TEXTSORTFLAGS, 0L);
  SetTextSortFlags(nTextSortFlags);
  
  // List view columns
  int nNameWidth = cfg->Read(SETTING_DIRECTORYVIEW_COLUMNWIDTH_NAME, 200L);
  int nTypeWidth = cfg->Read(SETTING_DIRECTORYVIEW_COLUMNWIDTH_TYPE, 100L);
  int nPermsWidth = cfg->Read(SETTING_DIRECTORYVIEW_COLUMNWIDTH_PERMS, 100L);
  SetColumnWidth(0, nNameWidth);
  SetColumnWidth(1, nTypeWidth);
  SetColumnWidth(2, nPermsWidth);
  }

void DirectoryView::SaveSettings()
  {
  wxConfig * cfg = (wxConfig *) wxConfig::Get();
  
  // Sorting
  cfg->Write(SETTING_DIRECTORYVIEW_SORTKEY, des);
  cfg->Write(SETTING_DIRECTORYVIEW_TEXTSORTFLAGS, nTextSortFlags);
  
  // List view columns
  cfg->Write(SETTING_DIRECTORYVIEW_COLUMNWIDTH_NAME, GetColumnWidth(0));
  cfg->Write(SETTING_DIRECTORYVIEW_COLUMNWIDTH_TYPE, GetColumnWidth(1));
  cfg->Write(SETTING_DIRECTORYVIEW_COLUMNWIDTH_PERMS, GetColumnWidth(2));
  }

///////////////////////
// Threads

void DirectoryView::SetWantIdleEvents(bool bWantIdleEvents)
  {
  if(m_bWantIdleEvents == bWantIdleEvents)
		return;
	
	m_bWantIdleEvents = bWantIdleEvents;
	
	int nExtraStyle = GetExtraStyle();
	if(m_bWantIdleEvents)
		nExtraStyle |= wxWS_EX_PROCESS_IDLE;
	else
		nExtraStyle &= ~wxWS_EX_PROCESS_IDLE;

	SetExtraStyle(nExtraStyle);
	}

///////////////////////
// List display

void DirectoryView::SetPath(const wxString& sNewPath)
  {
  wxLogTrace(DIRECTORYVIEW_EVENTS, wxT("SetPath('%s')"), sNewPath.c_str());
	
  m_sPath = sNewPath;
  Refresh(false);
  }

// Clear the list and then reselect previously selected items
void DirectoryView::Refresh(bool bKeepSelection)
  {
	wxArrayString asSelected;
	int nPreSelectedCount = 0;
	if(bKeepSelection)
		{
		ItemArray tia = GetSelectedItems();
		nPreSelectedCount = tia.size();
		for(int i = 0; i < nPreSelectedCount; i++)
			{
			const DirectoryEntry * dre = DirectoryEntryArray::GetItem(i);
			if(dre != NULL)
				{
				wxString sSelectedPathName = dre->GetPathName();
				asSelected.push_back(sSelectedPathName);
				}
			}
		}

	ClearList();

  if(m_sPath.IsEmpty())
    return;

  if(des == desFILENAME && m_bDirectoriesFirst)
    {
    // Add the directories,
    AddDirectories(m_sPath);
    // sort them,
    DirectoryEntryArray::Sort();

    // Make a separate sorted list of files
    DirectoryEntryArray deaFiles(des, nTextSortFlags, bShowHiddenFiles);
    deaFiles.AddFiles(m_sPath);
    deaFiles.Sort();

    // add them to the directories.
    DirectoryEntryArray::operator+=(deaFiles);
    }
  else
    {
    AddDirectories(m_sPath);
    AddFiles(m_sPath);
    Sort();
    }
	
	for(int j = 0; j < nPreSelectedCount; j++)
		{
		wxString sSelectedPathName = asSelected[j];
		int nIndex = GetIndexFromPathName(sSelectedPathName);
		if(nIndex != -1)
			SelectItem(nIndex);
		}

	// Trigger idle thread details update,
	// otherwise we never get idle events
  int nCount = DirectoryEntryArray::GetCount();
	SetItemCount(nCount);
	
	SetWantIdleEvents(true);
	}

void DirectoryView::ClearList()
	{
	SetWantIdleEvents(false);

	// Workaround: wxListCtrl doesn't clear its selected items list
	// when all the items are removed - force it to clear the list
	ClearSelection();

	SetItemCount(0);
  DirectoryEntryArray::Clear();
	m_nNextUpdateIndex = 0;
  DeleteAllItems();
	}

int DirectoryView::ItemIndexFromPoint(const wxPoint& pnt)
	{
  int nFlags = wxLIST_HITTEST_ONITEMICON | wxLIST_HITTEST_ONITEMLABEL;
  int nItemIndex = HitTest(pnt, nFlags);
	
	return nItemIndex;
	}

///////////////////////
// Selection

DirectoryView::ItemArray DirectoryView::GetSelectedItems()
  {
  ItemArray tia;
  int nItemIndex = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  while(nItemIndex != -1)
    {
    tia.push_back(nItemIndex);
    nItemIndex = GetNextItem(nItemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }

  return tia;
  }

void DirectoryView::ClearSelection()
  {
  // Remove existing selection
  int nCount = GetItemCount();
  for(int i = 0; i < nCount; i++)
		{
    SetItemState(i, 0, wxLIST_STATE_SELECTED);
    /*
		GTK bug: if we deselect all, the focus remains on one item,
		which is ugly.
		But, we cannot remove focus, as doing so completely screws up
		the controls selection functions.
		SetItemState(i, 0, wxLIST_STATE_FOCUSED);
		*/
		}
	}

void DirectoryView::SetSelected(int nItemIndex)
  {
	ClearSelection();
  SelectItem(nItemIndex);
	SetItemState(nItemIndex, 1, wxLIST_STATE_FOCUSED);
  }

void DirectoryView::SelectItem(int nItemIndex)
  {
  int nCount = GetItemCount();
	if(nItemIndex < 0 || nItemIndex >= nCount)
		return;

  SetItemState(nItemIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}

bool DirectoryView::IsItemSelected(int nItemIndex)
  {
  int nCount = GetItemCount();
	if(nItemIndex < 0 || nItemIndex >= nCount)
		return false;

  int nSelectedState = GetItemState(nItemIndex, wxLIST_STATE_SELECTED);
  bool bSelected = (nSelectedState & wxLIST_STATE_SELECTED)? true : false;
  return bSelected;
  }

///////////////////////
// Item Manipulation

void DirectoryView::ActivateItem(int nItemIndex)
  {
  const DirectoryEntry * dre = DirectoryEntryArray::GetItem(nItemIndex);
  if(dre == NULL)
    {
    wxLogDebug(wxT("DirectoryEntryArray::ActivateItem(), DirectoryEntryArray::GetItem(%u) returned NULL"), nItemIndex);
    return;
    }

  DirectoryEntryType det = dre->GetType();

  switch(det)
    {
    case detDirectory:
      {
      bool bCanExecute = dre->CanExecute();
      if(!bCanExecute)
        {
        // TBI: Explain why can't open
        return;
        }
			
			wxString sPathName = dre->GetPathName();
			shSetPathEvent spe(sPathName);
			m_wndParent->GetEventHandler()->ProcessEvent(spe);
      break;
      }

    case detFile:
      dre->Activate();
      break;

    default:
      // TBI: Throw exception
      break;
    }
  }

void DirectoryView::DeleteSelected(bool bDeleteFully)
  {
  ItemArray tia = GetSelectedItems();
  int nCount = tia.size();

  // Remove items last first
  for(int i = nCount - 1; i >= 0; i--)
    {
    int nItemIndex = tia.at(i);
    RequestItemDeletion(nItemIndex, bDeleteFully);
    }
  }

void DirectoryView::RequestItemDeletion(int nItemIndex, bool bDeleteFully)
  {
  const DirectoryEntry * dre = DirectoryEntryArray::GetItem(nItemIndex);
  if(dre == NULL)
    {
    wxLogDebug(wxT("RequestItemDeletion: DirectoryEntryArray::GetItem(%u) returned NULL"), nItemIndex);
    return;
    }

  wxString sPathName = dre->GetPathName();
  DirectoryEntryType det = dre->GetType();

  switch(det)
    {
    case detDirectory:
    case detFile:
      {
			SetWantIdleEvents(false);
			if(bDeleteFully)
				{
				shDeleteEvent dle(sPathName);
				m_wndParent->GetEventHandler()->ProcessEvent(dle);
				}
			else
				{
				shToWastebinEvent twe(sPathName);
				m_wndParent->GetEventHandler()->ProcessEvent(twe);
				}
      break;
      }
    
    default:
    	// TBI: Throw exception
    	break;
    }
  }
