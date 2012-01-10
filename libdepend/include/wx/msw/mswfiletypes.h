/////////////////////////////////////////////////////////////////////////////
// Name:        mswfiletypes.h
// Author:      Joe Yates
// Purpose:     WIN32 file type information interface
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __msw_filetypes_h__
#define __msw_filetypes_h__

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/filetypes.h"
#include <wx/filename.h>

#include <map>

class wxWindowsFileTypes : public wxFileTypesImpl
	{
  private:
    class ExtensionCache : public std::map<wxString, wxArrayString *>
      {
      public:
        ~ExtensionCache()
          {
          }
      };
    class ClassCache : public std::map<wxString, wxString>
      {
      public:
        ~ClassCache()
          {
          }
      };

	public:
		wxWindowsFileTypes();
		~wxWindowsFileTypes()
      {
      UnloadCaches();
      };

		bool							IsLoaded() const;
		wxString					GetFileType(const wxString& sFileURL) const;
		wxFileType2				LoadFileType(const wxString& sFileType) const;
		bool							SaveFileType(const wxFileType2& fti) const;
	
		wxArrayString   	GetApplications(const wxString& sFileURL) const;
		bool					  	AddApplication(const wxString& sFileURL, const wxString& sNewApplication) const;
		bool					  	RemoveApplication(const wxString& sFileURL, const wxString& sApplication) const;
		wxString					GetCommand(const wxString& sApplicationId) const;
		bool							SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const;

    // TBI: should simply derive from wxObject and use its RTTI
    wxString          GetTypeName() const;
    //wxFileType2       LoadInfo(const wxString& sFileType) const;

	private:
		wxString					GetDescription(const wxString& sFileType) const;
		wxFileName				GetImagePath(const wxString& sFileType) const;
		wxString					GetDefaultCommand(const wxString& sFileType) const;
		wxArrayString 		GetExtensions(const wxString& sFileType) const;

    wxString          GetClassFromExtension(const wxString& sExtension) const;

    // Caches
    void              LoadCaches();
    void              UnloadCaches();

	private:
    ClassCache *      cls;
    ExtensionCache *  ext;
	};

#endif
	// ndef __msw_filetypes_h__
