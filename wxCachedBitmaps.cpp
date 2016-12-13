#include "../include/wxCachedBitmaps.h"
#include "../include/wxSqlitePrepare.h"
#include <wx/image.h>
#include <stdlib.h>
#include <wx/mstream.h>

//#define wxCachedBitmaps_USE_DATABASE

wxCachedBitmaps *g_cached_bitmaps = 0;

wxCachedBitmaps::wxCachedBitmaps(wxSqliteDatabase *db)
{
    m_database = db;
    if (g_cached_bitmaps == 0) {
        g_cached_bitmaps = this;
    }
    if (m_database != 0)
        InitCache();
    Clear(60);
}

wxCachedBitmaps::~wxCachedBitmaps()
{
    Clear(60);
    if (g_cached_bitmaps == this);
        g_cached_bitmaps = 0;
}

void wxCachedBitmaps::InitCache()
{
    /* Simply create the tables... */
    CreateSqlTables();
}

void wxCachedBitmaps::ClearAll()
{
    if (m_database != 0)
        Clear(0);
}

void wxCachedBitmaps::Clear(int secondsOld)
{
    // delete old bitmaps...
    if (secondsOld < 0)
        secondsOld = 700;
    wxSqlitePrepare prepare(m_database, wxT("DELETE FROM bitmap_cache WHERE timestamp<?001"));
    prepare.BindParameter(1, time(0) - secondsOld);
    m_database->PerformSql(&prepare, 0);
}

// Create necessary tables.
void wxCachedBitmaps::CreateSqlTables()
{
    m_database->CreateTable(wxT("bitmap_cache"), wxT("name TEXT, bitmap_data BLOB, timestamp INT"));
    m_database->CreateIndex(wxT("name"), wxT("bitmap_cache"));
}

wxBitmap wxCachedBitmaps::GetBitmap(const wxString &key)
{
    wxImage img = GetImage(key);
    wxBitmap ret;
    if (img.IsOk())
        ret = wxBitmap(img);
    return ret;
}

int wxCachedBitmaps::FindBitmap(const wxString &key)
{
    for (int x = 0; x < wxCachedBitmaps_BITMAP_CACHE_LIMIT; x++) {
        if (m_cachedData[x].m_key == key)
            return x;
    }
    return -1;
}

int wxCachedBitmaps::RemoveOldAccessed()
{
    unsigned long lastTime = 0;
    int found = -1;
    int removed = 0;
    for (int x = 0; x < wxCachedBitmaps_BITMAP_CACHE_LIMIT; x++) {
        if (m_cachedData[x].m_lastUsed > 0) {
            if (lastTime == 0 || lastTime > m_cachedData[x].m_lastUsed) {
                lastTime = m_cachedData[x].m_lastUsed;
                found = x;
            }
        }
    }
    if (found >= 0) {
        removed++;
        m_cachedData[found].Clear();
    }
    return removed;
}

int wxCachedBitmaps::RemoveOldBitmaps(int secs)
{
    if (secs == 0)
        secs = time(0);
    int removed = 0;
    for (int x = 0; x < wxCachedBitmaps_BITMAP_CACHE_LIMIT; x++) {
        if (m_cachedData[x].m_lastUsed <= secs) {
            m_cachedData[x].Clear();
            removed++;
        }
    }
    return removed;
}
int wxCachedBitmaps::AddBitmap(const wxString &key, wxImage image)
{
    int find = FindEmpty();
    if (find < 0) {
        if (RemoveOldAccessed() == 0)
            RemoveOldBitmaps();
        find = 0;
    }
    m_cachedData[find].SetData(key, image, time(0));
}

int wxCachedBitmaps::FindEmpty()
{
    for (int x = 0; x < wxCachedBitmaps_BITMAP_CACHE_LIMIT; x++) {
        if (m_cachedData[x].m_key.Len() == 0)
            return x;
    }
    return -1;
}

wxImage wxCachedBitmaps::GetImage(const wxString &key)
{
    wxImage ret;

    /* */
    int found = FindBitmap(key);
    if (found >= 0) {
        ret = m_cachedData[found].m_bitmap;
        m_cachedData[found].m_lastUsed = time(0);
    }
    return ret;
#if 0

    wxSqlitePrepare prepare(m_database, wxT("SELECT bitmap_data FROM bitmap_cache WHERE name=?001"));
    prepare.BindParameter(1, key);

    wxSqliteResult result;
    if (m_database->PerformSql(&prepare, &result)) {
        // get the bitmap data.
        mjsSharedBlob blob = result.GetDataBlob(0, 0);
        if (!blob.IsNull()) {
            wxMemoryInputStream input(blob.GetCData(), blob.GetDataSize());
            wxImage img(input);
            if (img.IsOk())
                ret = img;
        }
    }
    return ret;
#endif
}

void wxCachedBitmaps::SaveImage(const wxString &key, wxImage image)
{
    // see if the key exists first...
    int found = FindBitmap(key);
    if (found < 0)
        AddBitmap(key, image);
    else {
        m_cachedData[found].m_lastUsed = time(0);
        m_cachedData[found].m_bitmap = image;
    }
#if 0
    //wx
    wxMemoryOutputStream output;
    image.SaveFile(output, wxBITMAP_TYPE_PNG);

    int len = output.GetLength();
    if (len <= 0)
        return;

    void *blob_data = malloc(len);
    if (blob_data == 0)
        return;

    int copied = output.CopyTo(blob_data, len);

    if (copied == len) {

        wxSqlitePrepare prepare(m_database, wxT("INSERT INTO bitmap_cache (name, bitmap_data, timestamp) VALUES(?001, ?002, ?003)"));

        prepare.BindParameter(1, key);
        prepare.BindParameter(2, blob_data, copied);
        prepare.BindParameter(3, time(0));

        m_database->PerformSql(&prepare, 0);
    }

    free(blob_data);

//xSqlitePrepare prepare(m_database, wxT("DELETE FROM bitmap_cache WHERE timestamp<?001"));
  //  prepare.BindParameter(1, time(0) - secondsOld);
#endif
}

void wxCachedBitmaps::SaveBitmap(const wxString &key, wxBitmap bitmap)
{
    SaveImage(key, bitmap.ConvertToImage());
}

/****************************************
    wxCachedBitmapsData Implementation
*****************************************/

wxCachedBitmapsData::wxCachedBitmapsData(const wxCachedBitmapsData &src)
{
    SetData(src);
}
wxCachedBitmapsData::wxCachedBitmapsData()
{
    Init();
}

wxCachedBitmapsData::~wxCachedBitmapsData()
{
    // do nothing...
}

void wxCachedBitmapsData::Init()
{
    m_lastUsed = 0;
}

void wxCachedBitmapsData::SetData(const wxCachedBitmapsData &src)
{
    m_lastUsed = src.m_lastUsed;
    m_bitmap = src.m_bitmap;
    m_key = src.m_key;
}

wxCachedBitmapsData & wxCachedBitmapsData::operator = (const wxCachedBitmapsData &src)
{
    SetData(src);
    return *this;
}

void wxCachedBitmapsData::Clear()
{
    m_lastUsed = 0;
    m_bitmap = wxImage();
    m_key.Clear();
}

void wxCachedBitmapsData::SetData(const wxString &key, wxImage img, unsigned long lastused)
{
    m_lastUsed = lastused;
    m_key = key;
    m_bitmap = img;
}
