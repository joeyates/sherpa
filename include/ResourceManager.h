///////////////////////////////////////////////////////////////////////////////
// Name:        ResourceManager.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: ResourceManager.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __ResourceManager_h__
#define __ResourceManager_h__

class ThemeHandler;

enum ResourceId
	{
	// DirectoryView items (Keep this list at start - must match entries in ResourceManager's image list)
  resUnknown = 0,
  resCharacter,
  resBlock,
  resFIFO,
  resLink,
  resFolderNormal,
  resFolderHidden,
  resFolderNormalDropTarget,
  resFolderHiddenDropTarget,
  resFileNormal,
  resFileHidden,
	// DirectoryTree items
	resButtonPlus,
	resButtonMinus,
	// Other
	resApplication,
	};

/*

The ResourceManager's wxImageList holds images for the DirectoryView.
The images are:
- a number of standard images,
- images loaded according to file types.
	
*/

class ResourceManager : public wxImageList, public wxFileTypesManager
  {
	// friends, classes, enums and types
	private:
		typedef std::map<wxString, int> TypeIndexMap;

	// Constructors/destructors
  public:
    ResourceManager();
		~ResourceManager();

	// Operators

	// Slots

	// Methods
  public:
    // Methods
		wxBitmap 				GetImage(ResourceId res);
    int             GetDirectoryImageIndex(bool bHidden, bool bDropTarget);
    int             GetFileImageIndex(bool bHidden);
    bool            Activate(const wxString& sPathName);
  
    wxString				GetDescription(const wxString& sFileType);
    int							GetImageIndex(const wxString& sFileType, bool bHidden = false);
    int							GetLinkImageIndex(const wxString& sFileType, bool bHidden = false);
    bool     				IsBinaryExecutable(const wxString& sFileType);

	private:
		wxBitmap 			  GetImageForFileType(const wxString& sFileType);
		int							AddImage(const wxString& sFileType, const wxBitmap& bmp);

	private:
		int							nFolderIndexBase;
		int							nFileIndexBase;
		ThemeHandler *	thm; // thm is a pointer to a ThemeHandler subclass
		TypeIndexMap		hImageIndex;
  };

#endif // ndef __ResourceManager_h__
