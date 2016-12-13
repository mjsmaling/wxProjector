#include "../include/graphics.h"

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <assert.h>

wxIcon graphics_load_icon(const unsigned char *src, int size, const wxSize *sz)
{
    wxIcon ret;
    ret.CopyFromBitmap(graphics_load_bitmap(src,size));
    return ret;
}

wxBitmap graphics_load_bitmap(unsigned const char *src, int size, const wxSize *sz)
{
    wxMemoryInputStream stream((const char *)src, size);

    // if there is no size adjustment, return as is.
    if (sz == 0)
        return wxBitmap(stream);

    wxImage image(stream);
    assert(image.IsOk());

    // Maybe faster referencing with these local variables...
    int img_width = image.GetWidth(), img_height = image.GetHeight();
    int sz_width = sz->GetWidth(), sz_height = sz->GetHeight();

    // Want to keep aspect ratio...
    if (img_width != sz_width || sz_height !=img_height) {
        int width_diff = sz->GetWidth() - image.GetWidth();
        int height_diff = sz->GetHeight() - image.GetHeight();

        float diff;

        if (abs(width_diff) > abs(height_diff)) {
            if (sz_width == 0)
                diff = 1.0;
            else
                diff = (float) img_width / (float) sz_width;
        } else {
            if (sz_height == 0)
                diff = 1.0;
            else
                diff = (float) img_height / (float) sz_height;
        }

        image.Rescale(img_width / diff, img_height / diff, wxIMAGE_QUALITY_HIGH);
    }

    return wxBitmap(image);
}

/*
    text - the text to calculate the multiline text.
    dc - the dc context.
    rect - the bounding rect.
    strings - optional - where to put the strings
    rects - optional - where to put th calcualted rects of each string line.
    int align - alignment properties.
*/
void determine_multiline_text_extents(const wxString &text, wxDC *pdc, const wxRect &rect,
        wxArrayString *pstrings, std::vector<wxRect> *prects, int align)
{
    wxString src(text);

    wxArrayString strs;
    std::vector<wxRect> rects;

    int total_height = 0, max_width = 0;

    /* First get the widths */
    while (src.Length() > 0 && total_height < rect.GetHeight()) {
        wxSize sz;
        //  wxString determine_line_extent(const wxString &src, wxDC *dc, const wxRect &rect, wxSize *size);
        wxString midstr = determine_line_extent(src, pdc, rect, &sz);

        // if there is no maxlen, then break here.
        if (midstr.Length()== 0)
            break;

        src = src.Mid(midstr.Length());

        midstr.Trim();
        strs.Add(midstr);
        rects.push_back(wxRect(0, 0, sz.GetWidth(), sz.GetHeight()));

        total_height += sz.GetHeight();
        if (sz.GetWidth() > max_width)
            max_width = sz.GetWidth();
    }
    if (pstrings != 0) {
        for (int x = 0; x < strs.GetCount(); x++) {
            pstrings->Add(strs[x]);
        }
    }
    if (prects != 0) {
        /* Now align the text... for now assume center...*/
        int y1 = rect.GetTop() + (rect.GetHeight() - total_height) / 2;

        std::vector<wxRect>::iterator it;
        for (it = rects.begin(); it != rects.end(); ++it) {
            int x1 = rect.GetLeft() + (rect.GetWidth() - it->GetWidth()) / 2;
            wxRect rct(x1, y1, it->GetWidth(), it->GetHeight());
            y1 += it->GetHeight();
            prects->push_back(rct);
        }
    }
}

/*
    This will determine the maximum characters that can be displayed within 'size'.

    src - the source string.
    dc - the DC context. must be valid.
    rect - the bounding rect.
    size - optional - this will have the latest size extent propeties.

    will return the string of characters that would be permitted to be displayed.
*/
wxString determine_line_extent(const wxString &src, wxDC *dc, const wxRect &rect, wxSize *size)
{
   return determine_line_extent(src, dc, rect.GetWidth(), size);
}
/*
    This will determine the maximum characters that can be displayed within 'size'.

    src - the source string.
    dc - the DC context. must be valid.
    width - the max width.
    size - optional - this will have the latest size extent propeties.

    will return the string of characters that would be permitted to be displayed.
*/
wxString determine_line_extent(const wxString &src, wxDC *dc, int width, wxSize *size)
{
     wxASSERT(dc != 0);
    wxString ret;

    int len = src.Len();

    int high = len;
    int low = 0;
    int mid = high;
    int last_valid_char_length = 0;
    wxSize sz, last_valid_size;
loop_here:
    ret = src.Mid(0, mid);

    sz=dc->GetTextExtent(ret);

    if (sz.GetWidth() != width) {
        if (sz.GetWidth() > width) {
            high = mid;
        } else {
            low = mid + 1;
            last_valid_char_length = mid;
            last_valid_size = sz;
        }
        if (low < high) {
            mid = (low + high) / 2;
            goto loop_here;
        }
        ret = src.Mid(0, last_valid_char_length);
    } else {
        last_valid_char_length = mid;
        last_valid_size = sz;
    }
    if (size != 0)
        *size = last_valid_size;

    return ret;
}

/*
    This will generate a text image of src.


*/
wxBitmap generate_text_image(const wxString &src, int align, wxFont *font, wxColour *fontColour,
                                      wxColour *backColour)
{
    wxBitmap ret, dummy;

    wxMemoryDC dc(ret);

    /* see how much space src needs */
    int w, h;
    if (font != 0)
        dc.SetFont(*font);
    dc.GetMultiLineTextExtent(src, &w, &h, 0);

    dc.SelectObject(dummy);

    ret = wxBitmap(w, h);
    dc.SelectObject(ret);


    // Now render the contents.
    if (backColour != 0) {
        dc.SetBrush(wxBrush(*backColour));
        dc.SetPen(wxPen(*backColour));
        dc.DrawRectangle(0, 0, w, h);
    }

    if (fontColour != 0)
        dc.SetTextForeground(*fontColour);

    /* now align the text now */
    dc.DrawText(src, 0, 0);

    dc.SelectObject(dummy);

    return ret;
}

wxBitmap resize_bitmap(wxBitmap &src, const wxSize &size, bool preserve_ratio)
{
    wxImage image(src.ConvertToImage());

    int ww, hh;
    int orig_w = src.GetWidth(), orig_h = src.GetHeight();
    int quality;
    if (preserve_ratio == false) {
        ww = size.GetWidth();
        hh = size.GetHeight();
        quality = wxIMAGE_QUALITY_HIGH;
    } else {
        // recalculate size...
        float ratio_x, ratio_y;
        ratio_x = (float) size.GetWidth() / (float) src.GetWidth();
        ratio_y = (float) size.GetHeight() / (float) src.GetHeight();

        if (fabs(ratio_x) > fabs(ratio_y)) {
            ww = src.GetWidth() * ratio_y;
            hh = src.GetHeight() * ratio_y;
            quality = wxIMAGE_QUALITY_NORMAL;
        } else {
            ww = src.GetWidth() * ratio_x;
            hh = src.GetHeight() * ratio_x;
            quality = wxIMAGE_QUALITY_HIGH;
        }
    }
    image.Rescale(ww, hh, wxIMAGE_QUALITY_NORMAL);

    return wxBitmap(image);
}

wxBitmap resize_if_bigger(wxBitmap &src, const wxSize &size, bool preserve_ratio)
{
    wxBitmap ret;
    if (src.GetWidth() > size.GetWidth() || src.GetHeight() > size.GetHeight())
        ret = resize_bitmap(src, size, preserve_ratio);
    else
        ret = src;
    return ret;
}
