/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dbi/postgres.cpp
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef wxDBI_POSTGRES

#include <wx/dbi/postgres.h>

#ifdef _MSC_VER
#pragma warning (disable : 4355) // 'this' : used in base member initializer list
#endif // def _MSC_VER

namespace wx
{

namespace DBI
{

/*

To do:

  Handle db encoding. Currently relies on db being LATIN1

*/

long _nStatementId = 0;

// Values from src/include/catalog/pg_type.h in postgres source
// Also available in any db: pg_catalog/Types

// boolean, 'true'/'false'
#define PG_OID_BOOLEAN        16
// 63-character type for storing system identifiers
#define PG_OID_NAME           19
// integer, 4-byte storage
#define PG_OID_INT4           23
// variable-length string, no limit specified
#define PG_OID_TEXT           25
// object identifier(oid), maximum 4 billion
#define PG_OID_OID            26
// transaction id
#define PG_OID_TRANSACTIONID  28
// double-precision floating point number, 8-byte storage
#define PG_OID_DOUBLE         701
// text[]
#define PG_OID_TEXTARRAY      1009
// aclitem[]
#define PG_OID_ACLITEMARRAY   1034
// varchar(length)
#define PG_OID_VARCHAR_LENGTH 1043

///////////////////////////////////////////////////////////////////////////////////////
// Postgres
///////////////////////////////////////////////////////////////////////////////////////

Postgres::Postgres() :
  conn(NULL), stc(this)
  {
  }

//////////////////////////////////////
// Public methods

////////////////
// Connection

void Postgres::Connect(const ConnectionInfo& cni)
  {
  Disconnect();

  if(!cni.IsSet(DATABASE_NAME))
    throw wx::Exception(wxT("Postgres::Connect() No database name supplied."));

  wxString sConnection;
  sConnection = wxT("dbname='") + cni.Get(DATABASE_NAME) + wxT("'");

  if(cni.IsSet(DATABASE_HOST))
    sConnection += wxT(" host='") + cni.Get(DATABASE_HOST) + wxT("'");

  if(cni.IsSet(DATABASE_LOGON))
    sConnection += wxT(" user='") + cni.Get(DATABASE_LOGON) + wxT("'");

  if(cni.IsSet(DATABASE_PASSWORD))
    sConnection += wxT(" password='") + cni.Get(DATABASE_PASSWORD) + wxT("'");

  conn = PQconnectdb(sConnection.mbc_str());
  if(PQstatus(conn) != CONNECTION_OK)
    {
    char * szError = PQerrorMessage(conn);
    throw wx::Exception(wxT("Postgres::Connect() Failed to connect to database."));
    }
  }

void Postgres::Disconnect()
  {
  if(!IsConnected())
    return;

  if(conn != NULL)
    {
    PQfinish(conn);
    conn = NULL;
    }
  }

////////////////
// Transactions

void Postgres::Begin()
  {
  PgStatement::ExecuteTemporary
    (
    this,
    wxT("BEGIN;")
    );
  }

void Postgres::Rollback()
  {
  PgStatement::ExecuteTemporary
    (
    this,
    wxT("ROLLBACK;")
    );
  }

void Postgres::Commit()
  {
  PgStatement::ExecuteTemporary
    (
    this,
    wxT("COMMIT;")
    );
  }

////////////////
// Queries

Statement * Postgres::Prepare(const wxString& sQuery)
	{
  if(!IsConnected())
    throw wx::Exception(wxT("Postgres::Prepare() Database not connected."));

  Statement * stm = new PgStatement(this, sQuery);

  return stm;
  }

////////////////
// Database info

wxArrayString Postgres::GetDatabases()
  {
  if(!IsConnected())
    throw wx::Exception(wxT("Postgres::GetDatabases() Database not connected."));

  ArrayRecordArray ara;
  stc.CacheExecute
    (
    wxT("Postgres::GetDatabases"),
    wxT("SELECT datname FROM pg_database;"),
    ArrayRecord(),
    ara
    );

  wxArrayString as;
  for(ArrayRecordArray::iterator it = ara.begin(); it != ara.end(); it++)
    as.push_back(it->at(0));

  return as;
  }

bool Postgres::DatabaseExists(const wxString& sDatabaseName)
  {
  if(!IsConnected())
    throw wx::Exception(wxT("Postgres::DatabaseExists() Database not connected."));

  // The query uses field 'datdba' as it's lightweight (int4)
  wxString sqryDatabaseExists = wxT("         \
SELECT datdba                                 \
FROM pg_database                              \
WHERE datname = $1;                           \
");

  // Unfortunately, SQL has a stupid rule about case folding.
  // PG holds table names internally as lower case.
  wxString sDbLower = sDatabaseName.Lower();

  ArrayRecord arParams;
  arParams.push_back(wxVariant(sDbLower));
  ArrayRecordArray ara;
  stc.CacheExecute
    (
    wxT("Postgres::DatabaseExists"),
    sqryDatabaseExists,
    arParams,
    ara
    );

  return (ara.size() > 0)? true : false;
  }

wxArrayString Postgres::GetTables()
  {
  if(!IsConnected())
    throw wx::Exception(wxT("Postgres::GetTables() Database not connected."));

  wxString sqryTables = wxT("                 \
SELECT                                        \
  relname                                     \
FROM                                          \
  pg_class cl                                 \
INNER JOIN                                    \
  pg_namespace ns                             \
ON                                            \
  cl.relnamespace = ns.oid                    \
WHERE                                         \
  ns.nspname = 'public'                       \
AND                                           \
  cl.reltype <> 0                             \
AND                                           \
  cl.relkind = 'r';                           \
");

  ArrayRecordArray ara;
  stc.CacheExecute
    (
    wxT("Postgres::GetTables"),
    sqryTables,
    ArrayRecord(),
    ara
    );

  ArrayRecord ar;
  for(ArrayRecordArray::iterator it = ara.begin(); it != ara.end(); it++)
    ar.push_back(it->at(0));

  return ar.GetArrayString();
  }

bool Postgres::TableExists(const wxString& sTableName)
  {
  if(!IsConnected())
    throw wx::Exception(wxT("Postgres::TableExists() Database not connected."));

  wxString sqryTableExists = wxT("            \
SELECT                                        \
  cl.relname                                  \
FROM                                          \
  pg_class cl                                 \
INNER JOIN                                    \
  pg_namespace ns                             \
ON                                            \
  cl.relnamespace = ns.oid                    \
WHERE                                         \
  ns.nspname = 'public'                       \
AND                                           \
  cl.reltype <> 0                             \
AND                                           \
  cl.relkind = 'r'                            \
AND                                           \
  cl.relname = $1;                            \
");

  ArrayRecord arParams;
  arParams.push_back(wxVariant(sTableName));
  ArrayRecordArray ara;
  stc.CacheExecute
    (
    wxT("Postgres::TableExists"),
    sqryTableExists,
    arParams,
    ara
    );

  return (ara.size() != 0)? true : false;;
  }

//////////////////////////////////////
// Private methods

wxString Postgres::GetPgTypeFromVariantType(const wxString& sType)
  {
  if(sType == TYPENAME_BOOLEAN)
    return wxT("bool");
  else if(sType == TYPENAME_LONG)
    return wxT("int4");
  else if(sType == TYPENAME_DOUBLE)
    return wxT("float8");
  else if(sType == TYPENAME_STRING)
    return wxT("text");
  else
    throw wx::Exception(wxT("Postgres::GetPgTypeFromVariantType() Unknown type '%s' supplied."), sType);
  }

Oid Postgres::GetPgOidFromVariantType(const wxString& sType)
  {
  // Unmapped oid types:
  //  PG_OID_NAME,
  //  PG_OID_OID,
  //  PG_OID_TRANSACTIONID,
  //  PG_OID_TEXTARRAY,
  //  PG_OID_ACLITEMARRAY

  if(sType == TYPENAME_BOOLEAN)
    return PG_OID_BOOLEAN;
  else if(sType == TYPENAME_LONG)
    return PG_OID_INT4;
  else if(sType == TYPENAME_DOUBLE)
    return PG_OID_DOUBLE;
  else if(sType == TYPENAME_STRING)
    return PG_OID_VARCHAR_LENGTH;
  else
    throw wx::Exception(wxT("Postgres::GetPgOidFromVariantType() Unknown type '%s' supplied."), sType);
  }

long Postgres::GenerateStatementId()
	{
	return ++_nStatementId;
	}

///////////////////////////////////////////////////////////////////////////////////////
// PgStatement

///////////////////////////////////////////////////////////////////////////////////////
// PgStatement::PgParameters

PgStatement::PgParameters::PgParameters
  (
  const ArrayRecord& raParameters
  ) :
  paramValues(NULL),
  paramLengths(NULL),
  paramFormats(NULL)
  {
  nParamCount = raParameters.size();
  if(nParamCount > 0)
    {
    paramTypes = new Oid [nParamCount];
    paramValues = new char * [nParamCount];
    paramLengths = new int[nParamCount];
    paramFormats = new int[nParamCount];
    for(int i = 0; i < nParamCount; i++)
      {
      wxString sType = raParameters.at(i).GetType();
      Oid oid = Postgres::GetPgOidFromVariantType(sType);
      paramTypes[i] = oid;
      if(sType == TYPENAME_LONG)
        {
        // Binary data must be in big endian byte order
        unsigned long ul = (unsigned long) raParameters.at(i).GetLong();
        unsigned long ulBigEndian = htonl(ul);
        long * pl = (long *) malloc(sizeof(long));
        paramValues[i] = (char *) pl;
        *pl = ulBigEndian;
        paramLengths[i] = sizeof(long);
        paramFormats[i] = 1;
        }
      else if(sType == TYPENAME_BOOLEAN)
        {
        bool * pb = (bool *) malloc(sizeof(bool));
        paramValues[i] = (char *) pb;
        *pb = raParameters.at(i).GetBool();
        paramLengths[i] = sizeof(bool);
        paramFormats[i] = 1;
        }
      else if(sType == TYPENAME_DOUBLE)
        {
        // FIXME: Don't know the network conversion for double
        // so we can't pass binary data
        double d = raParameters.at(i).GetDouble();
        char * sz = (char *) malloc(20 * sizeof(char));
        sprintf(sz, "%.10f", d);
        paramValues[i] = sz;
        paramLengths[i] = 0;
        paramFormats[i] = 0; // Treat as text for now
        }
      else if(sType == TYPENAME_STRING)
        {
        wxString s = raParameters.at(i).GetString();

        char * sz = (char *) malloc((s.Length() + 1) * sizeof(char));
        strcpy(sz, s.mbc_str());
        paramValues[i] = sz;
        paramLengths[i] = 0;
        paramFormats[i] = 0;
        }
      else
        throw wx::Exception(wxT("PgStatement::PgParameters::PgParameters() Unknown type '%s'"), sType);
      }
    }
  }

PgStatement::PgParameters::~PgParameters()
  {
  // Cleanup parameter arrays
  if(nParamCount > 0)
    {
    for(int i = 0; i < nParamCount; i++)
      {
      free(paramValues[i]);
      paramValues[i] = NULL;
      }
    delete [] paramTypes;
    delete [] paramValues;
    delete [] paramLengths;
    delete [] paramFormats;
    }
  }

PgStatement::PgStatement
  (
  Database * pdbOwner,
  const wxString& sNewQuery
  ) :
  Statement(pdbOwner, sNewQuery),
  pg(dynamic_cast<Postgres *>(pdbOwner)),
  pgr(NULL), nCurrentRecord(-1)
  {
  // Make a unique name for the handle

	long nStatementId = pg->GenerateStatementId();
  sQueryHandle.Printf(wxT("pgs%08x"), nStatementId);
  }

PgStatement::~PgStatement()
  {
  CleanUpResult();
  }

bool PgStatement::DataAvailable()
  {
  if(pgr == NULL)
    return false;

  ExecStatusType exs = PQresultStatus(pgr);
  bool bDataRetrieved = (exs == PGRES_TUPLES_OK)? true : false;
  int nRecordCount = PQntuples(pgr);
  bool bStillToRead = (nCurrentRecord >= 0 && nCurrentRecord < nRecordCount)? true : false;

  return bDataRetrieved && bStillToRead;
  }

void PgStatement::ExecutePrepared(const ArrayRecord& raParameters)
  {
  if(pgr == NULL)
    PrepareQuery(raParameters);
  else
    CleanUpResult();

  PgParameters pgp(raParameters);
  int * pnParamFormats = pgp.GetParamFormats();

  try
    {
    pgr = PQexecPrepared
      (
      pg->conn,
      sQueryHandle.mbc_str(),
      pgp.GetParamCount(),
      pgp.GetParamValues(),
      pgp.GetParamLengths(),
      pnParamFormats,
      0
      );
    }
  catch(...)
    {
    ThrowPgError(wxT("PgStatement::ExecutePrepared() Query caused exception: %s."));
    }

  ExecStatusType exs = PQresultStatus(pgr);
  if(exs == PGRES_FATAL_ERROR)
    ThrowPgError(wxT("PgStatement::ExecutePrepared() Query execution failed: %s."));

  nCurrentRecord = 0;
  }

void PgStatement::ExecuteTemporary(const ArrayRecord& raParameters)
  {
  ExecuteTemporary(pg, sQuery, raParameters, &pgr);

  nCurrentRecord = 0;
  }

int PgStatement::GetFieldCount()
  {
  if(pgr == NULL)
    throw wx::Exception(wxT("PgStatement::GetFieldCount() No current record"));

  return PQnfields(pgr);
  }

void PgStatement::MoveNext()
  {
  if(pgr == NULL)
    throw wx::Exception(wxT("PgStatement::MoveNext() No current record"));

  nCurrentRecord++;
  }

void PgStatement::GetFieldValue
  (
  int         nIndex,
  wxVariant&  v
  )
  {
  int nIsNull = PQgetisnull(pgr, nCurrentRecord, nIndex);
  if(nIsNull != 0)
    {
    v.MakeNull();
    return;
    }

	char * sz = PQgetvalue(pgr, nCurrentRecord, nIndex);

  Oid typ = PQftype(pgr, nIndex);
  switch(typ)
    {
    case PG_OID_BOOLEAN:
      {
      bool b = (sz[0] == 't')? true : false;
      v = b;
      break;
      }
    case PG_OID_OID:
    case PG_OID_INT4:
    case PG_OID_TRANSACTIONID:
      {
      long n = atol(sz);
      v = n;
      break;
      }
    case PG_OID_DOUBLE:
      {
      double d = atof(sz);
      v = d;
      break;
      }
    case PG_OID_NAME:
    case PG_OID_TEXT:
    case PG_OID_VARCHAR_LENGTH:
      {
      //int nFormat = PQfformat(pgr, nIndex); // 0 => text, 1 => binary
      wxString s(sz, wxConvLocal);
      //wxString s = wxConvUTF8.cMB2WX(sz);
      v = s;
      break;
      }
    // Items we don't know how to handle
    case PG_OID_TEXTARRAY:
    case PG_OID_ACLITEMARRAY:
      v = wxT("");
      break;

    default:
      throw wx::Exception(wxT("PgStatement::GetFieldValue() Unhandled data type %d"), typ);
    }
  }

wxString PgStatement::GetFieldName(int nIndex)
  {
  char * szName = PQfname(pgr, nIndex);
	wxString sName = wxString::FromAscii(szName);

  return sName;
  }

// Static implementation
void PgStatement::ExecuteTemporary
  (
  Postgres * pg,
  const wxString& sQuery,
  const ArrayRecord& raParameters,
  PGresult ** ppgr
  )
  {
  PgParameters pgp(raParameters);
  int * pnParamFormats = pgp.GetParamFormats();

  PGresult * pgr = NULL;
  try
    {
    pgr = PQexecParams
      (
      pg->conn,
      sQuery.mbc_str(),
      pgp.GetParamCount(),
      pgp.GetParamTypes(),
      pgp.GetParamValues(),
      pgp.GetParamLengths(),
      pnParamFormats,
      0
      );
    }
  catch(...)
    {
    char * szPgError = PQerrorMessage(pg->conn);

    throw wx::Exception(wxT("PgStatement::ExecuteTemporary() Query caused exception: %s."), wxString::FromAscii(szPgError));
    }

  ExecStatusType exs = PQresultStatus(pgr);
  if(exs == PGRES_FATAL_ERROR)
    {
    char * szPgError = PQerrorMessage(pg->conn);

    throw wx::Exception(wxT("PgStatement::ExecuteTemporary() Query execution failed: %s."), wxString::FromAscii(szPgError));
    }

  if(ppgr == NULL)
    {
    if(pgr != NULL)
      {
      PQclear(pgr);
      pgr = NULL;
      }
    }
  else
    *ppgr = pgr;
  }

void PgStatement::PrepareQuery(const ArrayRecord& raParameters)
  {
  wxASSERT_MSG(pgr == NULL, wxT("PgStatement::PrepareQuery() should only get called if pgr is NULL"));

  // Prepare parameter types
  wxString sParamTypes;
  if(raParameters.size() != 0)
    {
    sParamTypes = wxT(" (");
    for(ArrayRecord::const_iterator it = raParameters.begin(); it != raParameters.end(); it++)
      {
      if(it != raParameters.begin())
        sParamTypes += wxT(", ");

      // Translate wxVariant type names into Postgres type names
      wxString sType = it->GetType();
      wxString sPgTypeName = Postgres::GetPgTypeFromVariantType(sType);
      sParamTypes += sPgTypeName;
      }
    sParamTypes += wxT(")");
    }
  else
    sParamTypes = wxT("");

  wxString sPrepareStatement = wxString::Format
    (
    wxT("PREPARE %s%s AS %s"),
    sQueryHandle.c_str(),
    sParamTypes.c_str(),
    sQuery.c_str()
    );

  PGresult * pgrPrepare = PQexec(pg->conn, sPrepareStatement.mbc_str());
  ExecStatusType exs = PQresultStatus(pgrPrepare);
  if(exs != PGRES_COMMAND_OK)
    ThrowPgError();
  if(pgrPrepare != NULL)
    {
    PQclear(pgrPrepare);
    pgr = NULL;
    }
  }

void PgStatement::CleanUpResult()
  {
  if(pgr != NULL)
    {
    PQclear(pgr);
    pgr = NULL;
    }
  nCurrentRecord = -1;
  }

void PgStatement::ThrowPgError(wxString sError)
  {
  char * szPgError = PQerrorMessage(pg->conn);
  wxString sPgError = wxString::FromAscii(szPgError);

  throw wx::Exception(__FILE__, __LINE__, sError.c_str(), sPgError.c_str());
  }

} // namespace DBI

} // namespace wx

#endif // def wxDBI_POSTGRES
