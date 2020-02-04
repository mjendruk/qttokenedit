#include <qtadvwidgets/AbstractTokenDragDropHandler.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <QDropEvent>
#include <QKeyEvent>

TokenLineEdit::TokenLineEdit(QWidget* parent)
    : QLineEdit{parent}, _dragDropHandler{nullptr} {
  setAttribute(Qt::WA_MacShowFocusRect, 0);
  setFrame(false);

  connect(this, &QLineEdit::textChanged, this, &TokenLineEdit::onTextChanged);
}

void TokenLineEdit::setDragDropHandler(AbstractTokenDragDropHandler* handler) {
  _dragDropHandler = handler;
}

void TokenLineEdit::keyPressEvent(QKeyEvent* event) {
  if (cursorPosition() == 0 && event->key() == Qt::Key_Backspace) {
    emit backspaceAtBeginning();
  }

  QLineEdit::keyPressEvent(event);
}

QSize TokenLineEdit::sizeHint() const {
  auto height = QLineEdit::sizeHint().height();
  return QSize{_widthHint, height};
}

void TokenLineEdit::dragEnterEvent(QDragEnterEvent* event) {
  if (acceptsDrop(event)) {
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}

void TokenLineEdit::dragMoveEvent(QDragMoveEvent* event) {
  if (acceptsDrop(event)) {
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}

void TokenLineEdit::dropEvent(QDropEvent* event) {
  if (acceptsDrop(event)) {
    event->acceptProposedAction();
    _dragDropHandler->dropMimeData(-1, event->mimeData(), event->source());
  }
}

bool TokenLineEdit::acceptsDrop(QDropEvent* event) const {
  if (!_dragDropHandler) {
    return false;
  }

  return _dragDropHandler->canDropMimeData(-1, event->mimeData(),
                                           event->source());
}

void TokenLineEdit::onTextChanged(QString const& text) {
  auto textWidth = fontMetrics().boundingRect(text).width();
  _widthHint = textWidth + fontMetrics().averageCharWidth();
  updateGeometry();
}

LineEditFocusChainNavigation::LineEditFocusChainNavigation(QLineEdit* lineEdit,
                                                           QObject* parent)
    : FocusChainNavigation{parent}, _lineEdit{lineEdit} {}

bool LineEditFocusChainNavigation::focusPrevious(QKeyEvent* event) const {
  return _lineEdit->cursorPosition() == 0 && event->key() == Qt::Key_Left;
}

bool LineEditFocusChainNavigation::focusNext(QKeyEvent* event) const {
  return _lineEdit->cursorPosition() == _lineEdit->text().count() &&
         event->key() == Qt::Key_Right;
}
