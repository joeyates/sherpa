///////////////////////////////////////////////////////////////////////////////
// Name:        fileutils.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: fileutils.cpp,v 1.1.1.1 2006/01/24 22:14:49 titusd Exp $
// Copyright:   (c) BID Snc
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/fileutils.h>

bool CreatePath(const wxFileName& filPath, int perm)
	{
  // If it already exists, we're done
	if(filPath.DirExists())
		return true;
  
  // Create preceding paths
	wxFileName filParentPath = filPath.GetPath();
	bool bCreated = CreatePath(filParentPath, perm);
	if(!bCreated)
		return false;
  
  // Create the path
  wxString sPath = filPath.GetFullPath();
	return wxMkdir(sPath, perm);
	}

bool DeletePath(const wxFileName& filPath)
	{
  wxString sPath = filPath.GetFullPath();

  //wxLogDebug(_("DeletePath('%s')"), sPath.c_str());
  // If it doesn't exist, we're done
	if(!filPath.DirExists())
		return true;
  
  // Delete child directories
  wxArrayString as = GetDirectories(sPath);
  int nCount = as.Count();
  for(int i = 0; i < nCount; i++)
    {
    wxString sSubPath = sPath + wxFileName::GetPathSeparator() + as.Item(i);
    DeletePath(sSubPath);
    }
  DeleteFiles(sPath);
  
  // Delete the path
	return wxRmdir(sPath);
	}

/*

Delete all files in a directory
  
*/

bool DeleteFiles(const wxFileName& filPath)
  {
  wxString sPath = filPath.GetFullPath();

  // Delete child files
  wxArrayString asFiles = GetFiles(sPath);
  int nCount = asFiles.Count();
  for(int i = 0; i < nCount; i++)
    {
    wxString sPathName = sPath + wxFileName::GetPathSeparator() + asFiles.Item(i);
    wxLogDebug(_("\tdeleting file '%s'"), sPathName.c_str());
    bool bFileRemoved = wxRemoveFile(sPathName);
    if(!bFileRemoved)
      {
      wxLogDebug(_("DeleteFiles: wxRemoveFile('%s') failed"), sPathName.c_str());
      return false;
      }
    }

  return true;
  }

wxArrayString GetDirectories(const wxFileName& filPath)
  {
  wxArrayString as;
  wxDir dir(filPath.GetFullPath());
  if(!dir.IsOpened())
    return as;
  wxString sDirectory = _("");
  bool bGotDirectory = dir.GetFirst(&sDirectory, wxEmptyString, wxDIR_DIRS);
  while(bGotDirectory)
    {
    as.Add(sDirectory);
    bGotDirectory = dir.GetNext(&sDirectory);
    }
  return as;
  }

wxArrayString GetFiles(const wxFileName& filPath)
  {
  wxArrayString as;
  wxDir dir(filPath.GetFullPath());
  if(!dir.IsOpened())
    return as;
  wxString sFile = _("");
  bool bGotFile = dir.GetFirst(&sFile, wxEmptyString, wxDIR_FILES);
  while(bGotFile)
    {
    as.Add(sFile);
    bGotFile = dir.GetNext(&sFile);
    }
  return as;
  }

bool PathNameExists(const wxString& sPathName)
	{
#ifdef __UNIX__
	struct stat 				st;
	int nStatResult = lstat(sPathName.fn_str(), &st);
#elif defined __WXMSW__
	struct _stat 				st;
# ifdef wxUSE_UNICODE
  int nStatResult = _wstat(sPathName.fn_str(), &st)
# else
  int nStatResult = _stat(sPathName.fn_str(), &st)
# endif // wxUSE_UNICODE
#endif // def __UNIX__

	bool bExists = (nStatResult == 0)? true : false;
  return bExists;
	}

#ifdef __UNIX__
wxString GetLinkDestination(const wxFileName& fil)
	{
#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(fil.GetFullPath().c_str());
	const char * szPathName = (const char*) mbBuffer;
// 	printf("szPathName: %s\n", szPathName);
#else // wxUSE_UNICODE
	const char * szPathName = fil.GetFullPath().c_str();
#endif // wxUSE_UNICODE

	size_t nBufferSize = 200;
	char * szBuffer = new char[nBufferSize + 1];
	int nLength = readlink(szPathName, szBuffer, nBufferSize);
// 	while(nLength == -1 || nLength == nBufferSize)
// 		{
// 		delete [] szBuffer;
// 		nBufferSize+= 200;
// 		szBuffer = new char[nBufferSize + 1];
// 		nLength = readlink(szPathName, szBuffer, nBufferSize);
// 		}
	// readlink() does not add the trailing NUL character
	szBuffer[nLength] = '\0';

#if wxUSE_UNICODE
	wxString sLinkDestination = wxConvLibc.cMB2WC(szBuffer);
#else // wxUSE_UNICODE
	wxString sLinkDestination = szBuffer;
#endif // wxUSE_UNICODE
	delete [] szBuffer;
	
	return sLinkDestination;
	}
#endif // def __UNIX__

wxString GetUniquePathName(const wxString& sDefaultPathName)
  {
	if(!PathNameExists(sDefaultPathName))
		return sDefaultPathName;

	wxString sPath;
	wxString sBaseName;
	wxString sExtension;
	wxFileName::SplitPath(sDefaultPathName, &sPath, &sBaseName, &sExtension);
  if(!sExtension.IsEmpty())
    sExtension = wxT(".") + sExtension;

  wxString sSeparator = wxFileName::GetPathSeparator();

  int i = 1;
	bool bExists = true;
  wxString sNewPathName;
  while(bExists)
    {
		sNewPathName.Printf
			(
			wxT("%s%s%s%u%s"),
			sPath.c_str(),
			sSeparator.c_str(),
			sBaseName.c_str(),
			i,
			sExtension.c_str()
			);
		bExists = PathNameExists(sNewPathName);
    i++;
    }

  return sNewPathName;
  }

/*
wxString GetUniqueDirectory(const wxString& sPath, const wxString& sDefaultDirectoryName)
  {
	wxString sDefaultPath = sPath + wxFileName::GetPathSeparator() + sDefaultDirectoryName;
	return sPath + wxFileName::GetPathSeparator() + GetUniqueDirectory(sDefaultPath);
	}

wxString GetUniqueDirectory(const wxString& sDefaultPath)
  {
	if(!wxDirExists(sDefaultPath))
		return sDefaultPath;

	wxString sPath;
	wxString sBaseName;
	wxString sExtension;
	wxFileName::SplitPath(sDefaultPath, &sPath, &sBaseName, &sExtension);
  if(!sExtension.IsEmpty())
    sExtension = wxT(".") + sExtension;

  wxString sNewPath;
  int i = 0;
  bool bFound = false;
  while(!bFound)
    {
    sNewPath.Printf(wxT("%s%u"), sDefaultDirectoryName.c_str(), i);
    sNewPath = sPath + wxFileName::GetPathSeparator() + sNewDirectory;
    bFound = !wxDirExists(sNewPath);
    i++;
    }

  return sNewPath;
  }
*/
