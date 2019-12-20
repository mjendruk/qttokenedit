#include <qtadvwidgets/TokenChain.h>
#include <qtadvwidgets/TokenChainElement.h>
#include <qtadvwidgets/LineEditChainElement.h>

#include <QWidget>
#include <QtGlobal>

TokenChain::TokenChain(QLineEdit* last)
    : _last{std::make_unique<LineEditChainElement>(last)} {}

TokenChain::~TokenChain() = default;

void TokenChain::add(TokenChainElement* element) {
  Q_ASSERT(element);

  auto prev = _last->previousElement();

  if (prev) {
    prev->setNextElement(element);
    element->setPreviousElement(prev);
  }

  _last->setPreviousElement(element);
  element->setNextElement(_last.get());
}

void TokenChain::remove(TokenChainElement* element) {
  Q_ASSERT(element);
  Q_ASSERT(element != _last.get());

  auto prev = element->previousElement();
  auto next = element->nextElement();

  if (prev) {
    prev->setNextElement(next);
  }

  if (next) {
    next->setPreviousElement(prev);
  }

  if (prev) {
    prev->widget()->setFocus();
  } else if (next) {
    next->widget()->setFocus();
  }
}
