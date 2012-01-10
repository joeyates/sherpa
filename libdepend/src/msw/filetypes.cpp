/////////////////////////////////////////////////////////////////////////////
// Name:        msw/filetypes.cpp
// Author:      Joe Yates
// Purpose:     File types implementation for MSW
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __WXMSW__

#pragma warning(disable: 4786)

#include "wx/msw/mswfiletypes.h"
#include "wx/msw/registry.h"

wxWindowsFileTypes::wxWindowsFileTypes() :
  cls(NULL),
  ext(NULL)
  {
  }

wxString wxWindowsFileTypes::GetTypeName() const
	{
	return wxT("class wxWindowsFileTypes");
	}

bool wxWindowsFileTypes::IsLoaded() const
  {
  return true;
  }

wxString wxWindowsFileTypes::GetFileType(const wxString& sFileURL) const
  {
  wxString sExtension = sFileURL.AfterLast('.');
  return GetClassFromExtension(sExtension);
  }

// Return the "Open with" list associated with the extension
wxArrayString wxWindowsFileTypes::GetApplications(const wxString& sFileURL) const
  {
  wxString sExtension = sFileURL.AfterLast('.');
  if(sExtension.IsEmpty())
    return wxArrayString();

  wxString sDottedExtension = wxT(".") + sExtension;
  wxRegKey keyFileExts(wxRegKey::HKCU, wxT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts"));

  // Find a matching extension (case insensitively)
  long nIndex = 0;
  wxString sExtensionKeyName = wxT("");
  bool bGotExtension = keyFileExts.GetFirstKey(sExtensionKeyName, nIndex);
  while(bGotExtension && sDottedExtension.CmpNoCase(sExtensionKeyName) != 0)
    bGotExtension = keyFileExts.GetNextKey(sExtensionKeyName, nIndex);

  if(sDottedExtension.CmpNoCase(sExtensionKeyName) != 0)
    return wxArrayString();

  wxString sOpenWithListKey = sExtensionKeyName + wxT("\\OpenWithList");
  wxRegKey keyOpenWithList(keyFileExts, sOpenWithListKey);
  if(!keyOpenWithList.Exists())
		return wxArrayString();

  // Get the 'Most Recently Used' list
  if(!keyOpenWithList.HasValue(wxT("MRUList")))
    return wxArrayString();
  wxString sMRUList;
  keyOpenWithList.QueryValue(wxT("MRUList"), sMRUList);

  wxArrayString asApplications;
  long nMRUListLength = sMRUList.Length();

  for(int i = 0; i < nMRUListLength; i++)
    {
    wxString sItem = sMRUList.Mid(i, 1);

    if(!keyOpenWithList.HasValue(sItem))
      return wxArrayString();

    wxString sApplication;
    bool bGotApp = keyOpenWithList.QueryValue(sItem, sApplication);
    if(bGotApp)
      asApplications.Add(sApplication);
    }

  return asApplications;
  }

bool wxWindowsFileTypes::AddApplication(const wxString& sFileURL, const wxString& sNewApplicationId) const
  {
  // TBI
  return false;
  }

bool wxWindowsFileTypes::RemoveApplication(const wxString& sFileURL, const wxString& sApplicationId) const
  {
  // TBI
  return false;
  }

wxString wxWindowsFileTypes::GetCommand(const wxString& sApplicationId) const
  {
  // TBI
  return wxT("");
  }

bool wxWindowsFileTypes::SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const
  {
  // TBI
  return false;
  }

wxFileType2 wxWindowsFileTypes::LoadFileType(const wxString& sFileType) const
  {
	if(sFileType.IsEmpty())
		{
		wxLogError(_T("No File type supplied."));
		return wxFileType2();
		}

  if(cls == NULL)
    {
    wxWindowsFileTypes * self = wxConstCast(this, wxWindowsFileTypes);
    self->LoadCaches();
    }

  // See if the class exists.
  // Can't use caches as a class may exist even if it has no associated
	// extension(s)
  wxRegKey keyClass(wxRegKey::HKCR, sFileType);
  if(!keyClass.Exists())
		return wxFileType2();

 	wxFileType2 fti(sFileType);

  if(keyClass.HasValue(NULL))
    {
    wxString sDescription = keyClass.QueryDefaultValue();
	  fti.SetDescription(sDescription);
    }

  // Icons on Windows are a pain as they are often
  // indexed items in files (exes, dlls, etc)
  wxRegKey keyIcon(keyClass, wxT("DefaultIcon"));
  if(keyIcon.Exists())
    {
    if(keyIcon.HasValue(NULL))
      {
      wxString sIconPath = keyIcon.QueryDefaultValue();
      if(!sIconPath.IsEmpty())
        {
        wxString sIndex = sIconPath.AfterLast(',');

        long nIconIndex = 0;
        bool bIsNumber = sIndex.ToLong((signed long *) &nIconIndex);
        if(sIndex.IsEmpty() || !bIsNumber)
  	      fti.SetIconPath(sIconPath);
        else
          {
          wxString sStrippedIconPath = sIconPath.BeforeLast(',');
  	      fti.SetIconPath(sStrippedIconPath, nIconIndex);
          }
        }
      }
    }

  wxRegKey keyCommand(keyClass, wxT("shell\\open\\command"));
  if(keyCommand.Exists())
    {
    if(keyCommand.HasValue(NULL))
      {
      wxString sCommand = keyCommand.QueryDefaultValue();
      if(!sCommand.IsEmpty())
        {
        fti.SetDefaultCommand(sCommand);
        }
      }
    }

  ExtensionCache::iterator eci = ext->find(sFileType);
  if(eci != ext->end())
    {
    wxArrayString asExtensions = *(eci->second);
    fti.SetExtensions(asExtensions);
    }

  return fti;
  }

bool wxWindowsFileTypes::SaveFileType(const wxFileType2& fti) const
  {
  // TBI
  return false;
  }

wxString wxWindowsFileTypes::GetClassFromExtension(const wxString& sExtension) const
  {
  if(sExtension.IsEmpty())
    return wxT("");

  if(cls == NULL)
    {
    wxWindowsFileTypes * self = wxConstCast(this, wxWindowsFileTypes);
    self->LoadCaches();
    }

  ClassCache::iterator cci = cls->find(sExtension);
	if(cci != cls->end())
    return cci->second;

  // FIXME: Should really try a case insensitive match on all items
  // Try upper- and lower-case versions
  wxString sCopy = sExtension;
  wxString sUPPER = sCopy.MakeUpper();
  cci = cls->find(sUPPER);
	if(cci != cls->end())
    return cci->second;

  wxString slower = sCopy.MakeLower();
  cci = cls->find(slower);
	if(cci != cls->end())
    return cci->second;

  return wxT("");
  }

void wxWindowsFileTypes::LoadCaches()
  {
  cls = new ClassCache();
  ext = new ExtensionCache();

  // Scan through all '.XXXXX' entries under HKCR in the registry
  wxRegKey keyClasses(wxRegKey::HKCR, wxT(""));
  long nIndex = 0;
  wxString sClassKeyName = wxT("");
  bool bGotExtension = keyClasses.GetFirstKey(sClassKeyName, nIndex);

  // Find the first extension
  while(bGotExtension && sClassKeyName[0] != '.')
    bGotExtension = keyClasses.GetNextKey(sClassKeyName, nIndex);

	// Loop through all extensions
	wxString sDottedExtension = sClassKeyName;
  while(bGotExtension && sDottedExtension[0] == '.')
    {
    wxRegKey keyExtension(wxRegKey::HKCR, sDottedExtension);
		// If the extension's key has a default value, that value tells us the
		// class
    if(keyExtension.HasValue(NULL))
      {
      wxString sClass = keyExtension.QueryDefaultValue();

      wxString sExtension = sDottedExtension.Mid(1);
			if(!sExtension.IsEmpty())
				{
				// Cache the mapping Extension -> Class
				(*cls)[sExtension] = sClass;
	
				// Get the array of extensions related to the class,
				// or create it if necessary

				wxArrayString * pas;
				ExtensionCache::iterator eci = ext->find(sClass);
				if(eci == ext->end())
					{
					pas = new wxArrayString();
					(*ext)[sClass] = pas;
					}
				else
					pas = eci->second;
	
				pas->Add(sExtension);
				}
      }
    bGotExtension = keyClasses.GetNextKey(sDottedExtension, nIndex);
    }
  }

void wxWindowsFileTypes::UnloadCaches()
  {
  if(ext != NULL)
    {
    ExtensionCache::iterator en = ext->end();
    for(ExtensionCache::iterator it = ext->begin(); it != en; ++it)
      delete it->second;
    ext->clear();
    wxDELETE(ext);
    }

  if(cls != NULL)
    {
    cls->clear();
    wxDELETE(cls);
    }
  }

#endif // def __WXMSW__
