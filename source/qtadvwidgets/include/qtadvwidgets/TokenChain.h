#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <memory>

class QLineEdit;

class TokenChainElement;

class QTADVWIDGETS_API TokenChain {
 public:
  TokenChain(QLineEdit* last);
  ~TokenChain();

  void add(TokenChainElement* element);
  void remove(TokenChainElement* element);

 private:
  std::unique_ptr<TokenChainElement> _last;
};
