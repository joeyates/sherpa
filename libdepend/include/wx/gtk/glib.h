/////////////////////////////////////////////////////////////////////////////
// Name:        glib.h
// Author:      Joe Yates
// Purpose:     GLib wrapper class
// Created:     2005-11-06
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wxGlib_h__
#define __wxGlib_h__

#include "wx/defs.h"
#include "wx/dynlib.h"
#include "wx/string.h"

class wxGlib
	{
	public:
    // constructors
		wxGlib();
		~wxGlib() {}
	
	// Methods
	public:
		// Memory Allocation
		void Free(void * pv) const;

	// Typedefs
	private:
		// Memory Allocation
		typedef void (* g_freeType)(gpointer mem);

	private:
		bool LoadGlib();

	private:
		bool 							m_bLoaded;
		wxDynamicLibrary	libglib;

		// Function pointers
		g_freeType										g_free;
	};

#endif // ndef __wxGlib_h__
