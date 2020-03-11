
#include <vector>

#include <gtest/gtest-param-test.h>

#include <QtTest/QtTest>

#include "TokenEditTest.h"

using Position = int;
using Count = int;

TEST_F(TokenEditTest, ItemModel_SetModel_PopulatedWithOneTokenForEachRow) {
  auto stringList = QStringList{"Abbie", "Jeramey", "Gabriella"};

  model->setStringList(stringList);
  tokenEdit->setModel(model);

  show();

  auto tokens = view()->tokens();
  compare(stringList, tokens, Qt::DisplayRole);
}

struct TokenEditItemModelTest_AddRowsAtPosition_Data {
  std::string name;
  QStringList initialStrings;
  Position position;
  QStringList stringsToBeInserted;
  QStringList stringsAfterInsertion;
};

auto TokenEditItemModelTest_AddRowsAtPosition_TestCases() {
  auto result = std::vector<TokenEditItemModelTest_AddRowsAtPosition_Data>{
      {
          "OneAtTheBeginning",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{0},
          QStringList{"Robert"},
          QStringList{"Robert", "Abbie", "Jeramey", "Gabriella"},
      },
      {
          "OneInTheMiddle",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{2},
          QStringList{"Robert"},
          QStringList{"Abbie", "Jeramey", "Robert", "Gabriella"},
      },
      {
          "OneAtTheEnd",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{3},
          QStringList{"Robert"},
          QStringList{"Abbie", "Jeramey", "Gabriella", "Robert"},
      },
      {
          "TwoInTheMiddle",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{2},
          QStringList{"Robert", "Rachel"},
          QStringList{"Abbie", "Jeramey", "Robert", "Rachel", "Gabriella"},
      },
      {
          "TwoAtTheEnd",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{3},
          QStringList{"Robert", "Rachel"},
          QStringList{"Abbie", "Jeramey", "Gabriella", "Robert", "Rachel"},
      }};

  return result;
}

class TokenEditItemModelTest_AddRowsAtPosition
    : public TokenEditTest,
      public testing::WithParamInterface<
          TokenEditItemModelTest_AddRowsAtPosition_Data> {};

TEST_P(TokenEditItemModelTest_AddRowsAtPosition, TokenAdded) {
  auto data = GetParam();

  model->setStringList(data.initialStrings);
  tokenEdit->setModel(model);

  show();

  model->insertRows(data.position, data.stringsToBeInserted.size());

  for (auto string : data.stringsToBeInserted) {
    model->setData(model->index(data.position++), string);
  }

  auto tokens = view()->tokens();
  compare(data.stringsAfterInsertion, tokens, Qt::DisplayRole);
}

INSTANTIATE_TEST_CASE_P(
    TokenEditTest, TokenEditItemModelTest_AddRowsAtPosition,
    testing::ValuesIn(TokenEditItemModelTest_AddRowsAtPosition_TestCases()),
    [](auto const& info) { return info.param.name; });

struct TokenEditItemModelTest_RemoveRowsAtPosition_Data {
  std::string name;
  QStringList initialStrings;
  Position position;
  Count count;
  QStringList stringsAfterRemoval;
};

std::vector<TokenEditItemModelTest_RemoveRowsAtPosition_Data>
TokenEditItemModelTest_RemoveRowsAtPosition_TestCases() {
  return {
      {
          "OneAtTheBeginning",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{0},
          Count{1},
          QStringList{"Jeramey", "Gabriella"},
      },
      {
          "OneInTheMiddle",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{1},
          Count{1},
          QStringList{"Abbie", "Gabriella"},
      },
      {
          "OneAtTheEnd",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{2},
          Count{1},
          QStringList{"Abbie", "Jeramey"},
      },
      {
          "TwoAtTheBeginning",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{0},
          Count{2},
          QStringList{"Gabriella"},
      },
      {
          "TwoAtTheEnd",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{1},
          Count{2},
          QStringList{"Abbie"},
      },
  };
}

class TokenEditItemModelTest_RemoveRowsAtPosition
    : public TokenEditTest,
      public testing::WithParamInterface<
          TokenEditItemModelTest_RemoveRowsAtPosition_Data> {};

TEST_P(TokenEditItemModelTest_RemoveRowsAtPosition, TokenRemoved) {
  auto data = GetParam();

  model->setStringList(data.initialStrings);
  tokenEdit->setModel(model);

  model->removeRows(data.position, data.count);

  show();

  auto tokens = view()->tokens();
  compare(data.stringsAfterRemoval, tokens, Qt::DisplayRole);
}

INSTANTIATE_TEST_CASE_P(
    TokenEditTest, TokenEditItemModelTest_RemoveRowsAtPosition,
    testing::ValuesIn(TokenEditItemModelTest_RemoveRowsAtPosition_TestCases()),
    [](auto const& info) { return info.param.name; });

struct TokenEditItemModelTest_ChangeDataAtPosition_Data {
  std::string name;
  QStringList initialStrings;
  Position position;
  QStringList newStrings;
  QStringList stringsAfterUpdate;
};

std::vector<TokenEditItemModelTest_ChangeDataAtPosition_Data>
TokenEditItemModelTest_ChangeDataAtPosition_TestCases() {
  return {
      {
          "TwoAtTheBeginning",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{0},
          QStringList{"Robert", "Rachel"},
          QStringList{"Robert", "Rachel", "Gabriella"},
      },
      {
          "OneAtTheEnd",
          QStringList{"Abbie", "Jeramey", "Gabriella"},
          Position{2},
          QStringList{"Rachel"},
          QStringList{"Abbie", "Jeramey", "Rachel"},
      },
  };
}

class TokenEditItemModelTest_ChangeDataAtPosition
    : public TokenEditTest,
      public testing::WithParamInterface<
          TokenEditItemModelTest_ChangeDataAtPosition_Data> {};

TEST_P(TokenEditItemModelTest_ChangeDataAtPosition, TokenTextUpdated) {
  auto data = GetParam();

  model->setStringList(data.initialStrings);
  tokenEdit->setModel(model);

  for (auto string : data.newStrings) {
    auto index = model->index(data.position++);
    model->setData(index, string, Qt::DisplayRole);
  }

  show();

  auto tokens = view()->tokens();
  compare(data.stringsAfterUpdate, tokens, Qt::DisplayRole);
}

INSTANTIATE_TEST_CASE_P(
    TokenEditTest, TokenEditItemModelTest_ChangeDataAtPosition,
    testing::ValuesIn(TokenEditItemModelTest_ChangeDataAtPosition_TestCases()),
    [](auto const& info) { return info.param.name; });

struct TokenEditItemModelTest_SetToolTipRole_Data {
  std::string name;
  Position position;
  QStringList toolTipsToSet;
  QStringList toolTipsSetOnTokens;
};

std::vector<TokenEditItemModelTest_SetToolTipRole_Data>
TokenEditItemModelTest_SetToolTipRole_TestCases() {
  return {
      {
          "OnFirstToken",
          Position{0},
          QStringList{"Abbie@mail.com"},
          QStringList{"Abbie@mail.com", QString{}, QString{}},
      },
  };
}

class TokenEditItemModelTest_SetToolTipRole
    : public TokenEditTest,
      public testing::WithParamInterface<
          TokenEditItemModelTest_SetToolTipRole_Data> {};

//! disabled: StringListModel does not support tooltips.
TEST_P(TokenEditItemModelTest_SetToolTipRole, DISABLED_ToolTipSetOnToken) {
  auto data = GetParam();

  auto initialStrings = QStringList{"Abbie", "Jeramey", "Gabriella"};

  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  for (auto string : data.toolTipsToSet) {
    auto index = model->index(data.position++);
    auto success = model->setData(index, string, Qt::ToolTipRole);
    QCOMPARE(true, success);
  }

  show();

  auto tokens = view()->tokens();
  compare(data.toolTipsSetOnTokens, tokens, Qt::ToolTipRole);
}

INSTANTIATE_TEST_CASE_P(
    TokenEditTest, TokenEditItemModelTest_SetToolTipRole,
    testing::ValuesIn(TokenEditItemModelTest_SetToolTipRole_TestCases()),
    [](auto const& info) { return info.param.name; });
