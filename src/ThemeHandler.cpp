///////////////////////////////////////////////////////////////////////////////
// Name:        ThemeHandler.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: ThemeHandler.cpp,v 1.1.1.1 2006/01/24 22:13:09 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"

#include "ThemeHandler.h"

FileThemeHandler::FileThemeHandler()
	{
	bThemeLoaded = false;
	wxChar ** psz = wxTheApp->argv;
	wxString sAppPathName = psz[0];
	sAppPath = wxPathOnly(sAppPathName);
	}

bool FileThemeHandler::LoadTheme(const wxString& sNewTheme)
	{
  wxString sSeparator = wxFileName::GetPathSeparator();
	sThemePath.Printf(wxT("%s%sThemes%s%s"), sAppPath.c_str(), sSeparator.c_str(), sSeparator.c_str(), sNewTheme.c_str());
	// TBI: Check directory exists

	hImageResources[resApplication] = wxT("Sherpa");
	// DirectoryView items
	hImageResources[resUnknown] = wxT("Unknown");
	hImageResources[resCharacter] = wxT("Character");
	hImageResources[resBlock] = wxT("Block");
	hImageResources[resFIFO] = wxT("File");
	hImageResources[resLink] = wxT("Link");
	hImageResources[resFolderNormal] = wxT("FolderClosed");
	hImageResources[resFolderHidden] = wxT("FolderClosedHidden");
	hImageResources[resFolderNormalDropTarget] = wxT("FolderClosedDropTarget");
	hImageResources[resFolderHiddenDropTarget] = wxT("FolderClosedDropTarget");
	hImageResources[resFileNormal] = wxT("File");
	hImageResources[resFileHidden] = wxT("FileHidden");
	// DirectoryTree items
	hImageResources[resButtonPlus] = wxT("ButtonPlus");
	hImageResources[resButtonMinus] = wxT("ButtonMinus");
	sTheme = sNewTheme;
	bThemeLoaded = true;

	return true;
	}

bool FileThemeHandler::ThemeLoaded()
	{
	return bThemeLoaded;
	}

wxBitmap FileThemeHandler::GetImage(ResourceId res)
	{
  std::map<int, wxString>::iterator it = hImageResources.find(res);
 	if(it == hImageResources.end())
		return wxBitmap();

	wxString sImageName = it->second;

//#ifdef __WXMSW__
//  wxIcon ico(sImageName, wxBITMAP_TYPE_BMP_RESOURCE);
//#else
  wxString sSeparator = wxFileName::GetPathSeparator();
	wxString sFilePathName = wxString::Format
    (
    wxT("%s%s%s.xpm"),
    sThemePath.c_str(),
    sSeparator.c_str(),
    sImageName.c_str()
    );
  if(!wxFileExists(sFilePathName))
    throw Exception(wxT("FileThemeHandler::GetImage() The file '%s' is missing."), sFilePathName.c_str());

  wxBitmap bmp(sFilePathName, wxBITMAP_TYPE_XPM);
//#endif // def __WXMSW__

	return bmp;
	}
