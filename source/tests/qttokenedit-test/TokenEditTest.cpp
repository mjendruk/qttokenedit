
#include <vector>

#include <QtCore/QStringList>
#include <QtCore/QStringListModel>
#include <QtTest/QtTest>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenEdit.h>
#include <qttokenedit/TokenEditView.h>

class TokenEditTest : public QObject {
  Q_OBJECT

 private slots:
  void init();
  void cleanup();

  void SetModel_PopulatedWithOneTokenForEachRow();

  void AddRowAtPosition_TokenAdded_data();
  void AddRowAtPosition_TokenAdded();

  void RemoveRowAtPosition_TokenRemoved_data();
  void RemoveRowAtPosition_TokenRemoved();

 private:
  void show();
  void processEvents();
  void compare(QStringList const& expectedStrings,
               QVector<mjendruk::Token*> const& actualTokens) const;

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
    QVector<mjendruk::Token*> const& actualTokens) const {
  QCOMPARE(expectedStrings.size(), actualTokens.size());

  for (auto index = 0; index < actualTokens.size(); ++index) {
    QCOMPARE(expectedStrings.at(index), actualTokens.at(index)->text());
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
  compare(stringList, tokens);
}

void TokenEditTest::AddRowAtPosition_TokenAdded_data() {
  QTest::addColumn<QStringList>("initialStrings");
  QTest::addColumn<int>("position");
  QTest::addColumn<QStringList>("stringsAfterInsertion");

  QTest::newRow("at_the_beginning")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << 0
      << QStringList{"Robert", "Abbie", "Jeramey", "Gabriella"};
  QTest::newRow("in_the_middle")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << 2
      << QStringList{"Abbie", "Jeramey", "Robert", "Gabriella"};
  QTest::newRow("at_the_end")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << 3
      << QStringList{"Abbie", "Jeramey", "Gabriella", "Robert"};
}

void TokenEditTest::AddRowAtPosition_TokenAdded() {
  QFETCH(QStringList, initialStrings);
  QFETCH(int, position);
  QFETCH(QStringList, stringsAfterInsertion);

  auto stringAdded = QString{"Robert"};

  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  show();

  model->insertRow(position);
  model->setData(model->index(position), stringAdded);

  auto tokens = tokenEdit->view({})->tokens();
  compare(stringsAfterInsertion, tokens);
}

void TokenEditTest::RemoveRowAtPosition_TokenRemoved_data() {
  QTest::addColumn<QStringList>("initialStrings");
  QTest::addColumn<int>("position");
  QTest::addColumn<QStringList>("stringsAfterRemoval");

  QTest::newRow("at_the_beginning")
      << QStringList{"Abbie", "Jeramey", "Gabriella"} << 0
      << QStringList{"Jeramey", "Gabriella"};
  QTest::newRow("in_the_middle") << QStringList{"Abbie", "Jeramey", "Gabriella"}
                                 << 1 << QStringList{"Abbie", "Gabriella"};
  QTest::newRow("at_the_end") << QStringList{"Abbie", "Jeramey", "Gabriella"}
                              << 2 << QStringList{"Abbie", "Jeramey"};
}

void TokenEditTest::RemoveRowAtPosition_TokenRemoved() {
  QFETCH(QStringList, initialStrings);
  QFETCH(int, position);
  QFETCH(QStringList, stringsAfterRemoval);

  model->setStringList(initialStrings);
  tokenEdit->setModel(model);

  show();

  model->removeRows(position, 1);

  show();

  auto tokens = tokenEdit->view({})->tokens();
  compare(stringsAfterRemoval, tokens);
}

QTEST_MAIN(TokenEditTest)

#include "TokenEditTest.moc"
