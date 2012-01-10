///////////////////////////////////////////////////////////////////////////////
// Name:        UndoStack.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: UndoStack.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __UndoStack_h__
#define __UndoStack_h__

#include <Undo.h>

#define shFILECHANGE_MOVE				wxT("FileMove")
#define shFILECHANGE_TOWASTEBIN	wxT("FileToWastebin")
#define shFILECHANGE_RESTORE		wxT("FileRestore")
#define shFILECHANGE_CREATE			wxT("FileCreate")

namespace Undo
{

/**  
 *  Base class for File modification items for the Undo stack.
 */

class FileChange
	{
	// friends, classes, enums and types

	// Constructors/destructors
  public:
		FileChange(wxString const& sType, wxString const& sPathName) :
			m_sType(sType),
			m_sPathName(sPathName)
			{}
		virtual ~FileChange()
			{}

	public:
		wxString GetType()
			{
			return m_sType;
			}
		wxString GetPathName()
			{
			return m_sPathName;
			}
		virtual wxString ToString() = 0;

	protected:
		wxString m_sType;
		wxString m_sPathName;
	};

class FileMove : public FileChange
	{
	// Constructors/destructors
  public:
		FileMove(wxString const& sOldPathName, wxString const& sNewPathName) :
			FileChange(shFILECHANGE_MOVE, sOldPathName),
			m_sNewPathName(sNewPathName)
			{}
		virtual ~FileMove()
			{}

	public:
		wxString GetNewPathName()
			{
			return m_sNewPathName;
			}
		virtual wxString ToString()
			{
			return GetType() << wxT(": ") << GetPathName() << wxT(" -> ") << m_sNewPathName;
			}

	public:
		wxString m_sNewPathName;
	};

class FileToWastebin : public FileChange
	{
	// Constructors/destructors
  public:
		FileToWastebin(wxString const& sPathName, int nWastebinId) :
			FileChange(shFILECHANGE_TOWASTEBIN, sPathName),
			m_nWastebinId(nWastebinId)
			{}
		virtual ~FileToWastebin()
			{}

  public:
		virtual wxString ToString()
			{
			return GetType() << wxT(": ") << GetPathName() << wxT(" sent to wastebin");
			}
		int GetWastebinId()
			{ return m_nWastebinId; }

	public:
		void SetPathName(const wxString& sPathName)
			{ m_sPathName = sPathName; }
		void SetWastebinId(int nWastebinId)
			{ m_nWastebinId = m_nWastebinId; }

	private:
		int m_nWastebinId; // Allows us to restore
	};

class FileRestore : public FileChange
	{
	// Constructors/destructors
  public:
		FileRestore(wxString const& sPathName) :
			FileChange(shFILECHANGE_RESTORE, sPathName)
			{}
		virtual ~FileRestore()
			{}

  public:
		virtual wxString ToString()
			{
			return GetType() << wxT(": ") << GetPathName();
			}
	};

class FileCreate : public FileChange
	{
	// Constructors/destructors
  public:
		FileCreate(wxString const& sPathName) :
			FileChange(shFILECHANGE_CREATE, sPathName)
			{}
		virtual ~FileCreate()
			{}

  public:
		virtual wxString ToString()
			{
			return GetType() << wxT(": ") << GetPathName();
			}
	};

class UndoStack : public Undo::DeletingStack<FileChange *>
  {
  // friends, classes, enums and types
  // Constructors/destructors
  public:
    UndoStack();
  // Operators
  // Slots
  // Methods
  public:
		wxString Dump();
  // Members
  };

} // namespace Undo


#endif // ndef __UndoStack_h__
