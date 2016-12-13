#include "../include/wxFontSet.h"

wxFontSet::wxFontSet(const wxFontSet &src)
{
    SetData(src);
}

wxFontSet::wxFontSet()
{
    //ctor
}

wxFontSet::~wxFontSet()
{
    //dtor
}

wxFontSet & wxFontSet::operator = (const wxFontSet & src)
{
    SetData(src);
    return *this;
}
void wxFontSet::SetData(const wxFontSet & src)
{
    m_titleFont = src.m_titleFont;
    m_contentsFont = src.m_contentsFont;
}

void wxFontSet::SetTitleFont(const wxFont &font)
{
    m_titleFont = font;
}
void wxFontSet::SetContentsFont(const wxFont &font)
{
    m_contentsFont = font;
}
#if 0
void wxFontSet::SetFont(const wxString &codename, const wxFont &font)
{
    m_fonts[codename] = font;
}
#endif
wxFont wxFontSet::GetTitleFont() const
{
    return m_titleFont;
}

wxFont wxFontSet::GetContentsFont() const
{
    return m_contentsFont;
}
#if 0
wxFont wxFontSet::GetFont(const wxString &fontname, const wxFont *defFont) const
{/*  std::map<wxString, wxColour> m_colours;
    std::map<wxString, wxFont> m_fonts;*/
    wxFont ret;
    if (m_fonts.count(fontname) > 0)
        ret = m_fonts[fontname];
    else if (defFont != 0)
        ret = *defFont;
    return ret;
}
#endif
