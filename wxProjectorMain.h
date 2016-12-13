/***************************************************************
 * Name:      wxProjectorMain.h
 * Purpose:   Defines Application Frame
 * Author:    Matthew Smaling (mjsmaling@hotmail.com)
 * Created:   2012-02-13
 * Copyright: Matthew Smaling (www.mjsmaling.com)
 * License:
 **************************************************************/

#ifndef WXPROJECTORMAIN_H
#define WXPROJECTORMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wxProjectorApp.h"

class wxSongList;
class wxSplitterWindow;
class wxServiceOutline;
class wxShowWindow;

class wxProjectorFrame: public wxFrame
{
public:
        wxProjectorFrame(wxFrame *frame, const wxString& title);
        ~wxProjectorFrame();

        static int ID_MENU_QUIT, ID_MENU_ABOUT, ID_SONGLIST_WIDGET_ID, ID_SPLITTER_WINDOW, ID_SERVICEOUTLINE_WIDGET_ID;
        static int ID_MENU_FONTS_N_COLOURS, ID_MENU_VACUUM;
        static int ID_MENU_EXPORT_SONGS, ID_MENU_IMPORT_SONGS;
        static int ID_SONG_DIALOG_ID;

        static int ID_ADD_SONG_BUTTON, ID_ADD_TO_SHOW, ID_EDIT_SONG_BUTTON, ID_REMOVE_SONG_BUTTON;
        static int ID_REMOVE_FROM_SHOW, ID_MOVE_LEFT_SHOW, ID_MOVE_RIGHT_SHOW, ID_PLAY_SHOW, ID_STOP_SHOW;
        static int ID_FONT_N_COLOURS_BUTTON, ID_MENU_PICTURES;
protected:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSize(wxSizeEvent &event);

        void OnAddSongButton(wxCommandEvent &event);
        void OnEditSongButton(wxCommandEvent &event);
        void OnRemoveSongButton(wxCommandEvent &event);

        void OnAddToShowButton(wxCommandEvent &event);
        void OnRemoveFromShowButton(wxCommandEvent &event);
        void OnMoveToLeftButton(wxCommandEvent &event);
        void OnMoveToRightButton(wxCommandEvent &event);
        void OnPlayShowButton(wxCommandEvent &event);
        void OnStopShowButton(wxCommandEvent &event);

        void OnVacuum(wxCommandEvent &event);
        void OnFontNColoursButton(wxCommandEvent &event);
        void OnPicturesButton(wxCommandEvent &event);

        void OnImportSongsMenu(wxCommandEvent &event);
        void OnExportSongsMenu(wxCommandEvent &event);

        void OnSearchSongSelected(wxCommandEvent &event);

        wxShowWindow *show_window(bool createIfNot = false);

        void AddCurrentSongToShow();

        DECLARE_EVENT_TABLE()
private:
        void UpdatePicturesManager();
        #if 0
        void IncreateDBWriteCounter();
        #endif
        void CreateToolBar();

        wxSongList *m_songListWidget;
        wxServiceOutline * m_serviceOutlineWidget;

        wxSplitterWindow *m_splitterWindow;

};


#endif // WXPROJECTORMAIN_H
