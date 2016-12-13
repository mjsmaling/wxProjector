#include "../include/wxShowControl.h"
#include "../include/globals.h"
#include "../include/graphics.h"
#include "../include/wxSongRenderer.h"
#include "../include/wxConfiguration.h"

#include <wx/dcclient.h>

int wxShowControl::MY_TIMER_ID = wxNewId();

BEGIN_EVENT_TABLE(wxShowControl, wxPanel)
    EVT_PAINT(wxShowControl::OnPaintEvent)
    EVT_SIZE(wxShowControl::OnSizeEvent)
    EVT_TIMER(MY_TIMER_ID, wxShowControl::OnTimerEvent)
END_EVENT_TABLE()

wxShowControl::wxShowControl(wxWindow* parent, wxWindowID id,
                  const wxPoint& pos , const wxSize& size ,
                  long style, const wxString& name)
    : wxPanel(parent, id, pos, size, style, name),
    m_timer(this, MY_TIMER_ID)
{
    //ctor
    m_pictureIndex = 0;
    m_ignorePaint = 0;
}

wxShowControl::~wxShowControl()
{
    //dtor
}

void wxShowControl::SetDisplay(const wxString *title, const wxString *contents, const wxFont *titleFont, const wxFont *contentsFont,
                    const wxColour *fontColour, const wxColour *backColour)
{

}

void wxShowControl::GetDisplay(wxString *title, wxString *contents, wxFont *titleFont, wxFont *contentsFont, wxColour *fontColour,
                    wxColour *backColour)
{

}

void wxShowControl::OnPaintEvent(wxPaintEvent &event)
{
    wxPaintDC dc(this);

    if (event.GetEventObject() != this) {
        event.Skip();
        return;
    }

    // ignore paint...
    if (m_ignorePaint > 0) {
        return;
    }

    /* Paint the background */
    //FillBackground(&dc);

    wxSongRenderer renderer;
    renderer.SetByConfiguration(g_configuration);

    renderer.SetPictureIndex(m_pictureIndex);
    //renderer.SetPictureIndex(m_songIndex);
#if 1
    renderer.SetTitle(m_title);
    renderer.SetContent(m_contents);
#endif
    wxSize sz = GetSize();
    wxRect rect(0, 0, sz.GetWidth(), sz.GetHeight());

    renderer.Render(dc, rect, true);
#if 0
    wxSize sz = GetSize();
    wxRect winrect(0,0, sz.GetWidth(), sz.GetHeight() );// = GetRect();

    if (m_need_to_calculate == true) {
        m_need_to_calculate = false;

        wxRect rect;
        rect = winrect;

        /* generate bitmaps */
        m_titleBitmap = generate_text_image(m_title, 0, &m_titleFont, &m_fontColour, &m_backColour);
        m_titleBitmap = resize_if_bigger(m_titleBitmap, sz, true);

        //rect.SetTop(m_titleBitmap.GetHeight() + 1);
       rect.SetHeight(rect.GetHeight() - (m_titleBitmap.GetHeight() + 5));

        m_contentsBitmap = generate_text_image(m_contents, 0, &m_contentsFont, &m_fontColour, &m_backColour);
        m_contentsBitmap = resize_if_bigger(m_contentsBitmap, rect.GetSize(), true);
    }

    int x1;
    if (m_titleBitmap.IsOk()) {
        // draw in center...
        x1 = (sz.GetWidth() - m_titleBitmap.GetWidth()) / 2;
        dc.DrawBitmap(m_titleBitmap, x1, 0);
        winrect.SetTop(m_titleBitmap.GetHeight());
        winrect.SetHeight(winrect.GetHeight() - m_titleBitmap.GetHeight());
    }

    if (m_contentsBitmap.IsOk()) {
        // resize bitmap contents...
        int y1;
        x1 = (winrect.GetWidth() - m_contentsBitmap.GetWidth()) / 2;
        y1 = (winrect.GetHeight() - m_contentsBitmap.GetHeight()) / 2 + winrect.GetTop();

        dc.DrawBitmap(m_contentsBitmap, x1, y1);
    }
#endif
}

void wxShowControl::FillBackground(wxDC *dc)
{
    wxColour colour = m_backColour;
    wxPen pen(colour);
    pen.SetStyle(wxSOLID);
    dc->SetPen(pen);
    wxBrush brush(colour);
    brush.SetStyle(wxSOLID);
    dc->SetBrush(brush);
    wxSize size = GetSize();
    dc->DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());
}


void wxShowControl::CalculateAndRefresh()
{
    m_need_to_calculate = true;
    // check that the window is shown, otherwise this may cause errors.
    if (this->IsShown())
        Refresh();
}

void wxShowControl::SetDisplay(const wxSongData &songData)
{
    m_title = songData.GetTitle();
    m_contents = songData.GetContents();
    CalculateAndRefresh();
}

void wxShowControl::SetColours(const wxColour &fontColour, const wxColour &backColour)
{
    m_fontColour = fontColour;
    m_backColour = backColour;
    CalculateAndRefresh();
}

void wxShowControl::SetFonts(const wxFont &titleFont, const wxFont &contentsFont)
{
    if (m_titleFont != titleFont)
        m_titleFont = titleFont;
    if (m_contentsFont != contentsFont)
        m_contentsFont = contentsFont;

    CalculateAndRefresh();
}

void wxShowControl::SetShowIndex(int index)
{
    m_pictureIndex = index;
}

void wxShowControl::OnSizeEvent(wxSizeEvent &event)
{
    m_ignorePaint++;
    // if timer has not started, then start it.
    if (m_timer.IsRunning() == false) {
        // trigger a timer, but only one shot.
        m_timer.Start(500);
    }
}

void wxShowControl::OnTimerEvent(wxTimerEvent &event)
{
    // cause a refresh in the paint...
    if (m_ignorePaint > 0)
        m_ignorePaint = 0;
    else {
        if (m_timer.IsRunning())
            m_timer.Stop();
        this->Refresh();
    }
}

