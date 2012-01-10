/////////////////////////////////////////////////////////////////////////////
// Name:        gtk.h
// Author:      Joe Yates
// Purpose:     Gtk Library wrapper class
// Created:     2005-11-06
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxGtk_h__
#define __wxGtk_h__

#include "wx/defs.h"
#include "wx/dynlib.h"
#include "wx/string.h"

#include <vector>
#include <wx/gtk/glib.h>
#include <gtk/gtkicontheme.h>

/*
GtkIconInfo * gtk_icon_theme_lookup_icon
	(
	GtkIconTheme* icon_theme,
	const gchar* icon_name,
	gint size,
	GtkIconLookupFlags flags
	);
*/

class wxGtk
	{
	public:
    // constructors
		wxGtk();
		~wxGtk() {}
	
	// Methods
	public:
		std::vector<long> GetIconSizes(const wxString& sIconName) const;
		wxString GetIconPath(const wxString& sIconName, int nSize) const;

	// Typedefs
	private:
		// Main Event Loop
		typedef void (* gtk_initType)(int * argc, char *** argv);

		typedef GtkIconTheme * (* gtk_icon_theme_get_defaultType)();
		typedef GtkIconInfo * (* gtk_icon_theme_lookup_iconType)(GtkIconTheme * icon_theme, const gchar * icon_name, gint size, GtkIconLookupFlags flags);
		typedef void (* gtk_icon_info_freeType)(GtkIconInfo * icon_info);
		typedef const gchar * (* gtk_icon_info_get_filenameType)(GtkIconInfo * icon_info);
		typedef gint * (* gtk_icon_theme_get_icon_sizesType)(GtkIconTheme * icon_theme, const gchar * icon_name);

	private:
		bool LoadGtk();

	private:
		bool 							m_bLoaded;
		wxGlib						libglib;
		wxDynamicLibrary	libgtkx11;

		// Function pointers
		gtk_initType										gtk_init;
		gtk_icon_theme_get_defaultType	gtk_icon_theme_get_default;
		gtk_icon_theme_lookup_iconType	gtk_icon_theme_lookup_icon;
		gtk_icon_info_freeType					gtk_icon_info_free;
		gtk_icon_info_get_filenameType	gtk_icon_info_get_filename;
		gtk_icon_theme_get_icon_sizesType	gtk_icon_theme_get_icon_sizes;
	};

#endif // ndef __wxGtk_h__
