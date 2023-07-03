#pragma once

#include <string>
#include <thread>

namespace Engine
{
namespace Utils
{
struct FileDialogParams
{
    const char *Title = nullptr;
    const char *DefaultPathAndFile = nullptr;
    int NumOfFilterPatterns = 0;
    const char *const *FilterPatterns = nullptr;
    const char *SingleFilterDescription = nullptr;
    int AllowMultipleSelects = 0;
};

class FileDialogs
{
  public:
    static void OpenFile(std::string id, FileDialogParams params = FileDialogParams{.Title = "Open File"});
    static void SaveFile(std::string id, FileDialogParams params = FileDialogParams{.Title = "Save File"});

    static void SetSelectedFile(const std::string &file) { m_SelectedFile = file; }
    static void SetSavedFile(const std::string &file) { m_SavedFile = file; }
    static void SetDone(bool done) { m_Done = done; }
    static void Reset();

    static bool FileIsOpened(std::string id);
    static bool FileIsSaved(std::string id);

  public:
    static std::thread m_Thread;
    static bool m_Done, m_IsFileOpened;

  public:
    static std::string m_SelectedFile, m_SavedFile, m_Id;
};

class Path
{
  public:
    static std::string GetAbsolute(const std::string &path);
    static std::string GetRelative(std::string path);
};
} // namespace Utils
} // namespace Engine