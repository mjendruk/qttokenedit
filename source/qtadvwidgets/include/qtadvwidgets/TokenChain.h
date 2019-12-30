#pragma once

#include <qtadvwidgets/TokenChainElement.h>
#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>
#include <memory>

class QLineEdit;

class QTADVWIDGETS_API TokenChain : public QObject {
  Q_OBJECT
 public:
  TokenChain(TokenEditMode mode, QLineEdit* last, QObject* parent = nullptr);
  ~TokenChain();

  void add(TokenChainElement* element);
  void remove(TokenChainElement* element);

  void setMode(TokenEditMode mode);

 signals:
  void gotFocus(TokenChainElement* element);
  void lostFocus(TokenChainElement* element);

 private:
  std::unique_ptr<TokenChainElement> _last;
  TokenEditMode _mode;
};
