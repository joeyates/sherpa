///////////////////////////////////////////////////////////////////////////////
// Name:        Wastebin.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: Wastebin.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __Wastebin_h__
#define __Wastebin_h__

#define wxDBI_SQLITE
#include <wx/dbi.h>

namespace Waste
{

class File
	{
  // friends, classes, enums and types
  // Constructors/destructors
	public:
		File
			(
			const wxString& sPathName,
			const wxString& sFileType
			);

	protected:
		File
			(
			const wxString& sPathName,
			time_t					nAccessTime,
			time_t					nModificationTime,
			time_t					nCreationTime,
#ifdef __UNIX__
			mode_t					nPermissions,
#endif // def __UNIX__
			const wxString&	sFileType,
			bool						bIsFile
			);

  // Operators
  // Methods
	public:
		wxString	GetPathName() const;
		time_t		GetAccessTime() const;
		time_t		GetModificationTime() const;
		time_t		GetCreationTime() const;
#ifdef __UNIX__
		mode_t    GetPermissions() const;
#endif // def __UNIX__
		wxString	GetFileType() const;
		bool			GetIsFile() const;
		virtual void Dump() const;

  // Members
	protected:
		wxString	m_sPathName;
		wxString	m_sFileType;
		bool			m_bIsFile;
		time_t		m_nAccessTime;
		time_t		m_nModificationTime;
		time_t		m_nCreationTime;
#ifdef __UNIX__
		mode_t    m_nPermissions;
#endif // def __UNIX__
	};

/** WastebinFile

 *  Represents a file in the wastebin.
 */
class WastebinFile : public File
	{
  // friends, classes, enums and types
	friend class Bin; // Only the Bin can create us

  // Constructors/destructors
	protected:
		WastebinFile
			(
			int nItemId,
			const wxString& sPathName,
			time_t nDeletionTime,
			time_t nAccessTime,
			time_t nModificationTime,
			time_t nCreationTime,
#ifdef __UNIX__
			mode_t nPermissions,
#endif // def __UNIX__
			const wxString& sFileType,
			bool bIsFile
			);

  // Methods
	public:
		int			GetItemId() const;
		time_t	GetDeletionTime() const;
		virtual void Dump() const;

  // Members
	private:
		int			m_nItemId;
		time_t	m_nDeletionTime;
	};

/** RestoreFile

 *  Represents a file to be restored.
 */
class RestoreFile : public WastebinFile
	{
  // friends, classes, enums and types
	friend class Bin; // Only the Bin can create us

  // Constructors/destructors
	protected:
		RestoreFile
			(
			int								nItemId,
			const wxString&		sPathName,
			time_t						nDeletionTime,
			time_t						nAccessTime,
			time_t						nModificationTime,
			time_t						nCreationTime,
#ifdef __UNIX__
			mode_t						nPermissions,
#endif // def __UNIX__
			const wxString&		sFileType,
			bool bIsFile,
			const wxString&		sNameInWastebin
			);

  // Methods
	public:
		bool		Restore();
		virtual void Dump() const;

  // Members
	private:
		wxString						m_sNameInWastebin;
	};

class Bin : private wx::DBI::SQLite
  {
  // friends, classes, enums and types
	public:
		typedef bool (* EnumerateWastebinCallbackType)(const WastebinFile&);

  // Constructors/destructors
  public:
    Bin(const wxString& sWastebinPath);

  // Operators
  // Methods
	public:
		bool 	Empty();
		int		GetCount();
		void	Enumerate(const EnumerateWastebinCallbackType fnCallback);
		virtual void Dump();

		int		Throw(File& dli);
		void	Delete(int nItemId);
		/** 
		  *	Restore a file from the Wastebin to the file system.  
		  *  
		  *  \param nItemId The item index in the Wastebin.
		  *  \param sPathName The full path and name of the restored file
		  *  \exception 
		  *  \return 
			*
			* Note that the restored name may not be the original name.
			*	This happens if there is a file with the same name in the destination folder.
		  */
		bool	Restore(int nItemId, wxString& sPathName);

	private:
		void	OpenDatabase(const wxString& sWastebinDatabasePathName);

  // Members
	private:
		wxString					m_sWastebinPath;
		wxString					m_sWastebinFilesPath;
		wx::DBI::StatementCache m_stc;
  };

} // namespace Waste

#endif // ndef __Wastebin_h__
