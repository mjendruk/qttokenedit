#include <qtadvwidgets/ElidableLabel.h>
#include <qtadvwidgets/FocusChainElement.h>
#include <qtadvwidgets/RemoveButton.h>
#include <qtadvwidgets/Token.h>

#include <QDrag>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <cmath>

Token::Token(AbstractTokenDragDropHandler* dragDropHandler, QWidget* parent)
    : Token{{}, dragDropHandler, parent} {}

Token::Token(QString const& text, AbstractTokenDragDropHandler* dragDropHandler,
             QWidget* parent)
    : BaseToken{text, parent},
      _dragDropHandler{dragDropHandler},
      _button{new RemoveButton{this}},
      _removable{true},
      _dropIndicator{DropIndicator::None} {
  setRightWidget(_button);

  setAcceptDrops(true);
  setCursor(Qt::ArrowCursor);

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
  pixmap.fill(QColor{0, 0, 0, 0});

  auto painter = QPainter{&pixmap};
  painter.setRenderHint(QPainter::Antialiasing, true);

  render(&painter);

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
  
  return _dragDropHandler->canDropMimeData(this, event->mimeData(),
                                           event->source(),
                                           dropHint(event->pos()));
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
  _dropIndicator = dropHint(mousePos) == TokenDropHint::Before
                       ? DropIndicator::Before
                       : DropIndicator::After;

  update();
}

void Token::resetDropIndicator() {
  _dropIndicator = DropIndicator::None;
  update();
}

void Token::drawDropIndicator(QPainter* painter) {
  if (_dropIndicator == DropIndicator::None) {
    return;
  }

  painter->save();

  auto const rect = QRectF{this->rect()};

#ifdef Q_OS_MACOS
  auto scaling = 1.0;
#else
  auto scaling = std::round(logicalDpiX() / 96.0);
#endif

  auto const width = scaling * 1.0;
  auto const height = rect.height();
  auto const size = QSizeF{width, height};

  auto pos = QPointF{0.0, 0.0};
  if (_dropIndicator == DropIndicator::Before) {
    pos.setX(0.0);
  } else {
    pos.setX(rect.width() - size.width());
  }

  painter->setBrush(palette().brush(QPalette::ButtonText));
  painter->setPen(Qt::NoPen);

  painter->drawRect(QRectF{pos, size});

  painter->restore();
}

int Token::dragStartDistance() const {
  return fontMetrics().averageCharWidth();
}

TokenDropHint Token::dropHint(QPoint const& mousePos) const {
  return mousePos.x() < width() / 2.0 ? TokenDropHint::Before : TokenDropHint::After;
}
