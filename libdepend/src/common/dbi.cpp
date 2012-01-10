/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dbi.cpp
// Purpose:     
// Author:      Joe Yates
// Created:     2004-11-17
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#include <wx/dbi.h>

namespace wx
{

namespace DBI
{

////////////////////////////////////////////////////////////////////////////////////
// Database

////////////////////////////////////
// Database::ConnectionInfo

Database::ConnectionInfo::ConnectionInfo(const ConnectionInfo& cni)
  {
	for(ConnectionInfo::const_iterator it = cni.begin(); it != cni.end(); it++)
    (*this)[it->first] = it->second;
  }

Database::ConnectionInfo& Database::ConnectionInfo::operator=(const ConnectionInfo& cni)
  {
	for(ConnectionInfo::const_iterator it = cni.begin(); it != cni.end(); it++)
    (*this)[it->first] = it->second;

  return *this;
  }

void Database::ConnectionInfo::Set(const wxString& sName, const wxString& sValue)
  {
  (*this)[sName] = sValue;
  }

wxString Database::ConnectionInfo::Get(const wxString& sName) const
  {
	ConnectionInfo::const_iterator it = this->find(sName);
	if(it == this->end())
    throw wx::Exception(wxT("Connection string '%s' does not exist."), sName.c_str());

  return it->second;
  }

bool Database::ConnectionInfo::IsSet(const wxString& sName) const
  {
	ConnectionInfo::const_iterator it = this->find(sName);

  return (it != this->end())? true : false;
  }

////////////////////////////////////////////////////////////////////////////////////
// Statement

Statement::Statement(Database * pdbOwner, const wxString& sNewQuery) :
  pdb(pdbOwner), sQuery(sNewQuery)
  {
	if(pdbOwner == NULL)
		throw Exception(_T("Statement::Statement() Invalid pdbNew parameter."));
  }

wxString Statement::GetQuery()
  {
  return sQuery;
  }

void Statement::Fetch(ArrayRecord& ar)
  {
  if(!DataAvailable())
    throw wx::Exception(wxT("Statement::Fetch(ArrayRecord&) No data available"));

  int nFields = GetFieldCount();
  for(int i = 0; i < nFields; i++)
    {
    wxVariant v;
    GetFieldValue(i, v);
		ar.push_back(v);
    }

  MoveNext();
  }

void Statement::Fetch(HashRecord& hr)
  {
  if(!DataAvailable())
    throw wx::Exception(wxT("Statement::Fetch(HashRecord&) No data available"));

  int nFields = GetFieldCount();

  for(int i = 0; i < nFields; i++)
    {
    wxVariant v;
    GetFieldValue(i, v);
    wxString sName = GetFieldName(i);
		hr[sName] = v;
    }

  MoveNext();
  }

void Statement::Fetch(ArrayRecordArray& ara)
  {
  while(DataAvailable())
    {
    ArrayRecord ar;
    Fetch(ar);
    ara.push_back(ar);
    }
  }

void Statement::Fetch(HashRecordArray& hra)
  {
  while(DataAvailable())
    {
    HashRecord hr;
    Fetch(hr);
    hra.push_back(hr);
    }
  }

long Statement::GetLastInsertId()
	{
	throw wx::Exception(wxT("The database engine you are using does not supply last insert ids."));
	}

////////////////////////////////////////////////////////////////////////////////////
// StatementCache

StatementCache::StatementCache(Database * pdbNew) :
  pdb(pdbNew)
	{
  }

StatementCache::~StatementCache()
	{
  Clear();
	}

void StatementCache::CacheExecute
  (
  const wxString&     sName,
  const wxString&     sQuery,
  const ArrayRecord&  arParameters
  )
  {
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
  }

void StatementCache::CacheExecute
  (
  const wxString&     sName,
  const wxString&     sQuery,
  const ArrayRecord&  arParameters,
	wxVariant&        	v
  )
  {
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
	ArrayRecord arResult;
  pstm->Fetch(arResult);
	v = arResult[0];
  }

void StatementCache::CacheExecute
  (
  const wxString&     sName,
  const wxString&     sQuery,
  const ArrayRecord&  arParameters,
  ArrayRecord&        arResult
  )
  {
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
  pstm->Fetch(arResult);
  }

void StatementCache::CacheExecute
  (
  const wxString&     sName,
  const wxString&     sQuery,
  const ArrayRecord&  arParameters,
  HashRecord&         hrResult
  )
  {
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
  pstm->Fetch(hrResult);
  }

void StatementCache::CacheExecute
  (
  const wxString&     sName,
  const wxString&     sQuery,
  const ArrayRecord&  arParameters,
  ArrayRecordArray&   araResult
  )
  {
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
  pstm->Fetch(araResult);
  }

void StatementCache::CacheExecute
  (
  const wxString&     sName,
  const wxString&     sQuery,
  const ArrayRecord&  arParameters,
  HashRecordArray&    hraResult
  )
  {
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
  pstm->Fetch(hraResult);
  }

void StatementCache::CacheExecuteInsert
	(
	const wxString&     sName,
	const wxString&     sQuery,
	const ArrayRecord&  arParameters,
	long&        				nLastInsertId
	)
	{
  Statement * pstm = HandleCaching(sName, sQuery);
  pstm->ExecutePrepared(arParameters);
	nLastInsertId = pstm->GetLastInsertId();
	}

void StatementCache::ExecuteTemporary
  (
  const wxString&     sQuery,
  const ArrayRecord&  arParameters
  )
  {
  Statement * pstm = pdb->Prepare(sQuery);
  pstm->ExecuteTemporary(arParameters);
  delete pstm;
  }

void StatementCache::ExecuteTemporary
  (
  const wxString&     sQuery,
  const ArrayRecord&  arParameters,
  ArrayRecordArray&   araResult
  )
  {
  Statement * pstm = pdb->Prepare(sQuery);
  pstm->ExecuteTemporary(arParameters);
  pstm->Fetch(araResult);
  delete pstm;
  }

void StatementCache::Clear()
  {
  for(Cache::iterator it = cch.begin(); it != cch.end(); it++)
		{
		Statement * st = it->second;
		delete st;
		}
	cch.clear();
  }

Statement * StatementCache::HandleCaching
  (
  const wxString& sName,
  const wxString& sQuery
  )
  {
  Statement * pstm = NULL;

	Cache::iterator it = cch.find(sName);
	if(it != cch.end())
		pstm = it->second;
	else
    {
 		pstm = pdb->Prepare(sQuery);
		cch.insert(std::pair<wxString, Statement *>(sName, pstm));
    }

  return pstm;
	}

void StatementCache::Delete(wxString const& sName)
	{
	Cache::iterator it = cch.find(sName);
	if(it != cch.end())
    cch.erase(it);
	}

bool StatementCache::Exists(wxString const& sName)
	{
	Cache::iterator it = cch.find(sName);

  return (it != cch.end())? true : false;
  }

} // namespace DBI

} // namespace wx
