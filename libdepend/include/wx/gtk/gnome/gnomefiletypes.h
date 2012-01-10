/////////////////////////////////////////////////////////////////////////////
// Name:        gnomefiletypes.h
// Author:      Joe Yates
// Purpose:     Gnome file type information interface
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __gnomefiletypes_h__
#define __gnomefiletypes_h__

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/filetypes.h"
#include <wx/filename.h>
#include <wx/gtk/gnome/gnomevfs.h>
#include <wx/gtk/gtk.h>

class wxGnomeFileTypes : public wxFileTypesImpl
	{
	public:
		wxGnomeFileTypes();
		virtual ~wxGnomeFileTypes();

		bool							IsLoaded() const;
		wxString					GetFileType(const wxString& sFileURL) const;
		wxFileType2				LoadFileType(const wxString& sFileType) const;
		bool							SaveFileType(const wxFileType2& fti) const;
	
		wxArrayString   	GetApplications(const wxString& sFileURL) const;
		bool					  	AddApplication(const wxString& sFileURL, const wxString& sNewApplication) const;
		bool					  	RemoveApplication(const wxString& sFileURL, const wxString& sApplication) const;
		wxString					GetCommand(const wxString& sApplicationId) const;
		bool							SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const;

	private:
		wxString					GetDescription(const wxString& sFileType) const;
		wxFileName				GetIconPath(const wxString& sFileType, int nSize = -1) const;
		wxString					GetDefaultCommand(const wxString& sFileType) const;
		wxArrayString 		GetExtensions(const wxString& sFileType) const;

	private:
		wxGnomeVfs	m_gnomevfs;
		wxGtk				m_gtk;
	};

#endif
	// ndef __gnomefiletypes_h__
