#include <qtadvwidgets/TokenChainElement.h>

TokenChainElement::TokenChainElement(QWidget* widget) : _widget{widget} {}

TokenChainElement* TokenChainElement::previousElement() const {
  return _previousElement;
}

void TokenChainElement::setPreviousElement(TokenChainElement* element) {
  _previousElement = element;
}

TokenChainElement* TokenChainElement::nextElement() const {
  return _nextElement;
}

void TokenChainElement::setNextElement(TokenChainElement* element) {
  _nextElement = element;
}

QWidget* TokenChainElement::widget() const { return _widget; }
