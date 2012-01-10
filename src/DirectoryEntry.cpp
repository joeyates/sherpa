///////////////////////////////////////////////////////////////////////////////
// Name:        DirectoryEntry.cpp
// Purpose:     Represents a single file or directory
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: DirectoryEntry.cpp,v 1.1.1.1 2006/01/24 22:13:09 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "DirectoryEntry.h"

#include "ResourceManager.h"

extern ResourceManager * res;
extern Persona           psn;

DirectoryEntryType GetDirectoryEntryType(const wxString& sPath)
	{
	struct STRUCT_STAT st;
	return GetDirectoryEntryType(sPath, st);
	}

DirectoryEntryType GetDirectoryEntryType(const wxString& sPath, struct STRUCT_STAT& st)
	{
	int nStatResult = STAT(sPath.fn_str(), &st);
  if(nStatResult != 0)
		{
    wxLogDebug(_T("DirectoryEntry::DirectoryEntry(): lstat('%s') failed"), sPath.c_str());
		return detNone;
		}

	DirectoryEntryType det;
#ifdef __UNIX__
	switch(st.st_mode & __S_IFMT)
		{
		case __S_IFDIR:
			det = detDirectory;
			break;

    case __S_IFCHR:
			det = detCharacterSpecial;
			break;

    case __S_IFBLK:
			det = detBlockSpecial;
			break;

    case __S_IFREG:
			det = detFile;
			break;

    case __S_IFIFO:
			det = detFIFO;
			break;

    case __S_IFLNK:
			det = detLink;
			break;

    default:
			det = detNone;
			break;
		}
#elif defined __WXMSW__
	switch(st.st_mode & _S_IFMT)
		{
		case _S_IFDIR:
			det = detDirectory;
			break;

    case _S_IFREG:
			det = detFile;
			break;

    default:
			det = detNone;
			break;
		}
#endif // def __UNIX__
	
	return det;
	}

/////////////////////////////////
// Constructors/destructors

DirectoryEntry::DirectoryEntry(const wxString& sNewPath, const wxString& sNewName, DirectoryEntrySortKey desNew, int nTextSortFlags) :
  m_sPath(sNewPath),
	m_sName(sNewName),
	m_des(desNew),
	m_bCacheValuesLoaded(false),
	m_nImageIndex(-1)
  {
#ifdef __UNIX__
	m_sPathName = (m_sPath == wxT("/"))? wxFileName::GetPathSeparator() + m_sName : m_sPath + wxFileName::GetPathSeparator() + m_sName;
#elif defined __WXMSW__
	m_sPathName = (m_sPath.Length() == 3)? sPath + m_sName : m_sPath + wxFileName::GetPathSeparator() + m_sName;
#endif
  m_bCaseSensitive = (nTextSortFlags & DIRECTORYENTRY_TEXTSORT_CASESENSITIVE)? true : false;
  m_bIgnoreHiddenDots = (nTextSortFlags & DIRECTORYENTRY_TEXTSORT_IGNOREHIDDENDOTS)? true : false;

	m_det = GetDirectoryEntryType(m_sPathName, m_st);

  m_bIsHidden = (m_sName.Left(1) == wxT("."))? true : false;

  m_sCompareName = m_sName;
  if(m_bIgnoreHiddenDots)
    {
    // FIXME: Untested
    // FIXME: Make more efficient
    while(m_sCompareName.Left(1) == wxT("."))
      m_sCompareName.Remove(0, 1);
    }
  }

/////////////////////////////////
// Operators

bool DirectoryEntry::operator<(const DirectoryEntry& dreOther) const
  {
  switch(m_des)
    {
    case desFILENAME:
      return CompareByName(dreOther);

    case desMODDATE:
      return CompareByModificationDate(dreOther);

    default:
      return false; // TBI: exception
    }
  }

/////////////////////////////////
// Public Methods

wxString DirectoryEntry::GetPathName() const
	{
	return m_sPathName;
	}

wxString DirectoryEntry::GetName() const
	{
	return m_sName;
	}

DirectoryEntryType DirectoryEntry::GetType() const
  {
  return m_det;
  }

bool DirectoryEntry::IsHidden() const
  {
  return m_bIsHidden;
  }

bool DirectoryEntry::CanExecute() const
  {
  return psn.CanExecute(m_st);
  }

int DirectoryEntry::GetPermissions() const
	{
	return (int) (m_st.st_mode & 0777);
	}

time_t DirectoryEntry::GetModificationTime() const
	{
	return m_st.st_mtime;
	}

wxString DirectoryEntry::GetDescription() const
	{
	// Return cached value if available
	if(!m_sDescription.IsEmpty())
		return m_sDescription;

	DirectoryEntry * self = const_cast<DirectoryEntry *>(this);
	
	// For values whose default description equals the full description,
	// save 'sDescription'
  switch(m_det)
    {
    case detDirectory:
      self->m_sDescription = _("Directory");
			return m_sDescription;

#ifdef __UNIX__
    case detCharacterSpecial:
      self->m_sDescription = _("Character Device");
			return m_sDescription;

    case detBlockSpecial:
      self->m_sDescription = _("Block Device");
			return m_sDescription;

    case detFIFO:
      self->m_sDescription = _("FIFO");
			return m_sDescription;

    case detLink:
			if(m_bCacheValuesLoaded)
				return self->m_sDescription;
			else
				return _("Link");
#endif // def __UNIX__

    case detFile:
			if(m_bCacheValuesLoaded)
				return self->m_sDescription;
			else
				return _("File");

    default:
		case detNone:
      self->m_sDescription = res->GetDescription(wxT(""));
			return m_sDescription;

    }
	}

int DirectoryEntry::GetImageIndex() const
	{
	if(m_nImageIndex != -1)
		return m_nImageIndex;

	DirectoryEntry * self = const_cast<DirectoryEntry *>(this);

  switch(m_det)
    {
    case detDirectory:
      {
      bool bHidden = IsHidden();
      self->m_nImageIndex = res->GetDirectoryImageIndex(bHidden, false);
      return m_nImageIndex;
      }

#ifdef __UNIX__
    case detCharacterSpecial:
      self->m_nImageIndex = resCharacter;
      return m_nImageIndex;

    case detBlockSpecial:
      self->m_nImageIndex = resBlock;
      return m_nImageIndex;

    case detFIFO:
      self->m_nImageIndex = resFIFO;
      return m_nImageIndex;

    case detLink:
			// No cache
      return resLink;
#endif // def __UNIX__

    case detFile:
      {
			// No cache
      bool bHidden = IsHidden();
			return res->GetFileImageIndex(bHidden);
      }

		case detNone:
    default:
      self->m_nImageIndex = resUnknown;
      return m_nImageIndex;

    }
	}

bool DirectoryEntry::CacheValues()
	{
	if(m_bCacheValuesLoaded)
		return false;
	
  switch(m_det)
    {
#ifdef __UNIX__
		case detLink:
			{
			m_sDescription = _("Link");
			wxString sDestination = GetLinkDestination(m_sPathName);
			if(!sDestination.IsEmpty())
				m_sDescription << wxT(" -> ") << sDestination;

			// TBI: Get file type for destination
			wxString sDestinationFileType = wxT("text/plain");
      bool bHidden = IsHidden();
			m_nImageIndex = res->GetLinkImageIndex(sDestinationFileType, bHidden);
			m_bCacheValuesLoaded = true;
      return true;
			}
#endif // def __UNIX__

    case detFile:
      {
			wxString sFileType = GetFileType();
			//wxLogDebug(wxT("%s: sFileType: '%s'"), m_sName.c_str(), sFileType.c_str());
			if(sFileType.IsEmpty())
				return false;

      bool bHidden = IsHidden();
			m_nImageIndex = res->GetImageIndex(sFileType, bHidden);
			m_sDescription = res->GetDescription(sFileType);
			m_bCacheValuesLoaded = true;
      return true;
      }

		default:
			{
			m_bCacheValuesLoaded = true;
			break;
			}
    }
	
	return false;
	}

wxString DirectoryEntry::GetFileType() const
	{
	// Return cached value if available
	if(!m_sFileType.IsEmpty())
		return m_sFileType;

	DirectoryEntry * self = const_cast<DirectoryEntry *>(this);
	self->m_sFileType = res->GetFileType(m_sPathName);
	
	return self->m_sFileType;
	}

bool DirectoryEntry::Activate() const
	{
	return res->Activate(m_sPathName);
	}

/////////////////////////////////
// Protected Methods

/*

The 'CompareName' is identical to the file name,
except when we have the 'IgnoreHiddenDots' option,
in which case it holds a copy with the leading dots stripped
    
*/

wxString DirectoryEntry::GetCompareName() const
  {
  return m_sCompareName;
  }

/////////////////////////////////
// Private Methods

/*

As we are in a constant function, g++ will not allow us to
create copies of strings in this function.

*/

bool DirectoryEntry::CompareByName(const DirectoryEntry& dreOther) const
  {
  int nResult;
  if(m_bCaseSensitive)
    nResult = GetCompareName().Cmp(dreOther.GetCompareName());
  else
    nResult = GetCompareName().CmpNoCase(dreOther.GetCompareName());
  bool bResult = (nResult < 0)? true : false;

  return bResult;
  }

bool DirectoryEntry::CompareByModificationDate(const DirectoryEntry& dreOther) const
  {
  wxDateTime dtThis = GetModificationTime();
  wxDateTime dtOther = dreOther.GetModificationTime();
  bool bResult = (dtThis > dtOther)? true : false;

  return bResult;
  }
