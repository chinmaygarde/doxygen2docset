#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

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

  size_t GetSize() const { return size_; }

  bool IsValid() const { return mapping_ != MAP_FAILED; }

 private:
  void* mapping_ = MAP_FAILED;
  size_t size_ = 0;

  D2D_DISALLOW_COPY_AND_ASSIGN(AutoMapping);
};

class Allocation {
 public:
  Allocation() = default;

  Allocation(size_t size)
      : allocation_(::calloc(size, sizeof(uint8_t))), size_(size) {}

  Allocation(Allocation&& o) : allocation_(o.allocation_), size_(o.size_) {
    o.allocation_ = nullptr;
    o.size_ = 0;
  }

  Allocation(const Allocation&) = delete;

  Allocation& operator=(const Allocation&) = delete;

  Allocation& operator=(Allocation&&) = delete;

  ~Allocation() {
    if (allocation_ != nullptr) {
      ::free(allocation_);
    }
  }

  bool IsValid() const { return allocation_ != nullptr; }

  uint8_t* Get() const { return static_cast<uint8_t*>(allocation_); };

  size_t GetSize() const { return size_; }

 private:
  void* allocation_ = nullptr;
  size_t size_ = 0;
};

std::string JoinPaths(const std::vector<std::string>& paths);

std::string JoinPaths(const std::string& path,
                      const std::vector<std::string>& paths);

std::string JoinPaths(const std::vector<std::string>& paths,
                      const std::vector<std::string>& other);

std::string JoinPaths(const std::vector<std::string>& paths,
                      const std::string& path);

bool MakeDirectories(const std::vector<std::string>& directories);

using CopyPredicate = std::function<bool(const std::string& from_file_name,  //
                                         const struct stat& from_stat,       //
                                         const AutoFD& from_fd,              //
                                         const std::string& to_file_name)>;

bool CopyFiles(const std::string& from, const std::vector<std::string>& to,
               CopyPredicate predicate);

bool CopyFile(const struct stat& from_stat, const AutoFD& from,
              const std::string& to_path);

bool CopyFile(const std::string& from, const std::string& to);

bool CopyData(const void* data, size_t length, const std::string& to);

std::unique_ptr<AutoMapping> OpenFileReadOnly(const std::string& path);

std::unique_ptr<AutoMapping> OpenFileReadOnly(const AutoFD& fd, size_t size);

}  // namespace d2d
