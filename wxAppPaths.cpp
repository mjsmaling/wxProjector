#include "../include/wxAppPaths.h"

#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/tokenzr.h>

/*
    Constructor.
*/
wxAppPaths::wxAppPaths()
{
    // Set the default application settings path.
    SetAppSettingsPath(wxEmptyString);
}

wxAppPaths::~wxAppPaths()
{
    //dtor
}


/* This will return the application settings path. */
wxString wxAppPaths::GetAppSettingsPath(const wxChar *subPath, bool endWithSeparator) const
{
    wxString ret(m_settingsPath), subpath(subPath);

    if (subpath.IsEmpty() == false) {
        ret.Append(wxFileName::GetPathSeparator());
        ret.Append(subpath);
    }

    if (endWithSeparator == true)
        ret.Append(wxFileName::GetPathSeparator());

    return ret;
}

/* This will set the application settings path. */
void wxAppPaths::SetAppSettingsPath(const wxString &path, bool ensure_exists)
{
    // Set the path.
    if (path.IsEmpty() == true) {
        // set default path.
        m_settingsPath = wxFileName::GetHomeDir();
        m_settingsPath.Append(wxFileName::GetPathSeparator());
        m_settingsPath.Append(wxT("mjs_application_settings"));
    } else {
        m_settingsPath = path;
    }

    // if the end of the path is a dir separator, then remove it.
    while (m_settingsPath.Last() == wxFileName::GetPathSeparator())
        m_settingsPath = m_settingsPath.Left(m_settingsPath.Length() - 1);

    if (ensure_exists)
        EnsurePathExists(m_settingsPath);
}

/*
    This will ensure that 'path' exists. It will create all directories - if allowed
    by the user - leading up to path.

    Will return true / false upon success / failure.
*/
bool wxAppPaths::EnsurePathExists(const wxString &path)
{
    /* Go through all the 'directory separators' seeing if they exist, or create them. */
    wxStringTokenizer token(path, wxFileName::GetPathSeparator());

    wxString current;

    while (token.HasMoreTokens() == true) {
        current.Append(token.GetNextToken());
        current.Append(wxFileName::GetPathSeparator());
        if (!wxDirExists(current)) {
            if (!wxMkdir(current))
                return false;
        }
    }

    return true;
}
