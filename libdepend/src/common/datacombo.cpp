/////////////////////////////////////////////////////////////////////////////
// Name:        .cpp
// Author:      Joe Yates
// Purpose: 		
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/datacombo.h"

namespace wx
{

DataCombo::DataCombo
  (
  wxWindow * parent,
  wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style
  ) :
  wxComboBox(parent, id, wxEmptyString, pos, size, wxArrayString(), style)
  {
  }

long DataCombo::Add(const wxString& sValue, long nData)
  {
  return wxComboBox::Append(sValue, (void *) nData);
  }

long DataCombo::GetData(long nIndex)
  {
  wxASSERT((nIndex > 0) && (nIndex < GetCount()));

  return (long) GetClientData(nIndex);
  }

bool DataCombo::GetSelectionData(long& nData)
  {
  int nSelection = GetSelection();
  if(nSelection == -1)
    return false;

  nData = (long) GetClientData(nSelection);
  return true;
  }

bool DataCombo::SetSelectionFromData(long nData, long nDefaultIndex)
  {
  long nIndex = GetIndexFromData(nData);
  if(nIndex == -1)
    nIndex = nDefaultIndex;
  if(nIndex == -1)
    return false;

  SetSelection(nIndex);

  return true;
  }

long DataCombo::GetIndexFromData(long nData)
  {
  for(long i = 0; i < GetCount(); i++)
    {
    long n = (long) GetClientData(i);
    if(n == nData)
      return i;
    }

  return -1;
  }

} // namespace wx
