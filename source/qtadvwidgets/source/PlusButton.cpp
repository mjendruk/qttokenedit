#include <qtadvwidgets/PlusButton.h>

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

PlusButton::PlusButton(QWidget* parent)
    : PlusButton{QPalette::Text, parent} {}

PlusButton::PlusButton(QPalette::ColorRole colorRole,
                       QWidget* parent)
    : AbstractTokenButton{colorRole, parent} {
  setPath(createPath(this->extent()));
}
QPainterPath PlusButton::createPath(int extent) const {
  auto const rect = QRectF{QPointF{0.0, 0.0}, QSizeF(extent, extent)};

  auto crossPath = QPainterPath{};
  crossPath.setFillRule(Qt::WindingFill);

  auto const crossMargin = std::round(rect.height() * 0.2);
  auto const crossLength = rect.height() - (2.0 * crossMargin);
  auto const crossLineWidth = std::round(rect.height() * 0.1);
  crossPath.addRect(rect.center().x() - (crossLineWidth * 0.5), crossMargin,
                    crossLineWidth, crossLength);
  crossPath.addRect(crossMargin, rect.center().y() - (crossLineWidth * 0.5),
                    crossLength, crossLineWidth);

  return crossPath;
}
