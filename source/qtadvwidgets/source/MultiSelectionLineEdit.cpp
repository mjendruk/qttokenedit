#include <qtadvwidgets/MultiSelectionLineEdit.h>

#include <QKeyEvent>

void MultiSelectionLineEdit::keyPressEvent(QKeyEvent *event)
{
  if (cursorPosition() == 0 && event->key() == Qt::Key_Backspace) {
    emit backspaceAtBeginning();
  }

  QLineEdit::keyPressEvent(event);
}
