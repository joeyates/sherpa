/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dbi/sqlite.cpp
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef wxDBI_SQLITE

#include <wx/dbi/sqlite.h>

namespace wx
{

namespace DBI
{

#if wxUSE_UNICODE
wxMBConvUTF16 cnv;
#endif // wxUSE_UNICODE

SQLite::SQLite() :
	plite(NULL)
	{
	}

void SQLite::Connect(const ConnectionInfo& cni)
  {
	Disconnect();

  if(!cni.IsSet(SQLITE_DATABASE_PATHNAME))
    throw wx::Exception(wxT("SQLite::Connect() No database file path name supplied."));

	wxString sNewDatabasePathName = cni.Get(SQLITE_DATABASE_PATHNAME);
	wxString sCreateIfMissing = cni.Get(SQLITE_CREATEIFMISSING);
	long nCreateIfMissing = 0;
	sCreateIfMissing.ToLong(&nCreateIfMissing);
	bool bCreateIfMissing = (nCreateIfMissing != 0)? true : false;
	if(!wxFileExists(sNewDatabasePathName))
		{
		if(!bCreateIfMissing)
			throw Exception(_T("SQLite::Open(), database does not exist"));

		// Db will be created...
		}

	wxCharBuffer chb = cnv.cWC2MB(sNewDatabasePathName.c_str());
	const SQLiteString sql = (const SQLiteString) chb.data();

	int nOpenResult = SQLite3_open(sql, &plite);

	if(nOpenResult != SQLITE_OK)
		{
		wxString sError = SQLiteStringTowxString(SQLite3_errmsg(plite)); // TBI: deos this leak memory?
		plite = 0;
		throw Exception(_T("SQLite::Open(), Error opening SQLite database: %s"), sError.c_str());
		}

  m_cni = cni;
	sDatabasePathName = sNewDatabasePathName;
  }

void SQLite::Disconnect()
  {
	if(plite != NULL)
		{
		SQLite3_close(plite);
		plite = 0;
		}
	sDatabasePathName = _T("");
  }

bool SQLite::IsConnected()
  {
	return (plite != NULL)? true : false;
  }

////////////////
// Transactions

void SQLite::Begin()
  {
  SQLiteStatement stmt(this, wxT("BEGIN;"));
  stmt.ExecuteTemporary();
  }

void SQLite::Rollback()
  {
  SQLiteStatement stmt(this, wxT("ROLLBACK;"));
  stmt.ExecuteTemporary();
  }

void SQLite::Commit()
  {
  SQLiteStatement stmt(this, wxT("COMMIT;"));
  stmt.ExecuteTemporary();
  }

////////////////
// Queries

Statement * SQLite::Prepare(const wxString& sQuery)
	{
  if(!IsConnected())
    throw wx::Exception(wxT("SQLite::Prepare() Database not connected."));

  return new SQLiteStatement(this, sQuery);
  }

////////////////
// Database info

wxArrayString SQLite::GetDatabases()
  {
  if(!IsConnected())
    throw wx::Exception(wxT("SQLite::GetDatabases() Database not connected."));

  // TBI

  return wxArrayString();
  }

bool SQLite::DatabaseExists(const wxString& sDatabaseName)
  {
  if(!IsConnected())
    throw wx::Exception(wxT("SQLite::DatabaseExists() Database not connected."));

  // TBI
  return false;
  }

wxArrayString SQLite::GetTables()
  {
  if(!IsConnected())
    throw wx::Exception(wxT("SQLite::GetTables() Database not connected."));


  // TBI
  return wxArrayString();
  }

bool SQLite::TableExists(const wxString& sTableName)
  {
  if(!IsConnected())
    throw wx::Exception(wxT("SQLite::TableExists() Database not connected."));

  // TBI
  return false;
  }

///////////////////////////////////////////////////////////////////////////////////////
// SQLiteStatement

SQLiteStatement::SQLiteStatement
  (
  Database * pdbOwner,
  const wxString& sNewQuery
  ) :
  Statement(pdbOwner, sNewQuery)
  {
	wxCharBuffer chb = cnv.cWC2MB(sQuery.c_str());
	const SQLiteString sql = (const SQLiteString) chb.data();
	int nBytes = wxStringBytes(sQuery);
	const SQLiteString sqlEnd = 0;

  //wx::DBI::SQLite * pdbLite = dynamic_cast<wx::DBI::SQLite *>(pdb);
  wx::DBI::SQLite * pdbLite = (wx::DBI::SQLite *) pdb;
  sqlite3 * plite = pdbLite->plite;
	int nPrepareResult = SQLite3_prepare(plite, sql, nBytes, &pls, &sqlEnd);

	if(nPrepareResult != SQLITE_OK)
		{
		wxString sError = SQLiteStringTowxString(SQLite3_errmsg(plite));
		pls = NULL;
    wxString sThrowError = wxString::Format
      (
		  wxT("wxSQLite3::Statement::Statement() Error preparing SQL statement '%s': %s"),
      sQuery.c_str(),
      sError.c_str()
      );
		throw Exception(sThrowError);
		}

	if(pls == 0)
		throw Exception(_T("wxSQLite3::Statement::Statement(): Error preparing SQL statement, no statement handle returned"));
  }

SQLiteStatement::~SQLiteStatement()
  {
	if(pls != 0)
		{
		SQLite3_finalize(pls);
		pls = 0;
		}
  }

bool SQLiteStatement::DataAvailable()
  {
	return (nStepReturn == SQLITE_ROW)? true : false;
  }

void SQLiteStatement::ExecutePrepared(const ArrayRecord& raParameters)
  {
	Execute(raParameters);
  }

void SQLiteStatement::ExecuteTemporary(const ArrayRecord& raParameters)
  {
	Execute(raParameters);
  }

long SQLiteStatement::GetLastInsertId()
	{
  //wx::DBI::SQLite * pdbLite = dynamic_cast<SQLite *>(pdb);
  wx::DBI::SQLite * pdbLite = (wx::DBI::SQLite *) pdb;
  sqlite3 * plite = pdbLite->plite;
	return sqlite3_last_insert_rowid(plite);
	}

int SQLiteStatement::GetFieldCount()
  {
  //wx::DBI::SQLite * pdbLite = dynamic_cast<SQLite *>(pdb);
  /*
  wx::DBI::SQLite * pdbLite = (wx::DBI::SQLite *) pdb;
  sqlite3 * plite = pdbLite->plite;
  */
	return sqlite3_column_count(pls);
  }

void SQLiteStatement::MoveNext()
  {
	if(pls == 0)
		throw Exception(_T("SQLiteStatement::MoveNext(): Invalid statement"));
	
	nStepReturn = sqlite3_step(pls);
  }

void SQLiteStatement::GetFieldValue
  (
  int         nIndex,
  wxVariant&  v
  )
  {
	if(pls == 0)
		throw Exception(_T("SQLiteStatement::GetFieldValue(): Invalid statement"));
	
	int nType = sqlite3_column_type(pls, nIndex);
	switch(nType)
		{
		case SQLITE_INTEGER:
			v = (long) sqlite3_column_int(pls, nIndex);
			break;

    case SQLITE_FLOAT:
			v = sqlite3_column_double(pls, nIndex);
			break;

    case SQLITE3_TEXT:
			{
			wxString s = SQLiteStringTowxString(sqlite3_column_text16(pls, nIndex));
			v = s;
			break;
			}
#if WX_DBI_USE_BLOB
		case SQLITE_BLOB:
			{
			int nBlobLength = sqlite3_column_bytes(pls, nIndex);
			const void * pvc = sqlite3_column_blob(pls, nIndex);
			BLOB * pblb = new BLOB(pvc, nBlobLength);  // Should be deleted by container
			// Next line uses wxVariant() and copy-ctor,
			// as the wxVariant '=' operator overload  for wxObject * does not exist,
			// so the pointer is incorrectly taken as 'void *'
			v = wxVariant((wxObject *) pblb);
			break;
			}
#endif // WX_DBI_USE_BLOB
		case SQLITE_NULL:
			v.MakeNull();
			break;

    default:
			throw wx::Exception(_T("Unknown SQLite data type %u"), nType);
		}
  }

wxString SQLiteStatement::GetFieldName(int nIndex)
  {
	if(pls == 0)
		throw Exception(_T("SQLiteStatement::GetFieldName(): Invalid statement"));
	
	wxString sFieldName = SQLiteStringTowxString(sqlite3_column_name16(pls, nIndex));
	return sFieldName;
  }

void SQLiteStatement::Execute(const ArrayRecord& raParameters)
	{
	if(pls == 0)
		throw Exception(_T("SQLiteStatement::Execute(): Invalid statement"));
	
	nStepReturn = SQLITE_ERROR;
	sqlite3_reset(pls); // N.B. Does NOT clear previously bound values

	int nCount = raParameters.size();
	for(int i = 0; i < nCount; i++)
		{
		// N.B. Wildcard indexes are 1-based
		int nParameter = i + 1;
		wxString sType = raParameters[i].GetType();
		if(sType == _T("long"))
			{
			sqlite3_bind_int(pls, nParameter, raParameters.GetLong(i));
			}
#if WX_DBI_USE_BLOB
		else if(sType == _T("wxObject"))
			{
			BLOB * pblb = raParameters.GetBLOB(i);
			sqlite3_bind_blob(pls, nParameter, pblb->GetPtr(), pblb->GetLength(), SQLITE_TRANSIENT);
			pblb->Release();
			}
#endif // WX_DBI_USE_BLOB
		else if(sType == _T("string"))
			{
			wxString s = raParameters.GetString(i);
			wxCharBuffer chb = cnv.cWC2MB(s.c_str());
			int nBytes = wxStringBytes(s);
			const SQLiteString sql = (const SQLiteString) chb.data();
			sqlite3_bind_text16(pls, nParameter, sql, nBytes, SQLITE_TRANSIENT);
			}
		else // TBI: double, NULL
      {
      wxString sThrowError = wxString::Format
        (
        wxT("wxSQLite3::Statement::Execute(): wxVariant type '%s' not implemented"),
        sType.c_str()
        );
      throw Exception(sThrowError);
      }
		}
	
	// Step to the first record
	nStepReturn = sqlite3_step(pls);
	}

} // namespace DBI

} // namespace wx

#endif // def wxDBI_SQLITE
