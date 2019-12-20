#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QWidget>

class QTADVWIDGETS_API TokenChainElement {
 public:
  TokenChainElement(QWidget* widget);

  TokenChainElement* previousElement() const;
  void setPreviousElement(TokenChainElement* element);

  TokenChainElement* nextElement() const;
  void setNextElement(TokenChainElement* element);

  QWidget* widget() const;

 private:
  TokenChainElement* _previousElement = nullptr;
  TokenChainElement* _nextElement = nullptr;
  QWidget* _widget = nullptr;
};
