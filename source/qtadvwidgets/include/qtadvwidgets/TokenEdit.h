#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QScrollArea>
#include <QVariant>
#include <QVector>
#include <cstdint>
#include <memory>

class QLineEdit;

class Token;
class FlexLayout;
class TokenLineEdit;
class TokenChain;

class QTADVWIDGETS_API TokenEdit : public QScrollArea {
  Q_OBJECT

 public:
  TokenEdit(QWidget* parent = nullptr);
  ~TokenEdit();

  int maxLineCount() const;
  void setMaxLineCount(int count);

  void addItem(QString const& text, QVariant const& userData = QVariant{});
  void addItems(QStringList const& texts);

  void setItemData(int index, QVariant const& value);
  void setItemText(int index, QString const& text);

  void removeItem(int index);

  QString itemText(int index) const;
  QVariant itemData(int index) const;

  int count() const;

  QLineEdit* lineEdit();

 signals:
  void itemAboutToBeRemoved(int index);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  void updateHeight();

 private:
  QVector<Token*> _items;
  TokenLineEdit* _lineEdit;
  std::unique_ptr<TokenChain> _tokenChain;
  FlexLayout* _layout;
  int _maxLineCount;
};
