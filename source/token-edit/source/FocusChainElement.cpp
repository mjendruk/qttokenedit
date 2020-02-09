#include <token-edit/FocusChainElement.h>

#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

#include <token-edit/FocusChainNavigation.h>

namespace mjendruk {

FocusChainElement::FocusChainElement(QWidget* widget,
                                     FocusChainNavigation* navigation,
                                     QObject* parent)
    : QObject{parent},
      _previousElement{nullptr},
      _nextElement{nullptr},
      _widget{widget},
      _navigation{navigation} {
  Q_ASSERT(widget);
  Q_ASSERT(navigation);

  if (navigation->parent() == nullptr) {
    navigation->setParent(this);
  }
  widget->installEventFilter(this);
}

FocusChainElement* FocusChainElement::previousElement() const {
  return _previousElement;
}

void FocusChainElement::setPreviousElement(FocusChainElement* element) {
  _previousElement = element;
}

FocusChainElement* FocusChainElement::nextElement() const {
  return _nextElement;
}

void FocusChainElement::setNextElement(FocusChainElement* element) {
  _nextElement = element;
}

QWidget* FocusChainElement::widget() const { return _widget; }

bool FocusChainElement::eventFilter(QObject* watched, QEvent* event) {
  Q_ASSERT(watched == _widget);

  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent*>(event);

    if (_navigation->focusPrevious(keyEvent)) {
      if (auto prev = previousElement()) {
        prev->widget()->setFocus();
        return true;
      }
    }

    if (_navigation->focusNext(keyEvent)) {
      if (auto next = nextElement()) {
        next->widget()->setFocus();
        return true;
      }
    }
  } else if (event->type() == QEvent::FocusIn) {
    emit gotFocus(_widget);
  } else if (event->type() == QEvent::FocusOut) {
    emit lostFocus(_widget);
  }

  return false;
}

}  // namespace mjendruk
