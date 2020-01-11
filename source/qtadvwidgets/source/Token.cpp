#include <qtadvwidgets/ElidableLabel.h>
#include <qtadvwidgets/RemoveButton.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenChainElement.h>
#include <qtadvwidgets/TokenMimeData.h>

#include <QBoxLayout>
#include <QDrag>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QToolButton>
#include <cmath>

Token::Token(QString const& text, QWidget* parent)
    : QWidget{parent},
      _chainElement{std::make_unique<TokenChainElement>(this)},
      _layout{new QBoxLayout{QBoxLayout::LeftToRight, this}},
      _label{new ElidableLabel{text, this}},
      _button{new RemoveButton{_label->sizeHint().height(), this}},
      _removable{true},
      _dragEnabled{false},
      _dropIndicator{DropIndicator::None} {
  _layout->addWidget(_label);
  _layout->addWidget(_button);
  _layout->setSpacing(spacing());
  _layout->setContentsMargins(horizontalTextMargin(), margin(), margin(),
                              margin());

  setAcceptDrops(true);
  setCursor(Qt::ArrowCursor);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setFocusPolicy(Qt::ClickFocus);
        
  setRemovable(false);

  connect(_button, &RemoveButton::clicked, this, &Token::removeClicked);
}

Token::~Token() = default;

QString const& Token::text() const { return _label->text(); }

void Token::setText(QString const& text) { _label->setText(text); }

bool Token::dragEnabled() const { return _dragEnabled; }

void Token::setDragEnabled(bool enable) { _dragEnabled = enable; }

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
  
  auto margins = _layout->contentsMargins();
  margins.setRight(_removable ? margin() : horizontalTextMargin());
  _layout->setContentsMargins(margins);
}

TokenChainElement* Token::chainElement() const { return _chainElement.get(); }

QPixmap Token::toPixmap() const {
  auto pixmap = QPixmap(size() * devicePixelRatio());
  pixmap.setDevicePixelRatio(devicePixelRatio());
  pixmap.fill(QColor{0, 0, 0, 0});

  auto painter = QPainter{&pixmap};
  painter.setRenderHint(QPainter::Antialiasing, true);

  drawBackground(&painter, palette().brush(QPalette::Highlight));

  painter.save();
  painter.translate(_label->pos());
  _label->draw(&painter);
  painter.restore();

  painter.save();
  painter.translate(_button->pos());
  _button->draw(&painter);
  painter.restore();

  return pixmap;
}

int Token::contentHeight() const { return _label->sizeHint().height(); }

int Token::horizontalTextMargin() const { return margin() * 2; }

int Token::margin() const { return std::lround(contentHeight() / 6.0); }

int Token::spacing() const { return std::lround(contentHeight() * 0.25); }

void Token::paintEvent(QPaintEvent* event) {
  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  auto brushRole = QPalette::Button;

  if (isEnabled() && underMouse() && _dropIndicator == DropIndicator::None) {
    brushRole = QPalette::Midlight;
  }

  if (hasFocus()) {
    brushRole = QPalette::Highlight;
  }

  drawBackground(&painter, palette().brush(brushRole));

  drawDropIndicator(&painter);

  // auto const penRole =
  //     hasFocus() ? QPalette::HighlightedText : QPalette::ButtonText;

  // drawText(&painter, palette().color(penRole));

  QWidget::paintEvent(event);
}

void Token::resizeEvent(QResizeEvent* event) { QWidget::resizeEvent(event); }

void Token::keyPressEvent(QKeyEvent* event) {
  if (removable() && event->key() == Qt::Key_Backspace) {
    emit removeClicked();
    event->accept();
    return;
  }

  QWidget::keyPressEvent(event);
}

void Token::focusInEvent(QFocusEvent* event) {
  _label->setTextColorRole(QPalette::HighlightedText);
  _button->setColorRole(QPalette::HighlightedText);
  QWidget::focusInEvent(event);
}

void Token::focusOutEvent(QFocusEvent* event) {
  _label->setTextColorRole(QPalette::ButtonText);
  _button->setColorRole(QPalette::ButtonText);
  QWidget::focusOutEvent(event);
}

void Token::leaveEvent(QEvent* event) {
  QWidget::leaveEvent(event);
  if (isEnabled()) {
    update();
  }
}

void Token::enterEvent(QEvent* event) {
  QWidget::enterEvent(event);
  if (isEnabled()) {
    update();
  }
}

void Token::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);

  if (dragEnabled() && event->buttons().testFlag(Qt::LeftButton)) {
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

void Token::dragLeaveEvent(QDragLeaveEvent* event) { resetDropIndicator(); }

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

void Token::drawBackground(QPainter* painter, QBrush brush) const {
  painter->save();

  auto rect = QRectF{QPointF{0.0, 0.0}, QSizeF{size()}};

  auto const rounding = contentHeight() / 8.0;

  painter->setBrush(brush);
  painter->setPen(Qt::NoPen);
  painter->drawRoundedRect(rect, rounding, rounding);

  painter->restore();
}

bool Token::shouldStartDrag(QPoint const& mousePos) const {
  if (!dragEnabled()) {
    return false;
  }
  auto const mouseMovement = mousePos - _mousePressedAt;
  return mouseMovement.manhattanLength() >= dragStartDistance();
}

void Token::startDrag(QPoint const& mousePos) {
  auto mimeData = new TokenMimeData{this};

  auto drag = new QDrag{this};
  drag->setHotSpot(mousePos);
  drag->setPixmap(toPixmap());
  drag->setMimeData(mimeData);

  drag->exec(Qt::MoveAction);

  _mousePressedAt = QPoint{};
}

bool Token::acceptsDrag(QDropEvent* event) const {
  auto const token = qobject_cast<Token*>(event->source());

  if (!token) {
    return false;
  }

  if (token == this) {
    return false;
  }

  if (token->parentWidget() != this->parentWidget()) {
    return false;
  }

  return true;
}

void Token::acceptDrag(QDragMoveEvent* event) {
  event->acceptProposedAction();
  showDropIndicator(event->pos());
}

void Token::finishDrag(QDropEvent* event) {
  event->acceptProposedAction();

  auto const token =
      qobject_cast<TokenMimeData const*>(event->mimeData())->token();

  emit token->dragged(this, dropHint(event->pos()));

  resetDropIndicator();
}

void Token::showDropIndicator(QPoint const& mousePos) {
  _dropIndicator = dropHint(mousePos) == DropHint::Before
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

auto Token::dropHint(QPoint const& mousePos) const -> DropHint {
  return mousePos.x() < width() / 2.0 ? DropHint::Before : DropHint::After;
}
