///////////////////////////////////////////////////////////////////////////////
// Name:        OptionsDialog.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: OptionsDialog.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __OptionsDialog_h__
#define __OptionsDialog_h__

enum
  {
  ID_OPTIONS_CHKSHOWSHERPAFRAMEEVENTS = 2000,
  ID_OPTIONS_CHKSHOWDIRECTORYTREEVENTS,
  ID_OPTIONS_CHKSHOWDIRECTORYVIEWEVENTS,
	ID_OPTIONS_CMBTHEME
  };

class OptionsDialog : public wxDialog
  {
  // friends, classes, enums and types

  // Constructors/destructors
  public:
    OptionsDialog(wxWindow * wndParent);
  
  // Operators
  
  // Slots
  protected:
    void OnTraceMaskCheck(wxCommandEvent& evt);
  
  // Methods
	private:
		void AddPanels();

  // Members
  private:
    wxNotebook  nbk;
  
	DECLARE_EVENT_TABLE()
  };

#endif // ndef __OptionsDialog_h__
