#include <qtadvwidgets/Token.h>

#include <cmath>

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <qtadvwidgets/AbstractTokenDragDropHandler.h>
#include <qtadvwidgets/ElidableLabel.h>
#include <qtadvwidgets/FocusChainElement.h>
#include <qtadvwidgets/RemoveButton.h>

#include "DropIndicator.h"

Token::Token(AbstractTokenDragDropHandler* dragDropHandler, QWidget* parent)
    : Token{{}, dragDropHandler, parent} {}

Token::Token(QString const& text, AbstractTokenDragDropHandler* dragDropHandler,
             QWidget* parent)
    : BaseToken{text, parent},
      _dragDropHandler{dragDropHandler},
      _button{new RemoveButton{this}},
      _removable{true},
      _dropIndicatorPosition{DropIndicatorPosition::None} {
  setRightWidget(_button);

  setAcceptDrops(true);
  setCursor(Qt::ArrowCursor);
  setAutoFillBackground(false);

  auto sizePolicy = QSizePolicy{QSizePolicy::Preferred, QSizePolicy::Fixed};
  sizePolicy.setRetainSizeWhenHidden(true);
  setSizePolicy(sizePolicy);

  setRemovable(false);

  _button->setColorRole(QPalette::ButtonText);

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

void Token::paintEvent(QPaintEvent* event) {
  BaseToken::paintEvent(event);

  auto painter = QPainter{this};
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

void Token::focusInEvent(QFocusEvent* event) {
  _button->setColorRole(QPalette::HighlightedText);
  BaseToken::focusInEvent(event);
}

void Token::focusOutEvent(QFocusEvent* event) {
  _button->setColorRole(QPalette::ButtonText);
  BaseToken::focusOutEvent(event);
}

void Token::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);

  if (_dragDropHandler->canDrag(this) &&
      event->buttons().testFlag(Qt::LeftButton)) {
    _mousePressedAt = event->pos();
  }
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

QPixmap Token::toPixmap() {
  auto pixmap = QPixmap(size() * devicePixelRatio());
  pixmap.setDevicePixelRatio(devicePixelRatio());
  pixmap.fill(QColor{Qt::transparent});

  render(&pixmap, QPoint{}, QRegion{}, QWidget::DrawChildren);

  return pixmap;
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
  auto drag = new QDrag{this};
  drag->setHotSpot(mousePos);
  drag->setPixmap(toPixmap());
  drag->setMimeData(_dragDropHandler->mimeData(this));

  if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
    auto success = _dragDropHandler->dropAccepted(this);
    Q_ASSERT(success);
  }

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
