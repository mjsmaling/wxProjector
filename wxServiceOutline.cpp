#include "../include/wxServiceOutline.h"
#include "../include/common.h"
#include "../include/graphics.h"
#include "../include/globals.h"
#include "../include/wxConfiguration.h"
#include "../include/wxSongRenderer.h"

#include <wx/wx.h>
#include <wx/dcclient.h>
#include <wx/settings.h>

#include <stdlib.h>

BEGIN_EVENT_TABLE(wxServiceOutline, wxWindow)
    EVT_PAINT(wxServiceOutline::OnPaintEvent)

    EVT_MOTION(wxServiceOutline::OnMouseMotionEvent)

    EVT_LEFT_DOWN(wxServiceOutline::OnMouseButtonLeft)
    EVT_LEFT_UP(wxServiceOutline::OnMouseButtonLeft)
    EVT_LEFT_DCLICK(wxServiceOutline::OnMouseButtonDouble)

    EVT_SIZE(wxServiceOutline::OnSizeEvent)

 //   EVT_CLOSE(wxServiceOutline::OnCloseEvent)
END_EVENT_TABLE()

wxServiceOutline::wxServiceOutline(wxWindow* parent, wxWindowID id, const wxPoint& pos,
               const wxSize& size, long style, const wxString& name)
: wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{
    //ctor
    m_calculateFlag = true;

    m_columns = 5;
    m_rows = 1;

    m_selected_box_index = -1;
    m_current_hover_box_index = -1;

    if (g_songs_manager != 0)
        m_data.LoadSongData(wxT("current_session"), *g_songs_manager);
}

wxServiceOutline::~wxServiceOutline()
{
    if (g_songs_manager != 0)
        m_data.SaveSongData(wxT("current_session"), *g_songs_manager);
}

void wxServiceOutline::OnPaintEvent(wxPaintEvent &event)
{
    wxPaintDC dc(this);

    if (m_calculateFlag == true)
        CalculatePoints();
#if 0
    float rnd = (float) 255 / (float) RAND_MAX;

    dc.SetBrush(wxBrush(wxColour((float)rand() * rnd, (float) rand() * rnd, (float) rand() * rnd)));

    wxSize sz = GetSize();
    dc.DrawRectangle(0, 0, sz.GetWidth(), sz.GetHeight());
#endif
    wxSongRenderer renderer;
    renderer.SetByConfiguration(g_configuration);

    int c = m_data.SongCount();

    for (int x = 0; x < c; x++)
        RenderBox(x, &dc, &renderer, true);
}

void wxServiceOutline::RenderBox(int index)
{
    wxClientDC dc(this);
    wxSongRenderer renderer;
    renderer.SetByConfiguration(g_configuration);

    RenderBox(index, &dc, &renderer, false);
}

void wxServiceOutline::RenderBox(int index, wxDC *dc, wxSongRenderer *renderer, bool clearBackground)
{
    if (index < 0 || index >= SongCount())
        return;

    wxRect rect(DetermineBoxRect(index));

    wxSongData song;
    if (index >= 0 && index < m_data.SongCount())
        song = m_data.GetSong(index);

    renderer->SetPictureIndex(index);
    renderer->SetTitle(song.GetTitle());
    renderer->SetContent(song.GetContents());

    // set back colour...
    if (clearBackground == true) {
        wxColour bgcolour = g_configuration->GetColour(wxT("back_colour"));
        int red = 0, green = 0, blue = 0;

        if (m_current_hover_box_index == index) {
            red += 10;
            green += 10;
            blue += 10;
        }
        if (m_selected_box_index == index) {
            red += 10;
            green += 10;
            blue -= 10;
        }

        bgcolour.Set( (bgcolour.Red() + red) % 256, (bgcolour.Green() + green) % 256, (bgcolour.Blue() + blue) % 256 );

        renderer->SetBackColour(bgcolour);
    }

    renderer->Render(*dc, rect, clearBackground);
#if 0

    /* Get Text And Render */
    if (index >= 0 && index < m_data.SongCount()) {
        wxFontSet fontset = g_configuration->GetFonts();
        wxFont titleFont, contentsFont;
        titleFont = fontset.GetTitleFont();
        contentsFont = fontset.GetContentsFont();

        wxSongData song = m_data.GetSong(index);

        wxString title = song.GetTitle();
        wxString contents = song.GetContents();

         wxSize sz = rect.GetSize();
        wxRect winrect = rect;

        /* generate bitmaps */
        wxBitmap bitmap = generate_text_image(title, 0, &titleFont, &fgcolor, &bgcolor);
        bitmap = resize_if_bigger(bitmap , sz, true);

        //rect.SetTop(m_titleBitmap.GetHeight() + 1);
        rect.SetHeight(rect.GetHeight() - (bitmap.GetHeight() + 5));

         int x1 = 0;
            if (bitmap.IsOk()) {
                // draw in center...
                x1 = (sz.GetWidth() - bitmap.GetWidth()) / 2 + winrect.GetLeft();
                dc->DrawBitmap(bitmap, x1, winrect.GetTop());
                winrect.SetTop(bitmap.GetHeight() + winrect.GetTop());
                winrect.SetHeight(winrect.GetHeight() - bitmap.GetHeight());
            }

        bitmap = generate_text_image(contents, 0, &contentsFont, &fgcolor, &bgcolor);
        bitmap = resize_if_bigger(bitmap , winrect.GetSize(), true);

            if (bitmap.IsOk()) {
            // resize bitmap contents...
                int y1;
                x1 = (winrect.GetWidth() - bitmap.GetWidth()) / 2 + winrect.GetLeft();
                y1 = (winrect.GetHeight() - bitmap.GetHeight()) / 2 + winrect.GetTop();

                dc->DrawBitmap(bitmap, x1, y1);
        }
    }
#endif
}

/*
*/
void wxServiceOutline::CalculatePoints()
{
    m_calculateFlag = false;

    wxSize winsize = GetSize();
    m_avg_width = (float) winsize.GetWidth() / (float) m_columns;

    /* */
    m_rows = (m_data.SongCount() + m_columns-1)/ m_columns;

    m_avg_height = (float) winsize.GetHeight() / (float) m_rows;
}

void wxServiceOutline::SetData(const wxServiceOutlineData &src)
{
    m_data = src;
}

void wxServiceOutline::GetData(wxServiceOutlineData *dest) const
{
    if (dest == 0)
        return;
    *dest = m_data;
}

void wxServiceOutline::AddSong(const wxSongData &song, int index)
{
    if (index < 0)
        m_data.AddSong(song);
    else
        m_data.InsertSong(song, index);

    CalculateRefresh();
}

void wxServiceOutline::RemoveSong(int index)
{
    m_data.RemoveSong(index);
    CalculateRefresh();
}

void wxServiceOutline::CalculateRefresh()
{
    m_calculateFlag = true;
    Refresh(true);
}

/*
    Catch mouse movement.
*/
void wxServiceOutline::OnMouseMotionEvent(wxMouseEvent &event)
{
    // TODO: Detect if the mouse has hovered to another box.
    event.Skip();

    int hover = DetermineBoxIndex(event.m_x, event.m_y);
    if (hover != m_current_hover_box_index) {
#if 1
        int copy = m_current_hover_box_index;
        m_current_hover_box_index = hover;

        wxSongRenderer renderer;
        renderer.SetByConfiguration(g_configuration);

        wxClientDC dc(this);
        RenderBox(copy, &dc, &renderer, true);
        RenderBox(m_current_hover_box_index, &dc, &renderer, true);
#else
        Refresh();
#endif
    }

}

void wxServiceOutline::OnSizeEvent(wxSizeEvent &event)
{
    CalculateRefresh();
}

wxRect wxServiceOutline::DetermineBoxRect(int index) const
{
    int row = (index / m_columns);
    int column = (index % m_columns);

    return wxRect(column * m_avg_width, row * m_avg_height,
                  m_avg_width, m_avg_height);
}

int wxServiceOutline::DetermineBoxIndex(int x, int y) const
{
    x = x / m_avg_width;
    y = y / m_avg_height;
    return (y * m_columns + x);
}

void wxServiceOutline::OnMouseButtonLeft(wxMouseEvent &event)
{
    bool update = false;
    wxArrayInt updateInts;
    if (event.m_leftDown == true) {
        updateInts.Add(m_selected_box_index);
        updateInts.Add(m_current_hover_box_index);
        m_selected_box_index = m_current_hover_box_index;

        update = true;
#if 0
        if (m_current_hover_box_index != m_selected_box_index) {
            m_selected_box_index = -1;
            update = true;
        }
#endif
    } else {
        if (m_current_hover_box_index != m_selected_box_index) {
            updateInts.Add(m_selected_box_index);
            m_selected_box_index = -1;
            update = true;
        }
    }
    if (updateInts.GetCount() > 0) {
        wxClientDC dc(this);
        wxSongRenderer renderer;
        renderer.SetByConfiguration(g_configuration);
        for (int x = 0; x < updateInts.GetCount(); x++)
            RenderBox(x, &dc, &renderer, true);
    }
}

/*
    What happens here???
*/
void wxServiceOutline::OnMouseButtonDouble(wxMouseEvent &event)
{

}

int wxServiceOutline::GetSelected() const
{
    if (m_selected_box_index >= 0 && m_selected_box_index < SongCount())
        return m_selected_box_index;
    return -1;
}

void wxServiceOutline::SwapSong(int indexFrom, int indexTo)
{
    if (indexFrom >= 0 && indexFrom < SongCount() && indexTo >= 0 && indexTo < SongCount()) {
        wxSongData copy;

        copy = m_data.GetSong(indexFrom);
        m_data.SetSong(indexFrom, m_data.GetSong(indexTo));
        m_data.SetSong(indexTo, copy);

        Refresh();
    }
}

void wxServiceOutline::MoveSongToLeft(int index)
{
    if (index > 0)
        SwapSong(index, index - 1);
}

void wxServiceOutline::MoveSongToRight(int index)
{
    if (index < (SongCount() - 1))
        SwapSong(index, index + 1);
}

void wxServiceOutline::MoveCurrentSongToLeft()
{
    if (m_selected_box_index > 0)
        MoveSongToLeft(m_selected_box_index--);
}

void wxServiceOutline::MoveCurrentSongToRight()
{
    if (m_selected_box_index < (SongCount() - 1))
        MoveSongToRight(m_selected_box_index++);
}

void wxServiceOutline::RemoveCurrentSong()
{
    if (m_selected_box_index >= 0 && m_selected_box_index < SongCount()) {
        m_data.RemoveSong(m_selected_box_index);
        Refresh();
    }

}

void wxServiceOutline::OnCloseEvent(wxCloseEvent &event)
{
    event.Skip();
}

void wxServiceOutline::UpdateSongs()
{
    wxSongData song;
    for (int x = 0; x < SongCount(); x++) {
        long song_no = m_data.GetSong(x).GetRowId();
        if (g_songs_manager->GetSongByRowId(song_no, &song))
            m_data.SetSong(x, song);
    }
    Refresh();
}
