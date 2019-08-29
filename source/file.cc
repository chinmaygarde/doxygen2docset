#include "file.h"

#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <sstream>

#include "logger.h"

namespace d2d {

class AutoFD {
 public:
  AutoFD(int fd) : fd_(fd) {}

  AutoFD(AutoFD&& fd) = delete;

  AutoFD(const AutoFD& fd) = delete;

  AutoFD& operator=(const AutoFD&) = delete;

  AutoFD& operator=(AutoFD&& other) {
    Reset(other.fd_);
    other.fd_ = -1;
    return *this;
  };

  int Get() const { return fd_; };

  bool IsValid() const { return fd_ > 0; };

  void Reset(int fd = -1) {
    if (fd_ != fd && fd_ > 0) {
      int result = D2D_TEMP_FAILURE_RETRY(::close(fd_));
      if (result == -1) {
        D2D_ERROR << "Could not close a file descriptor.";
      }
    }
    fd_ = fd;
  }

  ~AutoFD() { Reset(); }

 private:
  int fd_ = -1;
};

class AutoDir {
 public:
  AutoDir(DIR* dir) : dir_(dir) {}

  DIR* Get() const { return dir_; }

  bool IsValid() const { return dir_ != nullptr; }

  ~AutoDir() {
    if (dir_ != nullptr) {
      ::closedir(dir_);
    }
  }

 private:
  DIR* dir_ = nullptr;
  D2D_DISALLOW_COPY_AND_ASSIGN(AutoDir);
};

class AutoMapping {
 public:
  AutoMapping(void* mapping, size_t size) : mapping_(mapping), size_(size) {}

  ~AutoMapping() {
    if (mapping_ != MAP_FAILED) {
      if (::munmap(mapping_, size_) != 0) {
        D2D_ERROR << "Error unmapping file.";
      }
    }
  }

  void* Get() const { return mapping_; }

  bool IsValid() const { return mapping_ != MAP_FAILED; }

 private:
  void* mapping_ = MAP_FAILED;
  size_t size_ = 0;

  D2D_DISALLOW_COPY_AND_ASSIGN(AutoMapping);
};

bool MakeDirectories(const std::vector<std::string>& directories) {
  AutoFD current_level(AT_FDCWD);

  for (const auto& directory : directories) {
    if (::mkdirat(current_level.Get(), directory.c_str(),
                  S_IRUSR | S_IWUSR | S_IXUSR) != 0 &&
        errno != EEXIST) {
      D2D_ERROR << "Could not open or create directory " << directory
                << " while attempting to create the directory structure "
                << JoinPaths(directories) << ". Error: " << strerror(errno);
      return false;
    }

    AutoFD sub_dir(::openat(current_level.Get(), directory.c_str(),
                            O_RDONLY | O_DIRECTORY));

    if (!sub_dir.IsValid()) {
      D2D_ERROR << "Could not open or create directory " << directory
                << " while attempting to create the directory structure "
                << JoinPaths(directories) << ". Error: " << strerror(errno);
      return false;
    }

    current_level = std::move(sub_dir);
  }

  return true;
}

bool CopyData(const void* data, size_t length, const std::string& to_path) {
  AutoFD to_file(
      D2D_TEMP_FAILURE_RETRY(::open(to_path.c_str(), O_CREAT | O_TRUNC | O_RDWR,
                                    S_IRUSR | S_IWUSR | S_IXUSR)));
  if (!to_file.IsValid()) {
    D2D_ERROR << "Could not create the file " << to_path
              << " to write to: " << strerror(errno);
    return false;
  }

  if (::ftruncate(to_file.Get(), length) != 0) {
    D2D_ERROR << "Could not truncate file " << to_path;
    return false;
  }
  AutoMapping to_mapping(::mmap(nullptr, length, PROT_WRITE,
                                MAP_FILE | MAP_SHARED, to_file.Get(), 0),
                         length);

  if (!to_mapping.IsValid()) {
    D2D_ERROR << "Could not setup mapping to perform file copy.";
    return false;
  }

  ::memcpy(to_mapping.Get(), data, length);

  if (::msync(to_mapping.Get(), length, MS_SYNC) != 0) {
    D2D_ERROR << "Could not sync file contents.";
    return false;
  }

  return true;
}

static bool CopyFile(const struct stat& from_stat, const AutoFD& from,
                     const std::string& to_path) {
  AutoMapping from_mapping(::mmap(nullptr, from_stat.st_size, PROT_READ,
                                  MAP_FILE | MAP_PRIVATE, from.Get(), 0),
                           from_stat.st_size);

  if (!from_mapping.IsValid()) {
    D2D_ERROR << "Could not setup mapping to perform file copy.";
    return false;
  }

  return CopyData(from_mapping.Get(), from_stat.st_size, to_path);
}

bool CopyFile(const std::string& from, const std::string& to) {
  struct stat from_stat = {};
  if (::stat(from.c_str(), &from_stat) != 0) {
    D2D_ERROR << "Could not stat file " << from;
    return false;
  }

  AutoFD from_fd(D2D_TEMP_FAILURE_RETRY(::open(from.c_str(), O_RDONLY)));

  if (!from_fd.IsValid()) {
    D2D_ERROR << "From file was not valid.";
    return false;
  }

  return CopyFile(from_stat, from_fd, to);
}

bool CopyFiles(const std::string& from_path,
               const std::vector<std::string>& to_path,
               CopyPredicate predicate) {
  AutoDir from(::opendir(from_path.c_str()));
  if (!from.IsValid()) {
    D2D_ERROR << "Could not open the directory to copy from.";
    return false;
  }

  if (!MakeDirectories(to_path)) {
    D2D_ERROR << "Could not create the directory structure "
              << JoinPaths(to_path);
    return false;
  }

  while (auto dir_ent = ::readdir(from.Get())) {
    std::string file_name(dir_ent->d_name);

    if (file_name.size() == 0) {
      D2D_ERROR << "Encountered empty filename.";
      return false;
    }

    if (file_name == "." || file_name == "..") {
      continue;
    }

    if (!predicate(file_name)) {
      continue;
    }

    AutoFD from_fd(D2D_TEMP_FAILURE_RETRY(
        ::openat(::dirfd(from.Get()), file_name.c_str(), O_RDONLY)));
    if (!from_fd.IsValid()) {
      D2D_ERROR << "From file could not be opened: " << file_name;
      return false;
    }

    struct stat from_stat = {};
    if (::fstat(from_fd.Get(), &from_stat) != 0) {
      D2D_ERROR << "Could not stat file: " << file_name;
      return false;
    }

    if (S_ISDIR(from_stat.st_mode)) {
      std::vector<std::string> to_subpath = to_path;
      to_subpath.push_back(file_name);
      if (!CopyFiles(JoinPaths({from_path, file_name}), to_subpath,
                     predicate)) {
        D2D_ERROR << "Could not copy directory " << file_name;
        return false;
      }
    } else {
      if (!CopyFile(from_stat, from_fd, JoinPaths(to_path, file_name))) {
        D2D_ERROR << "Could not copy file " << file_name;
        return false;
      }
    }
  }

  return true;
}

std::string JoinPaths(const std::vector<std::string>& paths) {
  std::stringstream stream;
  for (size_t i = 0, len = paths.size(); i < len; i++) {
    stream << paths[i];
    if (i < (len - 1)) {
      stream << "/";
    }
  }
  return stream.str();
}

std::string JoinPaths(const std::vector<std::string>& p1,
                      const std::vector<std::string>& p2) {
  std::vector<std::string> merged;
  std::merge(p1.begin(), p1.end(), p2.begin(), p2.end(),
             std::back_inserter(merged));
  return JoinPaths(merged);
}

std::string JoinPaths(const std::string& path,
                      const std::vector<std::string>& paths) {
  std::vector<std::string> merged;
  merged.push_back(path);
  for (const auto& p : paths) {
    merged.push_back(p);
  }
  return JoinPaths(merged);
}

std::string JoinPaths(const std::vector<std::string>& paths,
                      const std::string& path) {
  std::vector<std::string> merged = paths;
  merged.push_back(path);
  return JoinPaths(merged);
}

}  // namespace d2d
