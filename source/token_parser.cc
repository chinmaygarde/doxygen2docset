
#include "token_parser.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logger.h"

namespace d2d {

TokenParser::TokenParser(const std::string& file_path) {
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

  auto parse_result =
      xml_document_.Parse(static_cast<const char*>(mapping), stat_buf.st_size);

  ::munmap(mapping, stat_buf.st_size);

  if (parse_result != tinyxml2::XML_SUCCESS) {
    D2D_ERROR << "Could not parse XML file: " << file_path;
    return;
  }

  is_valid_ = true;
}

TokenParser::~TokenParser() = default;

bool TokenParser::IsValid() const { return is_valid_; }

std::vector<Token> TokenParser::ReadTokens() const {
  if (!is_valid_) {
    return {};
  }

  std::vector<Token> tokens;

  if (auto xml_tokens = xml_document_.FirstChildElement("Tokens")) {
    for (auto xml_token = xml_tokens->FirstChildElement("Token");
         xml_token != nullptr;
         xml_token = xml_token->NextSiblingElement("Token")) {
      Token token(xml_token);
      if (token.IsValid()) {
        tokens.emplace_back(std::move(token));
      }
    }
  }

  return tokens;
}

}  // namespace d2d
