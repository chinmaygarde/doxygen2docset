

#include <gtest/gtest.h>

#include "docset_index.h"
#include "token_parser.h"

namespace d2d {
namespace testing {

TEST(DoxyGen2DocsetTest, CanParseTokens) {
  TokenParser parser(
      "/Users/chinmaygarde/VersionControlled/doxygen2docset/tests/fixtures/"
      "Tokens.xml");

  ASSERT_TRUE(parser.IsValid());
  auto tokens = parser.ReadTokens();
  ASSERT_EQ(tokens.size(), 24877u);
  ASSERT_EQ(tokens[0].GetName(), "Main");
  ASSERT_EQ(tokens[0].GetLanguage(), "cpp");
  ASSERT_EQ(tokens[0].GetType(), "func");
  ASSERT_EQ(tokens[0].GetScope(), "benchmarking");
  ASSERT_EQ(tokens[0].GetPath(), "namespacebenchmarking.html");
  ASSERT_EQ(tokens[0].GetAnchor(), "a96ee0ac7e6719129af3e0e0eac71bb72");
  ASSERT_EQ(tokens[0].GetDeclaredIn(), "benchmarking.cc");
}

TEST(DoxyGen2DocsetTest, CanCreateDocsetIndex) {
  DocsetIndex index("/Users/chinmaygarde/Desktop/docsetindex.db");
  ASSERT_TRUE(index.IsValid());
}

TEST(DoxyGen2DocsetTest, CanInsertTokensIntoIndex) {
  DocsetIndex index("/Users/chinmaygarde/Desktop/docsetindex.db");
  ASSERT_TRUE(index.IsValid());
  TokenParser parser(
      "/Users/chinmaygarde/VersionControlled/doxygen2docset/tests/fixtures/"
      "Tokens.xml");

  ASSERT_TRUE(parser.IsValid());
  auto tokens = parser.ReadTokens();
  ASSERT_EQ(tokens.size(), 24877u);

  ASSERT_TRUE(index.AddTokens(tokens));
}

}  // namespace testing
}  // namespace d2d
