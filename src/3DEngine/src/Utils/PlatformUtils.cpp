#include "PlatformUtils.h"

#include "Log.h"
#include <iostream>

#include <tinyfiledialogs.h>

namespace Engine
{
namespace Utils
{
std::thread FileDialogs::m_Thread = std::thread();
std::string FileDialogs::m_SelectedFile = "";
std::string FileDialogs::m_SavedFile = "";
bool FileDialogs::m_Done = false;
bool FileDialogs::m_IsFileOpened = false;

static void openDialogOperation(FileDialogParams params)
{
    LOG_CORE_TRACE("Opening file dialog");
    const char *result =
        tinyfd_openFileDialog(params.Title, params.DefaultPathAndFile, params.NumOfFilterPatterns,
                              params.FilterPatterns, params.SingleFilterDescription, params.AllowMultipleSelects);

    FileDialogs::SetSelectedFile(result ? result : "");
    FileDialogs::SetDone(true);
}

static void saveFileDialogOperation(FileDialogParams params)
{
    LOG_CORE_TRACE("Opening save file dialog");
    const char *result = tinyfd_saveFileDialog(params.Title, params.DefaultPathAndFile, params.NumOfFilterPatterns,
                                               params.FilterPatterns, params.SingleFilterDescription);

    FileDialogs::SetSavedFile(result ? result : "");
    FileDialogs::SetDone(true);
}

void FileDialogs::OpenFile(FileDialogParams params)
{
    // reset all the variables
    Reset();

    m_Thread = std::thread(std::bind(openDialogOperation, params));
}

void FileDialogs::Reset()
{
    if (m_Thread.joinable()) m_Thread.join();
    m_Thread = std::thread();
    m_SelectedFile = "";
    m_SavedFile = "";
    m_Done = false;
    m_IsFileOpened = false;
}

bool FileDialogs::FileIsOpened()
{
    if (m_Done && m_SelectedFile != "" && !m_IsFileOpened)
    {
        m_IsFileOpened = true;
        return true;
    }
    return false;
}

bool FileDialogs::FileIsSaved()
{
    if (m_Done && m_SavedFile != "" && !m_IsFileOpened)
    {
        m_IsFileOpened = true;
        return true;
    }
    return false;
}

void FileDialogs::SaveFile(FileDialogParams params)
{
    // reset all the variables
    Reset();

    m_Thread = std::thread(std::bind(saveFileDialogOperation, params));
}
} // namespace Utils
} // namespace Engine