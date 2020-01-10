#include <qtadvwidgets/RemoveButton.h>

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

RemoveButton::RemoveButton(QColor const& color, int diameter, QWidget* parent)
    : QAbstractButton{parent}, _diameter{diameter}, _color{color} {
  setCheckable(false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::NoFocus);
  initSize();
  initButtonShapePath();
}

QSize RemoveButton::sizeHint() const { return _size; }

QColor RemoveButton::color() const { return _color; }

void RemoveButton::setColor(QColor const& color) {
  _color = color;
  update();
}

void RemoveButton::draw(QPainter* painter) const
{
  painter->setBrush(QBrush{_color});
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
