/////////////////////////////////////////////////////////////////////////////
// Name:        gnomevfs.h
// Author:      Joe Yates
// Purpose:     gnomevfs wrapper class
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxGnomeVfs_h__
#define __wxGnomeVfs_h__

#include "wx/defs.h"
#include "wx/dynlib.h"
#include "wx/string.h"
#include <wx/filename.h>
#include <wx/dynarray.h>

#include "glib-2.0/glib/gtypes.h"
#include "libgnomevfs/gnome-vfs.h"
#include "libgnomevfs/gnome-vfs-mime-utils.h"
#include "libgnomevfs/gnome-vfs-mime-handlers.h"

/*
From libgnomevfs/gnome-vfs-mime-handlers.h

typedef struct {
        char *id;
        char *name;
        char *command;
        gboolean can_open_multiple_files;
        GnomeVFSMimeApplicationArgumentType expects_uris;
        GList *supported_uri_schemes;
        gboolean requires_terminal;
        void *reserved1;
        void *reserved2;
                                                                                                                             
} GnomeVFSMimeApplication;

*/

class wxGnomeVfs;
	
class wxGnomeVFSMimeApplication
	{
	friend class wxGnomeVfs;

	public:
		// Constructor for invalid instances
		wxGnomeVFSMimeApplication();
		wxGnomeVFSMimeApplication(const wxGnomeVFSMimeApplication& vma);

	protected:
		wxGnomeVFSMimeApplication(GnomeVFSMimeApplication * gvma);

	public:
		bool IsValid()
			{ return (m_gvma != NULL)? true : false; }
		wxString GetId()
			{ return m_sId; }
		wxString GetCommand()
			{ return m_sCommand; }

	private:
		GnomeVFSMimeApplication * m_gvma;
		wxString	m_sId;
		wxString	m_sName;
		wxString	m_sCommand;
		wxString	m_sIconName;
	};

WX_DECLARE_OBJARRAY(wxGnomeVFSMimeApplication, wxArrayGnomeVFSMimeApplication);

class wxGnomeVfs
	{
	public:
    // constructors
		wxGnomeVfs();
		~wxGnomeVfs();
	
	// Methods
	public:
		bool							IsLoaded() const;
		wxString					GetMimeType(const wxString& Uri) const;
		wxString					GetDescription(const wxString& sMimeType) const;
		bool							SetDescription(const wxString& sMimeType, const wxString& sNewDescription) const;
		/** GetIconPath - DEPRECATED, use gtk function
		  *  
		  *  Retrieve the path for the icon for a given MIME type.
		  */
		wxFileName				GetIconPath(const wxString& sMimeType) const;
		bool							SetIconPath(const wxString& sMimeType, const wxFileName& fnmNewIcon) const;
		wxArrayString			GetExtensions(const wxString& sMimeType) const;
		bool							SetExtensions(const wxString& sMimeType, const wxArrayString& asNewExtensions) const;

		wxArrayGnomeVFSMimeApplication	GetShortListApplications(const wxString& sMimeType) const;
		wxString					GetCommand(const wxString& sApplicationId) const;

		wxGnomeVFSMimeApplication	GetDefaultApplication(const wxString& sMimeType) const;
		void							FreeGnomeVFSMimeApplication(wxGnomeVFSMimeApplication& gva) const;
		wxString					GetIconName(wxGnomeVFSMimeApplication& gva) const;

	// Typedefs
	private:
		typedef gboolean (* wxgnome_vfs_initType)();
		typedef gboolean (* wxgnome_vfs_initializedType)();
		typedef void (* wxgnome_vfs_shutdownType)();
		typedef char* (* wxgnome_vfs_get_mime_typeType)(char*);
		typedef const char* (* wxgnome_vfs_mime_get_descriptionType)(const char *);
		typedef const char* (* wxgnome_vfs_mime_get_iconType)(const char *);
		typedef GList * (* wxgnome_vfs_mime_get_short_list_applicationsType)(const char*);
		typedef GnomeVFSMimeApplication * (* wxgnome_vfs_mime_get_default_applicationType)(const char*);
		typedef void (* gnome_vfs_mime_application_freeType)(GnomeVFSMimeApplication *);
		typedef void (* wxgnome_vfs_mime_application_list_freeType)(GList *);
		typedef GList * (* wxgnome_vfs_mime_get_extensions_listType)(const char*);
		typedef void (* wxgnome_vfs_mime_extensions_list_freeType)(GList *);
		typedef GnomeVFSMimeApplication * (* wxgnome_vfs_mime_application_new_from_idType)(const char *);
		typedef const char* (* gnome_vfs_mime_application_get_iconType)(GnomeVFSMimeApplication *app);

	private:
		bool LoadGnomeVFS();

	private:
		wxDynamicLibrary	m_libgnomevfs;
		bool							m_bLoaded;
		bool							m_bNeedsShutdown;

		// Function pointers
		wxgnome_vfs_initType															wxgnome_vfs_init;
		wxgnome_vfs_initializedType												wxgnome_vfs_initialized;
		wxgnome_vfs_shutdownType													wxgnome_vfs_shutdown;
		wxgnome_vfs_get_mime_typeType											wxgnome_vfs_get_mime_type;
		wxgnome_vfs_mime_get_descriptionType							wxgnome_vfs_mime_get_description;
		wxgnome_vfs_mime_get_iconType											wxgnome_vfs_mime_get_icon;
		wxgnome_vfs_mime_get_short_list_applicationsType	wxgnome_vfs_mime_get_short_list_applications;
		wxgnome_vfs_mime_get_default_applicationType			wxgnome_vfs_mime_get_default_application;
		gnome_vfs_mime_application_freeType								gnome_vfs_mime_application_free;
		wxgnome_vfs_mime_application_list_freeType				wxgnome_vfs_mime_application_list_free;
		wxgnome_vfs_mime_get_extensions_listType					wxgnome_vfs_mime_get_extensions_list;
		wxgnome_vfs_mime_extensions_list_freeType					wxgnome_vfs_mime_extensions_list_free;
		wxgnome_vfs_mime_application_new_from_idType			wxgnome_vfs_mime_application_new_from_id;
		gnome_vfs_mime_application_get_iconType						gnome_vfs_mime_application_get_icon;
	};

#endif // ndef __wxGnomeVfs_h__
