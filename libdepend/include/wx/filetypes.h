/////////////////////////////////////////////////////////////////////////////
// Name:        filetypes.h
// Author:      Joe Yates
// Purpose:     File type information manager interface
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

/*

To do
-----
- Finish implementation of file type saving information,

*/

#ifndef __filetypes_h__
#define __filetypes_h__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

#include "wx/defs.h"

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/filename.h>
#include <wx/image.h>

// Declare a structure to handle extra verbs associated with a File Type
/*
The following commented out as all the wx Hash code in CVS HEAD is (was?) causing segfaults
// WX_DECLARE_STRING_HASH_MAP(wxString, ExtraVerbs);
*/

#if _MSC_VER > 1000
# pragma warning (disable : 4786) // identifier was truncated to '255' characters in the browser information
#endif // _MSC_VER > 1000

#include <map>

typedef std::map<wxString, wxString> ExtraVerbs;

class wxFileType2
	{
	public:
		// ctor for valid instances
		wxFileType2(const wxString& sNewName);

		// ctor for invalid instances
		wxFileType2();

	public:
		bool					  IsValid() const;
		bool					  IsExecutable() const;
	
		// Properties interface
		wxString			  GetName() const;
		void					  SetName(const wxString& sNewName);
		wxString			  GetDescription() const;
		void					  SetDescription(const wxString& sNewDescription);
		wxIcon 		      GetIcon() const;
		wxFileName      GetIconPath(long * nIndex = NULL) const;
		void 					  SetIconPath(const wxFileName& fnmIcon, const long& nIndex = 0);
		wxString			  GetDefaultCommand() const;
		void					  SetDefaultCommand(const wxString& sNewDefaultCommand);
		wxString				GetVerb(const wxString& sVerb) const;
		void						SetVerb(const wxString& sVerb, const wxString& sValue);
		void						DeleteVerb(const wxString& sVerb);
		wxArrayString	  GetExtensions() const;
		void					  SetExtensions(const wxArrayString& asNewExtensions);

		// Static functions
		static bool			IsExecutable(const wxString& sFileType);

	private:
		wxString			  sName;
		wxString			  sDescription;
		wxIconLocation  icl;
		wxString	      sCommand;
		wxArrayString	  asExtensions;
		ExtraVerbs      exv;
	};

class wxFileTypesImpl
	{
	public:
		virtual ~wxFileTypesImpl()
			{};

		// IsLoaded() is a NOOP on Windows,
	  // on GTK it indicates if we've loaded the necessary libs
		virtual bool				  IsLoaded() const																= 0;
		virtual wxString 		  GetFileType(const wxString& sFileURL) const			= 0;
		virtual wxFileType2		LoadFileType(const wxString& sFileType) const		= 0;
		virtual bool				  SaveFileType(const wxFileType2& fti) const			= 0;
	
		// Applications and commands
		virtual wxArrayString GetApplications(const wxString& sFileURL) const																		= 0;
		virtual bool					AddApplication(const wxString& sFileURL, const wxString& sNewApplicationId) const	= 0;
		virtual bool					RemoveApplication(const wxString& sFileURL, const wxString& sApplicationId) const	= 0;
		virtual wxString			GetCommand(const wxString& sApplicationId) const																	= 0;
		virtual bool					SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const			= 0;
	};


/*

File type source enumeration:

wxFILE_TYPE_SOURCE_DEFAULT: the default source for the current state of the system.
	If a window manager is running under X, get data from that window manager.

wxFILE_TYPE_SOURCE_MAILCAP: load data from traditional mailcap files.

*/

enum wxFileTypeSource
	{
	wxFILE_TYPE_SOURCE_DEFAULT = 0,
	wxFILE_TYPE_SOURCE_MAILCAP = 1,
	wxFILE_TYPE_SOURCE_GNOME = 2,
	wxFILE_TYPE_SOURCE_KDE = 3,
	wxFILE_TYPE_SOURCE_WINDOWSREGISTRY = 4,
	};

class wxFileTypesManager
	{
	public:
    wxFileTypesManager(const wxFileTypeSource& src = wxFILE_TYPE_SOURCE_DEFAULT);
    ~wxFileTypesManager();

  public:
		bool				    	IsLoaded() const;
		wxString		    	GetFileType(const wxString& sFileURL) const;
		wxFileType2			  LoadFileType(const wxString& sFileType) const;
		bool				    	SaveFileType(const wxFileType2& fti) const;

		wxArrayString   	GetApplications(const wxString& sFileURL) const;
		bool					  	AddApplication(const wxString& sFileURL, const wxString& sNewApplicationId) const;
		bool					  	RemoveApplication(const wxString& sFileURL, const wxString& sApplicationId) const;
		wxString					GetCommand(const wxString& sApplicationId) const;
		bool							SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const;

	private:
		wxFileTypesImpl *	m_pImpl;
	};

#ifdef __UNIX__
#ifdef __WXGTK__
#include <wx/gtk/gnome/gnomefiletypes.h>
#endif
	// def __WXGTK__
#endif
	// def __UNIX__
#ifdef __WXMSW__
#include <wx/msw/mswfiletypes.h>
#endif
	// def __WXMSW__

#endif
	// ndef __filetypes_h__
