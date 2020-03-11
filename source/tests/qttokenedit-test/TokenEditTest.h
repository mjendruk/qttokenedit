
#include <gmock/gmock.h>

#include <QtCore/QStringList>
#include <QtCore/QStringListModel>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenEdit.h>
#include <qttokenedit/TokenEditView.h>

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