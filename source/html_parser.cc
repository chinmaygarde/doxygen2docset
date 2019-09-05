// This source file is part of doxygen2docset.
// Licensed under the MIT License. See LICENSE.md file for details.

#include "html_parser.h"

#include <map>
#include <sstream>

#include "logger.h"

namespace d2d {

HTMLParser::HTMLParser(std::unique_ptr<AutoMapping> mapping)
    : mapping_(std::move(mapping)) {
  if (!mapping_ || !mapping_->IsValid()) {
    D2D_ERROR << "HTML mapping was not valid.";
    return;
  }

  parser_ = ::gumbo_parse_with_options(
      &kGumboDefaultOptions, static_cast<const char*>(mapping_->Get()),
      mapping_->GetSize());

  if (!parser_ || parser_->root == nullptr) {
    D2D_ERROR << "Could not parse HTML file";
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

static std::string DashAnchorFromToken(const Token& token) {
  std::stringstream stream;
  stream << "<a name=\"//apple_ref/cpp/" << token.GetIndexType() << "/"
         << token.GetIndexName() << "\" class=\"dashAnchor\">&nbsp;</a>";
  return stream.str();
}

static void WalkHTMLTreeForAnchors(
    GumboNode* parent,
    std::function<void(const GumboElement& element)> callback) {
  if (!parent) {
    return;
  }

  if (parent->type != GumboNodeType::GUMBO_NODE_ELEMENT) {
    return;
  }

  auto element = parent->v.element;

  if (element.tag == GumboTag::GUMBO_TAG_A) {
    callback(element);
  }

  const auto& children = element.children;
  for (size_t i = 0; i < children.length; ++i) {
    auto child_node = reinterpret_cast<GumboNode**>(children.data)[i];
    WalkHTMLTreeForAnchors(child_node, callback);
  }
}

Allocation HTMLParser::BuildHTMLWithTOC(
    const std::vector<Token>& tokens) const {
  if (!IsValid()) {
    return {};
  }

  std::map<std::string, Token> known_anchors;
  for (const auto& token : tokens) {
    known_anchors["#" + token.GetAnchor()] = token;
    known_anchors[token.GetIndexPath()] = token;
  }

  std::map<size_t, std::string, std::less<size_t>> source_insertions;

  WalkHTMLTreeForAnchors(parser_->root, [&](const GumboElement& element) {
    const auto& attributes = element.attributes;

    for (int i = 0; i < attributes.length; ++i) {
      const auto& attribute =
          reinterpret_cast<GumboAttribute**>(attributes.data)[i];
      if (strncmp(attribute->name, "href", strlen("href")) != 0) {
        return;
      }

      auto found_anchor = known_anchors.find(std::string{attribute->value});
      if (found_anchor == known_anchors.end()) {
        return;
      }

      source_insertions[element.start_pos.offset] =
          DashAnchorFromToken(found_anchor->second);
    }
  });

  size_t insertions_size = 0;
  for (const auto& insertion : source_insertions) {
    insertions_size += insertion.second.size();
  }

  Allocation rewritten_allocation(mapping_->GetSize() + insertions_size);

  if (!rewritten_allocation.IsValid()) {
    D2D_ERROR << "Could not allocate bytes for HTML rewrite.";
    return {};
  }

  size_t source_offset = 0;
  size_t destination_offset = 0;
  uint8_t* destination = rewritten_allocation.Get();
  const uint8_t* source = static_cast<const uint8_t*>(mapping_->Get());

  for (const auto& insertion : source_insertions) {
    const auto insertion_offset = insertion.first;
    const auto& insertion_string = insertion.second;

    // Copy the HTML.
    ::memmove(destination + destination_offset, source + source_offset,
              insertion_offset - source_offset);

    destination_offset += (insertion_offset - source_offset);
    source_offset += (insertion_offset - source_offset);

    // Copy the insertion.
    ::memmove(destination + destination_offset, insertion_string.data(),
              insertion_string.size());
    destination_offset += (insertion_string.size());
  }

  // Copy final block.
  ::memmove(destination + destination_offset, source + source_offset,
            mapping_->GetSize() - source_offset);

  return rewritten_allocation;
}

}  // namespace d2d
