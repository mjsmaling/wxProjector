/***************************************************************
 * Name:      wxProjectorApp.h
 * Purpose:   Defines Application Class
 * Author:    Matthew Smaling (mjsmaling@hotmail.com)
 * Created:   2012-02-13
 * Copyright: Matthew Smaling (www.mjsmaling.com)
 * License:
 **************************************************************/

#ifndef WXPROJECTORAPP_H
#define WXPROJECTORAPP_H

#include <wx/app.h>
#include <wx/string.h>

#include "include/wxSqliteDatabase.h"
#include "include/wxAppPaths.h"

class wxConfiguration;
class wxSongsManager;
class wxPicturesManager;
class wxCachedBitmaps;

class wxProjectorApp : public wxApp
{
public:

    virtual bool OnInit();

    virtual int OnExit();

    /* Will return the application settings folder */
    const wxString &GetAppSettingsFolder() const;

    wxString GetAppFilePath(const wxChar *filename) const;

    static wxString AppSettingsFolder();
    static wxString AppFilePath(const wxChar *filename);

    // Generates a config filename.
    wxString AppConfigFilename();

    static wxProjectorApp *GetAppInstance();
protected:
    bool SetApplicationSettingsPath();

    bool LoadAppConfig();
    bool SaveAppConfig();

    void InitGlobals();
    void CleanUpGlobals();

    int FilterEvent(wxEvent &event);

private:
    // Application settings path.
    wxString m_settingsPath;

    wxConfiguration *m_config;
    wxSongsManager *m_songsManager;
    wxPicturesManager *m_picturesManager;
    wxCachedBitmaps *m_cachedBitmaps;
};

#endif // WXPROJECTORAPP_H
