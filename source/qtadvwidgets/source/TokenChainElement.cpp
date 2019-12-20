#include <qtadvwidgets/TokenChainElement.h>

#include <QEvent>
#include <QKeyEvent>

TokenChainElement::TokenChainElement(QWidget* widget, bool isToken) : _widget{widget} {
  if (isToken) {
    widget->installEventFilter(this);
  }
}

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

bool TokenChainElement::eventFilter(QObject *watched, QEvent *event)
{
  Q_ASSERT(watched == _widget);

  if (event->type() != QEvent::KeyPress) {
    return false;
  }

  auto keyEvent = static_cast<QKeyEvent*>(event);

  if (keyEvent->key() == Qt::Key_Left) {
    if (auto prev = previousElement()) {
      prev->widget()->setFocus();
      return true;
    }
  }

  if (keyEvent->key() == Qt::Key_Right) {
    if (auto next = nextElement()) {
      next->widget()->setFocus();
      return true;
    }
  }

  return false;
}
