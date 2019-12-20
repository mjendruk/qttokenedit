#include <qtadvwidgets/TokenLineEdit.h>

#include <QKeyEvent>

void TokenLineEdit::keyPressEvent(QKeyEvent *event)
{
  if (cursorPosition() == 0 && event->key() == Qt::Key_Backspace) {
    emit backspaceAtBeginning();
  }

  QLineEdit::keyPressEvent(event);
}
