/////////////////////////////////////////////////////////////////////////////
// Name:        gnomevfs.cpp
// Author:      Joe Yates
// Purpose:     gnomevfs wrapper class
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#include <wx/gtk/gnome/gnomevfs.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayGnomeVFSMimeApplication);

///////////////////////////////////////////
// ctors and dtors

wxGnomeVFSMimeApplication::wxGnomeVFSMimeApplication()
	{
	m_gvma = NULL;
	m_sId = wxT("");
	m_sName = wxT("");
	m_sCommand = wxT("");
	m_sIconName = wxT("");
	}

wxGnomeVFSMimeApplication::wxGnomeVFSMimeApplication
	(
	GnomeVFSMimeApplication * gvma
	) :
	m_gvma(gvma)
	{
	if(gvma == NULL)
		{
		return;
		}

#if wxUSE_UNICODE
	m_sId = wxConvLibc.cMB2WC(gvma->id);
	m_sName = wxConvLibc.cMB2WC(gvma->name);
	m_sCommand = wxConvLibc.cMB2WC(gvma->command);
#else // wxUSE_UNICODE
	m_sId = gvma->id;
	m_sName = gvma->name;
	m_sCommand = gvma->command;
#endif // wxUSE_UNICODE

	m_sIconName = wxT("");
	};

wxGnomeVFSMimeApplication::wxGnomeVFSMimeApplication(const wxGnomeVFSMimeApplication& vma)
	{
	m_gvma = vma.m_gvma;
	m_sId = vma.m_sId;
	m_sName = vma.m_sName;
	m_sCommand = vma.m_sCommand;
	m_sIconName = vma.m_sIconName;
	}

///////////////////////////////////////////////////////////////////////////////
// wxGnomeVfs

wxGnomeVfs::wxGnomeVfs()
	{
	m_bLoaded = LoadGnomeVFS();
	if(!m_bLoaded)
		{
		wxPrintf(_T("LoadGnomeVFS() failed.\n"));
		return;
		}

	bool bInitialised = wxgnome_vfs_initialized();
	m_bNeedsShutdown = !bInitialised;

	bool bInitOk = wxgnome_vfs_init();
	}

wxGnomeVfs::~wxGnomeVfs()
	{
	if(!m_bNeedsShutdown)
		return;

	try
		{
		wxgnome_vfs_shutdown();
		}
	catch(...)
		{
		// Shutdown seems to segfault if called too soon
		wxLogDebug(wxT("\twxGnomeVfs::~wxGnomeVfs(): Caught exception"));
		}
	m_bNeedsShutdown = false;
	}

bool wxGnomeVfs::IsLoaded() const
	{
	return m_bLoaded;
	}

wxString wxGnomeVfs::GetMimeType(const wxString& sUrl) const
	{
	if(!m_bLoaded)
		return wxT("");

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sUrl.c_str());
	const char * szUrl = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szUrl = (const char*) sUri.c_str();
#endif // wxUSE_UNICODE

	char * szMimeType = wxgnome_vfs_get_mime_type((char *) szUrl);
	
#if wxUSE_UNICODE
	wxString sMimeType = wxConvLibc.cMB2WC(szMimeType);
#else // wxUSE_UNICODE
	wxString sMimeType = szMimeType;
#endif // wxUSE_UNICODE

	return sMimeType;
	}

wxString wxGnomeVfs::GetDescription(const wxString& sMimeType) const
	{
	if(!m_bLoaded)
		return wxT("");

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sMimeType.c_str());
	const char * szMimeType = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szMimeType = (const char*) sMimeType.c_str();
#endif // wxUSE_UNICODE

	const char * szDescription = wxgnome_vfs_mime_get_description(szMimeType);
	if(szDescription == NULL)
		return wxT("");

#if wxUSE_UNICODE
	wxString sDescription = wxConvLibc.cMB2WC(szDescription);
#else // wxUSE_UNICODE
	wxString sDescription = szDescription;
#endif // wxUSE_UNICODE

	return sDescription;
	}

bool wxGnomeVfs::SetDescription(const wxString& sMimeType, const wxString& sNewDescription) const
	{
	if(!m_bLoaded)
		return false;

	// TBI
	return false;
	}

wxString wxGnomeVfs::GetIconName(wxGnomeVFSMimeApplication& gva) const
	{
	if(!gva.IsValid())
		return wxT("");

	if(!gva.m_sIconName.IsEmpty())
		return gva.m_sIconName;
	
#if wxUSE_UNICODE
	gva.m_sIconName = wxConvLibc.cMB2WC(gnome_vfs_mime_application_get_icon(gva.m_gvma));
#else // wxUSE_UNICODE
	gva.m_sIconName = gnome_vfs_mime_application_get_icon(gva.m_gvma);
#endif // wxUSE_UNICODE
	
	return gva.m_sIconName;
	}

bool wxGnomeVfs::SetIconPath(const wxString& sMimeType, const wxFileName& fnmNewIcon) const
	{
	if(!m_bLoaded)
		return false;

	// TBI
	return false;
	}

wxGnomeVFSMimeApplication wxGnomeVfs::GetDefaultApplication(const wxString& sMimeType) const
	{
	if(!m_bLoaded)
		return wxGnomeVFSMimeApplication();

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sMimeType.c_str());
	const char * szMimeType = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szMimeType = (const char*) sMimeType.c_str();
#endif // wxUSE_UNICODE

	GnomeVFSMimeApplication * gvma = wxgnome_vfs_mime_get_default_application((char *) szMimeType);
	if(gvma == NULL)
		return wxGnomeVFSMimeApplication();

	wxGnomeVFSMimeApplication vma(gvma);

	return vma;
	}

void wxGnomeVfs::FreeGnomeVFSMimeApplication(wxGnomeVFSMimeApplication& gva) const
	{
	if(gva.m_gvma != NULL)
		{
		gnome_vfs_mime_application_free(gva.m_gvma);
		gva.m_gvma = NULL;
		}
	}

wxArrayString	wxGnomeVfs::GetExtensions(const wxString& sMimeType) const
	{
	if(!m_bLoaded)
		return wxArrayString();

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sMimeType.c_str());
	const char * szMimeType = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szMimeType = (const char*) sMimeType.c_str();
#endif // wxUSE_UNICODE

	GList * glst = wxgnome_vfs_mime_get_extensions_list(szMimeType);
	if(glst == NULL)
		return wxArrayString();

	wxArrayString asExtensions;
	GList * glstItem = glst->next;
	while(glstItem != NULL)
		{
		char * szExtension = (char *) glstItem->data;
#if wxUSE_UNICODE
		wxString sExtension = wxConvLibc.cMB2WC(szExtension);
#else // wxUSE_UNICODE
		wxString sExtension = szIconPathName;
#endif // wxUSE_UNICODE
		asExtensions.Add(sExtension);
		
		glstItem = glstItem->next;
		}
	wxgnome_vfs_mime_extensions_list_free(glst);
	
	return asExtensions;
	}

bool wxGnomeVfs::SetExtensions(const wxString& sMimeType, const wxArrayString& asNewExtensions) const
	{
	if(!m_bLoaded)
		return false;

	// TBI
	return false;
	}

// TBI: The apps should NOT be freed until later
wxArrayGnomeVFSMimeApplication wxGnomeVfs::GetShortListApplications(const wxString& sMimeType) const
	{
	if(!m_bLoaded)
		return wxArrayGnomeVFSMimeApplication();

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sMimeType.c_str());
	const char * szMimeType = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szMimeType = (const char*) sMimeType.c_str();
#endif // wxUSE_UNICODE

	GList * glst = wxgnome_vfs_mime_get_short_list_applications((char *) szMimeType);
	if(glst == NULL)
		{
		return wxArrayGnomeVFSMimeApplication();
		}

	wxArrayGnomeVFSMimeApplication avma;
	GList * glstItem = glst->next;
	while(glstItem != NULL)
		{
		GnomeVFSMimeApplication * gvma = (GnomeVFSMimeApplication *) glstItem->data;
		wxGnomeVFSMimeApplication vma(gvma);
		avma.Add(vma);

		glstItem = glstItem->next;
		}
	wxgnome_vfs_mime_application_list_free(glst);

	return avma;
	}

wxString wxGnomeVfs::GetCommand(const wxString& sApplicationId) const
	{
	if(!m_bLoaded)
		return wxT("");

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sApplicationId.c_str());
	const char * szApplicationId = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szApplicationId = (const char*) sApplicationId.c_str();
#endif // wxUSE_UNICODE

	GnomeVFSMimeApplication * gvma = wxgnome_vfs_mime_application_new_from_id(szApplicationId);
	if(gvma == NULL)
		return wxT("");

	wxGnomeVFSMimeApplication vma(gvma);
	
	return vma.GetCommand();
	}

////////////////////////////////////////////////////////////////////////////////
// Private functions

bool wxGnomeVfs::LoadGnomeVFS()
	{
	// Attempt to load the gnome library
	// Try known gnome versions, latest first
	wxArrayString asGnomeVersions;
	asGnomeVersions.Add(wxT("libgnomevfs-2.so"));
		
	int nGnomeLibCount = asGnomeVersions.size();
	wxString sLibGnomeVfs_so;
	for(int i = 0; i < nGnomeLibCount; i++)
		{
		sLibGnomeVfs_so = asGnomeVersions[i];
		m_libgnomevfs.Load(sLibGnomeVfs_so);
		if(m_libgnomevfs.IsLoaded())
			break;
		}
	if(!m_libgnomevfs.IsLoaded())
		{
		wxLogError(_("Failed to load '%s'.\n"), sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_INIT = wxT("gnome_vfs_init");
	wxgnome_vfs_init = (wxgnome_vfs_initType) m_libgnomevfs.GetSymbol(GNOME_VFS_INIT);
	if(wxgnome_vfs_init == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GNOME_VFS_INIT, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_INITIALIZED = wxT("gnome_vfs_initialized");
	wxgnome_vfs_initialized = (wxgnome_vfs_initializedType) m_libgnomevfs.GetSymbol(GNOME_VFS_INITIALIZED);
	if(wxgnome_vfs_initialized == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GNOME_VFS_INITIALIZED, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_SHUTDOWN = wxT("gnome_vfs_shutdown");
	wxgnome_vfs_shutdown = (wxgnome_vfs_shutdownType) m_libgnomevfs.GetSymbol(GNOME_VFS_SHUTDOWN);
	if(wxgnome_vfs_shutdown == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GNOME_VFS_SHUTDOWN, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_GET_MIME_TYPE = wxT("gnome_vfs_get_mime_type");
	wxgnome_vfs_get_mime_type = (wxgnome_vfs_get_mime_typeType) m_libgnomevfs.GetSymbol(GNOME_VFS_GET_MIME_TYPE);
	if(wxgnome_vfs_get_mime_type == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_GET_MIME_TYPE, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_GET_DESCRIPTION = wxT("gnome_vfs_mime_get_description");
	wxgnome_vfs_mime_get_description = (wxgnome_vfs_mime_get_descriptionType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_GET_DESCRIPTION);
	if(wxgnome_vfs_mime_get_description == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_GET_DESCRIPTION, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_GET_ICON = wxT("gnome_vfs_mime_get_icon");
	wxgnome_vfs_mime_get_icon = (wxgnome_vfs_mime_get_iconType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_GET_ICON);
	if(wxgnome_vfs_mime_get_icon == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_GET_ICON, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_GET_SHORT_LIST_APPLICATIONS = wxT("gnome_vfs_mime_get_short_list_applications");
	wxgnome_vfs_mime_get_short_list_applications = (wxgnome_vfs_mime_get_short_list_applicationsType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_GET_SHORT_LIST_APPLICATIONS);
	if(wxgnome_vfs_mime_get_short_list_applications == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_GET_SHORT_LIST_APPLICATIONS, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_GET_DEFAULT_APPLICATION = wxT("gnome_vfs_mime_get_default_application");
	wxgnome_vfs_mime_get_default_application = (wxgnome_vfs_mime_get_default_applicationType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_GET_DEFAULT_APPLICATION);
	if(wxgnome_vfs_mime_get_default_application == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_GET_DEFAULT_APPLICATION, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_APPLICATION_FREE = wxT("gnome_vfs_mime_application_free");
	gnome_vfs_mime_application_free = (gnome_vfs_mime_application_freeType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_APPLICATION_FREE);
	if(gnome_vfs_mime_application_free == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_APPLICATION_FREE, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_APPLICATION_LIST_FREE = wxT("gnome_vfs_mime_application_list_free");
	wxgnome_vfs_mime_application_list_free = (wxgnome_vfs_mime_application_list_freeType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_APPLICATION_LIST_FREE);
	if(wxgnome_vfs_mime_application_list_free == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_APPLICATION_LIST_FREE, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_GET_EXTENSIONS_LIST = wxT("gnome_vfs_mime_get_extensions_list");
	wxgnome_vfs_mime_get_extensions_list = (wxgnome_vfs_mime_get_extensions_listType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_GET_EXTENSIONS_LIST);
	if(wxgnome_vfs_mime_get_extensions_list == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_GET_EXTENSIONS_LIST, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_EXTENSIONS_LIST_FREE = wxT("gnome_vfs_mime_extensions_list_free");
	wxgnome_vfs_mime_extensions_list_free = (wxgnome_vfs_mime_extensions_list_freeType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_EXTENSIONS_LIST_FREE);
	if(wxgnome_vfs_mime_extensions_list_free == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_EXTENSIONS_LIST_FREE, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_APPLICATION_NEW_FROM_ID = wxT("gnome_vfs_mime_application_new_from_id");
	wxgnome_vfs_mime_application_new_from_id = (wxgnome_vfs_mime_application_new_from_idType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_APPLICATION_NEW_FROM_ID);
	if(wxgnome_vfs_mime_application_new_from_id == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_APPLICATION_NEW_FROM_ID, sLibGnomeVfs_so.c_str());
		return false;
		}

	const wxChar * GNOME_VFS_MIME_APPLICATION_GET_ICON = wxT("gnome_vfs_mime_application_get_icon");
	gnome_vfs_mime_application_get_icon = (gnome_vfs_mime_application_get_iconType) m_libgnomevfs.GetSymbol(GNOME_VFS_MIME_APPLICATION_GET_ICON);
	if(gnome_vfs_mime_application_get_icon == NULL)
		{
		wxPrintf(_T("ERROR: function '%s' wasn't found in '%s'.\n"), GNOME_VFS_MIME_APPLICATION_GET_ICON, sLibGnomeVfs_so.c_str());
		return false;
		}
	
	return true;
	}
