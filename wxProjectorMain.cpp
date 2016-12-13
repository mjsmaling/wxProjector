/***************************************************************
 * Name:      wxProjectorMain.cpp
 * Purpose:   Code for Application Frame
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

#include "wxProjectorMain.h"

#include <wx/splitter.h>
#include <wx/toolbar.h>

#include "include/globals.h"
#include "include/wxSongList.h"
#include "include/wxServiceOutline.h"
#include "include/graphics.h"
#include "include/icons.h"
#include "include/wxSongEditor.h"
#include "include/globals.h"
#include "include/wxSongsManager.h"
#include "include/wxShowWindow.h"
#include "include/wxFontsNColoursDialog.h"
#include "include/wxPicturesDialog.h"
#include "include/wxConfiguration.h"
#include "include/wxPicturesManager.h"

#define WXPROJECTMAIN_SHOW_FULLSCREEN

//helper functions
enum wxbuildinfoformat {
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild;//(wxT("wxProjector\n\nwww.mjsmaling.com\nmjsmaling@hotmail.com\n\n"));
    wxbuild << wxVERSION_STRING;

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

int wxProjectorFrame::ID_MENU_QUIT = wxNewId();
int wxProjectorFrame::ID_MENU_ABOUT = wxNewId();
int wxProjectorFrame::ID_SONGLIST_WIDGET_ID = wxNewId();
int wxProjectorFrame::ID_SPLITTER_WINDOW = wxNewId();
int wxProjectorFrame::ID_SERVICEOUTLINE_WIDGET_ID = wxNewId();
int wxProjectorFrame::ID_ADD_SONG_BUTTON = wxNewId();
int wxProjectorFrame::ID_ADD_TO_SHOW = wxNewId();
int wxProjectorFrame::ID_SONG_DIALOG_ID = wxNewId();
int wxProjectorFrame::ID_EDIT_SONG_BUTTON = wxNewId();
int wxProjectorFrame::ID_REMOVE_SONG_BUTTON = wxNewId();
int wxProjectorFrame::ID_REMOVE_FROM_SHOW = wxNewId();
int wxProjectorFrame::ID_MOVE_LEFT_SHOW = wxNewId();
int wxProjectorFrame::ID_MOVE_RIGHT_SHOW = wxNewId();
int wxProjectorFrame::ID_PLAY_SHOW = wxNewId();
int wxProjectorFrame::ID_STOP_SHOW = wxNewId();

int wxProjectorFrame::ID_FONT_N_COLOURS_BUTTON = wxNewId();
int wxProjectorFrame::ID_MENU_FONTS_N_COLOURS = wxNewId();
int wxProjectorFrame::ID_MENU_VACUUM = wxNewId();
int wxProjectorFrame::ID_MENU_PICTURES = wxNewId();

int wxProjectorFrame::ID_MENU_EXPORT_SONGS = wxNewId();
int wxProjectorFrame::ID_MENU_IMPORT_SONGS = wxNewId();

BEGIN_EVENT_TABLE(wxProjectorFrame, wxFrame)
    EVT_SIZE(wxProjectorFrame::OnSize)

    EVT_MENU(ID_MENU_QUIT, wxProjectorFrame::OnQuit)
    EVT_MENU(ID_MENU_ABOUT, wxProjectorFrame::OnAbout)
    EVT_MENU(ID_FONT_N_COLOURS_BUTTON, wxProjectorFrame::OnFontNColoursButton)
    EVT_MENU(ID_MENU_VACUUM, wxProjectorFrame::OnVacuum)
    EVT_TOOL(ID_ADD_SONG_BUTTON, wxProjectorFrame::OnAddSongButton)
    EVT_TOOL(ID_ADD_TO_SHOW, wxProjectorFrame::OnAddToShowButton)
    EVT_TOOL(ID_EDIT_SONG_BUTTON, wxProjectorFrame::OnEditSongButton)
    EVT_TOOL(ID_REMOVE_SONG_BUTTON, wxProjectorFrame::OnRemoveSongButton)

    EVT_TOOL(ID_REMOVE_FROM_SHOW, wxProjectorFrame::OnRemoveFromShowButton)
    EVT_TOOL(ID_MOVE_LEFT_SHOW, wxProjectorFrame::OnMoveToLeftButton)
    EVT_TOOL(ID_MOVE_RIGHT_SHOW, wxProjectorFrame::OnMoveToRightButton)
    EVT_TOOL(ID_PLAY_SHOW, wxProjectorFrame::OnPlayShowButton)
    EVT_TOOL(ID_STOP_SHOW, wxProjectorFrame::OnStopShowButton)

    EVT_TOOL(ID_MENU_FONTS_N_COLOURS, wxProjectorFrame::OnFontNColoursButton)

    EVT_MENU(ID_MENU_PICTURES, wxProjectorFrame::OnPicturesButton)

    EVT_MENU(ID_MENU_EXPORT_SONGS, wxProjectorFrame::OnExportSongsMenu)
    EVT_MENU(ID_MENU_IMPORT_SONGS, wxProjectorFrame::OnImportSongsMenu)

    EVT_COMMAND(ID_SONGLIST_WIDGET_ID, wxSONGLIST_SELECTED, wxProjectorFrame::OnSearchSongSelected)

    EVT_CLOSE(wxProjectorFrame::OnClose)
END_EVENT_TABLE()

wxProjectorFrame::wxProjectorFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));

    fileMenu->Append(ID_MENU_EXPORT_SONGS, wxT("Export Songs"), wxT("Export songs in database"));
    fileMenu->Append(ID_MENU_IMPORT_SONGS, wxT("Import Songs"), wxT("Import songs into local database"));

    fileMenu->Append(ID_MENU_QUIT, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu *editMenu = new wxMenu(_T(""));
    editMenu->Append(ID_MENU_FONTS_N_COLOURS, wxT("Fonts and Colours ..."), wxT("Edit the default fonts and colours"));
    editMenu->Append(ID_MENU_VACUUM, wxT("Vacuum Database"), wxT("Will clean up unnecessary space in song database."));
    editMenu->Append(ID_MENU_PICTURES, wxT("Pictures ..."), wxT("Edit background pictures"));
    mbar->Append(editMenu, wxT("Edit"));


    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(ID_MENU_ABOUT, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

    /* Create the central widget... which will comprise of a splitter window. */

    m_splitterWindow = new wxSplitterWindow(this, ID_SPLITTER_WINDOW);

    m_songListWidget = new wxSongList(m_splitterWindow, ID_SONGLIST_WIDGET_ID);

    m_serviceOutlineWidget = new wxServiceOutline(m_splitterWindow, ID_SERVICEOUTLINE_WIDGET_ID);

    m_splitterWindow->SetMinimumPaneSize(150);
    m_splitterWindow->SplitVertically(m_songListWidget, m_serviceOutlineWidget, 200);

    //m_splitterWindow->Initialize(m_songListWidget);

   // m_songListWidget = new wxSongList(this, ID_SONGLIST_WIDGET_ID);
  //  this->SetCentralWidget(m_songListWidget);

    CreateToolBar();

#ifdef __LINUX__
    SetIcon(graphics_load_icon(pink_cleft_png, sizeof(pink_cleft_png)));
#endif

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

   g_pictures_manager->RandomizeFilenames();
}


wxProjectorFrame::~wxProjectorFrame()
{
}

void wxProjectorFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void wxProjectorFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void wxProjectorFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void wxProjectorFrame::OnSize(wxSizeEvent &event)
{
    event.Skip();
#if 0
    // get the position and size for the central widget.
    m_splitterWindow->SetSize(GetSize());
    m_splitterWindow->SetPosition(GetClientAreaOrigin());
//    m_splitterWindow->UpdateSize();

    wxSize sz = m_splitterWindow->GetSize();

    printf("wxProjectorFrame::OnSizeEvent %d x %x \n", sz.GetWidth(), sz.GetHeight());

    wxString text = wxString::Format(wxT("%d x %d"),  sz.GetWidth(),  sz.GetHeight());

    SetStatusText(text,0);
#endif
}

/*
    Create the tool bar for the main window.
*/
void wxProjectorFrame::CreateToolBar()
{
    // create a tool bar here.
    wxToolBar *toolbar = wxFrame::CreateToolBar();

    wxSize size(32, 32);

    // Song list, stuff...
    toolbar->AddTool(ID_ADD_SONG_BUTTON, wxEmptyString, graphics_load_bitmap(pink_music_plus_png, sizeof(pink_music_plus_png), &size), wxT("Add song(s) ..."));

    toolbar->AddTool(ID_EDIT_SONG_BUTTON, wxEmptyString, graphics_load_bitmap(pink_music_note_png, sizeof(pink_music_note_png), &size), wxT("Edit Song ..."));

    toolbar->AddTool(ID_REMOVE_SONG_BUTTON, wxEmptyString, graphics_load_bitmap(pink_music_minus_png, sizeof(pink_music_minus_png), &size), wxT("Remove Song ..."));

    toolbar->AddSeparator();

    toolbar->AddTool(ID_ADD_TO_SHOW, wxEmptyString, graphics_load_bitmap(pink_plus_png, sizeof(pink_plus_png), &size), wxT("Add to show"));
    toolbar->AddTool(ID_REMOVE_FROM_SHOW, wxEmptyString, graphics_load_bitmap(pink_solid_minus_png, sizeof(pink_solid_minus_png), &size), wxT("Remove from show"));
    toolbar->AddTool(ID_MOVE_LEFT_SHOW, wxEmptyString, graphics_load_bitmap(pink_solid_left_arrow_png, sizeof(pink_solid_left_arrow_png), &size), wxT("Move to left"));
    toolbar->AddTool(ID_MOVE_RIGHT_SHOW, wxEmptyString, graphics_load_bitmap(pink_solid_right_arrow_png, sizeof(pink_solid_right_arrow_png), &size), wxT("Move to right"));

    toolbar->AddSeparator();

    toolbar->AddTool(ID_PLAY_SHOW, wxEmptyString, graphics_load_bitmap(pink_solid_right_triangle_png, sizeof(pink_solid_right_triangle_png), &size), wxT("Play Show"));
    toolbar->AddTool(ID_STOP_SHOW, wxEmptyString, graphics_load_bitmap(pink_solid_rectangle_png, sizeof(pink_solid_rectangle_png), &size), wxT("Stop show"));

    toolbar->AddSeparator();

    toolbar->AddTool(ID_MENU_FONTS_N_COLOURS, wxEmptyString, graphics_load_bitmap(pink_spiral_png, sizeof(pink_spiral_png), &size), wxT("Font's 'n Colours"));

    toolbar->Realize();
}

/*
    Triggered when user clicks on the 'add' button.

    Show the manipulate song dialog.
*/
void wxProjectorFrame::OnAddSongButton(wxCommandEvent &event)
{
    wxSongEditor dialog(this, ID_SONG_DIALOG_ID, wxT("Song Editor"));
/*  wxSongEditor(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("dialogBox"));*/

    if (dialog.ShowModal() == wxID_OK) {
        // add this song to the database.
        wxSongData data = dialog.GetSongData();
        g_songs_manager->AddSong(data);

        m_songListWidget->RefreshList();
    }
}

/*
    Triggered
*/
void wxProjectorFrame::OnEditSongButton(wxCommandEvent &event)
{
    wxSongData song;

    long row_id = m_songListWidget->GetCurrentSongSelectedId();
    if (row_id < 1)
        return;

    // get song...
    if (g_songs_manager->GetSongByRowId(row_id, &song) > 0) {
        wxSongEditor dialog(this, ID_SONG_DIALOG_ID, wxT("Song Editor"));
        dialog.SetSongData(song);
        if (dialog.ShowModal() == wxID_OK) {
            // add this song to the database.
            /* We want to preserve the row_id... thus we're copying certain items over */
            wxSongData data = dialog.GetSongData();
            song.SetTitle(data.GetTitle());
            song.SetContents(data.GetContents());

            g_songs_manager->UpdateSong(song);

            m_songListWidget->RefreshList();
            m_serviceOutlineWidget->UpdateSongs();
        }
    }
}

/*
    Triggered when user clicks on the 'add to show' button.

    This will add a selected song to the show widget.
*/
void wxProjectorFrame::OnAddToShowButton(wxCommandEvent &event)
{
    AddCurrentSongToShow();
}

void wxProjectorFrame::AddCurrentSongToShow()
{
    wxSongData song;
    if (m_songListWidget->GetCurrentSelectedSong(&song)) {
        m_serviceOutlineWidget->AddSong(song);
    }
}

void wxProjectorFrame::OnRemoveSongButton(wxCommandEvent &event)
{
    // Confirm with user before deleting.
    wxSongData song;
    if (m_songListWidget->GetCurrentSelectedSong(&song) == true) {
        wxString message(wxT("Are you sure you wish to delete this song:\n"));
        message << song.GetTitle() << wxT("?");
        if (wxMessageBox(message, wxT("Delete Song?"), wxYES_NO, this) == wxYES) {
            g_songs_manager->DeleteSong(song);
            m_songListWidget->RefreshList();
        }
    }
}

void wxProjectorFrame::OnRemoveFromShowButton(wxCommandEvent &event)
{
    m_serviceOutlineWidget->RemoveCurrentSong();

}
void wxProjectorFrame::OnMoveToLeftButton(wxCommandEvent &event)
{
    m_serviceOutlineWidget->MoveCurrentSongToLeft();

}
void wxProjectorFrame::OnMoveToRightButton(wxCommandEvent &event)
{
    m_serviceOutlineWidget->MoveCurrentSongToRight();
}

void wxProjectorFrame::OnPlayShowButton(wxCommandEvent &event)
{
    wxShowWindow *win = show_window(true);
    wxServiceOutlineData show_data;

    m_serviceOutlineWidget->GetData(&show_data);

    show_data.SetFonts(g_configuration->GetFonts());
    show_data.SetColours(g_configuration->GetColours());

    win->PlayShow(show_data, 0);
}

void wxProjectorFrame::OnStopShowButton(wxCommandEvent &event)
{
    wxShowWindow *win = show_window(false);
    if (win != 0)
        win->StopShow();
}

wxShowWindow *wxProjectorFrame::show_window(bool createIfNot)
{
    wxShowWindow *win = wxShowWindow::get_instance();
    if (win == 0 && createIfNot == true) {
        win = new wxShowWindow(this, -1, wxT("Show"), wxDefaultPosition, wxSize(800, 600));
        win->Show();
#ifdef WXPROJECTMAIN_SHOW_FULLSCREEN
        win->ShowFullScreen(true);
#endif
        win->SetFocusFromKbd();
    }
    return win;
}

/*

*/
void wxProjectorFrame::OnFontNColoursButton(wxCommandEvent &event)
{
    // show the font's 'n colours dialog.
    wxFontsNColoursDialog dialog(this, -1, wxT("Fonts N Colours"));

    dialog.SetColours(g_configuration->GetColours());
    dialog.SetFonts(g_configuration->GetFonts());

    if (dialog.ShowModal() == wxID_OK) {
        g_configuration->SetFonts(dialog.GetFonts());
        g_configuration->SetColours(dialog.GetColours());

        Refresh();
    }
}
#if 0
void wxProjectorFrame::IncreateDBWriteCounter()
{
    wxString name(wxT("db_write_counter"));
    int count = g_configuration->GetInt(name, 0);
    if (++count > 100) {
        g_songs_manager->VacuumSongs();
        count = 0;
    }
    g_configuration->SetInt(name, count);
}
#endif
void wxProjectorFrame::OnVacuum(wxCommandEvent &event)
{
    g_songs_manager->VacuumSongs();
}

void wxProjectorFrame::OnPicturesButton(wxCommandEvent &event)
{
     wxPicturesDialog dialog(this, -1, wxT("Pictures Settings"));

    dialog.LoadSettings(g_configuration);

     if (dialog.ShowModal() == wxID_OK) {
         // something here... refresh screen maybe...
        dialog.SaveSettings(g_configuration);

        UpdatePicturesManager();

        Refresh();
     }
}

void wxProjectorFrame::UpdatePicturesManager()
{
    // update the pictures manager
    g_pictures_manager->Clear();
    g_pictures_manager->AddByConfiguration(g_configuration);
    g_pictures_manager->RandomizeFilenames();
}

void wxProjectorFrame::OnImportSongsMenu(wxCommandEvent &event)
{

}

void wxProjectorFrame::OnExportSongsMenu(wxCommandEvent &event)
{

}

void wxProjectorFrame::OnSearchSongSelected(wxCommandEvent &event)
{
    // check if double... which the int > 0 will indicate that it is.
    if (event.GetInt() > 0)
        AddCurrentSongToShow();
}
