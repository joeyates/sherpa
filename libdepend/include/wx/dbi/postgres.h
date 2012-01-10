/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dbi/postgres.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_dbi_postgres_h__
#define __wx_dbi_postgres_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef wxDBI_POSTGRES

#include "wx/wx.h"

#include <wx/dbi.h>
#include <postgres/libpq-fe.h>

namespace wx
{

namespace DBI
{

class Postgres : public Database
  {
  // friends, classes, enums and types
  // PgStatement nees access to the Pg connection
  friend class PgStatement;

  // Constructors/destructors
  public:
    /**
     *  ctor.
     */
    Postgres();

    /**
     *  dtor.
     */
    ~Postgres()
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
     *  Connect to a Postgres database.
     */
    void    Connect(const ConnectionInfo& cni);
    void    Disconnect();
    inline bool IsConnected()
      {
      return (conn != NULL)? true : false;
      }

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
  // PgDatabase-specific methods
  public:
    static wxString   GetPgTypeFromVariantType(const wxString& sType);
    static Oid        GetPgOidFromVariantType(const wxString& sType);

	private:
		long	GenerateStatementId();

  ////////////////////////////////
  // Members
  private:
    PGconn     *  conn;
    StatementCache stc;
  };

class PgStatement : public Statement
  {
  // friends, classes, enums and types
  private:

    /**
     *  A container class for query parameters.
     */
    class PgParameters
      {
      public:
        PgParameters(const ArrayRecord& raParameters);
        ~PgParameters();

      public:
        int GetParamCount()
          { return nParamCount; }
        const Oid * GetParamTypes()
          { return paramTypes; }
        const char * const * GetParamValues()
          { return paramValues; }
        int * GetParamLengths()
          { return paramLengths; }
        int * GetParamFormats()
          { return paramFormats; }

      private:
        int     nParamCount;
        Oid *   paramTypes;
        char ** paramValues;
        int *   paramLengths;
        int *   paramFormats;
      };

  public:
		PgStatement(Database * pdbOwner, const wxString& sNewQuery);
    ~PgStatement();

  public:
		bool  DataAvailable();
    void  ExecutePrepared(const ArrayRecord& raParameters);
    void  ExecuteTemporary(const ArrayRecord& raParameters = ArrayRecord());

    /**
     *  Execute the given SQL query on-the-fly.
     *  As there is no need to prepare or keep any state,
     *  the implementation of ExecuteTemporary() can be static.
     *  \param ppgr Optional. If the PGresult is desired, pass in a non-NULL value.
     *              Otherwise the result will be cleared up.
     */
    static void ExecuteTemporary
      (
      Postgres * pdb,
      const wxString& sQuery,
      const ArrayRecord& raParameters = ArrayRecord(),
      PGresult ** ppgr = NULL
      );

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
    /** 
     *  Call Postgres's 'PREPARE ...' on the query
     *  \param 			raParameters  The query parameters. The paramters'
     *     wxVariant types are used to prepare the query.
     *     Note, this means parameter types must remain constant
     *     between calls to the query.
     *  \exception 							  None
     */
    void        PrepareQuery(const ArrayRecord& raParameters);
    void        CleanUpResult();
    void        ThrowPgError(wxString sError = wxT("%s"));

  private:
    Postgres *  pg;
    PGresult *  pgr; // Value being non-null also indicated that the query has been prepared
    long        nCurrentRecord;

    /**
     *  The internally created query handle for the prepared query.
     *  The handle is used as a unique identifier for the query.
     *  \return The query handle.
     */
    wxString    sQueryHandle;
  };

} //namespace DBI

} //namespace wx

#endif // def wxDBI_POSTGRES

#endif // ndef __wx_dbi_postgres_h__
