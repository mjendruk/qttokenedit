#include "FocusChain.h"

#include <algorithm>

#include <QtGlobal>
#include <QtWidgets/QWidget>

#include <token-edit/FocusChainNavigation.h>
#include <token-edit/Token.h>

#include "FocusChainElement.h"

namespace mjendruk {

FocusChain::FocusChain(QObject* parent) : QObject{parent} {}

FocusChain::~FocusChain() = default;

void FocusChain::add(QWidget* widget, UpdateFocus updateFocus,
                     FocusChainNavigation* navigation) {
  insert(count(), widget, updateFocus, navigation);
}

void FocusChain::insert(int index, QWidget* widget, UpdateFocus updateFocus,
                        FocusChainNavigation* navigation) {
  Q_ASSERT(widget);

  if (!navigation) {
    navigation = new DefaultFocusChainNavigation{};
  }

  if (updateFocus == UpdateFocus::Yes) {
    widget->setFocus();
  }

  auto element = new FocusChainElement{widget, navigation, this};
  insert(index, element, UpdateSignalConnection::Yes);
}

void FocusChain::move(int from, int to) {
  if (from == to) {
    return;
  }

  auto element = takeAt(from, UpdateSignalConnection::No, UpdateFocus::No);
  insert(to, element, UpdateSignalConnection::No);
}

void FocusChain::remove(QWidget* widget, UpdateFocus updateFocus) {
  auto index = indexOf(widget);
  Q_ASSERT(index >= 0);
  auto element = takeAt(index, UpdateSignalConnection::Yes, updateFocus);
  delete element;
}

int FocusChain::count() const { return _elements.count(); }

bool FocusChain::isEmpty() const { return count() == 0; }

void FocusChain::insert(int index, FocusChainElement* element,
                        UpdateSignalConnection updateConnection) {
  Q_ASSERT(0 <= index && index <= count());

  if (updateConnection == UpdateSignalConnection::Yes) {
    connect(element, &FocusChainElement::gotFocus, this, &FocusChain::gotFocus);
    connect(element, &FocusChainElement::lostFocus, this,
            &FocusChain::lostFocus);
  }

  if (isEmpty()) {
    _elements.append(element);
    return;
  }

  auto next = static_cast<FocusChainElement*>(nullptr);
  auto prev = static_cast<FocusChainElement*>(nullptr);

  if (index == count()) {
    prev = _elements.at(index - 1);
    Q_ASSERT(!prev->nextElement());
  } else {
    next = _elements.at(index);
    prev = next->previousElement();
  }

  if (next) {
    next->setPreviousElement(element);
  }

  if (prev) {
    prev->setNextElement(element);
  }

  element->setPreviousElement(prev);
  element->setNextElement(next);

  _elements.insert(index, element);
}

FocusChainElement* FocusChain::takeAt(int index,
                                      UpdateSignalConnection updateConnection,
                                      UpdateFocus updateFocus) {
  Q_ASSERT(0 <= index && index < count());

  auto element = _elements.takeAt(index);

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

  if (updateConnection == UpdateSignalConnection::Yes) {
    element->disconnect(this);
  }

  return element;
}

int FocusChain::indexOf(QWidget* widget) const {
  auto isEqualWidget = [=](auto element) {
    return element->widget() == widget;
  };

  auto it = std::find_if(_elements.cbegin(), _elements.cend(), isEqualWidget);

  if (it == _elements.cend()) return -1;

  return std::distance(_elements.cbegin(), it);
}

}  // namespace mjendruk
