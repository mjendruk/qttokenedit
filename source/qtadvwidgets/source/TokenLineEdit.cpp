#include <qtadvwidgets/TokenLineEdit.h>

#include <QKeyEvent>

TokenLineEdit::TokenLineEdit(QWidget* parent) {
  setAttribute(Qt::WA_MacShowFocusRect, 0);
}

void TokenLineEdit::keyPressEvent(QKeyEvent* event) {
  if (cursorPosition() == 0 && event->key() == Qt::Key_Backspace) {
    emit backspaceAtBeginning();
  }

  QLineEdit::keyPressEvent(event);
}
