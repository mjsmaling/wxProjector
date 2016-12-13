#include "../include/wxSongsManager.h"
#include "../include/wxSqlitePrepare.h"

#include <wx/filefn.h>

wxSongsManager *g_songs_manager = 0;

wxSongsManager::wxSongsManager()
{
    if (g_songs_manager == 0)
        g_songs_manager = this;
}

wxSongsManager::~wxSongsManager()
{
    //dtor
    if (g_songs_manager == this)
        g_songs_manager = 0;
}


// Create songs file.
bool wxSongsManager::CreateSongs(const wxString &filename)
{
    // Create the database.
    bool ret = CreateDatabase(filename);
    if (ret)
        InitDatabase();
    return ret;
}

// Load songs file.
bool wxSongsManager::LoadSongs(const wxString &filename)
{
    bool ret = OpenDatabase(filename);
    if (ret)
        InitDatabase();
    return ret;
}

void wxSongsManager::InitDatabase()
{
    /* Create tables now... */
    CreateTable(wxT("songs"), wxT("name TEXT, contents TEXT, image_url TEXT"));
    CreateIndex(wxT("name"), wxT("songs"));

    /* Create session data */
    CreateTable(wxT("session"), wxT("name TEXT, data TEXT"));
    CreateIndex(wxT("name"), wxT("session"));

    /* Create session data */
    CreateTable(wxT("service_names"), wxT("name TEXT"));
    CreateIndex(wxT("name"), wxT("service_names"));

    /* */
    CreateTable(wxT("service_songs"), wxT("service_names_id INTEGER, song_id INTEGER, order_no INTEGER, extra_data TEXT"));
    CreateIndex(wxT("service_names_id"), wxT("service_songs"));

    /* Create bitmap cache table */
    CreateTable(wxT("bitmap_cache"), wxT("name TEXT, bitmap_data BLOB, timestamp INT"));
    CreateIndex(wxT("name"), wxT("bitmap_cache"));

}

// Load or create songs file.
bool wxSongsManager::LoadOrCreateSongs(const wxString &filename)
{
    if (wxFileExists(filename))
        return LoadSongs(filename);
    return CreateSongs(filename);
}

// Save songs someplace else.
bool wxSongsManager::SaveSongs(const wxString &filename)
{
    return false;
}

// Clean up wasted space.
void wxSongsManager::VacuumSongs()
{
    Vacuum();
}

int wxSongsManager::GetSongByRowId(long row_id, wxSongData *dest)
{
    wxString sql(wxT("SELECT rowid,name,contents FROM songs where rowid=?001"));

    wxSqlitePrepare prepare(this, sql);
    prepare.BindParameter(1, (int) row_id);

    wxSqliteResult result;
    PerformSql(&prepare, &result);

    if (result.GetCount() > 0) {
        dest->SetRowId(result.GetDataInt(0, 0));
        dest->SetTitle(result.GetDataString(0, 1));
        dest->SetContents(result.GetDataString(0, 2));
    }

    return result.GetCount();
}

// Search songs by title.
int wxSongsManager::SearchSongs(const wxString &title, int limit, std::vector<wxSongData> *results)
{
    wxSqlitePrepare *prepare;//(this);
#if 0
    std::string sql("SELECT rowid, name, contents FROM songs");
    if (title.Len() > 0) {
    }
#endif
    wxString sql(wxT("SELECT rowid,name,contents FROM songs"));

    if (title.Len() > 0) {
        sql << wxT(" WHERE name LIKE ?001 ORDER BY (name)");

        prepare = new wxSqlitePrepare(this, sql);

        wxString cpy;
        cpy << wxT("%") << title << wxT("%");
        prepare->BindParameter(1, cpy);
    } else {
        sql.Append(wxT(" ORDER BY (name)"));

        prepare = new wxSqlitePrepare(this, sql);
    }

    assert(prepare != 0);

    wxSqliteResult result; //PerformSql(const wxString &sql, wxSqliteResult *result)

    PerformSql(prepare, &result);

    for (int x = 0; x < result.GetCount(); x++) {
        wxSongData song;
        song.SetRowId(result.GetDataInt(x, 0));
        song.SetTitle(result.GetDataString(x, 1));
        song.SetContents(result.GetDataString(x, 2));
        results->push_back(song);
    }

    delete prepare;

    return result.GetCount();
}

// Count songs.
int wxSongsManager::CountSongs()
{
    return 0;
}

// Get songs from to...
void wxSongsManager::GetSongs(int from, int to)
{

}

// Get song from... limit.
void wxSongsManager::GetSongsLimit(int from, int limit)
{

}

/*
    Add song to song maanger
*/
void wxSongsManager::AddSong(const wxSongData &song)
{
    // Database must be open.
    wxASSERT(IsOpen() == true);

  //  wxSqlitePrepare prepare()

    wxSqlitePrepare prepare(this, wxT("INSERT INTO songs (name, contents) VALUES(?001, ?002);"));

    prepare.BindParameter(1, song.GetTitle());
    prepare.BindParameter(2, song.GetContents());

    PerformSql(&prepare, 0);
}

int wxSongsManager::UpdateSong(const wxSongData &song)
{
    wxASSERT(IsOpen() == true);

    wxSqlitePrepare prepare(this, wxT("UPDATE songs SET name=?001, contents=?002 WHERE rowid=?003"));

    prepare.BindParameter(1, song.GetTitle());
    prepare.BindParameter(2, song.GetContents());
    prepare.BindParameter(3, (int) song.GetRowId());

    PerformSql(&prepare, 0);

    return 0;
}

int wxSongsManager::DeleteSong(const wxSongData &song)
{
    wxASSERT(IsOpen() == true);

    wxSqlitePrepare prepare(this, wxT("DELETE FROM songs WHERE rowid=?001"));
    prepare.BindParameter(1, song.GetRowId());

    PerformSql(&prepare, 0);

    return 0;
}
