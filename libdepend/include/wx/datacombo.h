/////////////////////////////////////////////////////////////////////////////
// Name:        wx/datacombo.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_datacombo_h__
#define __wx_datacombo_h__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

namespace wx
{

class DataCombo : public wxComboBox
  {
  // friends, classes, enums and types

  // Constructors/destructors
  public:
    DataCombo
      (
      wxWindow * parent,
      wxWindowID id,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = 0
      );

  // Operators

  // Slots
  private:

  // Methods
  public:
    long Add(const wxString& sValue, long nData);
    long GetData(long nIndex);
    bool SetSelectionFromData(long nData, long nDefaultIndex = -1);

    /**
     *  Returns the item data for the currently selected item.
     */
    bool GetSelectionData(long& nData);
    long GetIndexFromData(long nData);

    // Re-implementation - declared and left unimplemented
    // If these functions are wanted, then wxCombo should be used, not this wrapper
    void Append(const wxArrayString& strings);
    int Append(const wxString& item);
    int Append(const wxString& item, void *clientData);
    int Append(const wxString& item, wxClientData *clientData);

  // Members
  private:
  };

} // namespace wx

#endif // ndef __wx_datacombo_h__
