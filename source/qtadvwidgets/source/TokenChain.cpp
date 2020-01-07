#include <qtadvwidgets/LineEditChainElement.h>
#include <qtadvwidgets/Token.h>
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

void TokenChain::insert(int index, TokenChainElement* element) {
  insert(index, element, true);
}

std::string toStdString(TokenChainElement* element) {
  if (auto token = dynamic_cast<Token*>(element->widget())) {
    return token->text().toStdString();
  } else {
    return "";
  }
}

void TokenChain::move(int from, int to) {
  if (from == to) {
    return;
  }

  auto element = takeAt(from, false);
  insert(to, element, false);

  element->widget()->setFocus();
}

void TokenChain::remove(TokenChainElement* element) { remove(element, true); }

void TokenChain::setMode(TokenEditMode mode) { _mode = mode; }

void TokenChain::insert(int index, TokenChainElement* element, bool _connect) {
  Q_ASSERT(0 <= index &&
           index <= indexLast());  // last element is always the line edit
  Q_ASSERT(element);

  auto next = at(index);
  auto prev = next->previousElement();

  if (prev) {
    prev->setNextElement(element);
  }

  next->setPreviousElement(element);

  element->setPreviousElement(prev);
  element->setNextElement(next);

  ++_size;

  if (_connect) {
    connect(element, &TokenChainElement::gotFocus, this, &TokenChain::gotFocus);
    connect(element, &TokenChainElement::lostFocus, this,
            &TokenChain::lostFocus);
  }

  if (_mode == TokenEditMode::Single) {
    element->widget()->setFocus();
  }
}

void TokenChain::remove(TokenChainElement* element, bool disconnect) {
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

  if (disconnect) {
    element->disconnect(this);
  }
}

TokenChainElement* TokenChain::takeAt(int index, bool disconnect) {
  auto element = at(index);
  remove(element, disconnect);
  return element;
}

int TokenChain::indexLast() const { return _size - 1; }

TokenChainElement* TokenChain::at(int index) const {
  Q_ASSERT(0 <= index && index <= indexLast());

  auto element = _last.get();

  auto reverseIndex = indexLast() - index;

  while (reverseIndex--) {
    element = element->previousElement();
  }

  return element;
}
