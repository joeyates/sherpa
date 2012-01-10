/////////////////////////////////////////////////////////////////////////////
// Name:        .cpp
// Author:      Joe Yates
// Purpose: 		
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include "wx/flatgridcelleditor.h"

namespace wx
{

FlatGridCellNumberEditor::FlatGridCellNumberEditor() :
  wxGridCellNumberEditor()
  {
  }

void FlatGridCellNumberEditor::Create
  (
  wxWindow *      parent,
  wxWindowID      id,
  wxEvtHandler *  evtHandler
  )
  {
  m_control = new FlatTextCtrl
    (
    parent,
    id
    );

  wxGridCellEditor::Create(parent, id, evtHandler);
  }

FlatTextCtrl::FlatTextCtrl
  (
  wxWindow *parent,
  wxWindowID id
  ) :
  wxTextCtrl
    (
    parent,
    id,
    wxEmptyString,
    wxDefaultPosition,
    wxDefaultSize,
    wxSIMPLE_BORDER | wxTE_RIGHT
    )
  {
  }

BEGIN_EVENT_TABLE(FlatTextCtrl, wxTextCtrl)
//  EVT_SIZE(                         FlatTextCtrl::OnSize)
  EVT_KEY_DOWN(                    FlatTextCtrl::OnKeyDown)
END_EVENT_TABLE()

void FlatTextCtrl::OnKeyDown(wxKeyEvent& evt)
  {
  long nKeycode = evt.GetKeyCode();
  if(nKeycode == WXK_RIGHT)
    {
    // Se il punto di inserimento e' alla destra del testo,
    // spostiamo l'inserimento alla cella a destra
    long nFrom = 0;
    long nTo = 0;
    GetSelection(&nFrom, &nTo);
    int nLength = GetLineLength(0);
    if(nTo == nLength)
      {
	    wxWindow * wndParent = GetParent();
	    if(wndParent == 0)
		    return;
      wxCommandEvent evtFire(wxEVT_COMMAND_MENU_SELECTED, EVT_FlatTextCtrl_MoveNextCell);
      wxPostEvent(wndParent, evtFire);
      }
    }

  evt.Skip();
  }

} // namespace wx
