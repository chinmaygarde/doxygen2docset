#pragma once

#include <functional>
#include <string>
#include <vector>

namespace d2d {

std::string JoinPaths(const std::vector<std::string>& paths);

std::string JoinPaths(const std::string& path,
                      const std::vector<std::string>& paths);

std::string JoinPaths(const std::vector<std::string>& paths,
                      const std::vector<std::string>& other);

std::string JoinPaths(const std::vector<std::string>& paths,
                      const std::string& path);

bool MakeDirectories(const std::vector<std::string>& directories);

using CopyPredicate = std::function<bool(const std::string& file_name)>;

bool CopyFiles(const std::string& from, const std::vector<std::string>& to,
               CopyPredicate predicate);

bool CopyFile(const std::string& from, const std::string& to);

bool CopyData(const void* data, size_t length, const std::string& to);

}  // namespace d2d
