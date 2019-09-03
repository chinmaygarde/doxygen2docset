#include "html_parser.h"

#include "file.h"
#include "logger.h"

namespace d2d {

HTMLParser::HTMLParser(const std::string& path) {
  auto mapping = OpenFileReadOnly(path);
  if (!mapping || !mapping->IsValid()) {
    D2D_ERROR << "Could not open HTML file to parse.";
    return;
  }

  parser_ = ::gumbo_parse_with_options(&kGumboDefaultOptions,
                                       static_cast<const char*>(mapping->Get()),
                                       mapping->GetSize());

  if (!parser_ || parser_->root == nullptr) {
    D2D_ERROR << "Could not parse HTML file at path: " << path;
    return;
  }

  is_valid_ = true;
}

HTMLParser::~HTMLParser() {
  if (parser_) {
    ::gumbo_destroy_output(&kGumboDefaultOptions, parser_);
  }
}

bool HTMLParser::IsValid() const { return is_valid_; }

}  // namespace d2d
