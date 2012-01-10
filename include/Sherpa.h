///////////////////////////////////////////////////////////////////////////////
// Name:        Sherpa.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: Sherpa.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __Sherpa_h__
#define __Sherpa_h__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
# include "wx/wx.h"

#if !wxUSE_THREADS
# error wxUSE_THREADS is set to false. Thread support is required by this application
#endif // wxUSE_THREADS

#ifdef __WXMSW__

# if _MSC_VER > 1000

# ifdef _DEBUG
#   include "crtdbg.h"
#   define new                    new(_NORMAL_BLOCK, __FILE__, __LINE__)
#   define malloc(x)              _malloc_dbg(x, _CLIENT_BLOCK, __FILE__, __LINE__)
#   define free(x)                _free_dbg(x, _CLIENT_BLOCK)
#   define realloc(x, y)          _realloc_dbg(x, y, _CLIENT_BLOCK, __FILE__, __LINE__)
#   define calloc(x, y)           _calloc_dbg(x, y, _CLIENT_BLOCK, __FILE__, __LINE__)
# endif // def _DEBUG
# endif // _MSC_VER > 1000

#endif // def __WXMSW__

//////////////////////////////////////////////////////////////////////////////
// Standard C library includes

#include <errno.h>

//////////////////////////////////////////////////////////////////////////////
// STL includes

#if _MSC_VER > 1000
# pragma warning (disable : 4786) // identifier was truncated to '255' characters in the browser information
#endif // _MSC_VER > 1000

#include <algorithm> // Gives us 'sort'
#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////////
// wxWidgets includes

#include <wx/arrstr.h>
#include <wx/config.h>
#include <wx/dataobj.h>
#include <wx/dir.h>
#include <wx/dynarray.h>
#include <wx/event.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <wx/notebook.h>
#include <wx/regex.h>
#include <wx/splitter.h>
#include <wx/thread.h>
#include <wx/treectrl.h>
#include <wx/utils.h>

//////////////////////////////////////////////////////////////////////////////
// Sherpa classes

#include <wx/fileutils.h>
#include <wx/exception.h>
#include <wx/filetypes.h>

using namespace wx;

//////////////////////////////////////////////////////////////////////////////
// Global includes

#include <Persona.h>
#include <Undo.h>

//////////////////////////////////////////////////////////////////////////////////
// Global defines

#define APP_NAME								wxT("Sherpa")
#define SHERPA_VERSION					wxT("0.6.0802")
#define SHERPA_PROJECT					wxT("Sherpa Project")
#define SETTINGS_DIRECTORY			wxT(".sherpa")
#define CONFIG_FILE_NAME				wxT("Config.txt")

#ifdef __UNIX__
# define FILE_SYSTEM_ROOT   		wxT("/")
#else
# define FILE_SYSTEM_ROOT   		wxT("Windows")
#endif // def __UNIX__

#define CONFIG_SAVE_INITIAL_PATH	wxT("/Config/SaveInitialPath")

// Trace masks
#define SHERPAFRAME_EVENTS wxT("SHERPAFRAME_EVENTS")
#define DIRECTORYTREE_EVENTS wxT("DIRECTORYTREE_EVENTS")
#define DIRECTORYVIEW_EVENTS wxT("DIRECTORYVIEW_EVENTS")
#endif

#endif // ndef __Sherpa_h__
