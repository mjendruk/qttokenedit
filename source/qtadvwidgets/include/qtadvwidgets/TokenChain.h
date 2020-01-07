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
  void insert(int index, TokenChainElement* element);
  void move(int from, int to);
  void remove(TokenChainElement* element);

  void setMode(TokenEditMode mode);

 signals:
  void gotFocus(TokenChainElement* element);
  void lostFocus(TokenChainElement* element);

 private:
  void insert(int index, TokenChainElement* element, bool connect);
  void remove(TokenChainElement* element, bool disconnect);
  TokenChainElement* takeAt(int index, bool disconnect);

  int indexLast() const;
  TokenChainElement* at(int index) const;

 private:
  std::unique_ptr<TokenChainElement> _last;
  int _size;
  TokenEditMode _mode;
};
