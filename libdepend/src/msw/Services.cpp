/////////////////////////////////////////////////////////////////////////////
// Name:        
// Author:      Joe Yates
// Purpose:     
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifdef __WXMSW__

#include "Services.h"

namespace wx
{

#if wxUSE_UNICODE
wxMBConvUTF16 cnv;
#else
#define cnv wxConvLibc
#endif // wxUSE_UNICODE

ServiceArray Services::GetList(SC_HANDLE hSCM, ServiceState sta)
  {
  DWORD dwServiceState;
  switch(sta)
    {
    case staAny:
      dwServiceState = SERVICE_STATE_ALL;
      break;
    case staActive:
      dwServiceState = SERVICE_ACTIVE;
      break;
    case staInactive:
      dwServiceState = SERVICE_INACTIVE;
      break;
    default:
      return ServiceArray();
    }

  SC_LOCK scLock = LockServiceDatabase(hSCM);

  DWORD cbBytesNeeded = 0;
  DWORD dwServicesReturned = 0;
  DWORD dwResumeHandle = 0;
  EnumServicesStatus
    (
    hSCM,
    SERVICE_WIN32, 
    dwServiceState,
    NULL,
    0, 
    &cbBytesNeeded,
    &dwServicesReturned,
    &dwResumeHandle
    );

  ENUM_SERVICE_STATUS * pess = (ENUM_SERVICE_STATUS *) malloc(cbBytesNeeded);
  EnumServicesStatus
    (
    hSCM,
    SERVICE_WIN32, 
    dwServiceState,
    pess,
    cbBytesNeeded,
    &cbBytesNeeded,
    &dwServicesReturned,
    &dwResumeHandle
    );
  ServiceArray sa;
  for(DWORD dw = 0; dw < dwServicesReturned; dw++)
    {
    wxString sDisplayName(pess[dw].lpDisplayName, cnv);
    wxString sServiceName(pess[dw].lpServiceName, cnv);
    sa.push_back(Service(sDisplayName, sServiceName));
    }
  free(pess);

  UnlockServiceDatabase(scLock);

  return sa;
  }

long Services::FindDisplayNameMatch(const wxString& sNameMatch)
  {
  wxRegEx reName;
  reName.Compile(sNameMatch, wxRE_ICASE);
  for(long i = 0; i < sa.size(); i++)
    {
    wxString sDisplayName = sa[i].m_sDisplayName;
    if(reName.Matches(sDisplayName))
      return i;
    }
  return -1;
  }

Services::ServiceState Services::GetState(long nIndex)
  {
  Service srv = sa[nIndex];
  const TCHAR * sz = srv.m_sServiceName.c_str();

  SC_HANDLE scHandle = OpenService(m_hSCM, sz, SERVICE_QUERY_STATUS);
  SERVICE_STATUS sta;
  QueryServiceStatus(scHandle, &sta);
  CloseServiceHandle(scHandle);

  return CurrentStateToServiceState(sta.dwCurrentState);
  }

bool Services::Start(long nIndex)
  {
  if(GetState(nIndex) == staActive)
    return true;

  Service srv = sa[nIndex];
  const TCHAR * sz = srv.m_sServiceName.c_str();
  SC_HANDLE scHandle = OpenService(m_hSCM, sz, SERVICE_START);

  int nStarted = StartService(scHandle, 0, NULL);
  CloseServiceHandle(scHandle);

  return (nStarted != 0)? true : false;
  }

Services::ServiceState Services::CurrentStateToServiceState(DWORD dwCurrentState)
  {
  switch(dwCurrentState)
    {
    case SERVICE_RUNNING:
      return staActive;

    case SERVICE_STOPPED:
      return staInactive;

    case SERVICE_START_PENDING:
      return staStarting;

    default: // TBI
      return staInactive;
    }
  }

} // namespace wx

#endif
	// def __WXMSW__
