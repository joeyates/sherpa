///////////////////////////////////////////////////////////////////////////////
// Name:        Persona.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: Persona.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __Persona_h__
#define __Persona_h__

class Persona
  {
#ifdef __UNIX__
  // friends, classes, enums and types
  WX_DECLARE_HASH_MAP(gid_t, wxString, wxIntegerHash, wxIntegerEqual, GroupHash);
#endif // def __UNIX__

  // Constructors/destructors
  public:
    Persona()
      {
#ifdef __UNIX__
      uid = getuid();
      gid = getgid();
      GetGroups();
#endif // def __UNIX__
      sHome = wxGetHomeDir();
      };

  // Operators

  // Methods
  public:
    wxString GetHome()
      {
      return sHome;
      };

    bool CanExecute(const wxFileName& filPath)
      {
      wxString sPath = filPath.GetFullPath();
#ifdef __UNIX__
      struct stat st;
      lstat(sPath.fn_str(), &st);
#elif defined __WXMSW__
      struct _stat st;
      MSW_STAT(sPath.fn_str(), &st);
#endif // def __UNIX__
      return CanExecute(st);
      };

#ifdef __UNIX__
    bool CanExecute(const struct stat st)
      {
      if(st.st_mode & S_IXOTH)
        return true;

      bool bIsInGroup = IsInGroup(st.st_gid);
      if(bIsInGroup && (st.st_mode & S_IXGRP))
        return true;

      if(uid == st.st_uid && (st.st_mode & S_IXUSR))
        return true;

      return false;
      };
#elif defined __WXMSW__
    bool CanExecute(const struct _stat st)
      {
      return (st.st_mode & _S_IEXEC)? true : false; // FIXME
      }
#endif // def __UNIX__

#ifdef __UNIX__
    bool IsInGroup(gid_t gid)
      {
      GroupHash::iterator it = gph.find(gid);
      if(it != gph.end())
        return true;
      return false;
      };
#endif // def __UNIX__

  private:
#ifdef __UNIX__
    /*
    This function is based on the example in the 'Reading Persona' article of 
    Edition 0.10 of `The GNU C Library Reference Manual'
  
    */

    bool GetGroups(void)
      {
      int nGroupCount = getgroups(0, NULL);
      gid_t * agid = (gid_t *) malloc(nGroupCount * sizeof(gid_t));
      int nGetGroupsResult = getgroups(nGroupCount, agid);
      if(nGetGroupsResult < 0)
        {
        free(agid);
        return false;
        }
      
      for(int i = 0; i< nGroupCount; i++)
        {
        gid_t gid = agid[i];
        gph[gid] = wxT("GROUP");
        }

      free(agid);
      return true;
      }
#endif // def __UNIX__

  // Members
  private:
#ifdef __UNIX__
    uid_t     uid;
    gid_t     gid;
    GroupHash gph;
#endif // def __UNIX__
    wxString  sHome;
  };

#endif // ndef __Persona_h__
