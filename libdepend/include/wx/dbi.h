/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dbi.h
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_dbi_h__
#define __wx_dbi_h__

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#if _MSC_VER > 1000
# ifdef _DEBUG
#   include "crtdbg.h"
#   define new                    new(_NORMAL_BLOCK, __FILE__, __LINE__)
# endif // def _DEBUG
#endif // _MSC_VER > 1000

#ifdef _MSC_VER
#pragma warning (disable : 4786) // identifier was truncated to '255' characters in the browser information
#endif // def _MSC_VER

#include <map>

#include <wx/variant.h>

#include <wx/exception.h>
#include <wx/dbi/types.h>
#include <wx/dbi/record.h>

namespace wx
{

/**
 *  wx::DBI contains a standard interface to various database engines.
 *  The wx::DBI namespace holds a number of classes
 *  which present a standard interface for database access
 *  for various relational database engines.
 *
 *  This class was written as an alternative to wxDB.
 *  The aspects to wxDB which I do not like are:
 *
 *    -# Table- (not query-) oriented access.
 *       There is no reason why queries should be presumed to
 *       access only a single table.
 *    -# The whole query builder interface.
 *       You are supposed to supply the various parts of a query
 *       separately. Doing so makes no sense. The problem with
 *       querying is NOT building queries, it's passing params
 *       and getting results.
 *    -# The lack of a method for returning records.
 *       You have to step and get values of bound variables.
 */

namespace DBI
{

/*
To do:
- Named bind parameters
*/

#define DATABASE_HOST     wxT("Host")
#define DATABASE_NAME     wxT("Name")
#define DATABASE_LOGON    wxT("Logon")
#define DATABASE_PASSWORD wxT("Password")

class Statement;

/** 
 *  Generic interface to databases.
 *  Define one (or more) of the wxDBI_XXXX flags
 *  to include the interfaces to specific database engines.
 */
class Database
  {
  // friends, classes, enums and types

  public:
		/**
     *  A container for database connection parameters.
     *  The values needed to connect depend on the single database.
     */
    class ConnectionInfo : private std::map<wxString, wxString>
			{
			public:
        ConnectionInfo()
          {}
        ConnectionInfo(const ConnectionInfo& cni);
        ConnectionInfo& operator=(const ConnectionInfo& cni);

			public:
				/**
				 *  Add a parameter.
				 *  \param      sName The name of the connection parameter.
				 *  \param      sValue The value of the connection parameter.
				 */
				void Set(const wxString& sName, const wxString& sValue);
		
				/**
				 *  Get a value.
				 *  \param      sName The name of the connection parameter.
				 *  \exception  None.
				 *  \return     A wxString with the value.
				 */
				wxString  Get(const wxString& sName) const;
		
				/**
				 *  Check to see if a certain value is set.
				 *  \param      sName The name of the connection parameter.
				 *  \return     A bool. If true, the value is set. If false, it is not set.
				 */
				bool IsSet(const wxString& sName) const;
			};

  // Constructors/destructors
  public:
    /**
     *  Default ctor.
     */   
    Database()
      {}
    
    /**
     *  dtor.
		 *	Implementations should call the Disconnect() method.
     */   
		virtual ~Database()
			{}

  // Operators

  // Methods
  public:
		// Connection

    /** 
     *  Connect to the database.
     *  Use the values in the ConnectionInfo hash to connect to the database.
     *
     *  Normal connection parameters are:
     *  - Name
     *    The name of the database.
     *  - Logon
     *    The user name for authentication.
     *  - Password
     *    The password for authentication.
     *  - Host
     *    The IP or fully qualified name of the remote host.
     *  - Port
     *    The IP port on which to attempt the connection.
     */
    virtual void Connect(const ConnectionInfo& cni)     = 0;

    /** 
     *  Disconnect from the database.
     */
    virtual void Disconnect()                           = 0;

    /** 
     *  Is the database connected.
     */
    virtual bool IsConnected()                          = 0;

    // Transactions
    /**
     *  Start a new transaction.
     */
    virtual void Begin()
      {
      wxLogFatalError(wxT("Database::Begin() Transaction support not implemented"));
      }

    /** 
     *  Rollback to the start of the transaction.
     */
    virtual void Rollback()
      {
      wxLogFatalError(wxT("Database::Rollback() Transaction support not implemented"));
      }

    /** 
     *  Commit changes made since the start of the  transaction.
     */
    virtual void Commit()
      {
      wxLogFatalError(wxT("Database::Commit() Transaction support not implemented"));
      }

    // Queries

    /**
     *  Prepare a query.
     *  Note that the returned Statement pointer must be destroyed with delete.
     *  \param      sQuery  The SQL query.
     *  \return     A Statement pointer for the query.
     */
		virtual Statement * Prepare
      (
      const wxString& sQuery
      )                                                 = 0;

    // Database info

    /**
     *  List all other databases on the same server as the currently connected database.
     *  \return An wxStringArray of database names.
     */
    virtual wxArrayString GetDatabases()                = 0;

    /**
     *  Indicates whether a certain database exists on the current server.
     *  \param      sDatabaseName The database name.
     *  \return     bool True is the database exists, otherwise false.
     */
    virtual bool DatabaseExists
      (
      const wxString& sDatabaseName
      )                                                 = 0;

    /**
     *  Returns an array containing the names of tables in the database.
     */
    virtual wxArrayString GetTables()                   = 0;

    /**
     *  Indicates whether a certain table exists in the current databse.
     *  \param      sTableName The name of the table.
     *  \return     bool True is the table exists, otherwise false.
     */
    virtual bool TableExists
      (
      const wxString& sTableName
      )                                                 = 0;

  // Members
  protected:
    ConnectionInfo m_cni;
  };

class Statement
  {
  // friends, classes, enums and types

  // Constructors/destructors
  public:
		Statement(Database * pdbOwner, const wxString& sNewQuery);
		virtual ~Statement()
			{}

  // Operators

  // Methods
	public:

    /**
     *  Return the SQL query.
     *  \return The query.
     */
		wxString					  GetQuery();
		virtual bool		    DataAvailable()                 = 0;
		virtual void ExecutePrepared
      (
      const ArrayRecord& raParameters = ArrayRecord()
      )                                                 = 0;
		virtual void ExecuteTemporary
      (
      const ArrayRecord& raParameters = ArrayRecord()
      )                                                 = 0;
		void  Fetch(ArrayRecord& ar);
    void  Fetch(HashRecord& hr);
		void  Fetch(ArrayRecordArray& ara);
		void  Fetch(HashRecordArray& hra);

    /**
     *  Return the SQL query.
		 *	\exception	Raises an exception if the database engine does not support this functionality.
     *  \return The id of the last inserted record that caused an id to be generated.
     */
		virtual long  GetLastInsertId();

  protected:
    virtual int   GetFieldCount()                       = 0;
    virtual void  MoveNext()                            = 0;
    virtual void  GetFieldValue
      (
      int nIndex,
      wxVariant&  v
      )                                                 = 0;
    virtual wxString GetFieldName
      (
      int nIndex
      )                                                 = 0;

  // Members
	protected:
		Database *				pdb;
    wxString          sQuery;
  };

class StatementCache
	{
  // friends, classes, enums and types
  private:
    typedef std::map<wxString, Statement *> Cache;

	// Constructors/destructors
	public:
		StatementCache(Database * pdbNew);
		virtual ~StatementCache();
  private:
		StatementCache();

  // Methods
	public:
    /**
		 *	Prepare, cache and execute a query without returning a result.
     *  Executes the queries and ignores any results.
		 *  \param					sName					The name to use for the prepared query in the Cache.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
     */
		virtual void CacheExecute
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters = ArrayRecord()
      );

    /**
		 *	Prepare, cache and execute a query returning the first field of the first result.
		 *	Return a the value of the first field of the first record as a wxVariant.
		 *  \param					sName					The name to use for the prepared query in the Cache.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
		 *  \param[out]			v							The first field of the returned record.
     */
		virtual void CacheExecute
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      wxVariant&        	v
      );

    /**
		 *	Prepare, cache and execute a query returning an ArrayRecord.
		 *	Return a single result record as an ArrayRecord.
		 *  \param					sName					The name to use for the prepared query in the Cache.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
		 *  \param[out]			arResult			The first returned record.
     */
		virtual void CacheExecute
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      ArrayRecord&        arResult
      );

    /**
		 *	Prepare, cache and execute a query returning an HashRecord.
		 *	Return a single result record as an HashRecord.
		 *  \param					sName					The name to use for the prepared query in the Cache.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
		 *  \param[out]			hrResult			The first returned record.
     */
		virtual void CacheExecute
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      HashRecord&         hrResult
      );

    /**
		 *	Prepare, cache and execute a query returning an ArrayRecordArray.
		 *	Return a all result records as an ArrayRecordArray.
		 *  \param					sName					The name to use for the prepared query in the Cache.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
		 *  \param[out]			araResult			All returned records.
     */
		virtual void CacheExecute
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      ArrayRecordArray&   araResult
      );

    /**
		 *	Prepare, cache and execute a query returning an HashRecordArray.
		 *	Return a all result records as an HashRecordArray.
		 *  \param					sName					The name to use for the prepared query in the Cache.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters 	Parameters for the query.
		 *  \param[out]			hraResult			All returned records.
     */
		virtual void CacheExecute
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      HashRecordArray&    hraResult
      );

    /**
		 *	Prepare, cache and execute a query returning the new id.
		 *	This function calls the database-engine specific function which retrieves
		 *	the last insert id, if such a function is available.
		 *  \param					sName						The name to use for the prepared query in the Cache.
		 *  \param					sQuery 					The SQL query.
		 *  \param					arParameters		Parameters for the query.
		 *  \param[out]			nLastInsertId		The last id.
		 *	\exception			Throws an exception if the databse engine does not have a method
		 *		 for obtaining the last id inserted.
     */
		virtual void CacheExecuteInsert
      (
      const wxString&     sName,
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      long&        				nLastInsertId
      );

    /**  
     *  Execute a query.
		 *	The query is not cached, and the results are ignored.
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
     */
		virtual void ExecuteTemporary
      (
      const wxString&     sQuery,
      const ArrayRecord&  arParameters = ArrayRecord()
      );

    /**  
     *  Execute a query.
		 *	The query is not cached.
     *  The results are returned in an ArrayRecordArray
		 *  \param					sQuery 				The SQL query.
		 *  \param					arParameters	Parameters for the query.
		 *  \param[out]			araResult			All returned records.
     */
		virtual void ExecuteTemporary
      (
      const wxString&     sQuery,
      const ArrayRecord&  arParameters,
      ArrayRecordArray&   araResult
      );

    virtual void Clear();

  private:
    Statement * HandleCaching
      (
      const wxString& sName,
      const wxString& sQuery
      );

    /**  
     *  Delete a query from the cache.
		 *  \param sName				The name used for the prepared query in the Cache.
     */
    void  Delete(const wxString& sName);

    /**  
     *  Cehck if a query exists in the cache.
		 *  \param sName				The name used for the prepared query in the Cache.
     */
    bool  Exists(const wxString& sName);

  protected:
    Cache           cch;
    Database *      pdb;
	};

} // namespace DBI

} // namespace wx

#ifdef wxDBI_POSTGRES
#include <wx/dbi/postgres.h>
#endif // def wxDBI_POSTGRES
#ifdef wxDBI_SQLITE
#include <wx/dbi/sqlite.h>
#endif // def wxDBI_SQLITE

#endif // ndef __wx_dbi_h__
