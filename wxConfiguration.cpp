#include "../include/wxConfiguration.h"

#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/settings.h>
#include <wx/msgdlg.h>

wxConfiguration *g_configuration = 0;

wxConfiguration::wxConfiguration(const wxConfiguration &src)
{
    SetData(src);
}

 wxConfiguration::wxConfiguration()
 {
     if (g_configuration == 0)
        g_configuration = this;
#ifdef CONFIG_USE_STD_MAP
#else
     m_needToSort = 0;
     m_modified = 0;
#endif
 }
 wxConfiguration::~wxConfiguration()
 {
    if (g_configuration == this)
        g_configuration = 0;
 }

wxConfiguration & wxConfiguration::operator = (const wxConfiguration &src)
{
    SetData(src);
    return *this;
}

void wxConfiguration::SetData(const wxConfiguration &src)
{
#ifdef CONFIG_USE_STD_MAP
    m_data = src.m_data;
#else
    m_keys = src.m_keys;
    m_values = src.m_values;
    m_needToSort = src.m_needToSort;
#endif
    m_modified = src.m_modified;
}

void wxConfiguration::SetValue(const wxString &key, const wxString &value)
{
#ifdef CONFIG_USE_STD_MAP
    m_data[key] = value;
    m_modified++;
#else
    // See if the item exists...
    int i = FindItem(key);
    if (i < 0)
        AddItem(key, value);
    else
        SetValueAt(i, value);
#endif
}

#ifdef CONFIG_USE_STD_MAP
wxString wxConfiguration::GetKeyForIndex(int index)
{
    wxString ret;
    std::map<const wxString, wxString>::iterator it;

    for (it = m_data.begin(); it != m_data.end(); it++) {
        if (index-- == 0) {
            ret = (*it).first;
            break;
        }
    }

    return ret;
}
#endif
void wxConfiguration::SetValueAt(int index, const wxString &value)
{
    assert(index >= 0 && index < GetCount());

#ifdef CONFIG_USE_STD_MAP
    wxString key = GetKeyForIndex(index);
    m_data[key] = value;
#else
    m_values[index] = value;
#endif
    m_modified++;
}

wxString wxConfiguration::GetValue(const wxString &key, const wxChar *default_value)
{
    wxString ret;

    int i = FindItem(key);
    if (i >= 0)
        ret = GetValueAt(i);
    else if (default_value != 0)
        ret = default_value;

    return ret;
}

bool wxConfiguration::LoadConfiguration(const wxString &filename)
{
    Clear();

    // Open the file.
    if (wxFileExists(filename) == false)
        return true;
    wxFileInputStream in(filename);
    if (in.IsOk()) {
        wxTextInputStream txtin(in);
        wxString line, key, value;
        while (in.Eof() == false) {
            line = txtin.ReadLine();
            if (line.Len() > 0) {
                if (line[0] != wxChar('#')) {
                    // divide the string into key=value.
                    int equal = line.Find(wxT("="));
                    if (equal >= 0) {
                        key = line.Left(equal);
                        value = line.Mid(equal + 1);
                        AddItem(key, value);
                    }
                }
            }
        }
    }
    m_modified = 0;
    return true;
}

/*
    This will write the configuration file.
*/
bool wxConfiguration::SaveConfiguration(const wxString &filename)
{
    // save file as is.
    wxFileOutputStream out(filename);

    if (out.IsOk()) {
        wxTextOutputStream textout(out);
        textout << wxT("# wxConfiguration") << endl;
#ifdef CONFIG_USE_STD_MAP
        std::map<wxString, wxString>::iterator it;
        for (it = m_data.begin(); it != m_data.end(); ++it) {
            textout << (*it).first << wxT("=") << (*it).second << endl;
        }
#else
        int c = GetCount();
        for (int x = 0; x < c; x++) {
            textout << m_keys[x] << wxT("=") << m_values[x] << endl;
        }
#endif
    }

    m_modified = 0;

    return true;
}

int wxConfiguration::FindItem(const wxString &key)
{
#ifdef CONFIG_USE_STD_MAP
    int index = 0;
    std::map<wxString, wxString>::iterator it;
    for (it = m_data.begin(); it != m_data.end(); ++it) {
        if ((*it).first == key)
            return index;
        index++;
    }
#else
    if (m_needToSort > 0)
        Sort();

    int low = 0, high = GetCount();
    int middle, cmp;

loop_here:
    middle = (low + high) / 2;

    if (middle < high) {
        cmp = key.Cmp(m_keys.Item(middle));
        if (cmp == 0) {
            return middle;
        } else if (cmp > 0) { // key > middle
            low = middle+1;
        } else { // key < middle
            high = middle;
        }
        if (low <= high)
            goto loop_here;
    }
#endif
    return -1;
}

int wxConfiguration::GetCount()
{
#ifdef CONFIG_USE_STD_MAP
    return m_data.size();
#else
    return m_keys.Count();
#endif
}

#ifndef CONFIG_USE_STD_MAP
void wxConfiguration::Sort()
{
    int c = m_keys.GetCount();

    if (c-- > 0) {
        wxString copy;
        bool sorted = true;
        int adj;
        while(sorted == true) {
            sorted = false;
            for (int x = 0; x < c; x++) {
                adj = x + 1;
                if (m_keys[x] > m_keys[adj]) {
                    // swap keys first.
                    copy = m_keys[x];
                    m_keys[x] = m_keys[adj];
                    m_keys[adj] = copy;

                    // swap values now.
                    copy = m_values[x];
                    m_values[x] = m_values[adj];
                    m_values[adj] = copy;

                    sorted = true;
                }
            }
        }
    }
    m_needToSort = 0;

}
#endif

void wxConfiguration::Clear()
{
#ifdef CONFIG_USE_STD_MAP
    m_data.clear();
#else
    m_keys.Clear();
    m_values.Clear();
    m_needToSort = 0;
#endif
    m_modified = 1;
}

void wxConfiguration::DeleteValue(const wxString &key)
{
    int i = FindItem(key);
    if (i >= 0)
        DeleteAt(i);
}

void wxConfiguration::DeleteAt(int index)
{
    assert(index >= 0 && index < GetCount());
#ifdef CONFIG_USE_STD_MAP
    wxString key = GetKeyForIndex(index);
    m_data.erase(key);
#else
    m_keys.RemoveAt(index, 1);
    m_values.RemoveAt(index, 1);
#endif
}

wxString wxConfiguration::GetValueAt(int index)
{
  assert(index >= 0 && index < m_data.size());
#ifdef CONFIG_USE_STD_MAP
    wxString key = GetKeyForIndex(index);
    wxString ret(m_data[key]);
#else
    wxString ret(m_values[index]);

#endif
    return ret;
}

/*
    This will add an item at the end of the keys/values.
*/
void wxConfiguration::AddItem(const wxString &src, const wxString &value) // protected
{
#ifdef CONFIG_USE_STD_MAP
    m_data[src] = value;
#else
    m_keys.Add(src);
    m_values.Add(value);
    m_needToSort++;
#endif
    m_modified++;
}

void wxConfiguration::SetColour(const wxString &name, const wxColour &colour)
{
    SetValue(name, colour.GetAsString(wxC2S_HTML_SYNTAX));
  //  assert(colour == GetColour(name, 0));
}

wxColour wxConfiguration::GetColour(const wxString &name, const wxColour *defaultColour)
{
    wxColour ret;
    wxString val;
    val = GetValue(name);
  //  wxMessageBox(name + val);
    if (!val.IsEmpty()) {
        ret.Set(val);
   //     wxMessageBox(ret.GetAsString(wxC2S_HTML_SYNTAX));
    } else if (val.IsNull() && defaultColour != 0)
        ret = *defaultColour;
    return ret;
}

void wxConfiguration::SetFont(const wxString &name, const wxFont &font)
{
   // wxMessageBox(font.GetNativeFontInfoUserDesc());
    SetValue(name, font.GetNativeFontInfoUserDesc());
    //wxMessageBox(GetFont(name, 0).GetNativeFontInfoUserDesc());
 //   assert(font == GetFont(name, 0));
}

wxFont wxConfiguration::GetFont(const wxString &name, const wxFont *defaultFont)
{
    wxFont ret;
    wxString val = GetValue(name);
    if (!val.IsEmpty())
        ret.SetNativeFontInfoUserDesc(val);
    else if (defaultFont != 0)
        ret = *defaultFont;
    return ret;
}

wxColourSet wxConfiguration::GetColours()
{
    wxColourSet ret;

    wxColour col;
    col = GetColour(wxT("font_colour"), 0);
    if (!col.IsOk())
        col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    ret.SetFontColour(col);

    col = GetColour(wxT("back_colour"), 0);
    if (!col.IsOk())
        col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    ret.SetBackColour(col);

    return ret;
}

wxFontSet wxConfiguration::GetFonts()
{
    wxFontSet ret;

    wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    ret.SetTitleFont(GetFont(wxT("title_font"), &defFont));
    ret.SetContentsFont(GetFont(wxT("contents_font"), &defFont));

    return ret;
}

void wxConfiguration::SetColours(const wxColourSet &src)
{
    SetColour(wxT("font_colour"), src.GetFontColour());
    SetColour(wxT("back_colour"), src.GetBackColour());
}

void wxConfiguration::SetFonts(const wxFontSet &src)
{
    SetFont(wxT("title_font"), src.GetTitleFont());
    SetFont(wxT("contents_font"), src.GetContentsFont());
}

void wxConfiguration::SetInt(const wxString &name, int value)
{
    wxString str;
    str.sprintf(wxT("%d"), value);
    SetValue(name, str);
}
int wxConfiguration::GetInt(const wxString &name, int def)
{
    wxString val = GetValue(name, 0);
    if (val.IsNull())
        return def;
    long ret;
    val.ToLong(&ret);
    return ret;
}

void wxConfiguration::SetBool(const wxString &name, bool value)
{
    int val;
    if (value)
        val = 1;
    else
        val = 0;
    SetInt(name, val);
}
bool wxConfiguration::GetBool(const wxString &name, int default_value)
{
    return (GetInt(name, default_value) > 0);
}
