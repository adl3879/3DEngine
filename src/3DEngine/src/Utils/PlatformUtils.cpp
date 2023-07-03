#include "PlatformUtils.h"

#include "Log.h"
#include <iostream>
#include <filesystem>

#include <tinyfiledialogs.h>

namespace fs = std::filesystem;

namespace Engine
{
namespace Utils
{
std::thread FileDialogs::m_Thread = std::thread();
std::string FileDialogs::m_SelectedFile = "";
std::string FileDialogs::m_SavedFile = "";
bool FileDialogs::m_Done = false;
bool FileDialogs::m_IsFileOpened = false;
std::string FileDialogs::m_Id = "";

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

void FileDialogs::OpenFile(std::string id, FileDialogParams params)
{
    m_Id = id;
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

bool FileDialogs::FileIsOpened(std::string id)
{
    if (m_Done && m_SelectedFile != "" && !m_IsFileOpened && id == m_Id)
    {
        m_IsFileOpened = true;
        return true;
    }
    return false;
}

bool FileDialogs::FileIsSaved(std::string id)
{
    if (m_Done && m_SavedFile != "" && !m_IsFileOpened && id == m_Id)
    {
        m_IsFileOpened = true;
        return true;
    }
    return false;
}

void FileDialogs::SaveFile(std::string id, FileDialogParams params)
{
    m_Id = id;
    Reset();

    m_Thread = std::thread(std::bind(saveFileDialogOperation, params));
}
std::string Path::GetAbsolute(const std::string &path)
{
    auto cwd = fs::current_path().string();
    std::string buildSegment = "/build/";
    std::size_t found = cwd.find(buildSegment);
    std::string result = "";

    if (found != std::string::npos)
    {
        // Remove the build segment and the trailing slash
        result = cwd.substr(0, found) + cwd.substr(found + buildSegment.length() - 1);
    }

    return result + path;
}
std::string Path::GetRelative(std::string path)
{
    auto pos = path.find("/res");
    if (pos != std::string::npos) path.erase(0, pos);
    return path;
}
} // namespace Utils
} // namespace Engine