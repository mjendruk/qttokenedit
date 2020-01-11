#include <qtadvwidgets/RemoveButton.h>

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

RemoveButton::RemoveButton(int diameter, QWidget* parent)
    : RemoveButton{QPalette::Text, diameter, parent} {}

RemoveButton::RemoveButton(QPalette::ColorRole colorRole, int diameter,
                           QWidget* parent)
    : QAbstractButton{parent}, _colorRole(colorRole), _diameter{diameter} {
  setCheckable(false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::ClickFocus);
  initSize();
  initButtonShapePath();
}

QSize RemoveButton::sizeHint() const { return _size; }

QPalette::ColorRole RemoveButton::colorRole() const { return _colorRole; }

void RemoveButton::setColorRole(QPalette::ColorRole role) {
  if (_colorRole == role) {
    return;
  }

  _colorRole = role;
  update();
}

void RemoveButton::draw(QPainter* painter) const {
  painter->setBrush(palette().color(_colorRole));
  painter->setPen(Qt::NoPen);

  auto const rect = QRectF{this->rect()};

  painter->translate(+rect.center());
  painter->rotate(45.0);
  painter->translate(-rect.center());

  painter->drawPath(_path);
}

void RemoveButton::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};

  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(clippingRect);

  draw(&painter);
}

void RemoveButton::initSize() { _size = QSize{_diameter, _diameter}; }

void RemoveButton::initButtonShapePath() {
  auto const rect = QRectF{QPointF{0.0, 0.0}, QSizeF{_size}};

  auto crossPath = QPainterPath{};
  crossPath.setFillRule(Qt::WindingFill);

  auto const crossMargin = std::round(rect.height() * 0.2);
  auto const crossLength = rect.height() - (2.0 * crossMargin);
  auto const crossLineWidth = rect.height() * 0.1;
  crossPath.addRect(rect.center().x() - (crossLineWidth * 0.5), crossMargin,
                    crossLineWidth, crossLength);
  crossPath.addRect(crossMargin, rect.center().y() - (crossLineWidth * 0.5),
                    crossLength, crossLineWidth);

  _path = crossPath;
}
