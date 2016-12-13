#include "../include/wxSongEditor.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>

int wxSongEditor::ID_TITLE_TEXTCTRL = wxNewId();
int wxSongEditor::ID_PAGE_TEXTCTRL = wxNewId();

BEGIN_EVENT_TABLE(wxSongEditor, wxDialog)
//    EVT_SIZE(wxSongEditor::OnSizeEvent)
END_EVENT_TABLE()

wxSongEditor::wxSongEditor(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style, const wxString& name)
 : wxDialog(parent, id, title, pos, size, style, name)
{

    // Initialize Dialog...

    /* Simple Layout:

        [ Title ]

        [ Page numbers ] [ Add | Remove | Clear]

        [ Page Contents ]

        [Font Colour] [Back Colour]

        -----

        [ Dialog Buttons ]
    */

    wxBoxSizer *layout = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer *box_layout = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Song Details"));


    m_titleText = new wxTextCtrl(this, ID_TITLE_TEXTCTRL);

    AddToLayout(wxT("Title:"), m_titleText , box_layout);

    box_layout->AddSpacer(1);
// wxTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr)
    m_pageText = new wxTextCtrl(this, ID_PAGE_TEXTCTRL, wxEmptyString, wxDefaultPosition,
                                wxDefaultSize, wxTE_MULTILINE);

    AddToLayout(wxT("Song Contents:"), m_pageText, box_layout, 5);

    box_layout->AddSpacer(1);

  //  m_titleText = new wxTextCtrl(this, )
    //layout->Add(m_titleText, 0, );

    layout->Add(box_layout, 3, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

    layout->Add(wxDialog::CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxEXPAND, 5);

    SetSizer(layout);


}

wxSongEditor::~wxSongEditor()
{
    //dtor
}

void wxSongEditor::OnSizeEvent(wxSizeEvent &event)
{
    // May need to force layouts to behave.
  /*  wxSizer *sizer = this->GetSizer();
    wxSize sz = GetSize();

    sizer->SetDimension(0, 0, sz.GetWidth(), sz.GetHeight());
    sizer->Layout();*/

    event.Skip();
}

void wxSongEditor::AddToLayout(const wxChar *labelText, wxWindow *widget, wxSizer *sizer, int sz )
{
    if (labelText != 0) {
        sizer->Add(new wxStaticText(this, -1, labelText, wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_CENTER | wxST_NO_AUTORESIZE),
                    0,
                   wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND,
                    5);
    }
    sizer->Add(widget, sz, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);
}

 void wxSongEditor::SetSongData(const wxSongData &src)
 {
    m_titleText->SetValue(src.GetTitle());
    m_pageText->SetValue(src.GetContents());

 }

wxSongData wxSongEditor::GetSongData() const
{
    wxSongData ret;
    ret.SetTitle(m_titleText->GetValue());
    ret.SetContents(m_pageText->GetValue());
    return ret;
}

