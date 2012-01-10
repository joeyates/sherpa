/////////////////////////////////////////////////////////////////////////////
// Name:        command.h
// Author:      Joe Yates
// Purpose: 		An alternative to wxExecute
// Created:     2004-11-25
// Copyright:   (c) Joe Yates
// License:     BSD license (see the file 'LICENSE.txt')
/////////////////////////////////////////////////////////////////////////////

#ifndef __wx_msw_command_h__
#define __wx_msw_command_h__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/exception.h>

namespace wx
{

/**  
  *  The wxExecute class pops up a window on execution.
  *  This class runs a command "silently".
  */

class Command
  {
  public:
    Command
      (
      const wxString& sCommand,
      unsigned long   ulTimeOut = INFINITE
      ) :
      m_sCommand(sCommand),
      m_dwTimeOut(ulTimeOut)
      {}
  public:
    long Run()
      {
      STARTUPINFO si;
      wxZeroMemory(si);
      si.cb = sizeof(si);
      si.dwFlags     = STARTF_USESTDHANDLES;
      si.hStdInput   = GetStdHandle(STD_INPUT_HANDLE);
      si.hStdOutput  = GetStdHandle(STD_OUTPUT_HANDLE);
      si.hStdError   = GetStdHandle(STD_ERROR_HANDLE);

      PROCESS_INFORMATION pi;
      BOOL nOk = ::CreateProcess
        (
        NULL,
        (wxChar *) m_sCommand.c_str(),
        NULL,
        NULL,
        0,
        CREATE_NEW_CONSOLE, // CREATE_NO_WINDOW
        NULL,
        NULL,
        &si,
        &pi
        );

      if(nOk == 0)
        {
        DWORD dwLastError = GetLastError();
        LPTSTR szBuffer = NULL;
        FormatMessage
          (
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
          NULL,
          dwLastError,
          0,
          szBuffer,
          20,
          NULL
          );
        wxString sError = szBuffer;
        LocalFree(szBuffer);
        throw wx::Exception(sError);
        }

      WaitForSingleObject(pi.hProcess, m_dwTimeOut);

      DWORD dwExitCode = 0;
      GetExitCodeProcess(pi.hProcess, &dwExitCode);

      return (long) dwExitCode;
      }

  private:
    wxString  m_sCommand;
    DWORD     m_dwTimeOut;
  };

} // namespace wx

#endif // ndef __wx_msw_command_h__
