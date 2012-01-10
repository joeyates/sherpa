///////////////////////////////////////////////////////////////////////////////
// Name:        SherpaEvent.cpp
// Purpose:     
// Author:      Joe Yates
// Modified by:
// Created:     11/10/2004
// RCS-ID:      $Id: SherpaEvent.cpp,v 1.1.1.1 2006/01/24 22:13:09 titusd Exp $
// Copyright:   (c) BID Snc
// License:     BSD license (see the file 'LICENSE.txt')
///////////////////////////////////////////////////////////////////////////////

#include "Sherpa.h"
#include "SherpaEvent.h"

wxEventType shEVT_SET_PATH = wxNewEventType();
wxEventType shEVT_RENAME = wxNewEventType();
wxEventType shEVT_DELETE = wxNewEventType();
wxEventType shEVT_TOWASTEBIN = wxNewEventType();
wxEventType shEVT_RESTORE = wxNewEventType();
wxEventType shEVT_CREATE = wxNewEventType();

shSetPathEvent::shSetPathEvent(const wxString& sPath) :
	m_sPath(sPath)
	{
	m_eventType = shEVT_SET_PATH;
	}

shRenameEvent::shRenameEvent(const wxString& sOldPathName, const wxString& sNewPathName) :
	m_sOldPathName(sOldPathName),
	m_sNewPathName(sNewPathName)
	{
	m_eventType = shEVT_RENAME;
	}

shDeleteEvent::shDeleteEvent(const wxString& sPathName) :
	m_sPathName(sPathName)
	{
	m_eventType = shEVT_DELETE;
	}

shToWastebinEvent::shToWastebinEvent(const wxString& sPathName) :
	m_sPathName(sPathName)
	{
	m_eventType = shEVT_TOWASTEBIN;
	}

shRestoreEvent::shRestoreEvent(const wxString& sPathName) :
	m_sPathName(sPathName)
	{
	m_eventType = shEVT_RESTORE;
	}

shCreateEvent::shCreateEvent(const wxString& sPath, const wxString& sFileType) :
	m_sPath(sPath),
	m_sFileType(sFileType)
	{
	m_eventType = shEVT_CREATE;
	}
