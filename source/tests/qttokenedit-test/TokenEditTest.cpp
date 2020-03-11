#include "TokenEditTest.h"

#include <QtTest/QtTest>

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