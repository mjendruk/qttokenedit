#include <qttokenedit/Token.h>

#include <cmath>

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <qttokenedit/AbstractTokenDragDropHandler.h>
#include <qttokenedit/AbstractTokenSelectionHandler.h>
#include <qttokenedit/DropIndicator.h>
#include <qttokenedit/ElidableLabel.h>

#include "RemoveButton.h"

namespace mjendruk {

Token::Token(QWidget* parent) : Token{{}, nullptr, nullptr, parent} {}

Token::Token(AbstractTokenDragDropHandler* dragDropHandler,
             AbstractTokenSelectionHandler* selectionHandler, QWidget* parent)
    : Token{{}, dragDropHandler, selectionHandler, parent} {}

Token::Token(QString const& text, AbstractTokenDragDropHandler* dragDropHandler,
             AbstractTokenSelectionHandler* selectionHandler, QWidget* parent)
    : BaseToken{text, parent},
      _dragDropHandler{dragDropHandler},
      _selectionHandler{selectionHandler},
      _button{new RemoveButton{this}},
      _removable{true},
      _selected{false},
      _dropIndicatorPosition{DropIndicatorPosition::None} {
  setRightWidget(_button);

  setAcceptDrops(true);
  setCursor(Qt::ArrowCursor);
  setAutoFillBackground(false);

  auto sizePolicy = QSizePolicy{QSizePolicy::Preferred, QSizePolicy::Fixed};
  sizePolicy.setRetainSizeWhenHidden(true);
  setSizePolicy(sizePolicy);

  setRemovable(false);

  connect(_button, &RemoveButton::clicked, this, &Token::removeClicked);
}

Token::~Token() = default;

bool Token::removable() const { return _removable; }

void Token::setRemovable(bool enable) {
  if (_removable == enable) {
    return;
  }

  _removable = enable;

  if (_removable) {
    _button->show();
  } else {
    _button->hide();
  }

  updateMargins();
}

bool Token::selected() const { return _selected; }

void Token::setSelected(bool enable) {
  _selected = enable;

  if (_selected) {
    setBackgroundRole(QPalette::Highlight);
    setForegroundRole(QPalette::HighlightedText);
  } else {
    setBackgroundRole(QPalette::Button);
    setForegroundRole(QPalette::ButtonText);
  }
}

void Token::paintEvent(QPaintEvent* event) {
  BaseToken::paintEvent(event);

  QPainter painter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  drawDropIndicator(&painter);
}

void Token::keyPressEvent(QKeyEvent* event) {
  if (removable() && event->key() == Qt::Key_Backspace) {
    emit removeClicked();
    event->accept();
    return;
  }

  QWidget::keyPressEvent(event);
}

void Token::mousePressEvent(QMouseEvent* event) {
  BaseToken::mousePressEvent(event);

  _selectionHandler->select(this, event->buttons(), event->modifiers());

  if (_dragDropHandler->canDrag(this) &&
      event->buttons().testFlag(Qt::LeftButton)) {
    _mousePressedAt = event->pos();
  }

  event->accept();
}

void Token::mouseMoveEvent(QMouseEvent* event) {
  QWidget::mouseMoveEvent(event);

  if (shouldStartDrag(event->pos())) {
    startDrag(event->pos());
  }
}

void Token::dragEnterEvent(QDragEnterEvent* event) {
  if (acceptsDrag(event)) {
    acceptDrag(event);
  } else {
    event->ignore();
  }
}

void Token::dragLeaveEvent(QDragLeaveEvent* /* event */) {
  resetDropIndicator();
}

void Token::dragMoveEvent(QDragMoveEvent* event) {
  if (acceptsDrag(event)) {
    acceptDrag(event);
  } else {
    event->ignore();
  }
}

void Token::dropEvent(QDropEvent* event) {
  if (acceptsDrag(event)) {
    finishDrag(event);
  }
}

bool Token::shouldStartDrag(QPoint const& mousePos) const {
  if (!_dragDropHandler) {
    return false;
  }

  if (!_dragDropHandler->canDrag(this)) {
    return false;
  }
  auto const mouseMovement = mousePos - _mousePressedAt;
  return mouseMovement.manhattanLength() >= dragStartDistance();
}

void Token::startDrag(QPoint const& mousePos) {
  _dragDropHandler->execDrag(this, mousePos);
  _mousePressedAt = QPoint{};
}

bool Token::acceptsDrag(QDropEvent* event) const {
  if (!_dragDropHandler) {
    return false;
  }

  if (event->source() == this) {
    return false;
  }

  return _dragDropHandler->canDropMimeData(
      this, event->mimeData(), event->source(), dropHint(event->pos()));
}

void Token::acceptDrag(QDragMoveEvent* event) {
  event->acceptProposedAction();
  showDropIndicator(event->pos());
}

void Token::finishDrag(QDropEvent* event) {
  event->acceptProposedAction();
  _dragDropHandler->dropMimeData(this, event->mimeData(), event->source(),
                                 dropHint(event->pos()));
  resetDropIndicator();
}

void Token::showDropIndicator(QPoint const& mousePos) {
  _dropIndicatorPosition = dropHint(mousePos) == TokenDropHint::Before
                               ? DropIndicatorPosition::Before
                               : DropIndicatorPosition::After;

  update();
}

void Token::resetDropIndicator() {
  _dropIndicatorPosition = DropIndicatorPosition::None;
  update();
}

void Token::drawDropIndicator(QPainter* painter) {
  auto brush = palette().brush(QPalette::ButtonText);

  DropIndicator{}.draw(_dropIndicatorPosition, rect(), brush, painter);
}

int Token::dragStartDistance() const {
  return fontMetrics().averageCharWidth();
}

TokenDropHint Token::dropHint(QPoint const& mousePos) const {
  return mousePos.x() < width() / 2.0 ? TokenDropHint::Before
                                      : TokenDropHint::After;
}

}  // namespace mjendruk
