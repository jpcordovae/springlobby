/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: UpdaterApp
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
#include <wx/utils.h>
#include <wx/wfstream.h>

#include "updaterapp.h"
#include "../settings.h"
#include "../crashreport.h"
#include "../utils/platform.h"
#include "../utils/customdialogs.h"
#include "updater.h"
#include "../globalsmanager.h"

#include "versionchecker.h"
#include "updatermainwindow.h"
#include "../defines.h"

const unsigned int TIMER_ID         = 101;
const unsigned int TIMER_INTERVAL   = 100;


IMPLEMENT_APP(UpdaterApp)

BEGIN_EVENT_TABLE(UpdaterApp, wxApp)

    EVT_TIMER(TIMER_ID, UpdaterApp::OnTimer)


END_EVENT_TABLE()

UpdaterApp::UpdaterApp()
    : 	m_timer ( new wxTimer(this, TIMER_ID) ),
    m_updater_window( 0 ),
	m_version( _T("-1") )
{
    SetAppName( _T("springlobby_updater") );
}

UpdaterApp::~UpdaterApp()
{
    delete m_timer;
}


//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool UpdaterApp::OnInit()
{
    //this triggers the Cli Parser amongst other stuff
    if (!wxApp::OnInit())
        return false;

#if wxUSE_ON_FATAL_EXCEPTION && !defined(__WXMAC__)
    if (!m_crash_handle_disable) wxHandleFatalExceptions( true );
#endif

    //this needs to called _before_ mainwindow instance is created
    wxInitAllImageHandlers();
     //TODO needed?
    wxImage::AddHandler(new wxPNGHandler);
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxSocketBase::Initialize();

    m_timer->Start( TIMER_INTERVAL );

	if( m_version == _T("-1") )
		m_version = GetLatestVersion();
	
	m_updater_window = new UpdaterMainwindow( m_version );
	m_updater_window->Show( true );
	SetTopWindow( m_updater_window );
	Updater().StartUpdate( m_version, m_exe_to_update );
	return true;
}


//! @brief Finalizes the application
int UpdaterApp::OnExit()
{
  	m_timer->Stop();

  	sett().SaveSettings(); // to make sure that cache path gets saved before destroying unitsync

    SetEvtHandlerEnabled(false);
    DestroyGlobals();

    return 0;
}

//! @brief is called when the app crashes
void UpdaterApp::OnFatalException()
{
#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
    crashreport().GenerateReport(wxDebugReport::Context_Exception);
#else
    wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
#endif
}


//! @brief Is called every 1/10 seconds to update statuses
void UpdaterApp::OnTimer( wxTimerEvent& event )
{

}

void UpdaterApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    #ifndef HAVE_WX29
        #define STR _T
    #else
        #define STR
    #endif

    wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, STR("h"), STR("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_OPTION, STR("f"), STR("target-exe"),  _("the SpringLobby executeable to be updated"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY | wxCMD_LINE_NEEDS_SEPARATOR },
		{ wxCMD_LINE_OPTION, STR("r"), STR("target-rev"),  _("the SpringLobby revision to update to"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY | wxCMD_LINE_NEEDS_SEPARATOR },
        { wxCMD_LINE_NONE }//while this throws warnings, it is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html
    };

    parser.SetDesc( cmdLineDesc );
    parser.SetSwitchChars (_T("-"));

    #undef STR
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool UpdaterApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	#if wxUSE_CMDLINE_PARSER
        if ( parser.Found(_T("help")) )
            return false; // not a syntax error, but program should stop if user asked for command line usage
		if ( parser.Found(_T("target-exe"), &m_exe_to_update ) && parser.Found(_T("target-rev"),  &m_version) )
			return true;
	#endif
        return true;
}
