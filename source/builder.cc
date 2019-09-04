#include "builder.h"

#include <set>

#include "docset_index.h"
#include "file.h"
#include "html_parser.h"
#include "logger.h"
#include "plist_parser.h"
#include "token_parser.h"

namespace d2d {

bool BuildDocset(const std::string& docs, const std::string& location) {
  PlistParser plist_parser(JoinPaths({docs, "Info.plist"}));
  if (!plist_parser.IsValid()) {
    D2D_ERROR << "Could not parse Info.plist.";
    return false;
  }

  const auto docset_id = plist_parser.ReadDocsetID();
  const auto docset_name = plist_parser.ReadDocsetName();
  if (docset_id.size() == 0 || docset_name.size() == 0) {
    D2D_ERROR << "Docset name or ID could not be read from the Info.plist.";
    return false;
  }

  std::vector<std::string> resources_dir = {location, docset_id + ".docset",
                                            "Contents", "Resources"};
  if (!MakeDirectories(resources_dir)) {
    D2D_ERROR << "Could not not create docset directories.";
    return false;
  }

  DocsetIndex index(JoinPaths(resources_dir, "docSet.dsidx"));

  if (!index.IsValid()) {
    D2D_ERROR << "Could not create docset index.";
    return false;
  }

  TokenParser token_parser(JoinPaths({docs, "Tokens.xml"}));
  if (!token_parser.IsValid()) {
    D2D_ERROR << "Tokens.xml file was not found in " << docs
              << ". Did you make sure to generate Doxygen documentation with "
                 "the GENERATE_DOCSET option set to YES?";
    return false;
  }

  auto tokens = token_parser.ReadTokens();

  if (!index.AddTokens(tokens)) {
    D2D_ERROR << "Could not add tokens to docset index.";
    return false;
  }

  std::vector<std::string> documents_directory = {
      location, docset_id + ".docset", "Contents", "Resources", "Documents"};

  const std::set<std::string> filtered = {
      "Tokens.xml",
      "Info.plist",
      "Tokens.xml",
      "Makefile",
  };

  auto tokens_by_file = Token::GetTokensByFile(tokens);

  auto predicate = [&filtered, &tokens_by_file](
                       const std::string& from_file_name,  //
                       const struct stat& from_stat,       //
                       const AutoFD& from_fd,              //
                       const std::string& to_file_name) -> bool {
    // Check if this file needs to be filtered away.
    if (filtered.count(from_file_name) != 0) {
      return true;
    }

    // Check if this is a file in which a TOC needs to be generated.
    {
      const auto found = tokens_by_file.find(from_file_name);
      if (found != tokens_by_file.end()) {
        HTMLParser parser(OpenFileReadOnly(from_fd, from_stat.st_size));
        auto html_with_toc = parser.BuildHTMLWithTOC(found->second);
        if (html_with_toc.IsValid()) {
          if (!CopyData(html_with_toc.Get(),      //
                        html_with_toc.GetSize(),  //
                        to_file_name)) {
            D2D_ERROR << "Could not copy HTML with TOC to " << to_file_name
                      << ". Will try moving file without TOC.";

          } else {
            return true;
          }
        } else {
          D2D_ERROR << "Could not build TOC in file: " << from_file_name
                    << ". Skipping.";
        }
      }
    }

    // Copy file as-is.
    return CopyFile(from_stat, from_fd, to_file_name);
  };

  if (!CopyFiles(docs, documents_directory, predicate)) {
    D2D_ERROR << "Could not copy files to the Docset documents directory.";
    return false;
  }

  if (!WriteDocSetPlist(docset_id, docset_name,
                        JoinPaths({location, docset_id + ".docset", "Contents",
                                   "Info.plist"}))) {
    D2D_ERROR << "Could not write Info.plist to the docset.";
    return false;
  }

  return true;
}

}  // namespace d2d
