/////////////////////////////////////////////////////////////////////////////
// Name:        gnomefiletypes.cpp
// Author:      Joe Yates
// Purpose:     Gnome file type information interface
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#include <wx/gtk/gnome/gnomefiletypes.h>

#include "wx/filefn.h"

wxGnomeFileTypes::wxGnomeFileTypes()
	{
	}

wxGnomeFileTypes::~wxGnomeFileTypes()
	{
	}

bool wxGnomeFileTypes::IsLoaded() const
	{
	return m_gnomevfs.IsLoaded();
	}

wxString wxGnomeFileTypes::GetFileType(const wxString& sFileURL) const
	{
	if(!m_gnomevfs.IsLoaded())
		{
		wxLogError(_("m_gnomevfs not loaded."));
		return wxT("");
		}

	wxString sURL;
	// Look for a known protocol
	if(
		(sFileURL.Find(wxT("file://")) == 0) || 
		(sFileURL.Find(wxT("ftp://")) == 0) || 
		(sFileURL.Find(wxT("http://")) == 0)
		)
		{
		sURL = sFileURL;
		}
	else
		{
		// Treat 'sFileURL' as a file path name
		if(!::wxFileExists(sFileURL))
			{
			//wxLogError(_("The file '%s' does not exist."), sFileURL.c_str());
			return wxT("");
			}
	
		sURL = wxT("file://") + sFileURL;
		}

	wxString sFileType = m_gnomevfs.GetMimeType(sURL);

	return sFileType;
	}

wxFileType2 wxGnomeFileTypes::LoadFileType(const wxString& sFileType) const
	{
	if(sFileType.IsEmpty())
		{
		wxLogError(_T("wxGnomeFileTypes::LoadFileType(): No File type supplied."));
		return wxFileType2();
		}

	wxFileType2 fti(sFileType);

	wxString sDescription = GetDescription(sFileType);
	fti.SetDescription(sDescription);

	wxFileName fnmIcon = GetIconPath(sFileType);
	fti.SetIconPath(fnmIcon.GetFullPath());

	wxString sCommand = GetDefaultCommand(sFileType);
	if(!sCommand.IsEmpty())
		fti.SetDefaultCommand(sCommand);

	wxArrayString asExtensions = GetExtensions(sFileType);
	fti.SetExtensions(asExtensions);
	
	return fti;
	}

bool wxGnomeFileTypes::SaveFileType(const wxFileType2& fti) const
	{
	wxString sFileType = fti.GetName();

	m_gnomevfs.SetDescription(sFileType, fti.GetDescription());
	m_gnomevfs.SetIconPath(sFileType, fti.GetIconPath());
	m_gnomevfs.SetExtensions(sFileType, fti.GetExtensions());
	}

wxArrayString wxGnomeFileTypes::GetApplications(const wxString& sFileURL) const
	{
	wxString sFileType = GetFileType(sFileURL);
	if(sFileType.IsEmpty())
		return wxArrayString();

	wxArrayGnomeVFSMimeApplication avma = m_gnomevfs.GetShortListApplications(sFileType);
	
	int nCount = avma.Count();
	wxArrayString asApplicationIds;
	for(int i = 0; i < nCount; i++)
		asApplicationIds.Add(avma[i].GetId());

	return asApplicationIds;
	}

bool wxGnomeFileTypes::AddApplication(const wxString& sFileURL, const wxString& sNewApplication) const
	{
	// TBI
	return false;
	}

bool wxGnomeFileTypes::RemoveApplication(const wxString& sFileURL, const wxString& sApplication) const
	{
	// TBI
	return false;
	}

wxString wxGnomeFileTypes::GetCommand(const wxString& sApplicationId) const
	{
	return m_gnomevfs.GetCommand(sApplicationId);
	}

bool wxGnomeFileTypes::SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const
	{
	// TBI
	return false;
	}

///////////////////////////////////////////////////////////////////////////
// Private methods

wxString wxGnomeFileTypes::GetDescription(const wxString& sFileType) const
	{
	if(sFileType.IsEmpty())
		return wxT("");

	return m_gnomevfs.GetDescription(sFileType);
	}

wxFileName wxGnomeFileTypes::GetIconPath(const wxString& sFileType, int nSize) const
	{
	if(sFileType.IsEmpty())
		return wxFileName();

	wxGnomeVFSMimeApplication gma = m_gnomevfs.GetDefaultApplication(sFileType);
	if(!gma.IsValid())
		{
		wxLogDebug(wxT("wxGnomeFileTypes::GetIconPath('%s'): GetDefaultApplication() failed"), sFileType.c_str());
		return wxFileName();
		}

	wxString sIconName = m_gnomevfs.GetIconName(gma);
	m_gnomevfs.FreeGnomeVFSMimeApplication(gma);
		
	if(nSize == -1)
		{
		// Get the largest size available
		std::vector<long> al = m_gtk.GetIconSizes(sIconName);
		int nIconSizeCount = al.size();
		if(nIconSizeCount != 0)
			{
			int nBiggest = 0;
			for(std::vector<long>::iterator it = al.begin(); it <= al.end(); it++)
				{
				int nThisSize = *it;
				nBiggest = (nThisSize > nBiggest)? nThisSize : nBiggest;
				}
			if(nBiggest <= 0)
				{
				wxLogDebug(wxT("wxGnomeFileTypes::GetIconPath('%s'): nBiggest == %i."), sFileType.c_str(), nBiggest);
				return wxFileName();
				}
			
			nSize = nBiggest;
			}
		else
			{
			nSize = 16; // TBI: What default size can we use?
			}
		}
	wxString sIconPath = m_gtk.GetIconPath(sIconName, nSize);

	return sIconPath;
	}

wxString wxGnomeFileTypes::GetDefaultCommand(const wxString& sFileType) const
	{
	if(sFileType.IsEmpty())
		return wxT("");

	// Applications don't get run by applications (normally)
	if(sFileType == wxT("application/x-executable"))
		return wxT("");

	wxGnomeVFSMimeApplication vma = m_gnomevfs.GetDefaultApplication(sFileType);
	if(!vma.IsValid())
		return wxT("");

	wxString sCommand = vma.GetCommand();
	return sCommand;
	}

wxArrayString wxGnomeFileTypes::GetExtensions(const wxString& sFileType) const
	{
	if(sFileType.IsEmpty())
		return wxArrayString();

	return m_gnomevfs.GetExtensions(sFileType);
	}
