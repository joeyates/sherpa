/////////////////////////////////////////////////////////////////////////////
// Name:        wx/flatgridcelleditor.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __flatgridcelleditor_h__
#define __flatgridcelleditor_h__

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/grid.h>

#define EVT_FlatTextCtrl_Navigate 111 // FIXME: use a custom event

namespace wx
{

class FlatGridCellNumberEditor : public wxGridCellNumberEditor
  {
  // friends, classes, enums and types

  // Constructors/destructors
  public:
    FlatGridCellNumberEditor();
    ~FlatGridCellNumberEditor()
      {}

  // Operators

  // Slots
  private:

  // Methods
  public:
    // Implementation Overrides
    void Create
      (
      wxWindow *      parent,
      wxWindowID      id,
      wxEvtHandler *  evtHandler
      );

  // Members
  private:
  };

/**  
 *  1. We want the text edit to fit inside the grid better (can't get the size right)
 *  2. We want the right arrow key to be intercepted so we can
 *  move to the next cell when it is pressed and we are at the
 *  right of all text.
 */

class FlatTextCtrl : public wxTextCtrl
  {
  // friends, classes, enums and types

  // Constructors/destructors
  public:
    FlatTextCtrl
      (
      wxWindow *parent,
      wxWindowID id
      );
    ~FlatTextCtrl()
      {}

  // Operators

  // Slots
  private:
    void OnKeyDown(wxKeyEvent& evt);

  // Methods
  public:
    // Implementation Overrides

  // Members
  private:

  DECLARE_EVENT_TABLE()
  };

} // namespace wx

#endif // ndef __flatgridcelleditor_h__
