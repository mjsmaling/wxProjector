#include "../include/wxPicturesDialog.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/colordlg.h>
#include "../include/wxConfiguration.h"
#include "../include/string_func.h"

const wxChar *wxPicturesDialog::DirectoryTextParam = wxT("graphics_directory");
const wxChar *wxPicturesDialog::UseGraphicsEnabledParam = wxT("use_graphics");
const wxChar *wxPicturesDialog::GraphicsZoomStyleParam = wxT("graphics_zoom");
const wxChar *wxPicturesDialog::GraphicsBackgroundColourParam = wxT("graphics_background_colour");

int wxPicturesDialog::ID_DIRETORY_TEXT = wxNewId();
int wxPicturesDialog::ID_DIRECTORY_BUTTON = wxNewId();
int wxPicturesDialog::ID_ENABLED_CHECK = wxNewId();
int wxPicturesDialog::ID_ZOOMSTYLE_COMBO = wxNewId();
int wxPicturesDialog::ID_BACKGROUNDCOLOUR_BUTTON = wxNewId();

const wxChar *wxPicturesDialog::ZoomTextStyles[] = {
    wxT("Auto"),
    wxT("Centered"),
    wxT("Tiled"),
    wxT("Stretched"),
    wxT("Scaled"),
    wxT("Zoomed"),
    0
};

int wxPicturesDialog::SearchTextStyle(const wxString &text)
{
    int i = 0;
    while (ZoomTextStyles[i] != 0) {
        if (text.CmpNoCase(ZoomTextStyles[i]) == 0)
            return i;
        i++;
    }
    return -1;
}

BEGIN_EVENT_TABLE(wxPicturesDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxPicturesDialog::OnOKButton)
    EVT_BUTTON(ID_BACKGROUNDCOLOUR_BUTTON, wxPicturesDialog::OnBackgroundColourButton)
    EVT_BUTTON(ID_DIRECTORY_BUTTON, wxPicturesDialog::OnDirectoryButton)
END_EVENT_TABLE()

wxPicturesDialog::wxPicturesDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style, const wxString& name)
 : wxDialog(parent, id, title, pos, size, style, name)
{
    CreateGui();
}

wxPicturesDialog::~wxPicturesDialog()
{
    //dtor
}

void wxPicturesDialog::CreateGui()
{
    /*
        [ Pictures Folder ]
        [ Enabled ]
        [ Zoom Style ]
        [ Background Colour ]
        --
        [ OK | CANCEL ]
    */

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    wxSizer *picturesSettings = CreatePictureSettingsSizer();
    sizer->Add(picturesSettings, 4, wxALIGN_CENTER | wxEXPAND, 5);

    wxSizer *buttons = wxDialog::CreateButtonSizer(wxOK | wxCANCEL);
    sizer->Add(buttons, 1, wxALIGN_CENTER | wxEXPAND, 2);

    SetSizer(sizer);

}

wxSizer *wxPicturesDialog::CreatePictureSettingsSizer()
{
    wxStaticBoxSizer *sizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Pictures"));
/* [ Pictures Folder ]
        [ Enabled ]
        [ Zoom Style ]
        [ Background Colour ]*/
    wxFlexGridSizer *grid = new wxFlexGridSizer(4, 2, 2, 2);
// wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = "staticText")

    // Add first row...
    grid->Add(new wxStaticText(this, -1, wxT("Pictures Folder:")), 0, wxEXPAND, 0);
    wxBoxSizer *box = new wxBoxSizer(wxHORIZONTAL);
    m_directoryText = new wxTextCtrl(this, -1);
    // m_directoryText
    box->Add(m_directoryText, 2, wxEXPAND, 1);

    wxButton *btn = new wxButton(this, ID_DIRECTORY_BUTTON, wxT("..."));
    box->Add(btn, 1);

    grid->Add(box, 1, wxEXPAND, 1);

    // 2nd row... checkbox.
    grid->Add(new wxStaticText(this, -1, wxT("Enabled:")), 0, wxEXPAND, 0);
    m_enabledCheck = new wxCheckBox(this, ID_ENABLED_CHECK, wxT("Enable pictures to be shown."));
    grid->Add(m_enabledCheck, 1, wxEXPAND, 1);

    // 3rd row. Zoom stype.
    {
        wxArrayString choices;
        int size = wxchar_array_length(ZoomTextStyles);
        for (int x = 0; x < size; x++)
            choices.Add(ZoomTextStyles[x]);

        m_zoomStyleCombo = new wxComboBox(this, ID_ZOOMSTYLE_COMBO, wxEmptyString,
                                           wxDefaultPosition, wxDefaultSize, choices);
    }
    grid->Add(new wxStaticText(this, -1, wxT("Zoom Style:")), 0, wxEXPAND, 0);
    grid->Add(m_zoomStyleCombo, 1, wxEXPAND, 1);

    sizer->Add(grid, 1, wxEXPAND, 1);

    // 4th row... background colour.
    grid->Add(new wxStaticText(this, -1, wxT("Background Colour:")), 0, wxEXPAND, 0);
    btn = new wxButton(this, ID_BACKGROUNDCOLOUR_BUTTON, wxT("Set Colour"));
    grid->Add(btn, 1, wxEXPAND, 1);

    return sizer;
}

void wxPicturesDialog::OnOKButton(wxCommandEvent &event)
{
    // Save settings...
    EndModal(GetAffirmativeId());
}

void wxPicturesDialog::LoadSettings(wxConfiguration *config)
{
    wxASSERT(config != 0);
    /* Load settings from */
    m_backgroundColour = config->GetColour(GraphicsBackgroundColourParam);
    m_zoomStyleCombo->SetValue(config->GetString(GraphicsZoomStyleParam));
    m_directoryText->SetValue(config->GetString(DirectoryTextParam));
    m_enabledCheck->SetValue(config->GetBool(UseGraphicsEnabledParam, false));
}

void wxPicturesDialog::SaveSettings(wxConfiguration *config)
{
    wxASSERT(config != 0);

    config->SetColour(GraphicsBackgroundColourParam, m_backgroundColour);
    config->SetValue(GraphicsZoomStyleParam, m_zoomStyleCombo->GetValue());
    config->SetBool(UseGraphicsEnabledParam, m_enabledCheck->GetValue());
    config->SetValue(DirectoryTextParam, m_directoryText->GetValue());
}

void wxPicturesDialog::OnBackgroundColourButton(wxCommandEvent &event)
{
    // show colour dialog.
    wxColourData colourData;
    colourData.SetColour(m_backgroundColour);

    wxColourDialog dialog(this, &colourData);
    if (dialog.ShowModal() == wxID_OK)
        m_backgroundColour = dialog.GetColourData().GetColour();
}

void wxPicturesDialog::OnDirectoryButton(wxCommandEvent &event)
{
    /* Show directory dialog... */
    wxDirDialog dialog(this, wxT("Choose pictures directory"),
                       m_directoryText->GetValue(), wxDD_DIR_MUST_EXIST | wxDD_DEFAULT_STYLE);

    if (dialog.ShowModal() == wxID_OK)
        m_directoryText->SetValue(dialog.GetPath());
}

void wxPicturesDialog::SetSongsManager(wxSongsManager *songsManager)
{
    m_songsManager = songsManager;

}
