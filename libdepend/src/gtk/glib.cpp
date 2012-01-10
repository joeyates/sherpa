/////////////////////////////////////////////////////////////////////////////
// Name:        glib.cpp
// Author:      Joe Yates
// Purpose:     GLib wrapper class
// Created:     2005-11-06
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#include <wx/gtk/glib.h>

wxGlib::wxGlib()
	{
	m_bLoaded = LoadGlib();
	}

////////////////////////////////////////////////////////////////////////////////
// Public methods

// Memory Allocation
void wxGlib::Free(void * pv) const
	{
	g_free(pv);
	}

////////////////////////////////////////////////////////////////////////////////
// Private functions

bool wxGlib::LoadGlib()
	{
	wxArrayString asVersions;
	asVersions.Add(wxT("libglib-1.2.so.0"));
		
	int nLibCount = asVersions.size();
	wxString sLibName;
	for(int i = 0; i < nLibCount; i++)
		{
		sLibName = asVersions[i];
		libglib.Load(sLibName);
		if(libglib.IsLoaded())
			break;
		}
	if(!libglib.IsLoaded())
		{
		wxLogError(_("Failed to load '%s'.\n"), sLibName.c_str());
		return false;
		}

	const wxChar * G_FREE = wxT("g_free");
	g_free = (g_freeType) libglib.GetSymbol(G_FREE);
	if(g_free == NULL)
		{
		wxLogError(_("Function '%s' wasn't found in '%s'.\n"), G_FREE, sLibName.c_str());
		return false;
		}
	}
