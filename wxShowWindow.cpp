#include "../include/wxShowWindow.h"
#include "../include/globals.h"
#include "../include/graphics.h"
#include <wx/dcclient.h>
#include <wx/panel.h>
#include "../include/wxPicturesManager.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

wxShowWindow *wxShowWindow::single_instance = 0;

DEFINE_EVENT_TYPE(wxSHOW_WINDOW_EVENT)

int wxShowWindow::PLAYING = wxNewId();
int wxShowWindow::STOPPED = wxNewId();
int wxShowWindow::CLOSING_WINDOW = wxNewId();
int wxShowWindow::PLAY_NEXT_SONG;
int wxShowWindow::PLAY_PREVIOUS_SONG = wxNewId();
int wxShowWindow::SHOW_WINDOW_CONTROL = wxNewId();

BEGIN_EVENT_TABLE(wxShowWindow, wxFrame)
    EVT_KEY_UP(wxShowWindow::OnKeyUp)
    EVT_CLOSE(wxShowWindow::OnCloseEvent)
    //EVT_PAINT(wxShowWindow::OnPaintEvent)
    EVT_SIZE(wxShowWindow::OnSizeEvent)
END_EVENT_TABLE()

wxShowWindow::wxShowWindow(wxWindow* parent, wxWindowID id,
                 const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style, const wxString& name)
                 : wxFrame(parent, id, title, pos, size, style , name)
{
    if (single_instance == 0)
        single_instance = this;

   /* This is needed to catch key events */
 //  m_dummyPanel = new wxPanel(this, DUMMY_PANNEL_ID, wxPoint(0,0), wxSize(0, 0));
   //m_dummyPanel->SetNextHandler(this);

  // SetOwnBackgroundColour(wxColour(255,255,0));
   //SetBackgroundStyle()

   m_showControl = new wxShowControl(this, SHOW_WINDOW_CONTROL);
   m_showControl->SetNextHandler(this);


}

wxShowWindow::~wxShowWindow()
{
    if (single_instance == this)
        single_instance = 0;
}

void wxShowWindow::PlaySong(int index)
{
    wxSongData song;
    if (index >= 0 && index < m_data.SongCount())
        song = m_data.GetSong(index);
        m_showControl->SetShowIndex(index);
    m_showControl->SetDisplay(song);
    m_showControl->SetColours(m_data.GetColours().GetFontColour(), m_data.GetColours().GetBackColour());
    wxFont titleFont = m_data.GetFonts().GetTitleFont();
    m_showControl->SetFonts(m_data.GetFonts().GetTitleFont(), m_data.GetFonts().GetContentsFont());
    SetOwnBackgroundColour(m_data.GetColours().GetBackColour());
}

void wxShowWindow::PlayShow(const wxServiceOutlineData &data, int from_order)
{
    EmitShowWindowEvent(PLAYING, wxT("Playing"));

    m_data = data;
    m_current_song_index = from_order;

    PlaySong(from_order);
    m_y_offset = 0;

    GetFocus();
}

void wxShowWindow::PlayShow(const wxString &session_name, int from_order)
{
    assert(g_songs_manager != 0);

    wxServiceOutlineData data;
    data.LoadSongData(session_name, *g_songs_manager);

    PlayShow(data, from_order);
}

void wxShowWindow::StopShow(bool closeWindow)
{
    EmitShowWindowEvent(STOPPED, wxT("Stopping"));

    if (closeWindow == true)
        this->Close();
}

void wxShowWindow::NextSong()
{
    if (m_current_song_index < (m_data.SongCount() - 1)) {
        EmitShowWindowEvent(PLAY_NEXT_SONG, wxT("Playing Next Song"));
        PlaySong(++m_current_song_index);
    }
}

void wxShowWindow::PreviousSong()
{
    if (m_current_song_index > 0) {
        EmitShowWindowEvent(PLAY_PREVIOUS_SONG, wxT("Playing Previous Song"));
        PlaySong(--m_current_song_index);
    }
}

void wxShowWindow::UpParagraph()
{

}

void wxShowWindow::DownParagraph()
{

}


void wxShowWindow::OnKeyUp(wxKeyEvent &event)
{
    event.Skip();
    if (event.GetEventObject() == m_showControl) {
        switch (event.GetKeyCode()) {
        case WXK_ESCAPE:
            Close();
            break;
        case WXK_LEFT:
            PreviousSong();
            break;
        case WXK_RIGHT:
            NextSong();
            break;
        case WXK_UP:
            UpParagraph();
            break;
        case WXK_DOWN:
            DownParagraph();
            break;
        default:
            break;
        };
    }
}

void wxShowWindow::OnCloseEvent(wxCloseEvent &event)
{
    EmitShowWindowEvent(CLOSING_WINDOW, wxT("Closing"));
    this->Destroy();
}

void wxShowWindow::EmitShowWindowEvent(int mode, const wxChar *text)
{
    wxCommandEvent event(wxSHOW_WINDOW_EVENT, GetId());

    event.SetEventObject(this);
    event.SetInt(mode);
    if (text != 0)
        event.SetString(text);

    GetEventHandler()->ProcessEvent(event);
}

void wxShowWindow::OnSizeEvent(wxSizeEvent &event)
{
    event.Skip();
    m_showControl->SetSize(GetSize());
    m_showControl->CalculateAndRefresh();
}

void wxShowWindow::GetFocus()
{
    wxUsleep(100);
    // Get focus.
#ifdef __LINUX__
    Display *display = XOpenDisplay(NULL);

	XEvent event;
 	if(display == NULL) {
		fprintf(stderr, "Errore nell'apertura del Display !!!\n");
		return;
	}

	memset(&event, 0x00, sizeof(event));

	event.type = ButtonPress;
	event.xbutton.button = 1;
	event.xbutton.same_screen = True;

	XQueryPointer(display, RootWindow(display, DefaultScreen(display)),
               &event.xbutton.root, &event.xbutton.window,
               &event.xbutton.x_root, &event.xbutton.y_root,
               &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

	event.xbutton.subwindow = event.xbutton.window;

	while(event.xbutton.subwindow) {
		event.xbutton.window = event.xbutton.subwindow;
		XQueryPointer(display,
                event.xbutton.window,
                &event.xbutton.root,
                &event.xbutton.subwindow,
                &event.xbutton.x_root,
                &event.xbutton.y_root,
                &event.xbutton.x,
                &event.xbutton.y,
                &event.xbutton.state);
	}

	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");

	XFlush(display);

	wxUsleep(50);

	event.type = ButtonRelease;
	event.xbutton.state = 0x100;

	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0)
        fprintf(stderr, "Errore nell'invio dell'evento !!!\n");

	XFlush(display);

	XCloseDisplay(display);
#endif
}
