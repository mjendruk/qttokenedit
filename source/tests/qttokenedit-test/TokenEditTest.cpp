
#include <vector>

#include <QtCore/QStringList>
#include <QtCore/QStringListModel>
#include <QtTest/QtTest>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenEdit.h>
#include <qttokenedit/TokenEditView.h>

class TokenEditTest : public QObject {
  Q_OBJECT

 private:
  using Position = int;
  using Count = int;

 private slots:
  void init();
  void cleanup();

  void SetModel_PopulatedWithOneTokenForEachRow();

  void AddRowsAtPosition_TokenAdded_data();
  void AddRowsAtPosition_TokenAdded();

  void RemoveRowsAtPosition_TokenRemoved_data();
  void RemoveRowsAtPosition_TokenRemoved();

  void ChangeDataAtPosition_TokenTextUpdated_data();
  void ChangeDataAtPosition_TokenTextUpdated();

  void SetToolTipRole_ToolTipSetOnToken_data();
  void SetToolTipRole_ToolTipSetOnToken();

 private:
  void show();
  void processEvents();
  void compare(QStringList const& expectedStrings,
               QVector<mjendruk::Token*> const& actualTokens,
               Qt::ItemDataRole role) const;

 private:
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

void TokenEditTest::compare(
    QStringList const& expectedStrings,
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

void TokenEditTest::init() {
  tokenEdit = new mjendruk::TokenEdit{};
  model = new QStringListModel{};
}

void TokenEditTest::cleanup() {
  delete tokenEdit;
  delete model;
}

void TokenEditTest::SetModel_PopulatedWithOneTokenForEachRow() {
  auto stringList = QStringList{"Abbie", "Jeramey", "Gabriella"};

  model->setStringList(stringList);
  tokenEdit->setModel(model);

  show();

  auto tokens = tokenEdit->view({})->tokens();
  compare(stringList, tokens, Qt::DisplayRole);
}

void TokenEditTest::AddRowsAtPosition_TokenAdded_data() {
  QTest::addColumn<QStringList>("initialStrings");
  QTest::addColumn<Position>("position");
  QTest::addColumn<QStringList>("stringsToBeInserted");
  QTest::addColumn<QStringList>("stringsAfterInsertion");

  QTest::newRow("one at the beginning")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{0}
      << QStringList{"Robert"}
      << QStringList{"Robert", "Abbie", "Jeramey", "Gabriella"};

  QTest::newRow("one in the middle")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{2}
      << QStringList{"Robert"}
      << QStringList{"Abbie", "Jeramey", "Robert", "Gabriella"};

  QTest::newRow("one at the end")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{3}
      << QStringList{"Robert"}
      << QStringList{"Abbie", "Jeramey", "Gabriella", "Robert"};

  QTest::newRow("two in the middle")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{2}
      << QStringList{"Robert", "Rachel"}
      << QStringList{"Abbie", "Jeramey", "Robert", "Rachel", "Gabriella"};

  QTest::newRow("two at the end")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{3}
      << QStringList{"Robert", "Rachel"}
      << QStringList{"Abbie", "Jeramey", "Gabriella", "Robert", "Rachel"};
}

void TokenEditTest::AddRowsAtPosition_TokenAdded() {
  QFETCH(QStringList, initialStrings);
  QFETCH(Position, position);
  QFETCH(QStringList, stringsToBeInserted);
  QFETCH(QStringList, stringsAfterInsertion);

  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  show();

  model->insertRows(position, stringsToBeInserted.size());

  for (auto string : stringsToBeInserted) {
    model->setData(model->index(position++), string);
  }

  auto tokens = tokenEdit->view({})->tokens();
  compare(stringsAfterInsertion, tokens, Qt::DisplayRole);
}

void TokenEditTest::RemoveRowsAtPosition_TokenRemoved_data() {
  QTest::addColumn<QStringList>("initialStrings");
  QTest::addColumn<Position>("position");
  QTest::addColumn<Count>("count");
  QTest::addColumn<QStringList>("stringsAfterRemoval");

  QTest::newRow("one at the beginning")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{0} << Count{1}
      << QStringList{"Jeramey", "Gabriella"};

  QTest::newRow("one in the middle")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{1} << Count{1}
      << QStringList{"Abbie", "Gabriella"};

  QTest::newRow("one at the end")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{2} << Count{1}
      << QStringList{"Abbie", "Jeramey"};

  QTest::newRow("two at the beginning")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{0} << Count{2}
      << QStringList{"Gabriella"};

  QTest::newRow("two at the end")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{1} << Count{2}
      << QStringList{"Abbie"};
}

void TokenEditTest::RemoveRowsAtPosition_TokenRemoved() {
  QFETCH(QStringList, initialStrings);
  QFETCH(Position, position);
  QFETCH(Count, count);
  QFETCH(QStringList, stringsAfterRemoval);

  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  model->removeRows(position, count);

  show();

  auto tokens = tokenEdit->view({})->tokens();
  compare(stringsAfterRemoval, tokens, Qt::DisplayRole);
}

void TokenEditTest::ChangeDataAtPosition_TokenTextUpdated_data() {
  QTest::addColumn<QStringList>("initialStrings");
  QTest::addColumn<Position>("position");
  QTest::addColumn<QStringList>("newStrings");
  QTest::addColumn<QStringList>("stringsAfterUpdate");

  QTest::newRow("two at the beginning")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{0}
      << QStringList{"Robert", "Rachel"}
      << QStringList{"Robert", "Rachel", "Gabriella"};

  QTest::newRow("one at the end")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << Position{2}
      << QStringList{"Rachel"} << QStringList{"Abbie", "Jeramey", "Rachel"};
}

void TokenEditTest::ChangeDataAtPosition_TokenTextUpdated() {
  QFETCH(QStringList, initialStrings);
  QFETCH(Position, position);
  QFETCH(QStringList, newStrings);
  QFETCH(QStringList, stringsAfterUpdate);

  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  for (auto string : newStrings) {
    auto index = model->index(position++);
    model->setData(index, string, Qt::DisplayRole);
  }

  show();

  auto tokens = tokenEdit->view({})->tokens();
  compare(stringsAfterUpdate, tokens, Qt::DisplayRole);
}

void TokenEditTest::SetToolTipRole_ToolTipSetOnToken_data() {
  QTest::addColumn<Position>("position");
  QTest::addColumn<QStringList>("toolTipsToSet");
  QTest::addColumn<QStringList>("toolTipsSetOnTokens");

  QTest::newRow("on first token")
      << Position{0} << QStringList{"Abbie@mail.com"}
      << QStringList{"Abbie@mail.com", QString{}, QString{}};
  
  QSKIP("StringListModel does not support tooltips.");
}

void TokenEditTest::SetToolTipRole_ToolTipSetOnToken() {
  QFETCH(Position, position);
  QFETCH(QStringList, toolTipsToSet);
  QFETCH(QStringList, toolTipsSetOnTokens);

  auto initialStrings = QStringList{"Abbie", "Jeramey", "Gabriella"};
  
  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  for (auto string : toolTipsToSet) {
    auto index = model->index(position++);
    auto success = model->setData(index, string, Qt::ToolTipRole);
    QCOMPARE(true, success);
  }

  show();

  auto tokens = tokenEdit->view({})->tokens();
  compare(toolTipsSetOnTokens, tokens, Qt::ToolTipRole);
}

QTEST_MAIN(TokenEditTest)

#include "TokenEditTest.moc"
