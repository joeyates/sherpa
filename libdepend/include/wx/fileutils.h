///////////////////////////////////////////////////////////////////////////////
// Name:        fileutils.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: fileutils.h,v 1.1.1.1 2006/01/24 22:14:56 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __wxFileUtilities_h__
#define __wxFileUtilities_h__

#include "wx/wx.h"

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filename.h>

bool CreatePath(const wxFileName& filPath, int perm = 0777);
bool DeletePath(const wxFileName& filPath);
bool DeleteFiles(const wxFileName& filPath);
wxArrayString GetDirectories(const wxFileName& filPath);
wxArrayString GetFiles(const wxFileName& filPath);
/** PathNameExists
  *  
  * Indicates if a file or directory with the supplied path and name exists.
	* The wxWidgets functions are specific for files and directories, not both.
  */
bool PathNameExists(const wxString& sPathName);

#ifdef __UNIX__
wxString GetLinkDestination(const wxFileName& fil);
#endif // def __UNIX__

// Name generation
wxString	GetUniquePathName(const wxString& sDefaultPathName);
/*
wxString	GetUniqueDirectory(const wxString& sPath, const wxString& sDefaultDirectoryName);
wxString	GetUniqueDirectory(const wxString& sPath);
*/

#endif // ndef __wxFileUtilities_h__
