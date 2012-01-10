///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryTree.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryTree.cpp,v 1.1.1.1 2006/01/24 22:13:09 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "DirectoryTree.h"

#include "ResourceManager.h"

extern Persona						psn;
extern ResourceManager *	res;

/////////////////////////////////
// friends, classes, enums and types

DirectoryTree::DirectoryTreeItem::DirectoryTreeItem(const wxString& sNewPath) :
  m_sPath(sNewPath), m_bPopulated(false)
  {
  }

/////////////////////////////////
// Constructors/destructors

DirectoryTree::DirectoryTree
	(
	wxWindow * wndParent,
	const wxWindowID id,
	const wxPoint& pntPosition,
	const wxSize& siz,
	long nStyle
	) :
  wxTreeCtrl(wndParent, id, pntPosition, siz, nStyle),
	m_wndParent(wndParent),
  m_imlButtons(9, 9, true), m_iml(16, 16, true),
	m_bShowHidden(false)
  {
  wxASSERT_MSG(wndParent != 0, wxT("DirectoryTree::DirectoryTree() 'wndParent' == 0."));
  wxASSERT_MSG(res, wxT("DirectoryTree::DirectoryTree() 'res' not loaded."));

	// TBI: work around wx's silly behaviour of showing a different icon when
  // the tree item is expanded
	// wx works as follows: one item for unselected (expanded or not) and selected open,
  // another for selected closed
	// It sounds logical, but it's actually silly:
  // showing 'openness' is what buttons, not icons, are for.
	// We want a different selected icon
	wxBitmap bmpFolderClosed = res->GetImage(resFolderNormal);
	m_iml.Add(bmpFolderClosed);
	m_iml.Add(bmpFolderClosed);
	
  // FIXME: why is this not in the msw version?
#ifdef _GENERIC_TREECTRL_H_
	wxBitmap bmpPlus = res->GetImage(resButtonPlus);
	wxBitmap bmpMinus = res->GetImage(resButtonMinus);
	m_imlButtons.Add(bmpPlus); // Unselected closed button
	m_imlButtons.Add(bmpPlus); // Selected closed button
	m_imlButtons.Add(bmpMinus); // Unselected open button
	m_imlButtons.Add(bmpMinus); // Selected open button
  SetButtonsImageList(&m_imlButtons);
#endif // def __UNIX__

	SetImageList(&m_iml);
	InitTree();
  }

/////////////////////////////////
// Operators

/////////////////////////////////
// Slots

BEGIN_EVENT_TABLE(DirectoryTree, wxTreeCtrl)
	EVT_LEFT_DOWN(                            DirectoryTree::OnLeftMouseDown)
	EVT_TREE_ITEM_EXPANDED(-1,                DirectoryTree::OnItemExpanded)
  EVT_SET_PATH(															DirectoryTree::OnSetPath)
  EVT_TOWASTEBIN(														DirectoryTree::OnToWastebin)
  EVT_DELETE(																DirectoryTree::OnDelete)
  EVT_CREATE(																DirectoryTree::OnCreate)
END_EVENT_TABLE()

void DirectoryTree::OnLeftMouseDown(wxMouseEvent& evt)
	{
  int nFlags = 0;
	wxTreeItemId tid = HitTest(evt.GetPosition(), nFlags);
	if(!tid.IsOk())
		return;

  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("OnLeftMouseDown()"));
	if(nFlags & wxTREE_HITTEST_ONITEMBUTTON)
		{
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tToggling"));
		Toggle(tid);
		return;
		}

  int nSelectClick = (nFlags & (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL));
	if(nSelectClick == 0)
		return;

	wxString sPath;
	bool bGotPath = GetItemPath(tid, sPath);
	if(!bGotPath)
		{
		wxLogDebug(wxT("DirectoryTree::OnLeftMouseDown(): GetItemPath() failed."));
		return;
		}

	shSetPathEvent spe(sPath);
  m_wndParent->GetEventHandler()->ProcessEvent(spe);
  }

void DirectoryTree::OnItemExpanded(wxTreeEvent& evt)
  {
  wxTreeItemId tid = evt.GetItem();
	if(!tid.IsOk())
		{
    wxLogDebug(wxT("OnItemExpanded(): invalid event wxTreeItemId %u"), (int) tid);
		return;
		}

  wxString sPath;
  bool bGotPath = GetItemPath(tid, sPath);
	if(!bGotPath)
		{
    wxLogDebug(wxT("OnItemExpanded(): Can't get path from tree item %u"), (int) tid);
		return;
		}

  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("OnItemExpanded(): '%s'"), sPath.c_str());

	AddChildren(tid);

	// TBI: Improve this by using GetBoundingRect
  // Scroll the view so that we can see as much of the newly expanded branch as possible
	// Get the last 'nChildId', so we can call 'EnsureVisible'
	wxTreeItemId tidLastChild = GetLastChild(tid);
 	if(tidLastChild.IsOk())
 		EnsureVisible(tidLastChild);
	EnsureVisible(tid);
  }

void DirectoryTree::OnSetPath(shSetPathEvent& evt)
	{
	wxString sPath = evt.GetPath();
	SetPath(sPath);
	}

void DirectoryTree::OnToWastebin(shToWastebinEvent& evt)
  {
	wxLogTrace(DIRECTORYTREE_EVENTS, wxT("DirectoryTree::OnToWastebin()"));

	wxString sPathName = evt.GetPathName();
	RemoveItem(sPathName);
  }

void DirectoryTree::OnDelete(shDeleteEvent& evt)
  {
	wxLogTrace(DIRECTORYTREE_EVENTS, wxT("DirectoryTree::OnDelete()"));

	wxString sPathName = evt.GetPathName();
	RemoveItem(sPathName);
	}

/*

We need to add new directories if their parent directory exists in the tree
and is expanded

*/

void DirectoryTree::OnCreate(shCreateEvent& evt)
  {
	wxString sPath = evt.GetPath();
	if(!wxDirExists(sPath))
    {
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("DirectoryTree::OnCreate(): Path does not exist"));
    return;
    }

  wxString sSeparator = wxFileName::GetPathSeparator();
  wxChar chSeparator = sSeparator.GetChar(0);
	wxString sParent = sPath.BeforeLast(chSeparator);
	if(!wxDirExists(sParent))
    {
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("DirectoryAdded(): Parent directory does not exist."));
    return;
    }

	wxTreeItemId tidParent = PopulatePath(sParent);
	if(!tidParent.IsOk())
    {
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("DirectoryAdded(): Parent has not been added. Nothing to be done for now."));
    return;
    }

	wxTreeItemId tid = AddChild(tidParent, sPath);
	}

/////////////////////////////////
// Public Methods

void DirectoryTree::SetPath(const wxString& sNewPath)
  {
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("SetPath('%s')"), sNewPath.c_str());
	if(!wxDirExists(sNewPath))
		{
		// TBI: should find best fit and set that
    wxLogDebug(wxT("SetPath(): the path '%s' does not exist"), sNewPath.c_str());
		return;
		}

  wxTreeItemId tid = PopulatePath(sNewPath);
	if(!tid.IsOk())
		{
    wxLogDebug(wxT("SetPath(): invalid wxTreeItemId returned by PopulatePath() %u"), (int) tid);
		return;
		}

	DoSelection(tid);
  }

wxString DirectoryTree::GetPath()
  {
  wxTreeItemId tid = GetSelection();
  if(!tid.IsOk())
		{
    // FIXME: Should throw exception
    wxLogDebug(wxT("GetPath(): invalid wxTreeItemId returned by GetSelection() %u"), (int) tid);
    return wxT("");
		}

  wxString sPath;
  bool bGotPath = GetItemPath(tid, sPath);
  if(!bGotPath)
		{
    // FIXME: Should throw exception
    wxLogDebug(wxT("GetPath(): Can't get current path"));
    return wxT("");
		}
	return sPath;
  }

void DirectoryTree::SetShowHidden(bool bNewShowHidden, bool bUpdate)
  {
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("SetShowHidden(%u)"), bNewShowHidden);
  m_bShowHidden = bNewShowHidden;
	if(!bUpdate)
		return;
	
	// Save some state
	wxString sSelectedPath = GetPath();

	wxTreeItemId tid = GetItemFromPath(sSelectedPath);
	bool bExpandSelection = false;
	if(tid.IsOk())
		bExpandSelection = IsExpanded(tid);
	
	// Reset
  InitTree();
	
	// Restore state:
	// N.B. The selected item could be a hidden directory
	SetPath(sSelectedPath);
	if(bExpandSelection)
		{
		wxTreeItemId tid = GetItemFromPath(sSelectedPath);
		if(tid.IsOk())
			Expand(tid);
		}
  }

/////////////////////////////////
// Private Methods

void DirectoryTree::InitTree()
  {
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("InitTree()"));
  DeleteAllItems();

#ifdef __UNIX
  wxTreeItemId tidRoot = AddChild(wxTreeItemId(), FILE_SYSTEM_ROOT);
#elif _WINDOWS
  wxTreeItemId tidRoot = AddRoot(FILE_SYSTEM_ROOT, 0, 1, NULL);
#endif // OS
	if(!tidRoot.IsOk())
    {
    wxLogDebug(wxT("DirectoryTree::InitTree(): AddRoot() failed."));
    return; 
    }

#ifdef _WINDOWS
  // Enumerate the system volumes
  TCHAR szDrives[0x200];
  DWORD dwTotalLength = GetLogicalDriveStrings(0x200 * sizeof(TCHAR), szDrives);
  DWORD dwOffset = 0;
  while(dwOffset < dwTotalLength)
    {
    TCHAR * szDrive = szDrives + dwOffset;
    wxString sDrive(szDrive);
    AddChild(tidRoot, sDrive);

    DWORD dwLength = wcslen(szDrive);
    dwOffset += dwLength + 1;
    }
#endif // OS
  }

/*
  
Recurse along a path, populating tree items

Parameters:
	bShow: If true, open folders down to the path
				 If false, simply create the necessary folders

*/

wxTreeItemId DirectoryTree::PopulatePath(const wxString& sPath)
  {
  wxString sSeparator = wxFileName::GetPathSeparator();
  wxChar chSeparator = sSeparator.GetChar(0);

  // Ensure path finishes in path separator
	wxString sTerminatedPath = sPath;
  if(sTerminatedPath.Right(1) != sSeparator)
    sTerminatedPath += sSeparator;

  // Start from root path
  wxTreeItemId tid = GetRootItem(sPath);
	if(!tid.IsOk())
    {
    wxLogDebug(wxT("DirectoryTree::PopulatePath(): GetRootItem(sPath) failed;"), (int) tid);
    return wxTreeItemId(); 
    }

  sTerminatedPath = sTerminatedPath.AfterFirst(chSeparator);
  while(sTerminatedPath != wxT(""))
    {
		// Ensure all children exist
		AddChildren(tid);

		// Select the sub-directory
    wxString sDir = sTerminatedPath.BeforeFirst(chSeparator);
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("CreatePath(): Directory '%s'"), sDir.c_str());
    wxTreeItemId tidChild = GetChildByName(tid, sDir);
		// Fail if we can't find the sub-directory
    if(!tidChild.IsOk())
			{
			wxLogTrace(DIRECTORYTREE_EVENTS, wxT("CreatePath(): GetChildByName(%u, '%s') failed"), (int) tid, sDir.c_str());
			break;
			}

    sTerminatedPath = sTerminatedPath.AfterFirst(chSeparator);
    tid = tidChild;
    }
	
	// Return the item
  return tid;
  }

void DirectoryTree::AddChildren(const wxTreeItemId& tid)
	{
	if(!tid.IsOk())
    {
    wxLogDebug(wxT("DirectoryTree::AddChildren(): invalid wxTreeItemId %u"), (int) tid);
    return;
    }

  wxString sPath;
  bool bGotPath = GetItemPath(tid, sPath);
  if(!bGotPath)
    {
    wxLogDebug(wxT("DirectoryTree::AddChildren(): GetItemPath(%u) returned \"\""), (int) tid);
    return;
    }
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("AddChildren('%s')"), sPath.c_str());

	DirectoryTreeItem * dti = (DirectoryTreeItem *) GetItemData(tid);
	if(dti->IsPopulated())
		return;

	// Tell the tree to stop (falsely) showing the button...
	SetItemHasChildren(tid, false);
	dti->SetIsPopulated();

  // Can we enter this directory?
  bool bCanExecute = psn.CanExecute(sPath);
  if(!bCanExecute)
    {
    wxLogDebug(wxT("DirectoryTree::AddChildren(): Cannot enter directory '%s'"), sPath.c_str());
    return;
    }
  
  // Enumerate sub-directories
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("sPath: %s"), sPath.c_str());
  DirectoryEntryArray dea(desFILENAME, 0, m_bShowHidden);
  dea.AddDirectories(sPath);
  dea.Sort();
  
  // Add them to the tree
  int nCount = dea.GetCount();
  for(int i = 0; i < nCount; i++)
    {
    const DirectoryEntry * dre = dea.GetItem(i);
    wxString sPathName = dre->GetPathName();
    wxTreeItemId tidChild = AddChild(tid, sPathName);
    }
	}

wxTreeItemId DirectoryTree::AddChild(const wxTreeItemId& tidParent, const wxString& sPath)
  {
  DirectoryTreeItem * dti = new DirectoryTreeItem(sPath);
  
	wxTreeItemId tid;
	if(sPath == FILE_SYSTEM_ROOT)
		{
		// TBI: ASSERT tree is empty
		tid = AddRoot(FILE_SYSTEM_ROOT, 0, 1, dti);
		}
  else
		{
		if(!tidParent.IsOk())
			{
      wxLogDebug(wxT("DirectoryTree::AddChild(): invalid parent wxTreeItemId %u"), (int) tidParent);
			return wxTreeItemId();
			}
#ifdef __WXMSW__
    // Treat drives differently
		wxString sDirectory;
    if(sPath.Length() == 3)
      sDirectory = sPath;
    else
      {
		  wxFileName fil(sPath);
		  sDirectory = fil.GetFullName();
      }
#else
		wxFileName fil(sPath);
		wxString sDirectory = fil.GetFullName();
#endif // def __WXMSW__
		tid = AppendItem(tidParent, sDirectory, 0, 1, dti);
		}

	// Show the '+' button
	SetItemHasChildren(tid, true);
	
  return tid;
  }

void DirectoryTree::RemoveItem(const wxString& sPath)
  {
  // Find the item
  wxTreeItemId tidToWastebin = GetItemFromPath(sPath, false);
  if(!tidToWastebin.IsOk())
    {
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tNothing to remove."));
    return;
    }
  
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("GetItemFromPath('%s') returned %u"), sPath.c_str(), (int) tidToWastebin);
	// Sort out the new selection
  wxTreeItemId tidSelection = GetSelection();
  if(!tidSelection.IsOk())
    // FIXME: Should throw exception
    return;

  wxTreeItemId tidNewSelection;
  if(tidSelection == tidToWastebin)
    tidNewSelection = GetItemParent(tidSelection);
  else
    {
		// If we are in or below the path, climb up to directory above
    bool bIsParent = IsAncestorOf(tidToWastebin, tidSelection);
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tIsAncestorOf() returned %u"), bIsParent);
    if(bIsParent)
      tidNewSelection = GetItemParent(tidSelection);
    }

  // Remove item
  Delete(tidToWastebin);

  // Re select if necessary
  if(tidNewSelection.IsOk())
    DoSelection(tidNewSelection);
	}

wxTreeItemId DirectoryTree::GetItemFromPath(const wxString& sPath, bool bReturnBestFit)
  {
  wxString sRemainingPath = sPath;

  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("GetItemFromPath('%s')"), sPath.c_str());

  // Ensure path finishes in '/'
  wxString sSeparator = wxFileName::GetPathSeparator();
  wxChar chSeparator = sSeparator.GetChar(0);
  if(sRemainingPath.Right(1) != sSeparator)
    sRemainingPath += sSeparator;

  // Start from root path
  wxTreeItemId tid = GetRootItem(sPath);
  if(!tid.IsOk())
    {
    wxLogDebug(wxT("GetItemFromPath(): invalid wxTreeItemId root %u"), (int) tid);
    return wxTreeItemId();
    }

  sRemainingPath = sRemainingPath.AfterFirst(chSeparator);
  while(sRemainingPath != wxT(""))
    {
    wxString sDir = sRemainingPath.BeforeFirst(chSeparator);
    //wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tGetChildByName('%s')"), sDir.c_str());
    wxTreeItemId tidChild = GetChildByName(tid, sDir);
    if(!tidChild.IsOk())
      {
      wxLogTrace(DIRECTORYTREE_EVENTS, wxT("GetItemFromPath('%s'): Failed to find directory '%s'"), sPath.c_str(), sDir.c_str());
      if(bReturnBestFit)
        {
        // Allow selection of 'best fit' path
        return tid;
        }
      else
        {
        wxTreeItemId tidBad;
        return tidBad;
        }
      }
    sRemainingPath = sRemainingPath.AfterFirst(chSeparator);
    tid = tidChild;
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tFound child '%s'"), GetItemText(tid).c_str());
    }
  return tid;
  }

wxTreeItemId DirectoryTree::GetChildByName(wxTreeItemId& tidParent, const wxString& sName)
  {
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("GetChildByName(%u, '%s')"), (int) tidParent, sName.c_str());
  if(!tidParent.IsOk())
    {
    wxLogDebug(wxT("DirectoryTree::GetChildByName(): invalid wxTreeItemId tidParent %u"), (int) tidParent);
    return wxTreeItemId();
    }

  wxTreeItemIdValue tiv;
  wxTreeItemId tid = GetFirstChild(tidParent, tiv);
  while(tid.IsOk())
    {
    wxString sText = GetItemText(tid);
    //wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tText: '%s'"), sText.c_str());
    if(sText == sName)
      return tid;
    tid = GetNextSibling(tid);
    }
  wxTreeItemId tidNone;
  return tidNone;
  }

void DirectoryTree::DoSelection(wxTreeItemId& tid)
  {
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("DoSelection()"));
  if(!tid.IsOk())
    return;

  AddChildren(tid);
  EnsureVisible(tid);
	SetFocus();
	SelectItem(tid);
  }

bool DirectoryTree::GetItemPath(wxTreeItemId tid, wxString& sPath)
  {
	if(!tid.IsOk())
		return false;

  DirectoryTreeItem * dti = (DirectoryTreeItem *) GetItemData(tid);
  if(dti == NULL)
    {
    wxLogDebug(wxT("DirectoryTree::OnItemExpanding(): GetItemData(%u) returned NULL"), (int) tid);
    return false;
    }

  sPath = dti->GetPath();
  return true;
  }

bool DirectoryTree::IsAncestorOf(wxTreeItemId tidPutativeAncestor, wxTreeItemId tid)
  {
  wxLogTrace(DIRECTORYTREE_EVENTS, wxT("IsAncestorOf(%u, %u)"), (int) tidPutativeAncestor, (int) tid);
  wxTreeItemId tidParent = GetItemParent(tid);
  while(tidParent.IsOk())
    {
    wxLogTrace(DIRECTORYTREE_EVENTS, wxT("\tParent: %u"), (int) tidParent);
    if(tidParent == tidPutativeAncestor)
      return true;
    tidParent = GetItemParent(tidParent);
    }
  return false;
  }

wxTreeItemId DirectoryTree::GetRootItem(const wxString& sPath)
  {
  wxTreeItemId tidRoot = wxTreeCtrl::GetRootItem();

#ifdef __UNIX
  // There is only one file hierarchy on *NIX
	return tidRoot;
#elif defined _WINDOWS
  // Find the matching drive
  wxFileName fnm(sPath);
  wxString sVolumeToFind = fnm.GetVolume() + wxT(":\\");

  wxTreeItemIdValue tiv;
  wxTreeItemId tid = GetFirstChild(tidRoot, tiv);
  while(tid.IsOk())
    {
    wxString sVolume;
    bool bGotPath = GetItemPath(tid, sVolume);
    if(bGotPath)
      {
      if(sVolume == sVolumeToFind)
        return tid;
      }
    tid = GetNextSibling(tid);
    }
  return wxTreeItemId();
#else
	// Fail - OS is wrong
  return wxTreeItemId();
#endif // OS
  }
