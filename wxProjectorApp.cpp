/***************************************************************
 * Name:      wxProjectorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Matthew Smaling (mjsmaling@hotmail.com)
 * Created:   2012-02-13
 * Copyright: Matthew Smaling (www.mjsmaling.com)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "wxProjectorApp.h"
#include "wxProjectorMain.h"
#include "include/wxAppPaths.h"
#include "include/globals.h"
#include "include/wxSongsManager.h"
#include "include/wxShowWindow.h"
#include "include/wxConfiguration.h"
#include "include/wxPicturesManager.h"
#include "include/mjsSharedBlob.h"
#include "include/wxCachedBitmaps.h"

#include <stdlib.h>

#include <wx/image.h>

IMPLEMENT_APP(wxProjectorApp);

static const wxChar *Application_Settings_Folder = wxT("wxProjector");

bool wxProjectorApp::OnInit()
{
    bool ret = true;
    // Initialize globals.
    InitGlobals();
#if 0
    // mjsSharedBlob test.
    mjsSharedBlob one, two, three;
    const char *testBlob = "ABCDEFG123456";
    const int textBlobLen = strlen(testBlob) + 1;
    one.SetData(testBlob, textBlobLen);
    two.SetData(one);
    three.SetData(two);
#endif

    // Load app configuration.
    LoadAppConfig();

    // Initialize Image Types.
    wxInitAllImageHandlers();

    srand(time(0));

    wxProjectorFrame* frame = new wxProjectorFrame(0L, _("wxProjector"));

    wxString last_filename = m_config->GetValue(wxT("last_filename"), wxT(""));

    frame->Show();

    if (!ret)
        CleanUpGlobals();

    return ret;
}

/*
    On Exit.
*/
int wxProjectorApp::OnExit()
{
    // clean up globals.
    CleanUpGlobals();

    return wxApp::OnExit();
}

/*
    This will return the application settings folder string.
*/
const wxString &wxProjectorApp::GetAppSettingsFolder() const
{
    return m_settingsPath;
}

/*
    Set application settings path.
*/
bool wxProjectorApp::SetApplicationSettingsPath()
{
    wxASSERT(g_app_paths != 0);

    m_settingsPath = g_app_paths->GetAppSettingsPath(Application_Settings_Folder);
    return g_app_paths->EnsurePathExists(m_settingsPath);
}

/*
    Returns the path of application settings file name.

    filename - the filename to determine the path to. The filename does not need to exist, this just generates
    an absolute file name.
*/
wxString wxProjectorApp::GetAppFilePath(const wxChar *filename) const
{
    wxString ret = m_settingsPath;
    ret.Append(filename);
    return ret;
}

/*
    Returns the app instance.
*/
wxProjectorApp *wxProjectorApp::GetAppInstance()
{
    return g_project_app;
}

/*
    Load application configuration.
*/
bool wxProjectorApp::LoadAppConfig()
{
    wxASSERT(g_configuration != 0);

 //   g_configuration->LoadConfiguration(AppConfigFilename());
    return true;
}

/*
    Save application configuration - if modified.
*/
bool wxProjectorApp::SaveAppConfig()
{
//    if (g_configuration->IsModified() == true)
 //       g_configuration->SaveConfiguration(AppConfigFilename());
    return true;
}

wxString wxProjectorApp::AppConfigFilename()
{
    return GetAppFilePath(wxT("settings.txt"));
}

/*
    Initialize global variables.
*/
void wxProjectorApp::InitGlobals()
{
    g_project_app = this;

    // Initialize and set the global apps object.
    g_app_paths = new wxAppPaths();
    SetApplicationSettingsPath();

    // Initialize
    m_songsManager = new wxSongsManager();
    m_songsManager->LoadOrCreateSongs(GetAppFilePath(wxT("songs_db.sqlite")));

    m_config = new wxConfiguration();
    m_config->LoadConfiguration(GetAppFilePath(wxT("config.txt")));
#if 0
    wxString db_write(wxT("db_write_counter"));
    if (g_configuration->GetInt(db_write, 0) > 0) {
        g_songs_manager->VacuumSongs();
        g_configuration->SetInt(db_write, 0);
    }
#endif

    m_picturesManager = new wxPicturesManager();
    m_picturesManager->AddByConfiguration(m_config);
    m_picturesManager->RandomizeFilenames();

    m_cachedBitmaps = new wxCachedBitmaps(m_songsManager);
}

/*
    Clean up global variables.
*/
void wxProjectorApp::CleanUpGlobals()
{
    if (m_cachedBitmaps != 0) {
        delete m_cachedBitmaps;
        m_cachedBitmaps = 0;
    }

    if (m_songsManager != 0) {
        delete m_songsManager;
        m_songsManager = 0;
    }

    if (m_picturesManager != 0) {
        delete m_picturesManager;
        m_picturesManager = 0;
    }

    if (m_config != 0) {
        if (m_config->IsModified() == true)
            m_config->SaveConfiguration(GetAppFilePath(wxT("config.txt")));
        delete m_config;
        m_config = 0;
    }

    if (g_app_paths != 0) {
        delete g_app_paths;
        g_app_paths = 0;
    }
    g_project_app = 0;

}


int wxProjectorApp::FilterEvent(wxEvent &event)
{
    #if 0
    int event_type = event.GetEventType();
    if ( (event_type == wxEVT_KEY_DOWN || event_type == wxEVT_KEY_UP) && event.GetEventObject() == g_show_window ) {
            g_show_window->GetEventHandler()->ProcessEvent(event);
            return true;
    }
    #endif
    return -1;
}
