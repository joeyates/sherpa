///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryEntryArray.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryEntryArray.cpp,v 1.1.1.1 2006/01/24 22:13:13 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "DirectoryEntryArray.h"

DirectoryEntryArray::DirectoryEntryArray(DirectoryEntrySortKey desNew, int nNewTextSortFlags, bool bNewShowHiddenFiles) :
  des(desNew), nTextSortFlags(nNewTextSortFlags), bShowHiddenFiles(bNewShowHiddenFiles)
  {
  }

int DirectoryEntryArray::GetCount() const
  {
  return size();
  }

const DirectoryEntry * DirectoryEntryArray::GetItem(int i) const
  {
	if(i < 0 || (unsigned int) i >= size())
		throw Exception(_T("DirectoryEntryArray::GetItem() Item %u is out of range"), i);

  return (const DirectoryEntry *) &at(i);
  }

int DirectoryEntryArray::GetIndexFromPathName(const wxString& sPathName) const
	{
  int i = 0;
	for(const_iterator it = begin(); it < end(); it++)
		{
		if(it->m_sPathName == sPathName)
			return i;
    i++;
		}
	
	return -1;
	}

bool DirectoryEntryArray::RemoveItem(const DirectoryEntry * dre)
  {
  int nCount = size();
  for(int i = 0; i < nCount; i++)
    {
    if(&at(i) == dre)
      {
      //wxLogDebug(_T("DirectoryEntryArray::RemoveItem(): removing item %u"), i);
      erase(begin() + i);
      return true;
      }
    }
  return false;
  }

DirectoryEntryArray& DirectoryEntryArray::operator+=(const DirectoryEntryArray& dea)
  {
  int nCount = dea.GetCount();
  for(int i = 0; i < nCount; i++)
    push_back(*dea.GetItem(i));
  return *this;
  }

int DirectoryEntryArray::AddDirectoryEntry(const wxString& sPath, const wxString& sName)
  {
	// TBI exception if either string is empty
  // Pass on info necessary to DirectoryEntry to do proper sorting
  DirectoryEntry dre(sPath, sName, des, nTextSortFlags);
  push_back(dre);
  return size() - 1;
  }

void DirectoryEntryArray::AddDirectories(const wxString& sPath)
  {
  if(sPath.IsEmpty())
    return;

  int nHiddenFlag = (bShowHiddenFiles)? wxDIR_HIDDEN : 0;
  wxDir dir(sPath);
  if(!dir.IsOpened())
    {
    wxLogDebug(_T("wxDir('%s') failed"), sPath.c_str());
    return;
    }
  wxString sDirectory = _("");
  bool bGotDirectory = dir.GetFirst(&sDirectory, wxEmptyString, wxDIR_DIRS | nHiddenFlag);
  while(bGotDirectory)
    {
    AddDirectoryEntry(sPath, sDirectory);
    bGotDirectory = dir.GetNext(&sDirectory);
    }
  }

void DirectoryEntryArray::AddFiles(const wxString& sPath)
  {
  if(sPath.IsEmpty())
    return;

  int nHiddenFlag = (bShowHiddenFiles)? wxDIR_HIDDEN : 0;
  wxDir dir(sPath);
  if(!dir.IsOpened())
    return;
  wxString sFile = _("");
  bool bGotFile = dir.GetFirst(&sFile, wxEmptyString, wxDIR_FILES | nHiddenFlag);
  while(bGotFile)
    {
    AddDirectoryEntry(sPath, sFile);
    bGotFile = dir.GetNext(&sFile);
    }
  }

void DirectoryEntryArray::Sort()
  {
  std::sort(begin(), end());
  }

void DirectoryEntryArray::Clear()
  {
  clear();
  }
