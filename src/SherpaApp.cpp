///////////////////////////////////////////////////////////////////////////////
// Name:        SherpaApp.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: SherpaApp.cpp,v 1.1.1.1 2006/01/24 22:13:11 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"

#include "SherpaApp.h"
#include "SherpaFrame.h"
#include "ResourceManager.h"

Persona           	psn;
ResourceManager *		res = NULL;

/////////////////////////////////
// Constructors/destructors

IMPLEMENT_APP(SherpaApp)

SherpaApp::SherpaApp()
  {
  }

/////////////////////////////////
// Operators

/////////////////////////////////
// Slots

bool SherpaApp::OnInit()
  {
  bool bOK = Initialise();
  if(!bOK)
    {
    Terminate();
    return false;
    }

  SherpaFrame * fra = new SherpaFrame(APP_NAME);
  fra->Show();
  SetTopWindow(fra);
	
  return true;
  }

int SherpaApp::OnRun()
	{
  int nReturn = -1;
	try
		{
		nReturn = wxApp::OnRun();
		}
	catch(const wx::Exception& e)
		{
		wxLogDebug(wxT("SherpaApp::OnRun() Caught exception: %s"), e.GetString().c_str());
		}
	catch(...)
		{
		wxLogDebug(_T("SherpaApp::OnRun() Caught exception"));
		}

  return nReturn;
	}

int SherpaApp::OnExit()
  {
  Terminate();

  return wxApp::OnExit();
  }

void SherpaApp::OnUnhandledException()
  {
  wxLogDebug(wxT("SherpaApp::OnUnhandledException()"));
  }

/////////////////////////////////
// Public Methods

/////////////////////////////////
// Private Methods

bool SherpaApp::Initialise()
  {
	wxInitAllImageHandlers();
  wxLog * log = new wxLogStderr(0);
  wxLog::SetActiveTarget(log);

	wxString sHome = wxGetHomeDir();
  wxString sConfigPathName = wxString::Format
    (
	  wxT("%s/%s/%s"),
    sHome.c_str(),
    SETTINGS_DIRECTORY,
    CONFIG_FILE_NAME
    );
  wxConfig * cfg = new wxConfig(APP_NAME, SHERPA_PROJECT, sConfigPathName);
  wxConfig::Set(cfg);
  try
    {
    res = new ResourceManager();
    }
  catch(Exception& e)
    {
    wxMessageBox(e.GetString());
    return false;
    }
  
	// We only want idle events sent to windows that ask for them
	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);

  bool bShowSherpaFrameEvents = false;
  cfg->Read(SETTING_SHERPAAPP_SHOWEVENTS_SHERPAFRAME, &bShowSherpaFrameEvents);
  if(bShowSherpaFrameEvents)
    wxLog::AddTraceMask(SHERPAFRAME_EVENTS);
  bool bShowDirectoryTreeEvents = false;
  cfg->Read(SETTING_SHERPAAPP_SHOWEVENTS_DIRECTORYTREE, &bShowDirectoryTreeEvents);
  if(bShowDirectoryTreeEvents)
    wxLog::AddTraceMask(DIRECTORYTREE_EVENTS);
  bool bShowDirectoryViewEvents = false;
  cfg->Read(SETTING_SHERPAAPP_SHOWEVENTS_DIRECTORYVIEW, &bShowDirectoryViewEvents);
  if(bShowDirectoryViewEvents)
    wxLog::AddTraceMask(DIRECTORYVIEW_EVENTS);

  return true;
  }

void SherpaApp::Terminate()
  {
	try
		{
		wxConfig * cfgRemoved = (wxConfig *) wxConfig::Set(NULL);
    if(cfgRemoved != NULL)
      {
		  bool bShowSherpaFrameEvents = wxLog::IsAllowedTraceMask(SHERPAFRAME_EVENTS);
		  cfgRemoved->Write(SETTING_SHERPAAPP_SHOWEVENTS_SHERPAFRAME, bShowSherpaFrameEvents);
		  bool bShowDirectoryTreeEvents = wxLog::IsAllowedTraceMask(DIRECTORYTREE_EVENTS);
		  cfgRemoved->Write(SETTING_SHERPAAPP_SHOWEVENTS_DIRECTORYTREE, bShowDirectoryTreeEvents);
		  bool bShowDirectoryViewEvents = wxLog::IsAllowedTraceMask(DIRECTORYVIEW_EVENTS);
		  cfgRemoved->Write(SETTING_SHERPAAPP_SHOWEVENTS_DIRECTORYVIEW, bShowDirectoryViewEvents);
      }
		wxDELETE(cfgRemoved);
	
		wxLog * log = wxLog::SetActiveTarget(NULL);
		wxDELETE(log);
		wxDELETE(res);
		}
	catch(...)
		{
		wxLogDebug(_T("SherpaApp::Terminate() Caught exception"));
		}
  }
