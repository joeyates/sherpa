///////////////////////////////////////////////////////////////////////////////
// Name:        SherpaEvent.h
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: SherpaEvent.h,v 1.1.1.1 2006/01/24 22:13:19 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#ifndef __SherpaEvent_h__
#define __SherpaEvent_h__

// Event IDs
enum
  {
	// Menu Ids
  shMENUID_FILE_EXIT,
	shMENUID_FILE_EMPTY_WASTEBIN,
  shMENUID_EDIT_UNDO,
  shMENUID_EDIT_REDO,
  shMENUID_EDIT_PREFERENCES,
  shMENUID_VIEW_SHOWHIDDEN,
	shMENUID_DEBUG_WASTEBIN,
	shMENUID_DEBUG_UNDO,
	shMENUID_DELETE,
	shMENUID_TOWASTEBIN,
  shMENUID_NEW_DIRECTORY,
  shMENUID_NEW_FILE
  };

extern wxEventType shEVT_SET_PATH;
extern wxEventType shEVT_RENAME;
extern wxEventType shEVT_DELETE;
extern wxEventType shEVT_TOWASTEBIN;
extern wxEventType shEVT_RESTORE;
extern wxEventType shEVT_CREATE;

class shSetPathEvent: public wxCommandEvent
	{
	public:
		shSetPathEvent(const wxString& sPath);
	
	public:
		// accessors
		wxString GetPath()
			{return m_sPath;}
	
	private:
		wxString m_sPath;
	};

class shRenameEvent: public wxCommandEvent
	{
	public:
		shRenameEvent(const wxString& sOldPathName, const wxString& sNewPathName);
	
	public:
		// accessors
		wxString GetOldPathName()
			{return m_sOldPathName;}
		wxString GetNewPathName()
			{return m_sNewPathName;}
	
	private:
		wxString m_sOldPathName;
		wxString m_sNewPathName;
	};

class shDeleteEvent: public wxCommandEvent
	{
	public:
		shDeleteEvent(const wxString& sPathName);
	
	public:
		// accessors
		wxString GetPathName()
			{return m_sPathName;}
	
	private:
		wxString m_sPathName;
	};

class shToWastebinEvent: public wxCommandEvent
	{
	public:
		shToWastebinEvent(const wxString& sPathName);
	
	public:
		// accessors
		wxString GetPathName()
			{return m_sPathName;}
	
	private:
		wxString m_sPathName;
	};

class shRestoreEvent: public wxCommandEvent
	{
	public:
		shRestoreEvent(const wxString& sPathName);
	
	public:
		// accessors
		wxString GetPathName()
			{return m_sPathName;}
	
	private:
		wxString m_sPathName;
	};

class shCreateEvent: public wxCommandEvent
	{
	public:
		shCreateEvent(const wxString& sPath, const wxString& sFileType);
	
	public:
		// accessors
		wxString GetPath()
			{return m_sPath;}
		wxString GetFileType()
			{return m_sFileType;}
	
	private:
		wxString m_sPath;
		wxString m_sFileType;
	};

typedef void (wxEvtHandler::*shSetPathEventFunction)(shSetPathEvent&);
typedef void (wxEvtHandler::*shRenameEventFunction)(shRenameEvent&);
typedef void (wxEvtHandler::*shDeleteEventFunction)(shDeleteEvent&);
typedef void (wxEvtHandler::*shToWastebinEventFunction)(shToWastebinEvent&);
typedef void (wxEvtHandler::*shRestoreEventFunction)(shRestoreEvent&);
typedef void (wxEvtHandler::*shCreateEventFunction)(shCreateEvent&);

#define EVT_SET_PATH(fn) \
	wxEventTableEntry(shEVT_SET_PATH, -1, -1, \
	(wxObjectEventFunction) (wxEventFunction) (shSetPathEventFunction) &fn, (wxObject *) NULL ),

#define EVT_RENAME(fn) \
	wxEventTableEntry(shEVT_RENAME, -1, -1, \
	(wxObjectEventFunction) (wxEventFunction) (shRenameEventFunction) &fn, (wxObject *) NULL ),

#define EVT_DELETE(fn) \
	wxEventTableEntry(shEVT_DELETE, -1, -1, \
	(wxObjectEventFunction) (wxEventFunction) (shDeleteEventFunction) &fn, (wxObject *) NULL ),

#define EVT_TOWASTEBIN(fn) \
	wxEventTableEntry(shEVT_TOWASTEBIN, -1, -1, \
	(wxObjectEventFunction) (wxEventFunction) (shToWastebinEventFunction) &fn, (wxObject *) NULL ),

#define EVT_RESTORE(fn) \
	wxEventTableEntry(shEVT_RESTORE, -1, -1, \
	(wxObjectEventFunction) (wxEventFunction) (shRestoreEventFunction) &fn, (wxObject *) NULL ),

#define EVT_CREATE(fn) \
	wxEventTableEntry(shEVT_CREATE, -1, -1, \
	(wxObjectEventFunction) (wxEventFunction) (shCreateEventFunction) &fn, (wxObject *) NULL ),

#endif // ndef __SherpaEvent_h__
