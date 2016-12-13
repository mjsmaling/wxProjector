#include "../include/wxState.h"

wxState::wxState(const wxState &src)
{
    SetData(src);
}
wxState::wxState(const wxChar *message, int code)
{
    m_message = message;
    m_code = code;
}
wxState::~wxState()
{
    // destructor.
}

wxState &wxState::operator = (const wxState &src)
{
    SetData(src);
    return *this;
}
void wxState::SetData(const wxState &src)
{
    SetData(src.m_message, src.m_code);
}

void wxState::SetData(const wxString &message, int code)
{
    m_message = message;
    m_code = code;
}

void wxState::SetData(const char *message, int code)
{
    SetData(wxString(message, wxConvUTF8), code);
}

void wxState::GetData(wxString *message, int *code)
{
    if (message != 0)
        *message = m_message;
    if (code != 0)
        *code = m_code;
}

void wxState::SetOk()
{
    SetData(wxEmptyString, 0);
}
