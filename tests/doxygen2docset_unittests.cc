

#include <gtest/gtest.h>

#include "definition_parser.h"

namespace d2d {
namespace testing {

TEST(DoxyGen2DocsetTest, CanParseDefinition) {
  DefinitionParser parser(
      "/Users/chinmaygarde/VersionControlled/doxygen2docset/tests/fixtures/"
      "Tokens.xml");

  ASSERT_TRUE(parser.IsValid());
}

}  // namespace testing
}  // namespace d2d
