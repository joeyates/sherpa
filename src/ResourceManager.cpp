///////////////////////////////////////////////////////////////////////////////
// Name:        ResourceManager.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: ResourceManager.cpp,v 1.1.1.1 2006/01/24 22:13:11 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "ResourceManager.h"
#include "ThemeHandler.h"

extern Persona				psn;

ResourceManager::ResourceManager() :
  wxImageList(16, 16, true)
  {
	thm = new FileThemeHandler();
  wxString sTheme = wxT("Default");
	bool bLoaded = thm->LoadTheme(sTheme);
  if(!bLoaded)
		throw wx::Exception(wxT("ResourceManager::ResourceManager() Can't load theme '%s'"), sTheme.c_str());
	
	// The following list holds standard items for the DirectoryView
	// See ResourceManager::GetImageIndex() and ResourceManager::GetFileImageIndex()
	// before making any changes
  wxBitmap bmpUnknown = thm->GetImage(resUnknown);
  if(!bmpUnknown.Ok())
		throw wx::Exception(wxT("ResourceManager::ResourceManager() Can't load image 'Unknown'"));

	Add(bmpUnknown);
	Add(thm->GetImage(resCharacter));
	Add(thm->GetImage(resBlock));
	Add(thm->GetImage(resFIFO));
	Add(thm->GetImage(resLink));
	nFolderIndexBase = GetImageCount();
	Add(thm->GetImage(resFolderNormal));
	Add(thm->GetImage(resFolderHidden));
	Add(thm->GetImage(resFolderNormalDropTarget));
	Add(thm->GetImage(resFolderHiddenDropTarget));
	nFileIndexBase = GetImageCount();
	Add(thm->GetImage(resFileNormal));
	Add(thm->GetImage(resFileHidden));
  }

ResourceManager::~ResourceManager()
	{
  wxDELETE(thm);
	}

wxBitmap ResourceManager::GetImage(ResourceId res)
	{
	return thm->GetImage(res);
	}

int ResourceManager::GetDirectoryImageIndex(bool bHidden, bool bDropTarget)
  {
	int nIndex = nFolderIndexBase;
	if(bHidden)
		nIndex += 1;
	if(bDropTarget)
		nIndex += 2;
	
	return nIndex;
  }

int ResourceManager::GetFileImageIndex(bool bHidden)
	{
	int nIndex = nFileIndexBase;
	if(bHidden)
		nIndex += 1;

  return nIndex;
	}

bool ResourceManager::Activate(const wxString& sPathName)
  {
	wxString sFileType = GetFileType(sPathName);
	if(sFileType.IsEmpty())
		return false;

  // We run executables if we have the necessary permissions
	wxFileType2 mmt = LoadFileType(sFileType);
  if(mmt.IsExecutable())
    {
    bool bCanExecute = psn.CanExecute(sPathName);
    if(bCanExecute)
      {
      wxString sCommand = wxString::Format
        (
        wxT("\"%s\""),
        sPathName.c_str()
        );
      wxExecute(sPathName);
      return true;
      }
    else
      return false;
    }
	
	wxString sDefaultCommand = mmt.GetDefaultCommand();
	if(sDefaultCommand.IsEmpty())
		return false;

  wxString sCommand = wxString::Format
    (
	  wxT("%s \"%s\""),
    sDefaultCommand.c_str(),
    sPathName.c_str()
    );
	wxExecute(sCommand);
	
  return true;
  }

wxString ResourceManager::GetDescription(const wxString& sFileType)
  {
	if(sFileType.IsEmpty())
		return _("Unknown File Type");

	wxFileType2 ftp = LoadFileType(sFileType);
	wxString sDescription = ftp.GetDescription();
	if(sDescription.IsEmpty())
		return sFileType;

  return sDescription;
  }

/*
	
Get cached index, or load image and return new index

*/

int ResourceManager::GetImageIndex(const wxString& sFileType, bool bHidden)
  {
	ResourceId resDefault = (bHidden)? resFileHidden : resFileNormal;
	if(sFileType.IsEmpty())
		return resDefault;

	// Manage different images for hidden and non-
	// Add 'Hidden' to the start of hidden images
  wxString sFullFileType = wxString::Format
    (
	  wxT("%s%s"),
    (bHidden)? wxT("Hidden") : wxT(""),
    sFileType.c_str()
    );
	TypeIndexMap::iterator it = hImageIndex.find(sFullFileType);
 	if(it != hImageIndex.end())
		{
		// We have already cached the image, return it
		int nIndex = it->second;
		return nIndex;
		}

	// We load the image ourselves to allow user to resize if necessary
	wxBitmap bmp = GetImageForFileType(sFileType);
	if(!bmp.Ok())
		{
		wxLogDebug(wxT("GetImageForFileType('%s') failed, using 'resDefault'"), sFileType.c_str());
		hImageIndex[sFullFileType] = resDefault;
		return resDefault;
		}

	int nNewIndex = AddImage(sFullFileType, bmp);

	return nNewIndex;
	}

int ResourceManager::GetLinkImageIndex(const wxString& sFileType, bool bHidden)
  {
	// Prepend 'Link'
  wxString sFullFileType = wxString::Format
    (
	  wxT("Link%s%s"),
    (bHidden)? wxT("Hidden") : wxT(""),
    sFileType.c_str()
    );
 	if(hImageIndex.find(sFullFileType) == hImageIndex.end())
		{
		int nIndex = hImageIndex[sFullFileType];
		return nIndex;
		}

	// Overlay the link image on the image
	wxBitmap bmp = GetImageForFileType(sFileType);
	if(!bmp.Ok())
		return resLink;

/*
FIXME
// 	// Default to 'text'
// 	if(!img.Ok())
// 		img = GetImageForFileType(wxT("text/plain"));

	// TBI: Overlay link symbol
	unsigned char * puc = ico.GetIconData();
	for(int i = 0; i < 60; i += 3)
		puc[i] = 255;
*/

	return AddImage(sFullFileType, bmp);
	}

bool ResourceManager::IsBinaryExecutable(const wxString& sFileType)
  {
  return sFileType.Contains(wxT("application/x-executable")); // TBI: OS specific
  }

wxBitmap ResourceManager::GetImageForFileType(const wxString& sFileType)
	{
	if(sFileType.IsEmpty())
		{
		wxLogDebug(wxT("ResourceManager::GetImageForFileType() called with sFileType == \"\", returning invalid bitmap."));
		return wxBitmap();
		}

	wxFileType2 ftp = LoadFileType(sFileType);
	if(!ftp.IsValid())
	if(sFileType.IsEmpty())
		{
		wxLogDebug(wxT("ResourceManager::GetImageForFileType(): LoadFileType('%s') failed, returning invalid bitmap."), sFileType.c_str());
		return wxBitmap();
		}

#ifdef __WXMSW__
  long nIndex = 0;
	wxFileName fil = ftp.GetIconPath(&nIndex);
#else
	wxFileName fil = ftp.GetIconPath();
#endif // def __WXMSW__
	wxString sPathName = fil.GetFullPath();
	if(sPathName.IsEmpty())
		{
		wxLogDebug(wxT("ResourceManager::GetImageForFileType(): LoadFileType('%s') returned wxFileType2 with empty Pathname, returning invalid bitmap."), sFileType.c_str());
		return wxBitmap();
		}

	if(!wxFileExists(sPathName))
		{
		wxLogDebug(wxT("ResourceManager::GetImageForFileType(): LoadFileType('%s') returned wxFileType2 no existant Pathname '%s', returning invalid bitmap."), sFileType.c_str(), sPathName.c_str());
		return wxBitmap();
		}
	
#ifdef __WXMSW__
  wxIconLocation icl(sPathName, nIndex);
	wxIcon icoLoad(icl);
  wxBitmap bmp;
  bmp.CopyFromIcon(icoLoad);
	//wxImage img = bmp.ConvertToImage();
#else
	wxBitmap bmp(sPathName);
#endif // def __WXMSW__
	if(!bmp.Ok())
		{
		wxLogDebug(wxT("ResourceManager::GetImageForFileType(): wxBitmap('%s') failed, returning invalid bitmap."), sPathName.c_str());
		return wxBitmap();
		}
	
	int nWidth = bmp.GetWidth();
	int nHeight = bmp.GetHeight();
	// double dAspectRatio = (double) nHeight / (double) nWidth; // TBI: do no-stretch scaling
	
  wxImage img = bmp.ConvertToImage();
	int nListWidth = 0;
	int nListHeight = 0;
	GetSize(0, nListWidth, nListHeight);
	img.Rescale(nListWidth, nListHeight);

  wxBitmap bmpRescaled(img);

	//wxIcon ico;
	//ico.CopyFromBitmap(bmpToIcon);

  return bmpRescaled;
	}

int ResourceManager::AddImage(const wxString& sFileType, const wxBitmap& bmp)
	{
	int nNewIndex = GetImageCount();
	Add(bmp);

 	hImageIndex[sFileType] = nNewIndex;
	return nNewIndex;
	}
