/////////////////////////////////////////////////////////////////////////////
// Name:        gtk.cpp
// Author:      Joe Yates
// Purpose:     Gtk Library wrapper class
// Created:     2005-11-06
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#include <wx/gtk/gtk.h>

wxGtk::wxGtk()
	{
	m_bLoaded = LoadGtk();
	
	// TBI: Use the real command line
	int argc = 0;
	char ** argv = (char **) malloc(sizeof(char *));
	argv[0] = 0;
	gtk_init(&argc, &argv);
	free(argv);
	}

////////////////////////////////////////////////////////////////////////////////
// Public methods


/*

Currently (GTK+ 2.6.7), there is a problem with icon names:
gnome_vfs_mime_application_get_icon returns some ison names WITH the extension
(e.g. 'redhat-web-browser.png'), while the 'gtk_icon_theme_*'
functions expect the name WITHOUT the extension.

*/
std::vector<long> wxGtk::GetIconSizes(const wxString& sIconName) const
	{
	wxString sIconNameNoExtension = sIconName;
	sIconNameNoExtension.Replace(wxT(".png"), wxT(""));

	std::vector<long> al;
	
	GtkIconTheme * pgit = gtk_icon_theme_get_default();
	if(pgit == NULL)
		{
		wxLogDebug(wxT("wxGtk::GetIconSizes(): Failed to get default theme"));
		return al;
		}

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sIconNameNoExtension.c_str());
	const char * szIconName = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szIconName = (const char*) sIconNameNoExtension.c_str();
#endif // wxUSE_UNICODE

	gint * pnSizes = gtk_icon_theme_get_icon_sizes(pgit, szIconName);
	if(pgit == NULL)
		{
		wxLogDebug(wxT("wxGtk::GetIconSizes(): gtk_icon_theme_get_icon_sizes(pgit, '%s') returned NULL"), sIconName.c_str());
		return al;
		}

	int i = 0;
	while(pnSizes[i] != 0)
		{
		al.push_back(pnSizes[i]);
		i++;
		}

	libglib.Free(pnSizes);

	return al;
	}

/*
See above note
*/
wxString wxGtk::GetIconPath(const wxString& sIconName, int nSize) const
	{
	wxString sIconNameNoExtension = sIconName;
	sIconNameNoExtension.Replace(wxT(".png"), wxT(""));

	GtkIconTheme * pgit = gtk_icon_theme_get_default();
	if(pgit == NULL)
		{
		wxLogDebug(wxT("wxGtk::GetIconPath(): Failed to get default theme"));
		return wxT("");
		}

#if wxUSE_UNICODE
	const wxWX2MBbuf mbBuffer = wxConvCurrent->cWX2MB(sIconNameNoExtension.c_str());
	const char * szIconName = (const char*) mbBuffer;
#else // wxUSE_UNICODE
	const char * szIconName = (const char*) sIconNameNoExtension.c_str();
#endif // wxUSE_UNICODE

	GtkIconInfo * pgii = gtk_icon_theme_lookup_icon(pgit, szIconName, nSize, (GtkIconLookupFlags) GTK_ICON_LOOKUP_USE_BUILTIN);
	if(pgii == NULL)
		{
		wxLogDebug(wxT("wxGtk::GetIconPath(): gtk_icon_theme_lookup_icon(pgit, '%s', %u, 0) returned NULL"), sIconName.c_str(), nSize);
		return wxT("");
		}

	const char * szPathName = gtk_icon_info_get_filename(pgii);
	if(szPathName == NULL)
		{
		wxLogDebug(wxT("gtk_icon_info_get_filename() returned NULL"));
		return wxT("");
		}
	
	// Convert BEFORE freeing
#if wxUSE_UNICODE
	wxString sPathName = wxConvLibc.cMB2WC(szPathName);
#else // wxUSE_UNICODE
	wxString sPathName = szPathName;
#endif // wxUSE_UNICODE

	gtk_icon_info_free(pgii);

	return sPathName;
	}

////////////////////////////////////////////////////////////////////////////////
// Private functions

bool wxGtk::LoadGtk()
	{
	wxArrayString asVersions;
	asVersions.Add(wxT("libgtk-x11-2.0.so"));
		
	int nLibCount = asVersions.size();
	wxString sLibName;
	for(int i = 0; i < nLibCount; i++)
		{
		sLibName = asVersions[i];
		libgtkx11.Load(sLibName);
		if(libgtkx11.IsLoaded())
			break;
		}
	if(!libgtkx11.IsLoaded())
		{
		wxLogError(_("Failed to load '%s'.\n"), sLibName.c_str());
		return false;
		}

	const wxChar * GTK_INIT = wxT("gtk_init");
	gtk_init = (gtk_initType) libgtkx11.GetSymbol(GTK_INIT);
	if(gtk_init == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GTK_INIT, sLibName.c_str());
		return false;
		}

	const wxChar * GTK_ICON_THEME_GET_DEFAULT = wxT("gtk_icon_theme_get_default");
	gtk_icon_theme_get_default = (gtk_icon_theme_get_defaultType) libgtkx11.GetSymbol(GTK_ICON_THEME_GET_DEFAULT);
	if(gtk_icon_theme_get_default == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GTK_ICON_THEME_GET_DEFAULT, sLibName.c_str());
		return false;
		}

	const wxChar * GTK_ICON_THEME_LOOKUP_ICON = wxT("gtk_icon_theme_lookup_icon");
	gtk_icon_theme_lookup_icon = (gtk_icon_theme_lookup_iconType) libgtkx11.GetSymbol(GTK_ICON_THEME_LOOKUP_ICON);
	if(gtk_icon_theme_lookup_icon == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GTK_ICON_THEME_LOOKUP_ICON, sLibName.c_str());
		return false;
		}

	const wxChar * GTK_ICON_INFO_GET_FILENAME = wxT("gtk_icon_info_get_filename");
	gtk_icon_info_get_filename = (gtk_icon_info_get_filenameType) libgtkx11.GetSymbol(GTK_ICON_INFO_GET_FILENAME);
	if(gtk_icon_info_get_filename == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GTK_ICON_INFO_GET_FILENAME, sLibName.c_str());
		return false;
		}

	const wxChar * GTK_ICON_INFO_FREE = wxT("gtk_icon_info_free");
	gtk_icon_info_free = (gtk_icon_info_freeType) libgtkx11.GetSymbol(GTK_ICON_INFO_FREE);
	if(gtk_icon_info_free == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GTK_ICON_INFO_FREE, sLibName.c_str());
		return false;
		}

	const wxChar * GTK_ICON_THEME_GET_ICON_SIZES = wxT("gtk_icon_theme_get_icon_sizes");
	gtk_icon_theme_get_icon_sizes = (gtk_icon_theme_get_icon_sizesType) libgtkx11.GetSymbol(GTK_ICON_THEME_GET_ICON_SIZES);
	if(gtk_icon_theme_get_icon_sizes == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), GTK_ICON_THEME_GET_ICON_SIZES, sLibName.c_str());
		return false;
		}
	}
