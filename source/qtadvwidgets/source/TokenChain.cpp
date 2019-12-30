#include <qtadvwidgets/LineEditChainElement.h>
#include <qtadvwidgets/TokenChain.h>
#include <qtadvwidgets/TokenChainElement.h>

#include <QWidget>
#include <QtGlobal>

TokenChain::TokenChain(TokenEditMode mode, QLineEdit* last, QObject* parent)
    : QObject{parent},
      _last{std::make_unique<LineEditChainElement>(last)},
      _mode{mode} {
  connect(_last.get(), &TokenChainElement::gotFocus, this,
          &TokenChain::gotFocus);
  connect(_last.get(), &TokenChainElement::lostFocus, this,
          &TokenChain::lostFocus);
}

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

  connect(element, &TokenChainElement::gotFocus, this, &TokenChain::gotFocus);
  connect(element, &TokenChainElement::lostFocus, this, &TokenChain::lostFocus);

  if (_mode == TokenEditMode::Single) {
    element->widget()->setFocus();
  }
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

  element->disconnect(this);
}

void TokenChain::setMode(TokenEditMode mode) { _mode = mode; }
