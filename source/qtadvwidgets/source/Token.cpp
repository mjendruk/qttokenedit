#include <qtadvwidgets/RemoveButton.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenChainElement.h>
#include <qtadvwidgets/TokenMimeData.h>

#include <QBoxLayout>
#include <QDrag>
#include <QKeyEvent>
#include <QLabel>
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
      _text{text},
      _chainElement{std::make_unique<TokenChainElement>(this)},
      _dropIndicator{DropIndicator::None} {
  setAcceptDrops(true);
  setCursor(Qt::ArrowCursor);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setFocusPolicy(Qt::ClickFocus);

  _button = new RemoveButton{palette().color(QPalette::ButtonText),
                             contentHeight(), this};

  connect(_button, &RemoveButton::clicked, this, &Token::removeClicked);
}

Token::~Token() = default;

QString const& Token::text() const { return _text; }

void Token::setText(QString const& text) {
  _text = text;
  update();
}

QSize Token::sizeHint() const {
  auto const margins =
      QSize{horizontalTextMargin() + margin(), margin() + margin()};
  auto const buttonWidth = _button->sizeHint().width();

  return textSize() + margins + QSize{buttonWidth + spacing(), 0};
}

QSize Token::minimumSizeHint() const {
  auto const margins =
      QSize{horizontalTextMargin() + margin(), margin() + margin()};

  auto const buttonWidth = _button->sizeHint().width();

  auto const minimalTextWidth =
      fontMetrics().averageCharWidth() * std::min(5, _text.size());

  return margins + QSize{minimalTextWidth + buttonWidth + spacing(), 0};
}

TokenChainElement* Token::chainElement() const { return _chainElement.get(); }

QPixmap Token::toPixmap() const {
  auto pixmap = QPixmap(size() * devicePixelRatio());
  pixmap.setDevicePixelRatio(devicePixelRatio());
  pixmap.fill(QColor{0, 0, 0, 0});

  auto painter = QPainter{&pixmap};
  
  drawBackground(&painter, palette().brush(QPalette::Highlight));
  drawText(&painter, palette().color(QPalette::HighlightedText));

  return pixmap;
}

int Token::contentHeight() const { return fontMetrics().height(); }

int Token::horizontalTextMargin() const { return margin() * 2; }

int Token::margin() const { return std::lround(contentHeight() / 6.0); }

int Token::spacing() const { return std::lround(contentHeight() * 0.25); }

QSize Token::textSize() const {
  return fontMetrics().size(Qt::TextSingleLine, _text);
}

void Token::paintEvent(QPaintEvent* event) {
  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  auto brushRole = QPalette::Button;

  if (underMouse() && _dropIndicator == DropIndicator::None) {
    brushRole = QPalette::Midlight;
  }

  if (hasFocus()) {
    brushRole = QPalette::Highlight;
  }

  drawBackground(&painter, palette().brush(brushRole));
  
  drawDropIndicator(&painter);
  
  auto const penRole =
      hasFocus() ? QPalette::HighlightedText : QPalette::ButtonText;
  
  drawText(&painter, palette().color(penRole));

  QWidget::paintEvent(event);
}

void Token::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  updateElidedText(event->size());
  updateButtonPosition(event->size());
}

void Token::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Backspace) {
    emit removeClicked();
    event->accept();
    return;
  }

  QWidget::keyPressEvent(event);
}

void Token::focusInEvent(QFocusEvent* event) {
  _button->setColor(palette().color(QPalette::HighlightedText));
  QWidget::focusInEvent(event);
}

void Token::focusOutEvent(QFocusEvent* event) {
  _button->setColor(palette().color(QPalette::ButtonText));

  QWidget::focusOutEvent(event);
}

void Token::leaveEvent(QEvent* event) {
  QWidget::leaveEvent(event);
  update();
}

void Token::enterEvent(QEvent* event) {
  QWidget::enterEvent(event);
  update();
}

void Token::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);

  if (event->buttons().testFlag(Qt::LeftButton)) {
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

void Token::drawText(QPainter* painter, QPen pen) const {
  painter->save();
  painter->setPen(pen);

  auto const textPosition = QPointF(horizontalTextMargin(), margin());
  auto const textSize =
      QSizeF{QSize{elidedTextSize().width(), contentHeight()}};

  painter->drawText(QRectF{textPosition, textSize},
                   Qt::TextSingleLine | Qt::AlignLeft, _elidedText);
  painter->restore();
}

void Token::updateElidedText(QSize size) {
  auto availableWidth = size.width() - (horizontalTextMargin() + spacing() +
                                        _button->sizeHint().width() + margin());

  _elidedText = fontMetrics().elidedText(_text, Qt::ElideRight, availableWidth);
}

void Token::updateButtonPosition(QSize size) {
  auto const x = size.width() - _button->sizeHint().width() - margin();
  auto const y = margin();
  _button->move(x, y);
}

QSize Token::elidedTextSize() const {
  return fontMetrics().size(Qt::TextSingleLine, _elidedText);
}

QString Token::elidedText() const { return _elidedText; }

bool Token::shouldStartDrag(QPoint const& mousePos) const {
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

void Token::resetDropIndicator() { _dropIndicator = DropIndicator::None; update(); }

void Token::drawDropIndicator(QPainter* painter) {
  if (_dropIndicator == DropIndicator::None) {
    return;
  }
  
  painter->save();
  
  auto const rect = QRectF{this->rect()};

  auto scaling = logicalDpiX() / 72.0;
  
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
