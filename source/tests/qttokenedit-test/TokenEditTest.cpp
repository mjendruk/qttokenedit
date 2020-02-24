
#include <QtCore/QStringListModel>
#include <QtTest/QtTest>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenEdit.h>

class TokenEditTest : public QObject {
  Q_OBJECT

 private slots:
  void SetModel_PopulatedWithOneTokenForEachRow();
};

void TokenEditTest::SetModel_PopulatedWithOneTokenForEachRow() {
  auto stringList = QStringList{"Abbie", "Jeramey", "Gabriella"};

  auto stringModel = new QStringListModel{this};
  stringModel->setStringList(stringList);

  mjendruk::TokenEdit tokenEdit{};
  tokenEdit.setModel(stringModel);

  tokenEdit.show();
  tokenEdit.setFocus();

  auto tokens = tokenEdit.findChildren<mjendruk::Token*>();
  
  QCOMPARE(3, tokens.size());

  for (auto index = 0; index < tokens.size(); ++index) {
    QCOMPARE(stringList.at(index), tokens.at(index)->text());
  }
}

QTEST_MAIN(TokenEditTest)

#include "TokenEditTest.moc"
