#include "../include/wxColourSet.h"

wxColourSet::wxColourSet(const wxColourSet &src)
{
    SetData(src);
}

wxColourSet::wxColourSet()
{
    //ctor
}

wxColourSet::~wxColourSet()
{
    //dtor
}

wxColourSet &wxColourSet::operator = (const wxColourSet &src)
{
    SetData(src);
    return *this;
}

void wxColourSet::SetData(const wxColourSet &src)
{
    m_fontColour = src.m_fontColour;
    m_backColour = src.m_backColour;
}

void wxColourSet::SetFontColour(const wxColour &colour)
{
    m_fontColour = colour;
}

void wxColourSet::SetBackColour(const wxColour &colour)
{
    m_backColour = colour;
}
#if 0
void wxColourSet::SetColour(const wxString &codename, const wxColour &colour)
{
    m_colours[codename] = colour;
}

wxColour wxColourSet::GetColour(const wxString &codename, const wxColour *defColour) const
{
    wxString cpy(codename);
    wxColour ret;
    if (m_colours.count(codename) > 0)
        ret = m_colours[cpy];
    else if (defColour != 0)
        ret = *defColour;
    return ret;
}
#endif
wxColour wxColourSet::GetFontColour() const
{
    return m_fontColour;
    //return GetColour(wxT("font_colour"), defColour);
}

wxColour wxColourSet::GetBackColour() const
{
    return m_backColour;
    //return GetColour(wxT("back_colour"), defColour);
}
