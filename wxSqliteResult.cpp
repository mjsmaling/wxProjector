#include "../include/wxSqliteResult.h"
#include "../include/wxSqliteDatabase.h"
#include "../include/mjsVariant.h"

 wxSqliteResult::wxSqliteResult(const wxSqliteResult &src)
 {
     SetData(src);
 }

wxSqliteResult::wxSqliteResult()
{
    m_result = false;
}

wxSqliteResult::~wxSqliteResult()
{

}

wxSqliteResult & wxSqliteResult::operator = (const wxSqliteResult &src)
{
    SetData(src);
    return *this;
}

void wxSqliteResult::SetData(const wxSqliteResult &src)
{
    m_rows = src.m_rows;
    m_columns = src.m_columns;
    m_result = src.m_result;
}
#if 0
/*
    This will preform the sql query.
*/
bool wxSqliteResult::ExecSqliteSquery(const wxString &sql, wxSqliteDatabase *src)
{
    if (src == 0)
        return false;


}
#endif
/*

*/
int wxSqliteResult::sqlite_callback(void *userdata, int argc, char **argv, char **acolname) // static
{
    wxSqliteResult *result = (wxSqliteResult *)userdata;
    int x;
    // populate the columns.
    if (result->m_columns.GetCount() == 0) {
        if (acolname != 0) {
            for (x = 0; x < argc; x++)
                result->m_columns.Add(wxString(acolname[x], wxConvUTF8));
        }
    }

    // populate a row.
    std::vector <mjsVariant> row;
    //mjsVariantArray row;
    mjsVariant item;
    //wxArrayString row;
    for (x = 0; x < argc; x++) {
        item.SetData(wxString(argv[x], wxConvUTF8));
        row.push_back(item);
    }
    if (row.size() > 0)
        result->m_rows.push_back(row);

    return 0;
}

void wxSqliteResult::Clear()
{
    m_rows.clear();
    m_columns.Clear();
    m_result = false;
}


void wxSqliteResult::SetColumnNames(const wxArrayString &src)
{
    m_columns = src;
}

void wxSqliteResult::PushRow(const wxArrayString &row)
{
    mjsVariantArray varRow;
    mjsVariant item;
    wxString str;
    for (int x = 0; x < row.Count(); x++) {
        str = row.Item(x);
        item.SetData(str);
        varRow.Add(item);
    }
    PushRow(varRow);
}

void wxSqliteResult::PushRow(const std::vector<mjsVariant> &row)
{
    m_rows.push_back(row);
}

void wxSqliteResult::PushRow(const mjsVariantArray &row)
{
    std::vector<mjsVariant> items;
    for (int x = 0; x < row.Count(); x++) {
        items.push_back(row.Item(x));
    }
    m_rows.push_back(items);
    //m_rows.push(row);
}

void wxSqliteResult::GetRow(int row, wxArrayString &dest) const
{
    std::vector<mjsVariant> values;
    GetRow(row, values);
    dest.Clear();
    for (int x = 0; x < values.size(); x++)
        dest.Add(values.at(x).GetDataWXString());
}

void wxSqliteResult::GetRow(int row, std::vector<mjsVariant> &dest) const
{
    wxASSERT(row >= 0 && row < GetCount());
    dest = m_rows.at(row);
}

wxString wxSqliteResult::GetDataString(int row, int column, const wxChar *def) const
{
    const mjsVariant *item;
    item = GetVariantItem(row, column);
    wxString ret;
    if (item != 0)
        ret = item->GetDataWXString();
    else if (def != 0)
        ret = def;
    return ret;
}

mjsSharedBlob wxSqliteResult::GetDataBlob(int row, int column) const
{
    const mjsVariant *item = GetVariantItem(row, column);
    mjsSharedBlob ret;
    if (item != 0)
        ret = item->GetDataSharedBlob();
    return ret;
}

const mjsVariant *wxSqliteResult::GetVariantItem(int row, int column) const
{
    if (row >= 0 && row < GetCount()) {
        if (column >= 0 && column < m_rows.at(row).size()) {
            return (& (m_rows.at(row).at(column)));
        }
    }
    return 0;
}

long wxSqliteResult::GetDataInt(int row, int column, long def) const
{
    const mjsVariant *ret = GetVariantItem(row, column);
    if (ret == 0)
        return def;
    return ret->GetDataLong();
}

void wxSqliteResult::GetHeader(wxArrayString &dest) const
{
    dest = m_columns;
}

int wxSqliteResult::GetCount() const
{
    return m_rows.size();
}

