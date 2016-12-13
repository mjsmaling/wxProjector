#include "../include/wxSongData.h"

wxSongData::wxSongData(const wxSongData &src)
{
    SetData(src);
}

wxSongData::wxSongData()
{
    //ctor
    m_row_id = -1;
}

wxSongData::wxSongData(const wxString &title, const wxString &contents, int id)
{
    m_title = title;
    m_contents = contents;
    m_row_id = id;
}

wxSongData::~wxSongData()
{
    //dtor
}

wxSongData &wxSongData::operator = (const wxSongData &src)
{
    SetData(src);
    return *this;
}

void wxSongData::SetData(const wxSongData &src)
{
    m_title = src.m_title;
    m_contents = src.m_contents;
    m_row_id = src.m_row_id;
#if 0
    m_contentPages = src.m_contentPages;

    m_font = src.m_font;
    m_titleFont = src.m_titleFont;
    m_fontColour = src.m_fontColour;
    m_backColour = src.m_backColour;
#endif
}
