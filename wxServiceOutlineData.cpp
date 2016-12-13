#include "../include/wxServiceOutlineData.h"

#include "../include/wxSqlitePrepare.h"
#include "../include/wxSqliteDatabase.h"


wxServiceOutlineData::wxServiceOutlineData(const wxServiceOutlineData &src)
{
    SetData(src);
}
wxServiceOutlineData::wxServiceOutlineData()
{

}

wxServiceOutlineData::~wxServiceOutlineData()
{

}

void wxServiceOutlineData::SetData(const wxServiceOutlineData &src)
{
    m_songs = src.m_songs;
    m_colours = src.m_colours;
    m_fonts = src.m_fonts;
}

wxServiceOutlineData &wxServiceOutlineData::operator = (const wxServiceOutlineData &src)
{
    SetData(src);
    return *this;
}

void wxServiceOutlineData::AddSong(const wxSongData &src)
{
    m_songs.push_back(src);
}

void wxServiceOutlineData::RemoveSong(int index)
{
    if (SongCount() > index)
        m_songs.erase(m_songs.begin() + index);
}

void wxServiceOutlineData::InsertSong(const wxSongData &src, int position)
{
    if (position > SongCount())
        AddSong(src);
    else
        m_songs.insert(m_songs.begin() + position, src);
}

void wxServiceOutlineData::SetSong(int index, const wxSongData &src)
{
    m_songs[index] = src;
}

/*
    This will save the current song data.
*/
void wxServiceOutlineData::SaveSongData(const wxString &name, wxSongsManager &datasource) const
{
    int service_id = GetServiceNameId(&datasource, name, true);
    if (service_id == 0)
        return;

    wxString sql;
    sql << wxT("DELETE FROM service_songs WHERE service_names_id=") << service_id;
    datasource.PerformSql(sql, 0);

    for (int x = 0; x < SongCount(); x++) {
        wxSongData song = m_songs.at(x);
        InsertSongData(song, x, service_id, &datasource);
    }
}

int wxServiceOutlineData::InsertSongData(const wxSongData &song, int order_no, int service_names_rowid, wxSongsManager *datasource) const
{
    wxSqlitePrepare prepare(datasource, wxT("INSERT INTO service_songs (song_id, order_no, service_names_id) VALUES(?001, ?002, ?003)"));
    prepare.BindParameter(1, song.GetRowId());
    prepare.BindParameter(2, order_no);
    prepare.BindParameter(3, service_names_rowid);
//    prepare.BindParameter(1, song.GetTitle());
  //  prepare.BindParameter(2, song.GetContents());
    return datasource->PerformSql(&prepare, 0);
}

int wxServiceOutlineData::UpdateSongData(int rowid, const wxSongData &song, int order_no, int service_names_rowid, wxSongsManager *datasource) const
{
    wxSqlitePrepare prepare(datasource, wxT("UDPATE service_songs SET song_id=?001, order_no=?002, service_names_id=?003 WHERE rowid=?004"));
    prepare.BindParameter(1, song.GetRowId());
    prepare.BindParameter(2, order_no);
    prepare.BindParameter(3, service_names_rowid);
    prepare.BindParameter(4, rowid);
    return datasource->PerformSql(&prepare, 0);
}

int wxServiceOutlineData::SearchSongData(int song_id, int session_id, int order_no, wxSongsManager *datasource) const
{
    wxSqlitePrepare prepare(datasource, wxT("SELECT COUNT(*) FROM service_songs WHERE song_id=?001 AND service_names_id=?002 AND order_no=?003"));

    prepare.BindParameter(1, song_id);
    prepare.BindParameter(2, session_id);
    prepare.BindParameter(3, order_no);
    wxSqliteResult result;
    datasource->PerformSql(&prepare, &result);

    return result.GetDataInt(0, 0, 0);
}

/*
    This will load the song data, note: this will erase the current contents in this class.
*/
void wxServiceOutlineData::LoadSongData(const wxString &name, wxSongsManager &datasource)
{
    Clear();

    int service_id = GetServiceNameId(&datasource, name, false);

    /* It doesn't exist */
    if (service_id == 0)
        return;

    wxSqlitePrepare prepare(&datasource, wxT("SELECT song_id, order_no, extra_data FROM service_songs WHERE service_names_id=?001 ORDER BY (order_no)"));
    prepare.BindParameter(1, service_id);
// service_names_id INTEGER, song_id INTEGER, order_no INTEGER, extra_data
    wxSqliteResult result;
    datasource.PerformSql(&prepare, &result);

    for (int x = 0; x < result.GetCount(); x++) {
        wxSongData song; // GetDataInt(int row, int column) const
        if (datasource.GetSongByRowId(result.GetDataInt(x, 0), &song))
            AddSong(song);
    }
}

void wxServiceOutlineData::Clear()
{
    m_songs.clear();
}

int wxServiceOutlineData::GetServiceNameId(wxSongsManager *datasource, const wxString &name, bool createIfNotExists) const
{
    wxASSERT(datasource != 0);

    wxSqlitePrepare prepare(datasource, wxT("SELECT rowid FROM service_names WHERE name=?001"));
    prepare.BindParameter(1, name);

    wxSqliteResult result;
    datasource->PerformSql(&prepare, &result);

    if (result.GetCount() == 0) {
        if (createIfNotExists == true) {
            CreateServiceName(datasource, name);
            return GetServiceNameId(datasource, name, false);
        }
        return 0;
    }
    return result.GetDataInt(0, 0);
}

int wxServiceOutlineData::CreateServiceName(wxSongsManager *datasource, const wxString &name) const
{
    wxSqlitePrepare prepare(datasource, wxT("INSERT INTO service_names (name) VALUES(?001)"));
    prepare.BindParameter(1, name);

    return datasource->PerformSql(&prepare, 0);
}

