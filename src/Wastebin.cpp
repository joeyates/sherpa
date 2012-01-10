///////////////////////////////////////////////////////////////////////////////
// Name:        Wastebin.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: Wastebin.cpp,v 1.1.1.1 2006/01/24 22:13:09 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"

#include "Wastebin.h"

#include <wx/file.h>
#include <wx/filename.h>
#include <memory>

#include <wx/fileutils.h>

namespace Waste
{

bool WastebinDumpCallback(const Waste::WastebinFile& del)
	{
	del.Dump();
	wxLogDebug(wxT("\n"));
	}

////////////////////////////////////////////////////////////////////////////////////////////
// File

File::File
	(
	const wxString& sPathName,
	const wxString& sFileType
	) :
	m_sPathName(sPathName),
	m_sFileType(sFileType)
	{
	m_bIsFile = wxFileName::FileExists(m_sPathName);
	bool bIsDirectory = wxFileName::DirExists(m_sPathName);
	if(!m_bIsFile && !bIsDirectory)
		throw wx::Exception(wxT("Waste::File::File() File '%s' does not exist"), m_sPathName.c_str());
	
	// TBI: Ensure it's not a special file

	wxFileName fnm(m_sPathName);
	wxDateTime dtAccess;
	wxDateTime dtMod;
	wxDateTime dtCreate;
	bool bGotTimes = fnm.GetTimes(&dtAccess, &dtMod, &dtCreate);
	if(!bGotTimes)
		throw wx::Exception(wxT("Waste::File::File() GetTimes() failed on file '%s'"), m_sPathName.c_str());

	m_nAccessTime = dtAccess.GetTicks();
	m_nModificationTime = dtMod.GetTicks();
	m_nCreationTime = dtCreate.GetTicks();

#ifdef __UNIX__
  struct stat st;
  stat(m_sPathName.fn_str(), &st);
	m_nPermissions = st.st_mode; // TBI: Windows version
#endif // def __UNIX__
	}

File::File
	(
	const wxString& sPathName,
	time_t nAccessTime,
	time_t nModificationTime,
	time_t nCreationTime,
#ifdef __UNIX__
  mode_t nPermissions,
#endif // def __UNIX__
	const wxString& sFileType,
	bool bIsFile
	) :
	m_sPathName(sPathName),
	m_nAccessTime(nAccessTime),
	m_nModificationTime(nModificationTime),
	m_nCreationTime(nCreationTime),
#ifdef __UNIX__
	m_nPermissions(nPermissions),
#endif // def __UNIX__
	m_sFileType(sFileType),
	m_bIsFile(bIsFile)
	{
	}

wxString File::GetPathName() const
	{
	return m_sPathName;
	}

time_t File::GetAccessTime() const
	{
	return m_nAccessTime;
	}

time_t File::GetModificationTime() const
	{
	return m_nModificationTime;
	}

time_t File::GetCreationTime() const
	{
	return m_nCreationTime;
	}

#ifdef __UNIX__
mode_t File::GetPermissions() const
	{
	return m_nPermissions;
	}
#endif // def __UNIX__

wxString File::GetFileType() const
	{
	return m_sFileType;
	}

bool File::GetIsFile() const
	{
	return m_bIsFile;
	}
 
void File::Dump() const
	{
	wxLogDebug(wxT("PathName: '%s'"), m_sPathName.c_str());
	wxLogDebug(wxT("AccessTime: %u"), m_nAccessTime);
	wxLogDebug(wxT("ModificationTime: %u"), m_nModificationTime);
	wxLogDebug(wxT("CreationTime: %u"), m_nCreationTime);
#ifdef __UNIX__
	wxLogDebug(wxT("Permissions: 0%03o"), m_nPermissions & 0x1ff);
#endif // def __UNIX__
	wxLogDebug(wxT("FileType: '%s'"), m_sFileType.c_str());
	wxLogDebug(wxT("Is File: %u"), m_bIsFile);
	}

////////////////////////////////////////////////////////////////////////////////////////////
// WastebinFile

WastebinFile::WastebinFile
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
	) :
	File
    (
    sPathName,
    nAccessTime,
    nModificationTime,
    nCreationTime,
#ifdef __UNIX__
    nPermissions,
#endif // def __UNIX__
    sFileType,
		bIsFile
    ),
	m_nItemId(nItemId),
	m_nDeletionTime(nDeletionTime)
	{
	}

time_t WastebinFile::GetDeletionTime() const
	{
	return m_nDeletionTime;
	}

void WastebinFile::Dump() const
	{
	wxLogDebug(wxT("ItemId: %u"), m_nItemId);
	wxLogDebug(wxT("DeletionTime: %u"), m_nDeletionTime);
	File::Dump();
	}

////////////////////////////////////////////////////////////////////////////////////////////
// RestorableFile

RestoreFile::RestoreFile
	(
	int nItemId,
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
	) :
	WastebinFile
		(
		nItemId,
    sPathName,
		nDeletionTime,
    nAccessTime,
    nModificationTime,
    nCreationTime,
#ifdef __UNIX__
    nPermissions,
#endif // def __UNIX__
    sFileType,
		bIsFile
		),
	m_sNameInWastebin(sNameInWastebin)
	{
	}

bool RestoreFile::Restore()
	{
	// Check if there is a file with the same name at the 'destination'
	if(PathNameExists(m_sPathName))
		{
		wxString sNewDestinationPathName = GetUniquePathName(m_sPathName);
		m_sPathName = sNewDestinationPathName;
		}
	// Copy from wastebin to original location
	wxRenameFile(m_sNameInWastebin, m_sPathName);

  // Set file times to what they were
  wxFileName fnm(m_sPathName);
  wxDateTime dtmAccessTime(m_nAccessTime);
  wxDateTime dtmModificationTime(m_nModificationTime);
  wxDateTime dtmCreationTime(m_nCreationTime);
  fnm.SetTimes(&dtmAccessTime, &dtmModificationTime, &dtmCreationTime);
	
	return true;
	}

void RestoreFile::Dump() const
	{
	wxLogDebug(wxT("m_sNameInWastebin: '%s'"), m_sNameInWastebin.c_str());
	WastebinFile::Dump();
	}

////////////////////////////////////////////////////////////////////////////////////////////
// Bin

#if _MSC_VER > 1000
# pragma warning (disable : 4355) // 'this' : used in base member initializer list
#endif // _MSC_VER > 1000

Bin::Bin(const wxString& sWastebinPath) :
	m_sWastebinPath(sWastebinPath),
	m_stc(this)
	{
	m_sWastebinFilesPath = m_sWastebinPath + wxFileName::GetPathSeparator() + wxT(".Bin");

	// Create subdirectory for files
	if(!wxDirExists(m_sWastebinFilesPath))
		CreatePath(m_sWastebinFilesPath, 0700);

	wxString sWastebinDatabasePathName = m_sWastebinPath + wxFileName::GetPathSeparator() + wxT(".wastebin");

	OpenDatabase(sWastebinDatabasePathName);
	}

bool Bin::Empty()
	{
	if(!IsConnected())
		throw wx::Exception(wxT("Waste::Bin::Empty(), Database not open"));
		
	m_stc.CacheExecute(wxT("Waste::Bin::Empty"), wxT("DELETE FROM Wastebin;"));
	DeleteFiles(m_sWastebinFilesPath);
	
  return true;
	}

int Bin::GetCount()
	{
	if(!IsConnected())
		throw wx::Exception(wxT("Waste::Bin::GetCount(), Database not open"));
		
	wxString sqryGetCount = wxT("SELECT COUNT(WastebinItemId) FROM Wastebin;");
	wxVariant v;
	m_stc.CacheExecute(wxT("Waste::Bin::GetCount"), sqryGetCount, wx::DBI::ArrayRecord(), v);
	
	return v.GetLong();
	}

// N.B. This fn does NOT return the 'NameInWastebin' field
// as it is internal to the implementation
void Bin::Enumerate(const EnumerateWastebinCallbackType fnCallback)
	{
	if(!IsConnected())
		throw wx::Exception(wxT("Waste::Bin::Enumerate() Database not open"));
		
	if(fnCallback == 0)
		throw wx::Exception(wxT("Waste::Bin::Enumerate() Invalid 'fnCallback' parameter"));
		
	wxString sqryGetInfo = wxT(" 		\
SELECT 														\
    WastebinItemId, 							\
    PathName,											\
    DeleteTime,										\
    AccessTime,										\
    ModificationTime,							\
    CreationTime,									\
    Permissions,									\
    FileType, 										\
    IsFile 												\
FROM 															\
    Wastebin;											\
");
	wx::DBI::HashRecordArray hra;
	m_stc.CacheExecute(wxT("Waste::Bin::EnumerateAll"), sqryGetInfo, wx::DBI::ArrayRecord(), hra);
	for(wx::DBI::HashRecordArray::iterator it = hra.begin(); it != hra.end(); it++)
		{
		int nItemId = it->GetLong(wxT("WastebinItemId"));
		wxString sPathName = it->GetString(wxT("PathName"));
		time_t nDeletionTime = (time_t) it->GetLong(wxT("DeleteTime")); // TBI use long long
		time_t nAccessTime = (time_t) it->GetLong(wxT("AccessTime"));
		time_t nModificationTime = (time_t) it->GetLong(wxT("ModificationTime"));
		time_t nCreationTime = (time_t) it->GetLong(wxT("CreationTime"));
#ifdef __UNIX__
		mode_t nPermissions = (int) it->GetLong(wxT("Permissions"));
#endif // def __UNIX__
		wxString sFileType = it->GetString(wxT("FileType"));
		bool bIsFile = (bool) it->GetLong(wxT("IsFile"));

		WastebinFile del
      (
      nItemId,
      sPathName,
      nDeletionTime,
      nAccessTime,
      nModificationTime,
      nCreationTime,
#ifdef __UNIX__
      nPermissions,
#endif // def __UNIX__
      sFileType,
			bIsFile
      );
		fnCallback(del);
		}
	}

void Bin::Dump()
	{
	Enumerate(WastebinDumpCallback);
	}

// TBI: compress in, decompress out
int Bin::Throw(File& fil)
	{
	if(!IsConnected())
		throw wx::Exception(wxT("Waste::Bin::Throw(), Database not open"));
	
	wxString sPathName = fil.GetPathName();
	wxString sFileType = fil.GetFileType();
	bool bIsFile = fil.GetIsFile();

	// Make a unique name to give the file in the wastebin
	wxString sPath;
	wxString sBaseName;
	wxString sExtension;
	wxFileName::SplitPath(sPathName, &sPath, &sBaseName, &sExtension);
	if(sExtension != wxT(""))
		sExtension = wxT(".") + sExtension;
	wxString sDefaultNameInWastebin = m_sWastebinFilesPath + wxFileName::GetPathSeparator() + sBaseName + sExtension;

	wxString sNameInWastebin = GetUniquePathName(sDefaultNameInWastebin);
	// Move the file to the wastebin directory
	wxRenameFile(sPathName, sNameInWastebin);

	wx::DBI::ArrayRecord raInsert;
	raInsert.push_back(wxVariant(sPathName));
	raInsert.push_back(wxVariant((long) time(NULL)));
	raInsert.push_back(wxVariant(fil.GetAccessTime()));
	raInsert.push_back(wxVariant(fil.GetModificationTime()));
	raInsert.push_back(wxVariant(fil.GetCreationTime()));
#ifdef __UNIX__
	raInsert.push_back(wxVariant((long) fil.GetPermissions()));
#endif // def __UNIX__
	raInsert.push_back(wxVariant(sFileType));
	raInsert.push_back(wxVariant((long) fil.GetIsFile()));
	raInsert.push_back(wxVariant(sNameInWastebin));

	wxString sqryInsert = wxT("			\
INSERT INTO Wastebin 							\
	( 															\
	PathName,												\
	DeleteTime,											\
	AccessTime,											\
	ModificationTime,								\
	CreationTime,										\
	Permissions,										\
	FileType,												\
	IsFile,													\
	NameInWastebin 									\
	) 															\
VALUES 														\
	( 															\
	?, ?, ?, ?, ?, ?, ?, ?, ?				\
	);															\
");
	long nLastInsert = 0;
	try
		{
		m_stc.CacheExecuteInsert(wxT("Waste::Bin::Insert"), sqryInsert, raInsert, nLastInsert);
		}
	catch(wx::Exception exc)
		{
		throw wx::Exception(wxT("Bin::Throw() Failed to insert item in db. Error: %s"), exc.GetString().c_str());
		}

	return nLastInsert;
	}

void Bin::Delete(int nItemId)
	{
	if(!IsConnected())
		throw wx::Exception(wxT("Waste::Bin::Delete() Database not open"));

	// Throw away the copy in the wastebin
	wxString sqryGetNameInWastebin = wxT("SELECT NameInWastebin FROM Wastebin WHERE WastebinItemId = ?;");
	wx::DBI::ArrayRecord ar;
	ar.push_back(wxVariant((long) nItemId));

	wx::DBI::HashRecord hr;
	try
		{
		m_stc.CacheExecute(wxT("Waste::Bin::GetNameInWastebin"), sqryGetNameInWastebin, ar, hr);
		}
	catch(wx::Exception exc)
		{
		// The record does not exist
		return;
		}

	wxString sNameInWastebin = hr.GetString(wxT("NameInWastebin"));
	if(wxFileExists(sNameInWastebin))
		wxRemoveFile(sNameInWastebin);

	wxString sqryDelete = wxT("DELETE FROM Wastebin WHERE WastebinItemId = ?;");
	m_stc.CacheExecute(wxT("Waste::Bin::Delete"), sqryDelete, ar);
	}

bool Bin::Restore(int nItemId, wxString& sPathName)
	{
	if(!IsConnected())
		throw wx::Exception(wxT("Waste::Bin::Restore(): Database not open"));

	wx::DBI::ArrayRecord ra;
	ra.push_back(wxVariant((long) nItemId));
		
	wxString sqryGetAllInfo = wxT("SELECT * FROM Wastebin WHERE WastebinItemId = ?;");
	
	wx::DBI::HashRecord hr;
	try
		{
		m_stc.CacheExecute(wxT("Waste::Bin::GetAllInfo"), sqryGetAllInfo, ra, hr);
		}
	catch(wx::Exception exc)
		{
		throw wx::Exception(wxT("Waste::Bin::Restore() Item %u does not exist"), nItemId);
		}
	
	wxString sOriginalPathName = hr.GetString(wxT("PathName"));
	time_t nDeleteTime = (time_t) hr.GetLong(wxT("DeleteTime"));
	time_t nAccessTime = (time_t) hr.GetLong(wxT("AccessTime"));
	time_t nModificationTime = (time_t) hr.GetLong(wxT("ModificationTime"));
	time_t nCreationTime = (time_t) hr.GetLong(wxT("CreationTime"));
#ifdef __UNIX__
	mode_t nPermissions = (mode_t) hr.GetLong(wxT("Permissions"));
#endif // def __UNIX__
	wxString sFileType = hr.GetString(wxT("FileType"));
	bool bIsFile = hr.GetLong(wxT("IsFile"));
	wxString sNameInWastebin = hr.GetString(wxT("NameInWastebin"));

	RestoreFile rsf
		(
		nItemId,
		sOriginalPathName,
		nDeleteTime,
		nAccessTime,
		nModificationTime,
		nCreationTime,
#ifdef __UNIX__
		nPermissions,
#endif // def __UNIX__
		sFileType,
		bIsFile,
		sNameInWastebin
		);

	bool bRestored = rsf.Restore();
	Delete(nItemId);
	
	sPathName = rsf.GetPathName();

	return bRestored;
	}

void Bin::OpenDatabase(const wxString& sWastebinDatabasePathName)
	{
	if(IsConnected())
		Disconnect();

	bool bExists = wxFileExists(sWastebinDatabasePathName);
	bool bCreate = !bExists;

	wx::DBI::Database::ConnectionInfo cni;
	cni.Set(SQLITE_DATABASE_PATHNAME, sWastebinDatabasePathName);
	cni.Set(SQLITE_CREATEIFMISSING, (bCreate)? wxT("1") : wxT("0") );
	try
		{
		Connect(cni);
		}
	catch(wx::Exception exc)
		{
		throw wx::Exception(wxT("Bin::OpenDatabase() Failed to open wastebin database. Error: %s"), exc.GetString().c_str());
		}

	if(bCreate)
		{
		// Create the wastebin table
		wxString sqryCreateWastebinTable = wxT("		\
CREATE TABLE Wastebin 													\
	( 																						\
	WastebinItemId INTEGER PRIMARY KEY, 					\
	PathName TEXT, 																\
	DeleteTime INTEGER, 													\
	AccessTime INTEGER, 													\
	ModificationTime INTEGER, 										\
	CreationTime INTEGER, 												\
	Permissions INTEGER, 													\
	FileType TEXT, 																\
	IsFile INTEGER, 															\
	NameInWastebin TEXT 													\
	);																						\
");
		wxLogDebug(wxT("sqryCreateWastebinTable: '%s'"), sqryCreateWastebinTable.c_str());
		m_stc.ExecuteTemporary(sqryCreateWastebinTable, wx::DBI::ArrayRecord());
		}
	}

} // namespace Waste
