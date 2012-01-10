///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryEntry.h
// Purpose:     Represents a single file or directory
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryEntry.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __DirectoryEntry_h__
#define __DirectoryEntry_h__

#include <sys/stat.h>
#ifdef __UNIX__
#include <unistd.h>
#endif // def __UNIX__

#ifdef __UNIX__
#	define STAT(x,y) lstat(x,y)
#	define STRUCT_STAT stat
#elif defined __WXMSW__
#	define STRUCT_STAT _stat

# ifdef wxUSE_UNICODE
#   define STAT(x, y) _wstat(x, y)
# else
#   define STAT(x, y) _stat(x, y)
# endif // wxUSE_UNICODE
#endif // def OS

#define MIME_DIRECTORY wxT("Directory")

enum DirectoryEntryType
  {
  detNone = 0,
  detDirectory,
#ifdef __UNIX__
  detCharacterSpecial,
  detBlockSpecial,
#endif // def __UNIX__
  detFile,
#ifdef __UNIX__
	detFIFO,
  detLink,
#endif // def __UNIX__
  };

enum DirectoryEntrySortKey
  {
  desFILENAME = 1,
  desMODDATE = 2,
  desFILETYPE = 3
  };

/////////////////////////////////////////////////////////
// Text sort flags

#define DIRECTORYENTRY_TEXTSORT_CASESENSITIVE 1
#define DIRECTORYENTRY_TEXTSORT_IGNOREHIDDENDOTS 2

DirectoryEntryType GetDirectoryEntryType(const wxString& sPath);
DirectoryEntryType GetDirectoryEntryType(const wxString& sPath, struct STRUCT_STAT& st);

/** A 'DirectoryEntry' is a file or a folder.

 *  Each item in a DirectoryView is contained in a 'DirectoryEntry'.
 */
class DirectoryEntry
  {
	// friends, classes, enums and types
	friend class DirectoryEntryArray;

	// Constructors/destructors
  public:
    DirectoryEntry(const wxString& sNewPath, const wxString& sNewName, DirectoryEntrySortKey desNew, int nTextSortFlags);

	// Operators
  public:
    bool operator<(const DirectoryEntry& dreOther) const;

	// Methods
  public:
		wxString						GetPathName() const;
		wxString						GetName() const;
    DirectoryEntryType  GetType() const;
    bool                IsHidden() const;
    bool                CanExecute() const;
		int									GetPermissions() const;
		time_t							GetModificationTime() const;
		wxString						GetDescription() const;
		int									GetImageIndex() const;
		wxString						GetFileType() const;
		bool								Activate() const;
		
		// Get the full description and image index
		// 'true' return means the value was updated
		bool								CacheValues();

  protected:
    wxString  					GetCompareName() const;

  private:
    bool      					CompareByName(const DirectoryEntry& dreOther) const;
    bool      					CompareByModificationDate(const DirectoryEntry& dreOther) const;

	// Members
  private:
    wxString  					m_sPathName;
    wxString  					m_sPath;
    wxString  					m_sName;
    DirectoryEntryType 	m_det;
    bool      					m_bIsHidden;
    // Sort-related members
    DirectoryEntrySortKey m_des;
    bool      					m_bCaseSensitive;
    bool      					m_bIgnoreHiddenDots;

    // Cached values
		// 'm_sCompareName' is used to compare this file name with others
    wxString  					m_sCompareName;
		bool								m_bCacheValuesLoaded;
		// 'sDescription' is the full description, with file type, link destination, etc
    wxString  					m_sDescription;
		int									m_nImageIndex;
    wxString  					m_sFileType;
		struct STRUCT_STAT	m_st;
  };

#endif // ndef __DirectoryEntry_h__
