#include <qtadvwidgets/AbstractTokenButton.h>

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

AbstractTokenButton::AbstractTokenButton(QPainterPath path, QSize size,
                                         QWidget* parent)
    : AbstractTokenButton{path, size, QPalette::Text, parent} {}

AbstractTokenButton::AbstractTokenButton(QPainterPath path, QSize size,
                                         QPalette::ColorRole colorRole,
                                         QWidget* parent)
    : QAbstractButton{parent},
      _path{std::move(path)},
      _size{size},
      _colorRole{colorRole} {
  setCheckable(false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::ClickFocus);
}

QSize AbstractTokenButton::sizeHint() const { return _size; }

QPalette::ColorRole AbstractTokenButton::colorRole() const {
  return _colorRole;
}

void AbstractTokenButton::setColorRole(QPalette::ColorRole role) {
  if (_colorRole == role) {
    return;
  }

  _colorRole = role;
  update();
}

void AbstractTokenButton::draw(QPainter* painter) const {
  painter->setBrush(palette().color(_colorRole));
  painter->setPen(Qt::NoPen);

  auto const rect = QRectF{this->rect()};

  painter->drawPath(_path);
}

void AbstractTokenButton::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};

  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(clippingRect);

  draw(&painter);
}
