#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct Folder {
    std::string              id;
    std::string              name;
    std::vector<uint64_t>    titleIds;
};

class FolderManager {
public:
    void load(const std::string& path);
    void save() const;

    const std::vector<Folder>& folders() const { return m_folders; }
    int  count() const { return (int)m_folders.size(); }

    // CRUD
    std::string createFolder(const std::string& name);
    void deleteFolder(const std::string& id);

    // Assignments
    void addToFolder(const std::string& folderId, uint64_t titleId);
    void removeFromFolder(const std::string& folderId, uint64_t titleId);
    bool isInFolder(const std::string& folderId, uint64_t titleId) const;
    std::vector<std::string> folderNamesForTitle(uint64_t titleId) const;

private:
    Folder*       findFolder(const std::string& id);
    const Folder* findFolder(const std::string& id) const;

    std::string         m_path;
    std::vector<Folder> m_folders;
    int                 m_nextId = 0;
};
