#include "../include/wxCellGuiRect.h"

wxCellGuiRect::wxCellGuiRect()
{
    //ctor
}

wxCellGuiRect::wxCellGuiRect(const wxCellGuiRect &src)
{
    SetData(src);
}

wxCellGuiRect::~wxCellGuiRect()
{
    //dtor
}

void wxCellGuiRect::SetData(const wxCellGuiRect &src)
{
    m_rect = src.m_rect;
    m_padding = src.m_padding;
    m_border = src.m_border;
}

wxCellGuiRect & wxCellGuiRect::operator = (const wxCellGuiRect &src)
{
    SetData(src);
    return *this;
}

void wxCellGuiRect::SetData(const wxRect &src, int padding, int border)
{
    m_rect = src;
    m_padding = padding;
    m_border = border;
}

void wxCellGuiRect::GetData(wxRect *rect, int *padding, int *border)
{
    if (rect != 0)
        *rect = m_rect;
    if (padding != 0)
        *padding = m_padding;
    if (border != 0)
        *border = m_border;
}

wxRect wxCellGuiRect::GetRect() const
{
    wxRect rect(m_rect);
    return m_rect;
}

wxRect wxCellGuiRect::GetBorderRect() const
{
    wxRect rect(m_rect);
    rect.Inflate(m_border + m_padding);
    return rect;
}

wxRect wxCellGuiRect::GetPaddingRect() const
{
    wxRect rect(m_rect);
    rect.Inflate(m_padding);
    return rect;
}
