#include "FolderManager.hpp"
#include <fstream>
#include <algorithm>
#include <cstdio>

void FolderManager::load(const std::string& path) {
    m_path = path;
    m_folders.clear();
    m_nextId = 0;

    std::ifstream f(path);
    if (!f) return;

    Folder* cur = nullptr;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        if (line.front() == '[' && line.back() == ']') {
            Folder folder;
            folder.name = line.substr(1, line.size() - 2);
            folder.id   = "folder_" + std::to_string(m_nextId++);
            m_folders.push_back(std::move(folder));
            cur = &m_folders.back();
        } else if (cur && line.size() >= 16) {
            try {
                uint64_t tid = std::stoull(line.substr(0, 16), nullptr, 16);
                if (tid) cur->titleIds.push_back(tid);
            } catch (...) {}
        }
    }
}

void FolderManager::save() const {
    if (m_path.empty()) return;
    std::ofstream f(m_path);
    if (!f) return;
    f << "# SwitchU folders\n";
    for (const auto& folder : m_folders) {
        f << "[" << folder.name << "]\n";
        for (uint64_t tid : folder.titleIds) {
            char buf[17];
            std::snprintf(buf, sizeof(buf), "%016lX", tid);
            f << buf << "\n";
        }
        f << "\n";
    }
}

std::string FolderManager::createFolder(const std::string& name) {
    Folder f;
    f.id   = "folder_" + std::to_string(m_nextId++);
    f.name = name;
    m_folders.push_back(std::move(f));
    save();
    return m_folders.back().id;
}

void FolderManager::deleteFolder(const std::string& id) {
    m_folders.erase(
        std::remove_if(m_folders.begin(), m_folders.end(),
                       [&](const Folder& f) { return f.id == id; }),
        m_folders.end());
    save();
}

void FolderManager::addToFolder(const std::string& folderId, uint64_t titleId) {
    if (auto* f = findFolder(folderId)) {
        if (!isInFolder(folderId, titleId)) {
            f->titleIds.push_back(titleId);
            save();
        }
    }
}

void FolderManager::removeFromFolder(const std::string& folderId, uint64_t titleId) {
    if (auto* f = findFolder(folderId)) {
        auto& v = f->titleIds;
        v.erase(std::remove(v.begin(), v.end(), titleId), v.end());
        save();
    }
}

bool FolderManager::isInFolder(const std::string& folderId, uint64_t titleId) const {
    if (const auto* f = findFolder(folderId))
        for (uint64_t tid : f->titleIds)
            if (tid == titleId) return true;
    return false;
}

std::vector<std::string> FolderManager::folderNamesForTitle(uint64_t titleId) const {
    std::vector<std::string> result;
    for (const auto& f : m_folders)
        for (uint64_t tid : f.titleIds)
            if (tid == titleId) { result.push_back(f.name); break; }
    return result;
}

Folder* FolderManager::findFolder(const std::string& id) {
    for (auto& f : m_folders) if (f.id == id) return &f;
    return nullptr;
}

const Folder* FolderManager::findFolder(const std::string& id) const {
    for (const auto& f : m_folders) if (f.id == id) return &f;
    return nullptr;
}
