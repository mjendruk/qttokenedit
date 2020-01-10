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
  insert(index, element, UpdateSignalConnection::Yes, UpdateFocus::Yes);
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

  auto element = takeAt(from, UpdateSignalConnection::No, UpdateFocus::No);
  insert(to, element, UpdateSignalConnection::No, UpdateFocus::No);
}

void TokenChain::remove(TokenChainElement* element) {
  remove(element, UpdateSignalConnection::Yes, UpdateFocus::Yes);
}

void TokenChain::setMode(TokenEditMode mode) { _mode = mode; }

void TokenChain::insert(int index, TokenChainElement* element,
                        UpdateSignalConnection updateConnection,
                        UpdateFocus updateFocus) {
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

  if (updateConnection == UpdateSignalConnection::Yes) {
    connect(element, &TokenChainElement::gotFocus, this, &TokenChain::gotFocus);
    connect(element, &TokenChainElement::lostFocus, this,
            &TokenChain::lostFocus);
  }

  if (updateFocus == UpdateFocus::Yes &&
      _mode == TokenEditMode::ShowLineEditIfEmpty &&
      _last->widget()->hasFocus()) {
    element->widget()->setFocus();
  }
}

void TokenChain::remove(TokenChainElement* element,
                        UpdateSignalConnection updateConnection,
                        UpdateFocus updateFocus) {
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

  if (updateFocus == UpdateFocus::Yes && element->widget()->hasFocus()) {
    if (prev) {
      prev->widget()->setFocus();
    } else if (next) {
      next->widget()->setFocus();
    }
  }

  --_size;

  if (updateConnection == UpdateSignalConnection::Yes) {
    element->disconnect(this);
  }
}

TokenChainElement* TokenChain::takeAt(int index,
                                      UpdateSignalConnection updateConnection,
                                      UpdateFocus updateFocus) {
  auto element = at(index);
  remove(element, updateConnection, updateFocus);
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
