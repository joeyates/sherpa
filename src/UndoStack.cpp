///////////////////////////////////////////////////////////////////////////////
// Name:        UndoStack.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: UndoStack.cpp,v 1.1.1.1 2006/01/24 22:13:11 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"

#include "UndoStack.h"

namespace Undo
{

/////////////////////////////////
// Constructors/destructors

UndoStack::UndoStack()
	{
	}

wxString UndoStack::Dump()
	{
	int uiWaterline = m_uiWaterline;
	int nItemIndex = 0;
	wxString sDump = wxT("Undo:");
	ConstantIterator it = m_v.end();
	while(it > m_v.begin())
		{
		it--;
		nItemIndex++;
		FileChange * pfch = *it;
		sDump << wxT("[") << nItemIndex <<  wxT("]: ") << pfch->ToString() << wxT("\n");
		if(nItemIndex == m_uiWaterline)
			sDump << wxT("=====================================\nRedo:\n");
		}

	return sDump;
	}

} // namespace Undo
