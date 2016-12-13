#include "../include/string_func.h"

wxString get_last_mid_string(const wxString &src, wxChar first, wxChar last)
{
    wxString ret;

    int first_index = src.Find(first, true);
    if (first_index < 0)
        return ret;

    int last_index = src.Find(last, true);

    if (last_index <= 0)
        return ret;

    first_index += 1;

    ret = src.Mid(first_index, (last_index - first_index));

    return ret;
}

int wxchar_array_length(const wxChar **src)
{
    int ret = 0;

    if (src != 0) {
        while (src[ret] != 0)
            ret++;
    }

    return ret;
}
