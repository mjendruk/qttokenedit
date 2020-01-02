#include <qtadvwidgets/LineEditChainElement.h>
#include <qtadvwidgets/TokenChain.h>
#include <qtadvwidgets/TokenChainElement.h>

#include <QWidget>
#include <QtGlobal>

TokenChain::TokenChain(TokenEditMode mode, QLineEdit* last, QObject* parent)
    : QObject{parent},
      _last{std::make_unique<LineEditChainElement>(last)},
      _size{1},
      _mode{mode} {
  connect(_last.get(), &TokenChainElement::gotFocus, this,
          &TokenChain::gotFocus);
  connect(_last.get(), &TokenChainElement::lostFocus, this,
          &TokenChain::lostFocus);
}

TokenChain::~TokenChain() = default;

void TokenChain::add(TokenChainElement* element) {
  insert(indexLast(), element);
}

void TokenChain::insert(int index, TokenChainElement* element)
{
  Q_ASSERT(0 <= index && index < _size);  // last element is always the line edit
  Q_ASSERT(element);

  auto next = at(index);
  auto prev = next->previousElement();

  if (prev) {
    prev->setNextElement(element);
    element->setPreviousElement(prev);
  }

  next->setPreviousElement(element);
  element->setNextElement(next);

  ++_size;

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
  
   --_size;

  element->disconnect(this);
}

void TokenChain::setMode(TokenEditMode mode) { _mode = mode; }

int TokenChain::indexLast() const { return _size - 1; }

TokenChainElement* TokenChain::at(int index) const {
  auto element = _last.get();

  for (auto currentIndex = indexLast(); currentIndex > index; --currentIndex) {
    element = element->previousElement();
  }

  return element;
}
