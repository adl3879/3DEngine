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
    static void OpenFile(FileDialogParams params = FileDialogParams{.Title = "Open File"});
    static void SaveFile(FileDialogParams params = FileDialogParams{.Title = "Save File"});

    static void SetSelectedFile(const std::string &file) { m_SelectedFile = file; }
    static void SetSavedFile(const std::string &file) { m_SavedFile = file; }
    static void SetDone(bool done) { m_Done = done; }
    static void Reset();

    static bool FileIsOpened();
    static bool FileIsSaved();

  public:
    static std::thread m_Thread;
    static bool m_Done, m_IsFileOpened;

  public:
    static std::string m_SelectedFile, m_SavedFile;
};
} // namespace Utils
} // namespace Engine