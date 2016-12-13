#include "../include/wxSongList.h"
#include "../include/globals.h"
#include "../include/wxSongData.h"
#include "../include/wxSongsManager.h"
#include "../include/string_func.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dcclient.h>

#include <vector>

DEFINE_EVENT_TYPE(wxSONGLIST_SELECTED);

int wxSongList::ID_SEARCH_TEXT = wxNewId();
int wxSongList::ID_SONG_LIST = wxNewId();
int wxSongList::ID_SEARCH_BUTTON = wxNewId();

BEGIN_EVENT_TABLE(wxSongList, wxWindow)
    EVT_SIZE(wxSongList::OnSizeEvent)
    EVT_PAINT(wxSongList::OnPaintEvent)

    EVT_TEXT(ID_SEARCH_TEXT, wxSongList::OnSearchTextChange)
    EVT_LISTBOX(ID_SONG_LIST, wxSongList::OnSongSelected)
    EVT_LISTBOX_DCLICK(ID_SONG_LIST, wxSongList::OnSongDoubleSelected)

    EVT_BUTTON(ID_SEARCH_BUTTON, wxSongList::OnSearchButtonClick)
END_EVENT_TABLE()

wxSongList::wxSongList(wxWindow* parent, wxWindowID id, const wxPoint& pos,
               const wxSize& size, long style, const wxString& name)
: wxWindow(parent, id, pos, size, style, name)
{
    //ctor
    CreateGui();
}

wxSongList::~wxSongList()
{
    //dtor
}


void wxSongList::CreateGui()
{
    /* Create a layout... */
    wxBoxSizer *layout = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer *searchBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Search"));

    m_searchTextCtrl = new wxTextCtrl(this, ID_SEARCH_TEXT);
    searchBox->Add(m_searchTextCtrl, 0, wxALL | wxALIGN_CENTRE | wxEXPAND, 2);
    searchBox->Add(new wxButton(this, ID_SEARCH_BUTTON, wxT("Search")));

    layout->Add(searchBox, 0, wxEXPAND | wxALL, 2);
    // add push button.
    /* (wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "button")*/

    m_songsListBox = new wxListBox(this, ID_SONG_LIST);
    layout->Add(m_songsListBox, 4, wxEXPAND | wxALL, 2);


    // Get a list of songs now??
    if (g_songs_manager != 0)
        PopulateList(wxEmptyString, 100);

    SetSizer(layout);
    //wxListBox *m_songsListBox;
    //wxTextCtrl *m_searchTextCtrl;

 //   layout->SetSizeHints( this );
}

void wxSongList::PopulateList(const wxString &search, int limit)
{
    wxASSERT(g_songs_manager != 0);

    std::vector<wxSongData> songs;
    g_songs_manager->SearchSongs(search, limit, &songs);

    m_songsListBox->Clear();

    for (int x = 0; x < songs.size(); x++) {
        wxString item;
        wxSongData song = songs.at(x);
        item = song.GetTitle();
        item << wxT(" (");
        item << wxString::Format(wxT("%d"), song.GetRowId());
        item << wxT(")");
        m_songsListBox->Append(item);
    }
}

void wxSongList::OnSizeEvent(wxSizeEvent &event)
{
    /*
        Sizer fix it... have to force reszie the layout.
    */
    wxSizer *sizer = this->GetSizer();
    wxSize sz = GetSize();

    sizer->SetDimension(0, 0, sz.GetWidth(), sz.GetHeight());
    sizer->Layout();

    event.Skip();
}

void wxSongList::OnPaintEvent(wxPaintEvent &event)
{
    wxPaintDC dc(this);
#if 0
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxBLACK_BRUSH);

    wxSize sz = GetSize();
    dc.DrawRectangle(0, 0, sz.GetWidth(), sz.GetHeight());
#endif
    event.Skip();
}

void wxSongList::OnSearchTextChange(wxCommandEvent &event)
{
    event.Skip();

    RefreshList();
}

/*
    This will return the current song's record id.
    This may return 0, if none selected.
*/
long wxSongList::GetCurrentSongSelectedId() const
{
    long ret;
    int selection = m_songsListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        wxString title = m_songsListBox->GetString(selection);

        wxString number = get_last_mid_string(title, wxT('('), wxT(')'));

        if (!number.ToLong(&ret))
            ret = 0;
    } else {
        ret = 0;
    }
    return ret;
}

bool wxSongList::GetCurrentSelectedSong(wxSongData *dest) const
{
    int rowid = GetCurrentSongSelectedId();
    if (rowid < 1)
        return false;
    if (dest != 0) {
        g_songs_manager->GetSongByRowId((long) rowid, dest);
        dest->SetRowId(rowid);
    }
    return true;
}

void wxSongList::RefreshList()
{
    wxString query = m_searchTextCtrl->GetValue();
    query.Trim(true).Trim(false);
    PopulateList(query);
}
void wxSongList::OnSongSelected(wxCommandEvent &event)
{
    EmitEvent(GetCurrentSongSelectedId(), 0);
}
void wxSongList::OnSongDoubleSelected(wxCommandEvent &event)
{
    EmitEvent(GetCurrentSongSelectedId(), 1);
}
void wxSongList::EmitEvent(int song_id, int double_click)
{
    wxCommandEvent event(wxSONGLIST_SELECTED, GetId());
    event.SetEventObject(this);
    event.SetExtraLong(song_id);
    event.SetInt(double_click);
    GetEventHandler()->ProcessEvent(event);
}

void wxSongList::OnSearchButtonClick(wxCommandEvent &event)
{
    // Perform Search.
    sqlite3 *db = g_songs_maanger->access_db();
    wxASSERT(db != 0);

    // Perform Search.
    //sql::string sql("SELECT * FROM songs WHERE song LIKE ");

}
