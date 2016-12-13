#include "../include/wxSongRenderer.h"
#include "../include/wxConfiguration.h"
#include "../include/wxSongsManager.h"
#include "../include/wxSongData.h"
#include "../include/wxPicturesDialog.h"
#include "../include/graphics.h"
#include "../include/wxPicturesManager.h"
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include "../include/wxSongsManager.h"

#include "../include/wxCachedBitmaps.h"

wxSongRenderer::wxSongRenderer()
{
    m_padding = 5;
    m_pictureIndex = 0;
    m_refresh = 0;
    m_dc = 0;
}

wxSongRenderer::~wxSongRenderer()
{
    //dtor
}

void wxSongRenderer::Render(wxDC &dc)
{
    wxSize sz = dc.GetSize();
    Render(dc, wxRect(0, 0, sz.GetWidth(), sz.GetHeight()));
}

void wxSongRenderer::Printf(const wxRect &rect, const char *name)
{
    if (name != 0)
        printf("%s", name);
    printf("(%d,%d,%d,%d)", rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
}
void wxSongRenderer::Printf(wxBitmap bitmap, const char *name)
{
    if (name != 0)
        printf("%s", name);
    printf("(%d, %d)", bitmap.GetWidth(), bitmap.GetHeight());
}
void wxSongRenderer::RenderBitmap(wxDC &dc, const wxRect &size, const wxRect &rect, wxBitmap bitmap)
{
    /* just render zoom style now... */
    wxBitmap render;
    if (size.GetWidth() != bitmap.GetWidth() || size.GetHeight() != bitmap.GetHeight()) {
        render = resize_bitmap(bitmap, size.GetSize(), false);
    } else {
        render = bitmap;
    }

    int x1, y1;

    x1 = size.GetLeft() + (size.GetWidth() - render.GetWidth()) / 2;
    y1 = size.GetTop() + (size.GetHeight() - render.GetHeight()) / 2;

    dc.DrawBitmap(render, x1, y1);
}

wxString wxSongRenderer::DetermineBitmapKey(const wxString &filename, const wxSize &size)
{
    wxString ret(filename);
    ret.Append(wxT(" "));
    ret.Append(wxString::Format(wxT("%d %d"), size.GetWidth(), size.GetHeight()));
    return ret;
}

void wxSongRenderer::ClearRect(wxDC &dc, const wxRect &size, const wxRect &rect)
{
    /* If picture is opted... render that... */
    wxColour backColour = GetBackColour();

    if (m_usePictures > 0) {
        dc.SetPen(m_backgroundPictureColour);
        dc.SetBrush(m_backgroundPictureColour);
        dc.DrawRectangle(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());

        if (g_pictures_manager->GetSize() > 0) {
            int songIndex = m_pictureIndex;
            wxString pictureUrl = g_pictures_manager->GetFilename(songIndex);
            wxBitmap bitmap;
            wxSize sz = size.GetSize();
            wxString key = DetermineBitmapKey(pictureUrl, sz);
            bitmap = g_cached_bitmaps->GetBitmap(key);
            if (!bitmap.IsOk()) {
                bitmap = g_pictures_manager->GetBitmap(pictureUrl);
                wxImage img = bitmap.ConvertToImage();
                img.Rescale(sz.GetWidth(), sz.GetHeight());
                bitmap = wxBitmap(img);
                // save bitmap.
                g_cached_bitmaps->SaveBitmap(key, bitmap);
            }

            if (bitmap.IsOk())
                RenderBitmap(dc, size, size, bitmap);
        }

    } else {

        /* Draw backgroud */
        dc.SetPen(backColour);
        dc.SetBrush(backColour);
        dc.DrawRectangle(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight());
    }
}

wxRect wxSongRenderer::FigureOutRectAlignment(int ww, int hh, wxRect src, int alignment)
{
    int x1, y1;
    if (alignment & (wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTRE_HORIZONTAL)) {
        x1 = src.GetLeft() + (src.GetWidth() - ww) / 2;
    } else if (alignment & wxALIGN_RIGHT) {
        x1 = src.GetLeft() - ww;
    } else {
        x1 = src.GetLeft();
    }

    if (alignment & (wxALIGN_CENTER_VERTICAL | wxALIGN_CENTRE_VERTICAL)) {
        y1 = src.GetTop() + (src.GetHeight() - hh) / 2;
    } else if (alignment & wxALIGN_BOTTOM) {
        y1 =src.GetTop() - hh;
    } else {
        y1 = src.GetTop();
    }

    return wxRect(x1, y1, ww, hh);
}

int wxSongRenderer::RenderWithBitmap(wxDC &dc, wxString text, wxFont font, wxRect dest,
                         wxColour *background, wxRect *extent, int align)
{
    wxBitmap bitmap = generate_text_image(text, 0, &font, &m_fontColour, background);
    int render;
    // render bitmap...
    if (bitmap.IsOk()) {
        wxRect copy(dest);
        bitmap = resize_if_bigger(bitmap ,  dest.GetSize(), true);

        wxRect dest = FigureOutRectAlignment(bitmap.GetWidth(), bitmap.GetHeight(), copy, align);

        dc.DrawBitmap(bitmap, dest.GetLeft(), dest.GetTop());

        if (extent != 0)
            *extent = dest;

        render = 1;
    } else {
        render = 0;
    }
    return render;
}

/*
    This will render the text on dc...

    If the font is too big, it will shrink the font...
*/
int wxSongRenderer::RenderText(wxDC &dc, wxString text, wxFont font, wxRect dest,
                    wxColour *backColour /*= 0*/, wxRect *extent /*= 0*/,
                    int align)
{
    int ww, hh;
    int x1, y1;
    int render;
loop_here:
    dc.SetFont(font);
    dc.GetMultiLineTextExtent(text, &ww, &hh, 0, 0);

    if (ww > dest.GetWidth() || hh > dest.GetHeight()) {
        // text is too big right now.. shrink the font.
        int pt = font.GetPointSize();
        if (pt <= 5) {
            render = 0;
            ww = 0;
            hh = 0;
        } else {
            font.SetPointSize(pt-1);
            goto loop_here;
        }
    } else {
        render = 1;
    }

    wxRect alignrect = FigureOutRectAlignment(ww, hh, dest, align);

    if (render > 0) {
        if (backColour == 0) {
            // clear background.
        //   dc.SetBrush(wxNullBrush);
        // dc.SetPen(wxNullPen);
            dc.SetTextBackground(wxNullColour);
        } else {
            // set background.
//            wxColour col(*backColour);

            if (m_usePictures > 0) {
                // render text again... maybe 2 pixels off...
#if 0
               /* dc.SetBrush(*backColour);
                dc.SetPen(wxPen(*backColour));
                dc.DrawRectangle(x1 - 10, y1 - 10, hh + 20, hh + 20);*/
                DrawRectangle(dc, dest, m_backColour, 0.5);
#else
                dc.SetTextForeground(m_backColour);
                dc.DrawText(text, alignrect.GetLeft()-1,alignrect.GetTop() + 1);
                dc.DrawText(text, alignrect.GetLeft()+1,alignrect.GetTop() + 1);
                dc.DrawText(text, alignrect.GetLeft()+1,alignrect.GetTop() - 1);
                dc.DrawText(text, alignrect.GetLeft()-1,alignrect.GetTop() - 1);
#endif
            } else {
                dc.SetBrush(*backColour);
                dc.SetPen(wxPen(*backColour));
                dc.DrawRectangle(dest);
                dc.SetTextBackground(*backColour);
            }

        }
        dc.SetTextForeground(m_fontColour);

        /* now align the text now */
        dc.DrawText(text, alignrect.GetLeft(), alignrect.GetTop());
    } else {
        // use bitmap rendering as a last resort...
        render = RenderWithBitmap(dc, text, font, dest,
                    backColour, &alignrect, align);
    }

    if (extent != 0)
            *extent = alignrect;

    return render;
}

wxBitmap wxSongRenderer::GenerateBitmap()
{
    if (m_refresh > 0) {

    }
    return m_bitmap;
}

wxString wxSongRenderer::DetermineKey()
{
    wxString key;



    return key;
}

void wxSongRenderer::RenderRectangle(wxImage *dest, const wxRect &rect, wxColour colour, float alpha)
{
    //wxColour backColour = GetBackColour();

    unsigned char *img_data = dest->GetData();

    int x1 = rect.GetLeft(), x2 = rect.GetRight(), y1 = rect.GetTop(), y2 = rect.GetBottom();
    int line = (dest->GetWidth() * 3);
    int red, green, blue;
    red = colour.Red();
    green = colour.Green();
    blue = colour.Blue();
    unsigned char *src;
    for (int y = y1; y <= y2; y++) {
        src = img_data + line * y;
        for (int x = x1; x < x2; x++) {
            *src++ = red * alpha;
            *src++ = green* alpha;
            *src++ = blue * alpha;
        }
    }
}

void wxSongRenderer::Render(wxDC &dc, const wxRect &dest, bool clearBackground)
{
    m_dc = &dc;
#if 0
    wxRect imgrect(0, 0, dest.GetWidth(), dest.GetHeight());
    wxImage image(dest.GetWidth(), dest.GetHeight());
    wxSize bitmapSize = dest.GetSize();

    if (clearBackground)
        RenderRectangle(&image, imgrect, GetBackColour(), 0.5);



    dc.DrawBitmap(wxBitmap(image), dest.GetLeft(), dest.GetTop());
#else
    wxRect rect;
    int x1, y1;
    wxSize size = dest.GetSize();

    /* Render the image onto the dc */
    if (clearBackground)
        ClearRect(dc, dest, dest);

    /*
        Render Title...
    */
    wxColour backColour = GetBackColour();

#ifdef RENDER_WITH_BITMAP
    wxBitmap bitmap = generate_text_image(m_title, 0, &m_titleFont, &m_fontColour, &backColour);

    rect = dest;

    // render bitmap...
    if (bitmap.IsOk()) {
        bitmap = resize_if_bigger(bitmap , size, true);

        x1 = (rect.GetWidth() - bitmap.GetWidth()) / 2 + dest.GetLeft();

        dc.DrawBitmap(bitmap, x1, dest.GetTop());

        // reuse variable name.
        x1 = bitmap.GetHeight() + m_padding * 2;
        rect.SetTop(rect.GetTop() + x1);
        rect.SetHeight(rect.GetHeight() - x1);
    }

    // now render song text.
    bitmap = generate_text_image(m_content, 0, &m_contentFont, &m_fontColour, &backColour);
    if (bitmap.IsOk()) {
        bitmap = resize_if_bigger(bitmap, rect.GetSize(), true);
        x1 = (rect.GetWidth() - bitmap.GetWidth()) / 2 + rect.GetLeft();
        y1 = (rect.GetHeight() - bitmap.GetHeight()) / 2 + rect.GetTop();
        dc.DrawBitmap(bitmap, x1, y1);
    }
#else
/* void RenderText(wxDC &dc, wxString text, wxFont font, wxRect dest,
                    wxColour *background = 0, wxRect *extent = 0,
                    int align = wxALIGN_CENTRE);*/
    wxRect extentRect;
    rect = dest;
    // render the title text...
    RenderText(dc, m_title, m_titleFont, rect,
                    &m_backColour, &extentRect,
                    (wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL));

    rect.SetHeight(rect.GetBottom() - extentRect.GetBottom());
    rect.SetTop(extentRect.GetBottom());

    RenderText(dc, m_content, m_contentFont, rect, &m_backColour, 0, (wxALIGN_CENTER));
#endif
#endif
    m_dc = 0;
}

void wxSongRenderer::SetByConfiguration(wxConfiguration *config)
{
    wxASSERT(config != 0);

    m_backColour = config->GetColour(wxT("back_colour"));
    m_fontColour = config->GetColour(wxT("font_colour"));
    m_titleFont = config->GetFont(wxT("title_font"));
    m_contentFont = config->GetFont(wxT("contents_font"));

    m_usePictures = config->GetInt(wxPicturesDialog::UseGraphicsEnabledParam);
    m_backgroundPictureColour = config->GetColour(wxPicturesDialog::GraphicsBackgroundColourParam);
}

void wxSongRenderer::SetSong(int songIndex)
{
    wxASSERT(g_songs_manager != 0);

    wxSongData song;
    g_songs_manager->GetSongByRowId(songIndex, &song);
    m_title = song.GetTitle();
    m_content = song.GetContents();
//    m_picture_index = rand() % 100;
}

void wxSongRenderer::SetPictureIndex(int index)
{
    m_pictureIndex = index;
}

int wxSongRenderer::GetPictureIndex() const
{
    return m_pictureIndex;
}


wxColour wxSongRenderer::GetBackColour() const
{
    wxColour ret;
    //(m_backColour);
    if (m_usePictures > 0)
        ret.Set(m_backColour.Red(), m_backColour.Green(), m_backColour.Blue(), 50);
    else
        ret = m_backColour;
    return ret;
}

void wxSongRenderer::DrawRectangle(wxDC &dc, const wxRect &dest, const wxColour &colour, float blend /*= 1.0*/)
{
    if (blend >= 1.0) {
        dc.SetPen(colour);
        dc.SetBrush(colour);
        dc.DrawRectangle(dest.GetLeft(), dest.GetTop(), dest.GetWidth(), dest.GetHeight());
    } else {
        // blend... this will be slow...
#if 1
        int x1 = dest.GetLeft();
        int y1 = dest.GetTop();
        int x2 = x1 + dest.GetWidth();
        int y2 = y1 + dest.GetHeight();
        wxColour src_colour, dst_colour, last_src_colour;
        for (int x = x1; x < x2; x++) {
            for (int y = y1; y < y2; y++) {
                if (dc.GetPixel(x, y, &src_colour)) {
                    // blend colours.
                    if (last_src_colour != src_colour) {
                        dst_colour.Set(BlendInt(src_colour.Red(), colour.Red(), blend),
                                       BlendInt(src_colour.Green(), colour.Green(), blend),
                                       BlendInt(src_colour.Blue(), colour.Blue(), blend) );
                        dc.SetPen(dst_colour);
                        last_src_colour = src_colour;
                    }
                    dc.DrawPoint(x, y);
                }
            }
        }
#else

#endif
    }
}

int wxSongRenderer::BlendInt(int src, int dest, float blend)
{
    int ret = (dest-src) * blend;
    return ret;
}

int wxSongRenderer::WithinRange(int min, int max, int test)
{
    if (test < min)
        return min;
    else if (test > max)
        return max;
    return test;
}

wxString wxSongRenderer::GenerateBitmapKey()
{
    wxString ret;
    return ret;
}

bool wxSongRenderer::GetBitmapCache(wxBitmap *bitmap)
{


}
