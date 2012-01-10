///////////////////////////////////////////////////////////////////////////////
// Name:        ThemeHandler.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: ThemeHandler.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __ThemeHandler_h__
#define __ThemeHandler_h__

#include "ResourceManager.h"

/*

ThemeHandler virtual base class
	
*/

class ThemeHandler
	{
	// friends, classes, enums and types

	// Constructors/destructors
  public:
		virtual ~ThemeHandler()
			{};

	// Operators

	// Slots

	// Methods
  public:
		virtual bool		  LoadTheme(const wxString& sNewTheme)	= 0;
		virtual bool		  ThemeLoaded()													= 0;
		virtual wxBitmap  GetImage(ResourceId res)							= 0;

	protected:
		wxString	sTheme;
		bool			bThemeLoaded;
	};

/*
class ImageResourceHash : public std::map<int, wxString>
  {
  //public:
	//	typedef std::map<int, wxString>::iterator Iterator;
  };
*/

class FileThemeHandler : public ThemeHandler
	{
	// Constructors/destructors
  public:
		FileThemeHandler();

	// Method Implementation
  public:
		bool			LoadTheme(const wxString& sNewTheme);
		bool			ThemeLoaded();
		wxBitmap  GetImage(ResourceId res);

	private:
		std::map<int, wxString> hImageResources;
		wxString    sAppPath;
		wxString    sThemePath;
	};

#endif // ndef __ThemeHandler_h__
