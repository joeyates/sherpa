///////////////////////////////////////////////////////////////////////////////
// Name:        SherpaApp.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: SherpaApp.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __SherpaApp_h__
#define __SherpaApp_h__

#define SETTING_SHERPAAPP_SHOWEVENTS_SHERPAFRAME wxT("/SherpaApp/ShowEvents/SherpaFrame")
#define SETTING_SHERPAAPP_SHOWEVENTS_DIRECTORYTREE wxT("/SherpaApp/ShowEvents/DirectoryTree")
#define SETTING_SHERPAAPP_SHOWEVENTS_DIRECTORYVIEW wxT("/SherpaApp/ShowEvents/DirectoryView")

class SherpaApp: public wxApp
  {
	// friends, classes, enums and types

	// Constructors/destructors
  public:
    SherpaApp();

	// Operators

	// Slots
  protected:
    bool OnInit();
		int OnRun();
    int OnExit();
    void OnUnhandledException();

	// Methods
  private:
    bool Initialise();
    void Terminate();

	// Variables

  DECLARE_NO_COPY_CLASS(SherpaApp)
  };

#endif // ndef __SherpaApp_h__
