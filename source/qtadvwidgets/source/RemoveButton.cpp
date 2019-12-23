#include <qtadvwidgets/RemoveButton.h>

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

RemoveButton::RemoveButton(int diameter, QWidget *parent) : QAbstractButton{parent}, _diameter{diameter} {
  setCheckable(false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
  
  initSize();
  initButtonShapePath();
}

QSize RemoveButton::sizeHint() const { return _size; }

void RemoveButton::paintEvent(QPaintEvent *event) {
  auto const clippingRect = QRectF{event->rect()};
  auto const rect = QRectF{QPointF{0.0, 0.0}, QSizeF{_size}};
  
  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(this->palette().brush(QPalette::WindowText));
  painter.setPen(Qt::NoPen);
  painter.setClipRect(clippingRect);
  
  painter.translate(+rect.center());
  painter.rotate(45.0);
  painter.translate(-rect.center());
  
  painter.drawPath(_path);
}

void RemoveButton::initSize()
{
  _size = QSize{_diameter, _diameter};
}

void RemoveButton::initButtonShapePath()
{
  auto const rect = QRectF{QPointF{0.0, 0.0}, QSizeF{_size}};

  auto crossPath = QPainterPath{};
  crossPath.setFillRule(Qt::WindingFill);

  auto const crossMargin = rect.height() * 0.2;
  auto const crossLength = rect.height() * 0.6;
  auto const crossLineWidth = rect.height() * 0.1;
  crossPath.addRect(rect.center().x() - (crossLineWidth * 0.5), crossMargin,
                    crossLineWidth, crossLength);
  crossPath.addRect(crossMargin, rect.center().y() - (crossLineWidth * 0.5),
                    crossLength, crossLineWidth);

  _path = crossPath;
}
