/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dbi/sqlite.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_dbi_sqlite_h__
#define __wx_dbi_sqlite_h__

#ifdef wxDBI_SQLITE

#include <wx/dbi.h>
// Interface to SQLite
#include <sqlite3.h>

namespace wx
{

namespace DBI
{

// Map unicode names (and non-) onto common names
#if wxUSE_UNICODE
	#define SQLite3_open(a, b)							sqlite3_open16(a, b)
	#define SQLite3_prepare(a, b, c, d, e)	sqlite3_prepare16(a, b, c, d, e)
	#define SQLite3_errmsg(a)								sqlite3_errmsg16(a)
	
	#define SQLiteString										void *	
	#define wxStringBytes(x)								(2 * x.Length())
	#define SQLiteStringTowxString(x)				cnv.cMB2WC((const char *) x)
// 	#define wxStringToSQLiteString(x)				cnv.cWC2MB(x.c_str()).data()
#else // wxUSE_UNICODE
	#define SQLite3_open(a, b)							sqlite3_open(a, b)
	#define SQLite3_prepare(a, b, c, d, e)	sqlite3_prepare(a, b, c, d, e)
	#define SQLite3_errmsg(a)								sqlite3_errmsg(a)
	
// 	#define SQLiteStringTowxString(x)				x
	#define wxStringBytes(x)								(x.Length())
#endif // wxUSE_UNICODE

// Redefine the rest, just to keep the names in line
#define SQLite3_close(a)									sqlite3_close(a)
#define SQLite3_finalize(a)								sqlite3_finalize(a)

#define SQLITE_DATABASE_PATHNAME wxT("PathName")
#define SQLITE_CREATEIFMISSING wxT("CreateIfMissing")

class SQLite : public Database
  {
  // friends, classes, enums and types
	friend class SQLiteStatement;

  // Constructors/destructors
  public:
    /**
     *  ctor.
     */
    SQLite();

    /**
     *  dtor.
     *  Closes the connection if there is one.
     */
    ~SQLite()
      {
			Disconnect();
			}

  ////////////////////////////////
  // 'Database' Implementation

  // Operators

  // Methods
  public:
    // Connection

    /**
     *  Connect to a SQLite database.
		 *
     *  Normal connection parameters are:
     *  - PathName
     *    The file path name of the database.
     *  - CreateIfMissing
     *    A flag indicating whther the database should be created if
		 *		the file is missing ("1"), or if an exception should be thrown ("0").
     */
    virtual void      Connect(const ConnectionInfo& cni);
    virtual void      Disconnect();
    bool							IsConnected();

    // Transactions
    void Begin();
    void Rollback();
    void Commit();

    // Queries
    Statement *       Prepare(wxString const& sQuery);

    // Database info
    wxArrayString     GetDatabases();
    bool              DatabaseExists(const wxString& sDatabaseName);
    wxArrayString     GetTables();
    bool              TableExists(const wxString& sTableName);

  ////////////////////////////////
  // SQLite-specific methods
  public:

  ////////////////////////////////
  // Members
  private:
		sqlite3 * 				plite;
		wxString					sDatabasePathName;
  };

class SQLiteStatement : public Statement
  {
  // friends, classes, enums and types

  public:
		SQLiteStatement(Database * pdbOwner, const wxString& sNewQuery);
    ~SQLiteStatement();

  public:
		bool  DataAvailable();

    /**
     *  Exceute the query.
     *  Identical to ExecuteTemporary().
     */
    void  ExecutePrepared(const ArrayRecord& raParameters = ArrayRecord());

    /**
     *  Exceute the query.
     *  Identical to ExecutePrepared().
     */
    void  ExecuteTemporary(const ArrayRecord& raParameters = ArrayRecord());
		long	GetLastInsertId();

  protected:
    int   GetFieldCount();
    void  MoveNext();
    void  GetFieldValue
      (
      int             nIndex,
      wxVariant&      v
      );
    wxString GetFieldName
      (
      int         nIndex
      );

  private:
		void	Execute(const ArrayRecord& raParameters);

  private:
		sqlite3_stmt * 	pls;
		int							nStepReturn;
  };

} //namespace DBI

} //namespace wx

#endif // def wxDBI_SQLITE

#endif // ndef __wx_dbi_sqlite_h__

