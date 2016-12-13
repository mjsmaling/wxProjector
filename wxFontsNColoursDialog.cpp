#include "../include/wxFontsNColoursDialog.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/cmndata.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>

int wxFontsNColoursDialog::FONT_COLOUR_BTN_ID = wxNewId();
int wxFontsNColoursDialog::BACK_COLOUR_BTN_ID = wxNewId();
int wxFontsNColoursDialog::TITLE_FONT_BTN_ID = wxNewId();
int wxFontsNColoursDialog::CONTENTS_FONT_BTN_ID = wxNewId();

BEGIN_EVENT_TABLE(wxFontsNColoursDialog, wxDialog)
    EVT_BUTTON(FONT_COLOUR_BTN_ID, wxFontsNColoursDialog::OnFontColour)
    EVT_BUTTON(BACK_COLOUR_BTN_ID, wxFontsNColoursDialog::OnBackColour)
    EVT_BUTTON(TITLE_FONT_BTN_ID, wxFontsNColoursDialog::OnTitleFont)
    EVT_BUTTON(CONTENTS_FONT_BTN_ID, wxFontsNColoursDialog::OnContentsFont)
END_EVENT_TABLE()

wxFontsNColoursDialog::wxFontsNColoursDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style, const wxString& name)
: wxDialog(parent, id, title, pos, size, style, name)
{
    //ctor
    CreateGui();
}

wxFontsNColoursDialog::~wxFontsNColoursDialog()
{
    //dtor
}

void wxFontsNColoursDialog::SetFonts(const wxFontSet &fonts)
{
    m_fonts = fonts;
}

void wxFontsNColoursDialog::SetColours(const wxColourSet &colours)
{
    m_colours = colours;
}

const wxFontSet &wxFontsNColoursDialog::GetFonts() const
{
    return m_fonts;
}

const wxColourSet &wxFontsNColoursDialog::GetColours() const
{
    return m_colours;
}

void wxFontsNColoursDialog::CreateGui()
{
    wxBoxSizer *layout = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer *box_sizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Fonts"));

    m_titleFontBtn = new wxButton(this, TITLE_FONT_BTN_ID, wxT("Title Font"));
    box_sizer->Add(m_titleFontBtn, 1, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    m_contentsFontBtn = new wxButton(this, CONTENTS_FONT_BTN_ID, wxT("Contents Font"));
    box_sizer->Add(m_contentsFontBtn, 1, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    layout->Add(box_sizer, 1,  wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    box_sizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Colours"));

    m_fontColourBtn = new wxButton(this, FONT_COLOUR_BTN_ID, wxT("Text Colour"));
    box_sizer->Add(m_fontColourBtn, 1, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    m_backColourBtn = new wxButton(this, BACK_COLOUR_BTN_ID, wxT("Background Colour"));
    box_sizer->Add(m_backColourBtn, 1, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    layout->Add(box_sizer, 1,  wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    layout->Add(wxDialog::CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxEXPAND, 5);

    SetSizer(layout);
}

bool wxFontsNColoursDialog::DoColourDialog(wxColour &colour)
{
    wxColourData colourData;
    colourData.SetColour(colour);

    wxColourDialog dialog(this, &colourData);
    if (dialog.ShowModal() == wxID_OK) {
        colour = dialog.GetColourData().GetColour();
        return true;
    }
    return false;
}

bool wxFontsNColoursDialog::DoFontDialog(wxFont &font, wxColour &colour)
{
    bool ret;
    wxFontData fontData;
    fontData.SetColour(colour);
//    fontData.SetChosenFont(font);
    fontData.SetInitialFont(font);
    wxFontDialog dialog(this,fontData);
    if (dialog.ShowModal() == wxID_OK) {
        fontData = dialog.GetFontData();
        colour = fontData.GetColour();
        font = fontData.GetChosenFont();
        return true;
    }
    return false;
}

void wxFontsNColoursDialog::OnFontColour(wxCommandEvent &event)
{
    wxColour colour = m_colours.GetFontColour();
    if (DoColourDialog(colour) == true)
        m_colours.SetFontColour(colour);
}

void wxFontsNColoursDialog::OnBackColour(wxCommandEvent &event)
{
    wxColour colour = m_colours.GetBackColour();
    if (DoColourDialog(colour) == true)
        m_colours.SetBackColour(colour);
}

void wxFontsNColoursDialog::OnTitleFont(wxCommandEvent &event)
{
    wxColour colour = m_colours.GetFontColour();
    wxFont font = m_fonts.GetTitleFont();
    if (DoFontDialog(font, colour) == true) {
        m_fonts.SetTitleFont(font);
        m_colours.SetFontColour(colour);
    }
}

void wxFontsNColoursDialog::OnContentsFont(wxCommandEvent &event)
{
    wxFont font = m_fonts.GetContentsFont();
    wxColour colour = m_colours.GetFontColour();
    if (DoFontDialog(font, colour) == true) {
        m_fonts.SetContentsFont(font);
        m_colours.SetFontColour(colour);
    }

}
