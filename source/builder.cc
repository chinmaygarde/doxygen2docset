#include "builder.h"

#include <set>

#include "docset_index.h"
#include "file.h"
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

  const auto docset_name = plist_parser.ReadDocsetName();
  if (docset_name.size() == 0) {
    D2D_ERROR << "Docset name could not be read from the Info.plist.";
    return false;
  }

  std::vector<std::string> resources_dir = {location, docset_name + ".docset",
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

  if (!index.AddTokens(token_parser.ReadTokens())) {
    D2D_ERROR << "Could not add tokens to docset index.";
    return false;
  }

  std::vector<std::string> documents_directory = {
      location, docset_name + ".docset", "Contents", "Resources", "Documents"};

  const std::set<std::string> filtered = {
      "Tokens.xml",
      "Info.plist",
      "Tokens.xml",
      "Makefile",
  };
  auto predicate = [&filtered](const std::string& file_name) -> bool {
    return filtered.count(file_name) == 0;
  };

  if (!CopyFiles(docs, documents_directory, predicate)) {
    D2D_ERROR << "Could not copy files to the Docset documents directory.";
    return false;
  }

  if (!CopyFile(JoinPaths({docs, "Info.plist"}),
                JoinPaths({location, docset_name + ".docset", "Contents",
                           "Info.plist"}))) {
    D2D_ERROR << "Could not copy Info.plist to the docset.";
    return false;
  }

  return true;
}

}  // namespace d2d
