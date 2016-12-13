#include "../include/mjsSharedBlob.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int mjsSharedBlob::m_instances = 0;
std::vector<mjsSharedBlobData *> mjsSharedBlob::m_sharedBlobs;

mjsSharedBlob::mjsSharedBlob(const mjsSharedBlob &src)
{
    Init();
    SetData(src);
}
mjsSharedBlob::mjsSharedBlob()
{
    Init();
}

mjsSharedBlob::mjsSharedBlob(const void *src, int size)
{
    Init();
    SetData(src, size);
}

mjsSharedBlob::~mjsSharedBlob()
{
    Decrement();
}

void mjsSharedBlob::Decrement()
{
    Detatch();

    if (--m_instances == 0) {
        // delete all blobs... if any.
        RemoveAllFromList();
    }
}

int mjsSharedBlob::AddToList(mjsSharedBlobData *blob) // static
{
    m_sharedBlobs.push_back(blob);
    return 1;
}

int mjsSharedBlob::RemoveFromList(mjsSharedBlobData *blob) // static
{
    int found = 0;
    for (int x = 0; x < m_sharedBlobs.size(); x++) {
        if (m_sharedBlobs.at(x) == blob) {
            m_sharedBlobs.erase(m_sharedBlobs.begin() + x);
            found++;
            x--;
        }
    }
    return found;
}

int mjsSharedBlob::RemoveAllFromList() // static
{
    int removed = 0;
    mjsSharedBlobData *blob;
    while (m_sharedBlobs.size() > 0) {
        blob = m_sharedBlobs.back();
        m_sharedBlobs.pop_back();
        if (blob != 0) {
            removed++;
            delete blob;
        }
    }
    return removed;
}

void mjsSharedBlob::SetData(const mjsSharedBlob &src)
{
    Detatch();
    m_blob = src.m_blob;
    if (m_blob != 0)
        m_blob->m_reference_count++;
}

mjsSharedBlob &mjsSharedBlob::operator = (const mjsSharedBlob &src)
{
    SetData(src);
    return *this;
}

bool mjsSharedBlob::SameAs(const mjsSharedBlob &src) const
{
    return (m_blob == src.m_blob);
}

void mjsSharedBlob::SetData(const void *src, int size)
{
    AllocateNew();
    m_blob->SetData(src, size);
}

void mjsSharedBlob::Detatch()
{
    if (m_blob != 0) {
        if (--m_blob->m_reference_count == 0) {
            // find this blob and remove it from the list...
            RemoveFromList(m_blob);
            delete m_blob;
        }
        m_blob = 0;
    }
}

// This will allocate new only if the current blob data has other references to it.
void mjsSharedBlob::AllocateNew()
{
    if (m_blob != 0) {
        if (m_blob->m_reference_count > 1)
            Detatch();
    }
    if (m_blob == 0) {
        m_blob = new mjsSharedBlobData();
        AddToList(m_blob);
    }
}

void *mjsSharedBlob::GetData() const
{
    if (m_blob == 0)
        return 0;
    return m_blob->m_data;
}
const void *mjsSharedBlob::GetCData() const
{
    return GetData();
}

int mjsSharedBlob::GetDataSize() const
{
    if (m_blob == 0)
        return 0;
    return m_blob->m_used;
}

void mjsSharedBlob::Init()
{
    m_blob = 0;
    m_instances++;
}

mjsSharedBlobData *mjsSharedBlob::FindSharedBlob(const mjsSharedBlobData *item)
{
    for (int x = 0; x < m_sharedBlobs.size(); x++) {
        mjsSharedBlobData *it = m_sharedBlobs.at(x);
        if (it == item)
            return it;
    }
    return 0;
}


char mjsSharedBlob::GetChar(int at) const
{
    assert(at >= 0 && at < GetDataSize());
    return ((char *) m_blob->m_data)[at];
}

bool mjsSharedBlob::IsNull() const
{
    if (m_blob == 0)
        return true;
    return (m_blob->m_used == 0);
}

void mjsSharedBlob::TakeData(void *src, int size, int allocated)
{
    Detatch();
    AllocateNew();

    m_blob->m_data = src;
    m_blob->m_used = size;
    m_blob->m_allocated = allocated;
}

/****************************************
    mjsSharedBlobData Implementation
*****************************************/
mjsSharedBlobData::mjsSharedBlobData(const mjsSharedBlobData &src)
{
   Init();
    SetData(src);
}

mjsSharedBlobData::mjsSharedBlobData()
{
    Init();
}

mjsSharedBlobData::~mjsSharedBlobData()
{
    if (--m_reference_count <= 0)
        Free();
}

void mjsSharedBlobData::Init()
{
    m_data = 0;
    m_allocated = 0;
    m_used = 0;
    m_reference_count = 1; // start with 1.
}

void mjsSharedBlobData::Free()
{
    if (m_data != 0) {
        free(m_data);
        m_allocated = 0;
        m_used = 0;
    }
}

void mjsSharedBlobData::SetData(const void *src, int size)
{
    if (size == 0)
        Free();
    else {
        AllocateSpace(size, false);
        memcpy(m_data, src, size);
        m_used = size;
    }

}

void mjsSharedBlobData::AllocateSpace(int size, bool force_resize /*= false*/)
{
    if (force_resize || size > m_allocated || (size < (m_allocated / 2))) {
        m_data = realloc(m_data, size);
        m_allocated = size;
    }
}

mjsSharedBlobData &mjsSharedBlobData::operator = (const mjsSharedBlobData &src)
{
    SetData(src);
    return *this;
}

void mjsSharedBlobData::SetData(const mjsSharedBlobData &src)
{
    SetData(src.m_data, src.m_used);
}

const mjsSharedBlob *mjsSharedBlob::SearchBlob(const void *src, int length)
{
    for (int x = 0; x < m_sharedBlobs.size(); x++) {
        const mjsSharedBlob *ret = mjsSharedBlob::m_sharedBlobs.at(x);
        if (ret->GetDataSize() >= length) {
            if (memcmp(src, ret->GetCData(), length) == 0)
                return ret;
        }
    }
    return 0;
}

const mjsSharedBlob *mjsSharedBlob::SearchBlob(const char *src)
{
    if (src == 0)
        return 0;
    return (src, strlen(src) + 1);
}
