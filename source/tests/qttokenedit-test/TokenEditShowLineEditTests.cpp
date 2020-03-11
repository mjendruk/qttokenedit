
#include <vector>

#include <gtest/gtest-param-test.h>

#include <QtTest/QtTest>
#include <QtWidgets/QLineEdit>

#include "TokenEditTest.h"

struct TokenEditShowLineEditTest_Data {
  std::string name;
  QStringList strings;
};

std::vector<TokenEditShowLineEditTest_Data>
TokenEditShowLineEditTest_TestCases() {
  return {
      {
          "NoStrings",
          QStringList{},
      },
      {
          "OneString",
          QStringList{"Abbie"},
      },
      {
          "SeveralString",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
      },
  };
}

class TokenEditShowLineEditTest
    : public TokenEditTest,
      public testing::WithParamInterface<
          TokenEditShowLineEditTest_Data> {};

TEST_P(TokenEditShowLineEditTest, SetAlways_LineEditShown) {
  auto data = GetParam();

  model->setStringList(data.strings);
  tokenEdit->setModel(model);
  tokenEdit->setShowLineEdit(mjendruk::ShowLineEdit::Always);

  show();

  auto lineEdit = tokenEdit->lineEdit();

  EXPECT_TRUE(lineEdit->isVisible());
}

TEST_P(TokenEditShowLineEditTest, SetNever_LineEditNotShown) {
  auto data = GetParam();

  model->setStringList(data.strings);
  tokenEdit->setModel(model);
  tokenEdit->setShowLineEdit(mjendruk::ShowLineEdit::Never);

  show();

  auto lineEdit = tokenEdit->lineEdit();

  EXPECT_FALSE(lineEdit->isVisible());
}

TEST_P(TokenEditShowLineEditTest, SetWhenEmpty_LineEditShownIfEmpty) {
  auto data = GetParam();

  model->setStringList(data.strings);
  tokenEdit->setModel(model);
  tokenEdit->setShowLineEdit(mjendruk::ShowLineEdit::WhenEmpty);

  show();

  auto lineEdit = tokenEdit->lineEdit();

  EXPECT_EQ(data.strings.empty(), lineEdit->isVisible());
}

INSTANTIATE_TEST_CASE_P(
    TokenEditTest, TokenEditShowLineEditTest,
    testing::ValuesIn(TokenEditShowLineEditTest_TestCases()),
    [](auto const& info) { return info.param.name; });
