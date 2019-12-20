#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <memory>

class QWidget;

class TokenChainElement;

class QTADVWIDGETS_API TokenChain {
 public:
  TokenChain(QWidget* last);
  ~TokenChain();

  void add(TokenChainElement* element);
  void remove(TokenChainElement* element);

 private:
  std::unique_ptr<TokenChainElement> _last;
};
