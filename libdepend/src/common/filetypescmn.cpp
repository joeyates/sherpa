/////////////////////////////////////////////////////////////////////////////
// Name:        filetypescmn.cpp
// Author:      Joe Yates
// Purpose:     File type information manager interface
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/filetypes.h"

#ifdef __UNIX__
#ifdef __WXGTK__
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#endif

// Ids for all known desktop environments should be listed here
enum wxDesktopEnvironmentId
{
    // There is no window manager running
    wxDESKTOPENVIRONMENT_NONE = -1,
    // There is a window manager, but it is not recognised
    wxDESKTOPENVIRONMENT_UNKNOWN = 0, 
		// For future use
    wxDESKTOPENVIRONMENT_MACINTOSH,
		// For future use
    wxDESKTOPENVIRONMENT_WINDOWSEXPLORER,
    wxDESKTOPENVIRONMENT_GNOME,
    wxDESKTOPENVIRONMENT_KDE,
};

#define GNOME_CLASS_NAME  "Gnome-session"
#define KDE_CLASS_NAME    "Kwin"

// Recurse (depth first) into window tree looking at windows' class names.
// If we find one which is specific to a window manager,
// we return the relevant wxWindowManagerId
wxDesktopEnvironmentId GetWindowManagerFromWindow(Display * dpy, Window wnd)
{
    // See if the class info matches any string we know
    XClassHint clh;
    Status statGotClassHint = XGetClassHint(dpy, wnd, &clh);
    if ( statGotClassHint )
    {
		    wxDesktopEnvironmentId dei = wxDESKTOPENVIRONMENT_UNKNOWN;
        if ( clh.res_name != NULL && clh.res_class != NULL )
        {
            if ( strcmp(clh.res_class, GNOME_CLASS_NAME) == 0 )
                dei = wxDESKTOPENVIRONMENT_GNOME;
            else if ( strcmp(clh.res_class, KDE_CLASS_NAME) == 0 )
                dei = wxDESKTOPENVIRONMENT_KDE;
        }
        if ( clh.res_name != NULL )
            XFree(clh.res_name);
        if ( clh.res_name != NULL )
            XFree(clh.res_class);
		    if(dei != wxDESKTOPENVIRONMENT_UNKNOWN)
		      return dei;
    }

    // Check child windows
    Window wndRootReturn;
    Window wndParent;
    unsigned int unChildCount;
    Window * pwndChildren;
    Status stat = XQueryTree(dpy, wnd, &wndRootReturn, &wndParent, &pwndChildren, &unChildCount);
    if ( stat )
    {
        // Check the children
        unsigned int i;
        for (i = 0; i < unChildCount; i++)
        {
            wxDesktopEnvironmentId dsk = GetWindowManagerFromWindow(dpy, pwndChildren[i]);
            if ( dsk != wxDESKTOPENVIRONMENT_UNKNOWN )
            {
                if ( pwndChildren != NULL )
                    XFree(pwndChildren);
                return dsk;
            }
        }
        if ( pwndChildren != NULL )
            XFree(pwndChildren);
    }
    
    return wxDESKTOPENVIRONMENT_UNKNOWN;
}

// get the wxDesktopEnvironmentId of the window manager
// that is currently active
wxDesktopEnvironmentId GetDesktopEnvironment()
{
    // Get the main display
    // (avoiding dependency on wxGetDisplay())
    Display * disp = XOpenDisplay(":0");
    if ( disp == NULL )
        return wxDESKTOPENVIRONMENT_NONE;

    // Get the default window
    Window wndRoot = DefaultRootWindow(disp);
    wxDesktopEnvironmentId dsk = GetWindowManagerFromWindow(disp, wndRoot);

    XFree(disp);
    return dsk;
}

#endif
	// def __UNIX__

////////////////////////////////////////////////////////////////////////
// wxFileType2

wxFileType2::wxFileType2()
	{
	}

wxFileType2::wxFileType2(const wxString& sNewName) :
	sName(sNewName), sDescription(wxT(""))
	{
	}

bool wxFileType2::IsValid() const
	{
	return !sName.IsEmpty();
	}

bool wxFileType2::IsExecutable() const
	{
	if(!IsValid())
		{
		wxLogError(wxT("Invalid wxFileType2 object"));
		return false;
		}

	return IsExecutable(sName);
	}

wxString wxFileType2::GetName() const
	{
	return sName;
	}

void wxFileType2::SetName(const wxString& sNewName)
	{
	sName = sNewName;
	}

wxString wxFileType2::GetDescription() const
	{
	return sDescription;
	}

void wxFileType2::SetDescription(const wxString& sNewDescription)
	{
	sDescription = sNewDescription;
	}

wxIcon wxFileType2::GetIcon() const
	{
	wxFileName fil = icl.GetFileName();
	wxString s = fil.GetFullPath();
	if(s.IsEmpty())
		return wxIcon();

	return wxIcon(icl);
	}

wxFileName wxFileType2::GetIconPath(long * nIndex) const
	{
#ifdef __WXMSW__
  if(nIndex != NULL)
    *nIndex = icl.GetIndex();
#endif
	// def __WXMSW__
	return icl.GetFileName();
	}

void wxFileType2::SetIconPath(const wxFileName& sPathName, const long& nIndex)
	{
	icl.SetFileName(sPathName.GetFullPath());
#ifdef __WXMSW__
	icl.SetIndex(nIndex);
#endif
	// def __WXMSW__
	}

wxString wxFileType2::GetDefaultCommand() const
	{
	return sCommand;
	}

void wxFileType2::SetDefaultCommand(const wxString& sNewDefaultCommand)
	{
	sCommand = sNewDefaultCommand;
	}

wxArrayString wxFileType2::GetExtensions() const
	{
	return asExtensions;
	}

void wxFileType2::SetExtensions(const wxArrayString& asNewExtensions)
	{
	asExtensions = asNewExtensions;
	}

wxString wxFileType2::GetVerb(const wxString& sVerb) const
	{
  ExtraVerbs::const_iterator it = exv.find(sVerb);
	if(it == exv.end())
		return wxT("");

	return it->second;
	}

void wxFileType2::SetVerb(const wxString& sVerb, const wxString& sValue)
	{
	exv[sVerb] = sValue;
	}

void wxFileType2::DeleteVerb(const wxString& sVerb)
	{
  ExtraVerbs::iterator it = exv.find(sVerb);
	if(it == exv.end())
    return;

  exv.erase(it);
	}

/////////////////////////////////////////////////
// Static functions

bool wxFileType2::IsExecutable(const wxString& sFileType)
	{
#ifdef __UNIX__
	if(sFileType == wxT("application/x-executable"))
		return true;
#elif defined __WXMSW__
	if(sFileType == wxT("batfile"))
		return true;
	if(sFileType == wxT("cmdfile"))
		return true;
	if(sFileType == wxT("comfile"))
		return true;
	if(sFileType == wxT("exefile"))
		return true;
	if(sFileType == wxT("piffile"))
		return true;
#endif
		// def __UNIX__/__WXMSW__

	return false;
	}

////////////////////////////////////////////////////////////////////////
// wxFileTypesManager

wxFileTypesManager::wxFileTypesManager(const wxFileTypeSource& src) :
	m_pImpl(NULL)
	{
#ifdef __UNIX__
	
	wxFileTypeSource srcToLoad;
	if(src == wxFILE_TYPE_SOURCE_DEFAULT)
		{
		wxDesktopEnvironmentId dsk = GetDesktopEnvironment();
		// wxLogDebug(wxT("dsk: %u"), dsk);
		switch(dsk)
			{
			case wxDESKTOPENVIRONMENT_GNOME:
				srcToLoad = wxFILE_TYPE_SOURCE_GNOME;
				break;

      case wxDESKTOPENVIRONMENT_KDE:
				// TBI: ASSERT
				return;

      default:
				srcToLoad = wxFILE_TYPE_SOURCE_MAILCAP;
				break;
      }
		}
	else
		srcToLoad = src;

	switch(srcToLoad)
		{
		case wxFILE_TYPE_SOURCE_GNOME:
			m_pImpl = new wxGnomeFileTypes();
			break;

    default:
			wxLogError(_("Unknown file source id '%u' requested."), src);
			return;

    }

#elif defined __WXMSW__

	m_pImpl = new wxWindowsFileTypes();

#endif
	// def __UNIX__/__WXMSW__
	}

wxFileTypesManager::~wxFileTypesManager()
	{
	if(m_pImpl != NULL)
		{
		delete m_pImpl;
		m_pImpl = NULL;
		}
	}

bool wxFileTypesManager::IsLoaded() const
	{
	return (m_pImpl != NULL)? true : false;
	}

wxString wxFileTypesManager::GetFileType(const wxString& sFileURL) const
	{
	if(m_pImpl == NULL)
		return wxT("");

	return m_pImpl->GetFileType(sFileURL);
	}

wxFileType2 wxFileTypesManager::LoadFileType(const wxString& sFileType) const
	{
	if(m_pImpl == NULL)
		return wxFileType2();
	
	return m_pImpl->LoadFileType(sFileType);
	}

bool wxFileTypesManager::SaveFileType(const wxFileType2& fti) const
	{
	if(m_pImpl == NULL)
		return false;
	
	return m_pImpl->SaveFileType(fti);
	}

wxArrayString wxFileTypesManager::GetApplications(const wxString& sFileURL) const
	{
	if(m_pImpl == NULL)
		return wxArrayString();
	
	return m_pImpl->GetApplications(sFileURL);
	}

bool wxFileTypesManager::AddApplication(const wxString& sFileURL, const wxString& sNewApplication) const
	{
	if(m_pImpl == NULL)
		return false;
	
	return m_pImpl->AddApplication(sFileURL, sNewApplication);
	}

bool wxFileTypesManager::RemoveApplication(const wxString& sFileURL, const wxString& sApplication) const
	{
	if(m_pImpl == NULL)
		return false;
	
	return m_pImpl->RemoveApplication(sFileURL, sApplication);
	}

wxString wxFileTypesManager::GetCommand(const wxString& sApplicationId) const
	{
	if(m_pImpl == NULL)
		return wxT("");
	
	return m_pImpl->GetCommand(sApplicationId);
	}

bool wxFileTypesManager::SetCommand(const wxString& sApplicationId, const wxString& sNewCommand) const
	{
	if(m_pImpl == NULL)
		return false;

	return m_pImpl->SetCommand(sApplicationId, sNewCommand);
	}
