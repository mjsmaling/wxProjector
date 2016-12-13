#include "../include/wxSqliteDatabase.h"
#include "../include/wxSqlitePrepare.h"
#include <sqlite3.h>
//#include <wx/msgbox.h>

wxSqliteDatabase::wxSqliteDatabase()
    : wxDatabaseBase()
{
    m_db = 0;
}

wxSqliteDatabase::~wxSqliteDatabase()
{
    //dtor
    CloseDatabase();
}

bool wxSqliteDatabase::CreateDatabase(const wxString &filename)
{
    return OpenDatabase(filename);
#if 0

    /* Create the database and initialize it. */
    int ret = sqlite3_open(filename.mb_str(wxConvUTF8), &m_db);

    if (ret) {
  //      wxMessageBox(wxString(sqlite3_errmsg(m_db), wxConvUTF8));
        return false;
    }


    return true;
#endif
}

bool wxSqliteDatabase::OpenDatabase(const wxString &filename)
{
    /* Close the databse first. */
    CloseDatabase();

     /* Create the database and initialize it. */
    int ret = sqlite3_open(filename.mb_str(wxConvUTF8), &m_db);

    if (ret)
        return false;

    return true;
}

bool wxSqliteDatabase::TableExists(const wxString &table) const
{
    wxString sql = wxT("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
    return
}

int wxSqliteDatabase::GetTableList(wxArrayString &dest) const
{
    wxString sql = wxT("SELECT name FROM sqlite_master WHERE type='table' AND name='");
    sql << tableName << "'");
    int retval = 0;

    return retval;
}

bool wxSqliteDatabase::CreateTable(const wxString &tableName, const wxString &fieldDeclaration)
{
    /*
        First see if table exists...
    */
    wxString sql = wxT("CREATE TABLE IF NOT EXISTS ");

    sql << tableName;
    sql << wxT("(") << fieldDeclaration << wxT(");");

    return PerformSql(sql);
}

bool wxSqliteDatabase::PerformSql(const wxString &sql, wxSqliteResult *result)
{
    if (IsOpen() == false)
        return false;

    m_lastResult.Clear();

    bool ret = PerformSql(sql, wxSqliteResult::sqlite_callback, &m_lastResult);

    if (result != 0)
        *result = m_lastResult;

    return ret;
}

void wxSqliteDatabase::Vacuum()
{
    wxASSERT(IsOpen() == true);
    PerformSql(wxT("VACUUM"), 0);
}

bool wxSqliteDatabase::PerformSql(wxSqlitePrepare *prepare, int (*callback)(sqlite3_stmt *m_stmt, void *userdata), void *userdata)
{
    if (IsOpen() == false || prepare == 0)
        return false;

    return true;
}

bool wxSqliteDatabase::PerformSql(wxSqlitePrepare *prepare, wxSqliteResult *result)
{
    if (IsOpen() == false || prepare == 0)
        return false;

    // Step(wxSqliteResult *result)
    m_lastResult.Clear();
    bool cont = true, ret = true;
    const char *temp;
    while (cont) {
        int result = prepare->Step(&m_lastResult);
        switch (result) {
        case SQLITE_ROW: // keep going.
            break;
        case SQLITE_ERROR:
            temp = sqlite3_errmsg(m_db);
            printf("%s\n", temp);
        case SQLITE_DONE:
        default:
            cont = false;
            break;
        };
    }
    if (result != 0)
        *result = m_lastResult;
    return ret;
}

int wxSqliteDatabase::PerformCount(wxSqlitePrepare *prepare)
{
    PerformSql(prepare, 0);
    if (m_lastResult.GetCount() > 0)
        return m_lastResult.GetDataInt(0, 0);
    return 0;
}

int wxSqliteDatabase::PreformCount(const wxString &sql)
{
    int retval = 0;

    PerformSql(sql);

    return retval;
}

bool wxSqliteDatabase::PerformSql(const wxString &sql, int (*callback)(void *, int, char **, char **), void *userdata)
{
    // Copy the utf8 string fisrt.
    if (IsOpen() == false)
        return false;

    std::string cpy = (const char *)sql.mb_str(wxConvUTF8);
    char *error_message = 0;

    int rc = sqlite3_exec(m_db, cpy.c_str(), callback, userdata, &error_message);

    if (rc)
        m_lastState.SetData(error_message, rc);
    else
        m_lastState.SetOk();

    if (error_message != 0)
        sqlite3_free(error_message);

    return (rc == 0);
}

bool wxSqliteDatabase::CreateIndex(const wxString &field, const wxString &table)
{
    wxString sql = wxT("CREATE INDEX IF NOT EXISTS ");

    wxString indexName = field;
    indexName << wxT("_") << table;

    sql << indexName << wxT(" ON ") << table << wxT(" (") << field << wxT(" ASC)");

    return PerformSql(sql);
}

bool wxSqliteDatabase::CloseDatabase()
{
    if (m_db != 0) {
        sqlite3_close(m_db);
        m_db = 0;
    }
    return true;
}

bool wxSqliteDatabase::IsOpen()
{
    return (m_db != 0);
}
