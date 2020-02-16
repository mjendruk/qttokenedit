#include <token-edit/TokenLineEdit.h>

#include <QtGui/QDropEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPaintEvent>

#include <token-edit/AbstractTokenDragDropHandler.h>

#include <token-edit/TokenDropTarget.hpp>

namespace mjendruk {

TokenLineEdit::TokenLineEdit(AbstractTokenDragDropHandler* handler,
                             QWidget* parent)
    : TokenDropTarget<QLineEdit>{handler, parent}, _widthHint{0} {
  setAttribute(Qt::WA_MacShowFocusRect, 0);
  setFrame(false);
  setMinimumWidth(1);

  connect(this, &QLineEdit::textChanged, this, &TokenLineEdit::onTextChanged);
  updateGeometry();
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
  return QSize{_widthHint + fontMetrics().averageCharWidth(), height};
}

void TokenLineEdit::onTextChanged(QString const& text) {
  _widthHint = fontMetrics().boundingRect(text).width();
  updateGeometry();
}

}  // namespace mjendruk
