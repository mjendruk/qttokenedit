
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest-param-test.h>

#include <QtCore/QStringList>
#include <QtCore/QStringListModel>
#include <QtTest/QtTest>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenEdit.h>
#include <qttokenedit/TokenEditView.h>

using Position = int;
using Count = int;

class TokenEditTest : public testing::Test {
 protected:
  void SetUp() override;
  void TearDown() override;

  void show();
  void processEvents();
  mjendruk::TokenEditView const* view() const;
  void compare(QStringList const& expectedStrings,
               QVector<mjendruk::Token*> const& actualTokens,
               Qt::ItemDataRole role) const;

 protected:
  mjendruk::TokenEdit* tokenEdit;
  QStringListModel* model;
};

void TokenEditTest::show() {
  tokenEdit->raise();
  tokenEdit->show();
  tokenEdit->setFocus();

  processEvents();

  QCOMPARE(mjendruk::ActiveTokenEditMode::Edit, tokenEdit->activeMode());
}

void TokenEditTest::processEvents() { qApp->processEvents(); }

mjendruk::TokenEditView const* TokenEditTest::view() const {
  return tokenEdit->view({});
}

void TokenEditTest::compare(QStringList const& expectedStrings,
                            QVector<mjendruk::Token*> const& actualTokens,
                            Qt::ItemDataRole role) const {
  QCOMPARE(expectedStrings.size(), actualTokens.size());

  auto data = [=](mjendruk::Token const* token) {
    return role == Qt::DisplayRole ? token->text() : token->toolTip();
  };

  for (auto index = 0; index < actualTokens.size(); ++index) {
    QCOMPARE(expectedStrings.at(index), data(actualTokens.at(index)));
  }
}

void TokenEditTest::SetUp() {
  tokenEdit = new mjendruk::TokenEdit{};
  model = new QStringListModel{};
}

void TokenEditTest::TearDown() {
  delete tokenEdit;
  delete model;
}

TEST_F(TokenEditTest, SetModel_PopulatedWithOneTokenForEachRow) {
  auto stringList = QStringList{"Abbie", "Jeramey", "Gabriella"};

  model->setStringList(stringList);
  tokenEdit->setModel(model);

  show();

  auto tokens = view()->tokens();
  compare(stringList, tokens, Qt::DisplayRole);
}

struct TokenEditTest_AddRowsAtPosition_Data {
  std::string name;
  QStringList initialStrings;
  Position position;
  QStringList stringsToBeInserted;
  QStringList stringsAfterInsertion;
};

auto TokenEditTest_AddRowsAtPosition_TestCases() {
  auto result = std::vector<TokenEditTest_AddRowsAtPosition_Data>{
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

class TokenEditTest_AddRowsAtPosition
    : public TokenEditTest,
      public testing::WithParamInterface<TokenEditTest_AddRowsAtPosition_Data> {
};

TEST_P(TokenEditTest_AddRowsAtPosition, TokenAdded) {
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
    TokenEditTest, TokenEditTest_AddRowsAtPosition,
    testing::ValuesIn(TokenEditTest_AddRowsAtPosition_TestCases()),
    [](auto const& info) { return info.param.name; });

struct TokenEditTest_RemoveRowsAtPosition_Data {
  std::string name;
  QStringList initialStrings;
  Position position;
  Count count;
  QStringList stringsAfterRemoval;
};

auto TokenEditTest_RemoveRowsAtPosition_TestCases() {
  auto result = std::vector<TokenEditTest_RemoveRowsAtPosition_Data>{
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
  
  return result;
}

class TokenEditTest_RemoveRowsAtPosition
    : public TokenEditTest,
      public testing::WithParamInterface<
          TokenEditTest_RemoveRowsAtPosition_Data> {};

TEST_P(TokenEditTest_RemoveRowsAtPosition, TokenRemoved) {
  auto data = GetParam();

  model->setStringList(data.initialStrings);
  tokenEdit->setModel(model);

  model->removeRows(data.position, data.count);

  show();

  auto tokens = view()->tokens();
  compare(data.stringsAfterRemoval, tokens, Qt::DisplayRole);
}

INSTANTIATE_TEST_CASE_P(
    TokenEditTest, TokenEditTest_RemoveRowsAtPosition,
    testing::ValuesIn(TokenEditTest_RemoveRowsAtPosition_TestCases()),
    [](auto const& info) { return info.param.name; });

// void TokenEditTest::ChangeDataAtPosition_TokenTextUpdated_data() {
//   QTest::addColumn<QStringList>("initialStrings");
//   QTest::addColumn<Position>("position");
//   QTest::addColumn<QStringList>("newStrings");
//   QTest::addColumn<QStringList>("stringsAfterUpdate");

//   QTest::newRow("two at the beginning")
//       << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{0}
//       << QStringList{"Robert", "Rachel"}
//       << QStringList{"Robert", "Rachel", "Gabriella"};

//   QTest::newRow("one at the end")
//       << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{2}
//       << QStringList{"Rachel"} << QStringList{"Abbie", "Jeramey", "Rachel"};
// }

// void TokenEditTest::ChangeDataAtPosition_TokenTextUpdated() {
//   QFETCH(QStringList, initialStrings);
//   QFETCH(Position, position);
//   QFETCH(QStringList, newStrings);
//   QFETCH(QStringList, stringsAfterUpdate);

//   model->setStringList(initialStrings);
//   tokenEdit->setModel(model);

//   for (auto string : newStrings) {
//     auto index = model->index(position++);
//     model->setData(index, string, Qt::DisplayRole);
//   }

//   show();

//   auto tokens = tokenEdit->view({})->tokens();
//   compare(stringsAfterUpdate, tokens, Qt::DisplayRole);
// }

// void TokenEditTest::SetToolTipRole_ToolTipSetOnToken_data() {
//   QTest::addColumn<Position>("position");
//   QTest::addColumn<QStringList>("toolTipsToSet");
//   QTest::addColumn<QStringList>("toolTipsSetOnTokens");

//   QTest::newRow("on first token")
//       << Position{0} << QStringList{"Abbie@mail.com"}
//       << QStringList{"Abbie@mail.com", QString{}, QString{}};

//   QSKIP("StringListModel does not support tooltips.");
// }

// void TokenEditTest::SetToolTipRole_ToolTipSetOnToken() {
//   QFETCH(Position, position);
//   QFETCH(QStringList, toolTipsToSet);
//   QFETCH(QStringList, toolTipsSetOnTokens);

//   auto initialStrings = QStringList{"Abbie", "Jeramey", "Gabriella"};

//   model->setStringList(initialStrings);
//   tokenEdit->setModel(model);

//   for (auto string : toolTipsToSet) {
//     auto index = model->index(position++);
//     auto success = model->setData(index, string, Qt::ToolTipRole);
//     QCOMPARE(true, success);
//   }

//   show();

//   auto tokens = tokenEdit->view({})->tokens();
//   compare(toolTipsSetOnTokens, tokens, Qt::ToolTipRole);
// }
