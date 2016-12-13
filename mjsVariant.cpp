#include "../include/mjsVariant.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef mjsVARIANT_DEFINE_WXARRAY
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(mjsVariantArray);
#endif

char *mjsVariant::string_buffer = 0;
int mjsVariant::string_buffer_allocated = 0;
int mjsVariant::m_instances = 0;

void mjsVariant::SetStringBufferLength(int len)
{
    if (len == 0) {
         if (string_buffer != 0) {
            free(string_buffer);
            string_buffer = 0;
            string_buffer_allocated = 0;
        }
    } else if (len > string_buffer_allocated || len < (string_buffer_allocated - 100)) {
        string_buffer = (char *)realloc(string_buffer, len);
        string_buffer_allocated = len;
    }
}

void mjsVariant::SetStringBuffer(const char *string)
{
    if (string == 0) {
        SetStringBufferLength(0);
    } else {
        int l = strlen(string) + 1;
        SetStringBufferLength(l);
        strcpy(string_buffer, string);
    }
}

mjsVariant::mjsVariant(const mjsVariant &src)
{
    Init();
    SetData(src);
}

mjsVariant::mjsVariant()
{
    Init();
}

mjsVariant::~mjsVariant()
{
    Free();
    DecrementInstance();
}

void mjsVariant::IncrementInstance()
{
    if (m_instances++ == 0)
        SetStringBufferLength(50);
}
void mjsVariant::DecrementInstance()
{
    if (--m_instances == 0)
        SetStringBufferLength(0);
}

void mjsVariant::Init()
{
    m_type = None;
    m_size = 0;
    IncrementInstance();
}

void mjsVariant::SwitchType(int type)
{
    if (m_type != type) {
        Free();
        m_type = type;
        switch (m_type) {
        case Double:
            m_size = sizeof(double);
            break;
        case Long:
            m_size = sizeof(long);
            break;
        case String:
            m_string = 0;
            break;
        case Blob:
            m_blob = new mjsSharedBlob();
            break;
        case WXString:
            m_wxString= new wxString();
            break;
        case None:
            m_size = 0;
            break;
        };
    }
}

void mjsVariant::Free()
{
    switch (m_type) {
    case String:
        if (m_string != 0) {
            free(m_string);
            m_size = 0;
        }
        break;
    case Blob:
        if (m_blob != 0) {
            delete m_blob;
            //free(m_blob);
            m_size = 0;
        }
        break;
    case WXString:
        if (m_wxString != 0) {
            delete m_wxString;
            m_wxString = 0;
        }
        m_size = 0;
        break;
    };
    m_type = None;
}

void mjsVariant::SetData(long value)
{
    SwitchType(Long);
    m_long = value;
}

void mjsVariant::SetData(double value)
{
    SwitchType(Double);
    m_double = value;
}

void mjsVariant::SetData(const char *value)
{
#ifdef mjsVARIANT_USE_WXSTRING
    wxString str(value, wxConvUTF8);
    SetData(str);
#else
    SwitchType(String);
    if (value != 0) {
        int len = strlen(value);
        int sz = len + 1;
        if (sz > m_size) {
            m_string = (char *)realloc(m_string, sz);
            if (m_string == 0)
                m_size = 0;
            else
                m_size = sz;
        }
        if (m_string != 0)
            strcpy(m_string, value);
    }
#endif
}

void mjsVariant::SetData(const void *value, int size)
{
    SwitchType(Blob);
    if (value != 0) {
        m_blob->SetData(value, size);
        m_size = m_blob->GetDataSize();
#if 0
        if (size > m_size) {
            m_blob = realloc(m_blob, size);
            if (m_blob == 0)
                m_size = 0;
            else
                m_size = size;
        }
        if (m_blob != 0)
            memcpy(m_blob, value, size);
#endif
    }
}

void mjsVariant::SetData(const mjsSharedBlob &blob)
{
    SwitchType(Blob);
    m_blob->SetData(blob);
    m_size = m_blob->GetDataSize();
}

char *mjsVariant::GetDataString() const
{
    char *ret;
    if (m_type == String)
        ret = m_string;
    else if (m_type == None) {
        ret = 0;
    } else {
        ret = string_buffer;
        if (m_type == Blob) {
            // try and find a '\0' in the blob.
            int found = -1;
            for (int x = 0; x < m_size; x++) {
                if (m_blob->GetChar(x) == '\0') {
                    found = x;
                    break;
                }
            }
            if (found < 0) {
                SetStringBufferLength(m_size + 1);
                string_buffer[m_size] = '\0';
                found = m_size;
            } else
                SetStringBufferLength(m_size);
            memcpy(string_buffer, m_blob->GetData(), m_size);
#ifdef mjsVARIANT_USE_WXSTRING
        } else if (m_type == WXString) {
            if (m_wxString != 0) {
                SetStringBufferLength(m_wxString->Len()+1);
                strcpy(string_buffer, m_wxString->mb_str(wxConvUTF8));
            } else {
                ret = 0;
            }
#endif
        } else {
            SetStringBufferLength(50);
            if (m_type == Long)
                sprintf(string_buffer, "%ld", m_long);
            else if (m_type == Double)
                sprintf(string_buffer, "%lf", m_double);
            else
                ret = 0;
        }
    }
    return ret;
}

void *mjsVariant::GetDataBlob() const
{
    if (m_type == Blob && m_size > 0)
        return m_blob->GetData();
    return 0;
}

mjsSharedBlob mjsVariant::GetDataSharedBlob() const
{
    mjsSharedBlob ret;
    if (GetSize() > 0) {
        if (m_type == Blob) {
            ret = *m_blob;
        } else if (m_type == String) {
            ret.SetData(m_string, m_size + 1);
        } else if (m_type == Long)
            ret.SetData(&m_long, sizeof(m_long));
        else if (m_type == Double)
            ret.SetData(&m_double, sizeof(m_double));
#ifdef mjsVARIANT_USE_WXSTRING
        else if (m_type == WXString) {
            // need to get a char * of the wxstring.
            char *str = GetDataString();
            ret.SetData(str, strlen(str) + 1);
        }
#endif
    }
    return ret;
}

long mjsVariant::GetDataLong() const
{
    long ret;
    switch (m_type) {
    case Long:
        ret = m_long;
        break;
    case Double:
        ret = (long) m_double;
        break;
    case String:
        if (m_size > 0)
            sscanf(m_string, "%f", &ret);
        break;
    case Blob:
        if (m_size > sizeof(long))
            memcpy(&ret, m_blob, sizeof(long));
        break;
#ifdef mjsVARIANT_USE_WXSTRING
    case WXString:
        if (m_wxString != 0)
            m_wxString->ToLong(&ret);
        break;
#endif
    default:
        break;
    };
    return ret;
}

double mjsVariant::GetDataDouble() const
{
    double ret;
    switch (m_type) {
    case Double:
        ret = m_double;
        break;
    case Long:
        ret = m_long;
        break;
    case String:
        if (m_string != 0)
            sscanf(m_string, "%ld", &ret);
        break;
    case Blob:
        if (m_size >= sizeof(double))
            memcpy(&ret, m_blob, sizeof(double));
        break;
#ifdef mjsVARIANT_USE_WXSTRING
    case WXString:
        if (m_wxString != 0)
            m_wxString->ToDouble(&ret);
        break;
#endif
    default:
        break;
    };
    return ret;
}
#if 0
int mjsVariant::GetSize()
{
    int ret;
    switch(m_type) {
    case None:
        ret = 0;
        break;
    case Long:
        ret = sizeof(long);
        break;
    case Double:
        ret = sizeof(double);
        break;
    case String:
    case Blob:
        ret = m_size;
        break;
#ifdef mjsVARIANT_USE_WXSTRING
    case WXString:
        ret = m_wxString->GetLength();
        break;
#endif
    default:
        ret = 0;
        break;
    };
    return ret;
}
#endif

void mjsVariant::SetData(const mjsVariant &src)
{
    switch(src.GetType()) {
    case Long:
        SetData(src.m_long);
        break;
    case Double:
        SetData(src.m_double);
        break;
    case String:
        SetData(src.m_string);
        break;
    case Blob:
        SetData(src.GetDataBlob(), src.GetSize());
        break;
#ifdef mjsVARIANT_USE_WXSTRING
    case WXString:
        SetData(src.GetDataWXString());
        break;
#endif
    case None:
    default:
        Free();
        break;
    };
    if (!(*this == src)) {
        printf("This does not match. When it should.\n");
        printf("%d \n", *this == src);
    }
}

mjsVariant &mjsVariant::operator =(const mjsVariant &src)
{
    SetData(src);
    return *this;
}

bool mjsVariant::operator == (const mjsVariant &src)
{
    bool ret;

    if (m_type != src.m_type)
        return false;
    else {
        switch (m_type) {
        case Long:
            ret = (src.m_long == m_long);
            break;
        case Double:
            ret = (src.m_double == m_double);
            break;
        case String:
            if (m_string == 0 && src.m_string == 0)
                ret = true;
            else {
                if (m_string != 0 || src.m_string == 0)
                    ret = false;
                else
                    ret = (strcmp(m_string, src.m_string) == 0);
            }
            break;
        case Blob:
            if (m_blob == 0)
                ret = (src.m_blob == 0);
            else
                ret = m_blob->SameAs(*src.m_blob);
            break;
#ifdef mjsVARIANT_USE_WXSTRING
        case WXString:
            if (m_wxString == 0) {
                ret = (src.m_wxString == 0);
            } else {
                if (src.m_wxString == 0)
                    ret = false;
                else
                    ret = (m_wxString->Cmp(*src.m_wxString) == 0);
            }
            break;
#endif
        case None:
        default:
            ret = (src.m_type == None);
            break;
        };

    }

    return ret;
}

#ifdef mjsVARIANT_USE_WXSTRING
void mjsVariant::SetData(const wxString &src)
{
    SwitchType(WXString);
    if (m_wxString != 0)
        *m_wxString = src;
}
wxString mjsVariant::GetDataWXString() const
{
    wxString ret;
    if (m_type == WXString) {
        if (m_wxString != 0)
            ret = *m_wxString;
    } else {
        // convert to string.
        char *str = GetDataString();
        if (str != 0)
            ret = wxString(str, wxConvUTF8);
    }
    return ret;
}
#endif
