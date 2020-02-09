#include <token-edit/TokenLineEdit.h>

#include <QtGui/QDropEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPaintEvent>

#include <token-edit/AbstractTokenDragDropHandler.h>

#include "TokenDropTarget.hpp"

namespace mjendruk {

TokenLineEdit::TokenLineEdit(AbstractTokenDragDropHandler* handler,
                             QWidget* parent)
    : TokenDropTarget<QLineEdit>{handler, parent} {
  setAttribute(Qt::WA_MacShowFocusRect, 0);
  setFrame(false);
  setMinimumWidth(1);

  connect(this, &QLineEdit::textChanged, this, &TokenLineEdit::onTextChanged);
}

void TokenLineEdit::keyPressEvent(QKeyEvent* event) {
  if (cursorPosition() == 0 && event->key() == Qt::Key_Backspace) {
    emit backspaceAtBeginning();
  }

  QLineEdit::keyPressEvent(event);
}

void TokenLineEdit::paintEvent(QPaintEvent* event) {
  QLineEdit::paintEvent(event);

  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  this->drawIndicator(&painter);
}

QSize TokenLineEdit::sizeHint() const {
  auto height = QLineEdit::sizeHint().height();
  return QSize{_widthHint, height};
}

void TokenLineEdit::onTextChanged(QString const& text) {
  auto textWidth = fontMetrics().boundingRect(text).width();
  _widthHint = textWidth + fontMetrics().averageCharWidth();
  updateGeometry();
}

LineEditFocusChainNavigation::LineEditFocusChainNavigation(QLineEdit* lineEdit)
    : FocusChainNavigation{lineEdit}, _lineEdit{lineEdit} {}

bool LineEditFocusChainNavigation::focusPrevious(QKeyEvent* event) const {
  return _lineEdit->cursorPosition() == 0 && event->key() == Qt::Key_Left;
}

bool LineEditFocusChainNavigation::focusNext(QKeyEvent* event) const {
  return _lineEdit->cursorPosition() == _lineEdit->text().count() &&
         event->key() == Qt::Key_Right;
}

}  // namespace mjendruk
