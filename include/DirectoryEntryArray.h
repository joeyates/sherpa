///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryEntryArray.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryEntryArray.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __DirectoryEntryArray_h__
#define __DirectoryEntryArray_h__

#include "DirectoryEntry.h"

/*

DirectoryEntryArray holds lists of files and directories

*/

class DirectoryEntryArray : private std::vector<DirectoryEntry>
  {
	// friends, classes, enums and types
  friend class DirectoryView; // So it can add items

	//typedef std::vector<DirectoryEntry>::iterator				Iterator;
	//typedef std::vector<DirectoryEntry>::const_iterator	ConstantIterator;

	// Constructors/destructors
  public:
    DirectoryEntryArray(DirectoryEntrySortKey desNew = desFILENAME, int nNewTextSortFlags = 0, bool bNewShowHiddenFiles = true);

	// Operators
  public:
    DirectoryEntryArray& operator+=(const DirectoryEntryArray& dea);

	// Methods
  public:
    int      GetCount() const;
    const DirectoryEntry * GetItem(int i) const;
    int      GetIndexFromPathName(const wxString& sPathName) const;
    bool     RemoveItem(const DirectoryEntry * dre);
    void     AddDirectories(const wxString& sPath);
    void     Sort();
    void     Clear();

  private:
    int      AddDirectoryEntry(const wxString& sPath, const wxString& sName);
    void     AddFiles(const wxString& sPath);

	// Variables
  private:
    DirectoryEntrySortKey des;
    int       nTextSortFlags;
    // Settings for file list construction
    bool      bShowHiddenFiles;
  };

#endif // ndef __DirectoryEntryArray_h__
