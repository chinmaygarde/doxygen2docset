// This source file is part of doxygen2docset.
// Licensed under the MIT License. See LICENSE.md file for details.

#include <gtest/gtest.h>

#include "builder.h"
#include "docset_index.h"
#include "fixture.h"
#include "html_parser.h"
#include "token_parser.h"

#ifndef D2D_FIXTURES_LOCATION
#error Fixtures not available.
#endif

namespace d2d {
namespace testing {

TEST(DoxyGen2DocsetTest, CanParseTokens) {
  TokenParser parser(D2D_FIXTURES_LOCATION "/Tokens.xml");

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
  DocsetIndex index("/tmp/docsetindex.db");
  ASSERT_TRUE(index.IsValid());
}

TEST(DoxyGen2DocsetTest, CanInsertTokensIntoIndex) {
  DocsetIndex index("/tmp/docsetindex.db");
  ASSERT_TRUE(index.IsValid());
  TokenParser parser(D2D_FIXTURES_LOCATION "/Tokens.xml");

  ASSERT_TRUE(parser.IsValid());
  auto tokens = parser.ReadTokens();
  ASSERT_EQ(tokens.size(), 24877u);

  ASSERT_TRUE(index.AddTokens(tokens));
}

TEST(DoxyGen2DocsetTest, CanBuildCompleteDocset) {
  ASSERT_TRUE(BuildDocset(D2D_FIXTURES_LOCATION, "/tmp/builtdocset"));
}

TEST(DoxyGen2DocsetTest, CanParseHTML) {
  HTMLParser parser(
      OpenFileReadOnly(D2D_FIXTURES_LOCATION "/classflutter_1_1_shell.html"));
  ASSERT_TRUE(parser.IsValid());
}

TEST(DoxyGen2DocsetTest, CanGetTokensByFile) {
  TokenParser parser(D2D_FIXTURES_LOCATION "/Tokens.xml");
  ASSERT_TRUE(parser.IsValid());
  auto tokens = parser.ReadTokens();
  ASSERT_EQ(tokens.size(), 24877u);
  auto links = Token::GetTokensByFile(tokens);
  ASSERT_EQ(links.size(), 1630u);
}

}  // namespace testing
}  // namespace d2d
