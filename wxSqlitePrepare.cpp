#include "../include/wxSqlitePrepare.h"
#include "../include/wxSqliteDatabase.h"
#include <wx/mstream.h>

wxSqlitePrepare::wxSqlitePrepare(wxSqliteDatabase *src, const wxString &sql)
{
    wxASSERT(src != 0);

    SetMembers(src->access_db(), sql);
}

wxSqlitePrepare::wxSqlitePrepare(sqlite3 *db, const wxString &sql)
{
    wxASSERT(db != 0);
    SetMembers(db, sql);
}

wxSqlitePrepare::~wxSqlitePrepare()
{
    CleanUp();
}

void wxSqlitePrepare::SetMembers(sqlite3 *db, const wxString &src)
{
    wxString cpy(src);

    m_db = db;

    int ret = sqlite3_prepare_v2(m_db, cpy.mb_str(wxConvUTF8), cpy.Len(), &m_stmt, 0);

    SetAndRet(ret);
}

void wxSqlitePrepare::CleanUp()
{
    if (m_stmt != 0) {
        sqlite3_finalize(m_stmt);
        m_stmt = 0;
    }
}

bool wxSqlitePrepare::BindParameter(int col, const wxString &string)
{
    // bind text.
    mjsSharedBlob *blob;
    char *cpy = strdup(string.mb_str(wxConvUTF8));
    blob = new mjsSharedBlob()
    char *cpy = strdup(string.mb_str(wxConvUTF8));

    int ret = sqlite3_bind_text(m_stmt, col, cpy, strlen(cpy), wxSqlitePrepare::free_blob);

    return ret;
}

void wxSqlitePrepare::free_text(void *src)
{
    assert(src != 0);
    free(src);
}

void wxSqlitePrepare::free_blob(void *blob_ptr)
{
    if (blob_ptr != 0) {
        mjsSharedBlob *blob = (mjsSharedBlob *)blob_ptr;
        delete blob;
    }
}

bool wxSqlitePrepare::BindParameter(int col, long number)
{
    return sqlite3_bind_int(m_stmt, col, number);
}

bool wxSqlitePrepare::SetAndRet(int ret)
{
    if (ret) {
   //     m_state.
        m_lastState.SetCode(ret);
        return false;
    }
    m_lastState.SetOk();
    return true;
}

/*
*/
int wxSqlitePrepare::Step(wxSqliteResult *result)
{
    int ret = sqlite3_step(m_stmt);
    if (ret == SQLITE_ROW) {
        // got a row. populate the result.
        if (result != 0) {
            int cols = sqlite3_column_count(m_stmt);

            if (result->HasColumns() == false) {
                // can we get the column headers from here.
                wxArrayString header;
                for (int x = 0; x < cols; x++)
                    header.Add(wxString(sqlite3_column_name(m_stmt, x), wxConvUTF8));
                result->SetColumnNames(header);
            }

            std::vector<mjsVariant> row;
            mjsVariant value;
            for (int x = 0; x < cols; x++) {
                // see what type the column is.
                int type = sqlite3_column_type(m_stmt, x);
                switch (type) {
                case SQLITE_INTEGER:
                    value.SetData((long)sqlite3_column_int(m_stmt, x));
                    break;
                case SQLITE_FLOAT:
                    value.SetData(sqlite3_column_double(m_stmt, x));
                    break;
                case SQLITE_TEXT:
                    value.SetData(wxString((const char *)sqlite3_column_text(m_stmt, x), wxConvUTF8));
                    break;
                case SQLITE_BLOB:
                    value.SetData(sqlite3_column_blob(m_stmt, x), sqlite3_column_bytes(m_stmt, x));
                    break;
                default:
                    value.SetToNull();
                    break;
                };
                row.push_back(value);
            }
            result->PushRow(row);
        }
    }

    return ret;
}

bool wxSqlitePrepare::BindParameter(int col, const mjsSharedBlob &blob)
{
    return sqlite3_bind_blob(m_stmt, col, blob.GetCData(), blob.GetDataSize(), wxSqlitePrepare::free_blob);
}

bool wxSqlitePrepare::BindParameter(int col, const void *src, int size)
{
    if (size < 0)
        return false;
    mjsSharedBlob blob(src,size);
    return BindParameter(col, blob);
}
#if 0
bool wxSqlitePrepare::BindParameter(int col, wxMemoryInputStream *inputStream)
{
    // make a copy of the stream...
    //wxStreamBase *stream = streamBuffer->Stream();
    inputStream->SeekI(0);

    void *buffer;
    int len = inputStream->GetLength();
    buffer = malloc(len);
    if (buffer == 0)
        return false;
    memcpy(buffer, inputStream->GetInputStreamBuffer()->GetBufferStart(), len);
    return sqlite3_bind_blob(m_stmt, col, buffer, len, wxSqlitePrepare::free_text);
}
#endif
