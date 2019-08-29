
#include "definition_parser.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <tinyxml2.h>
#include <unistd.h>

#include "logger.h"

namespace d2d {

DefinitionParser::DefinitionParser(const std::string& file_path) {
  if (file_path.size() == 0) {
    D2D_ERROR << "File path was empty.";
    return;
  }

  struct stat stat_buf = {};
  if (::stat(file_path.c_str(), &stat_buf) != 0) {
    D2D_ERROR << "Could not stat file: " << file_path;
    return;
  }

  auto fd = D2D_TEMP_FAILURE_RETRY(::open(file_path.c_str(), O_RDONLY));

  if (fd < 0) {
    D2D_ERROR << "Could not open file: " << file_path;
    return;
  }

  auto mapping = ::mmap(nullptr, stat_buf.st_size, PROT_READ,
                        MAP_FILE | MAP_PRIVATE, fd, 0);

  D2D_TEMP_FAILURE_RETRY(::close(fd));

  if (mapping == MAP_FAILED) {
    D2D_ERROR << "Could not create file mapping: " << file_path;
    return;
  }

  tinyxml2::XMLDocument xml_document;

  auto parse_result =
      xml_document.Parse(static_cast<const char*>(mapping), stat_buf.st_size);

  ::munmap(mapping, stat_buf.st_size);

  if (parse_result != tinyxml2::XML_SUCCESS) {
    D2D_ERROR << "Could not parse XML file: " << file_path;
    return;
  }

  is_valid_ = true;
}

DefinitionParser::~DefinitionParser() = default;

bool DefinitionParser::IsValid() const { return is_valid_; }

}  // namespace d2d
