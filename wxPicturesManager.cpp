#include "../include/wxPicturesManager.h"

#include <wx/dir.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/globals.h"
#include "../include/wxPicturesDialog.h"
#include "../include/wxConfiguration.h"
#include <wx/file.h>
#include <wx/image.h>

wxPicturesManager *g_pictures_manager = 0;

wxPicturesManager::wxPicturesManager()
{
    //ctor
    if (g_pictures_manager == 0)
        g_pictures_manager = this;
}

wxPicturesManager::~wxPicturesManager()
{
    //dtor
    if (g_pictures_manager == this)
        g_pictures_manager = 0;
}

void wxPicturesManager::AddByConfiguration(wxConfiguration *config)
{
    wxASSERT(config != 0);
    wxString filename = config->GetString(wxPicturesDialog::DirectoryTextParam);
    AddPath(filename);
}

void wxPicturesManager::AddPath(const wxString &path)
{
    /* Add picture by path... */
    if (wxDir::Exists(path)) {
        wxArrayString files;
        wxDir::GetAllFiles(path, &files, wxEmptyString, wxDIR_FILES);
        for (int x = 0; x < files.Count(); x++)
            m_filenames.Add(files.Item(x));
    }
}

void wxPicturesManager::AddPicture(const wxString &filename)
{
    m_filenames.Add(filename);
}


void wxPicturesManager::RandomizeFilenames()
{
    /* Just go through the list and randomize where things go. */
    int len = GetSize();
    int indexA, indexB, max = (len - 1);
    wxString copy;
    for (int x = 0; x < len; x++) {
        indexA = generate_random_number(0, max);
        indexB = generate_random_number(0, max);

        if (indexA != indexB) {
            copy = m_filenames.Item(indexA);
            m_filenames[indexA] = m_filenames[indexB];
            m_filenames[indexB] = copy;
        }
    }
}

wxString wxPicturesManager::GetFilename(int index) const
{
    wxString ret;
    if (index >= 0 || index < GetSize())
        ret = m_filenames.Item(index);
    return ret;
}

wxString wxPicturesManager::GetRandomFilename() const
{
    int index = generate_random_number(0, GetSize() - 1);
    return GetFilename(index);
}

wxBitmap wxPicturesManager::GetBitmap(int index)
{
    return GetBitmap(GetFilename(index));
}

wxBitmap wxPicturesManager::GetBitmap(const wxString &filename)
{
    if (filename != m_currentUrl) {
        if (!wxFile::Exists(filename)) {
            m_currentBitmap = wxNullBitmap;
        } else {
            wxImage image;
            image.LoadFile(filename);
            if (image.IsOk())
                m_currentBitmap = wxBitmap(image);
            else
                m_currentBitmap = wxNullBitmap;
        }
    }
    return m_currentBitmap;
}
