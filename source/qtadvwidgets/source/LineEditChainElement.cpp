#include <qtadvwidgets/LineEditChainElement.h>

#include <QEvent>
#include <QKeyEvent>

LineEditChainElement::LineEditChainElement(QLineEdit* lineEdit)
    : TokenChainElement{lineEdit, false}, _lineEdit{lineEdit} {
  lineEdit->installEventFilter(this);
}

bool LineEditChainElement::eventFilter(QObject* watched, QEvent* event) {
  Q_ASSERT(watched == _lineEdit);

  if (event->type() != QEvent::KeyPress) {
    return false;
  }

  auto keyEvent = static_cast<QKeyEvent*>(event);

  if (_lineEdit->cursorPosition() == 0 && keyEvent->key() == Qt::Key_Left) {
    if (auto prev = previousElement()) {
      prev->widget()->setFocus();
      return true;
    }
  }

  return false;
}
