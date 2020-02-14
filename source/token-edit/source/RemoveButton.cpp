#include "RemoveButton.h"

#include <cmath>

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

namespace mjendruk {

RemoveButton::RemoveButton(QWidget* parent)
    : RemoveButton{QPalette::Text, parent} {}

RemoveButton::RemoveButton(QPalette::ColorRole colorRole, QWidget* parent)
    : AbstractTokenButton{colorRole, parent} {
  setFocusPolicy(Qt::NoFocus);
  setPath(createPath(this->extent()));
}

QPainterPath RemoveButton::createPath(int extent) const {
  auto const rect = QRectF{QPointF{0.0, 0.0}, QSizeF(extent, extent)};

  auto crossPath = QPainterPath{};
  crossPath.setFillRule(Qt::WindingFill);

  auto const crossMargin = std::round(rect.height() * 0.2);
  auto const crossLength = rect.height() - (2.0 * crossMargin);
  auto const crossLineWidth = std::round(rect.height() * 0.1);
  auto const rounding = crossLineWidth * 0.5;
  crossPath.addRoundedRect(rect.center().x() - (crossLineWidth * 0.5),
                           crossMargin, crossLineWidth, crossLength, rounding,
                           rounding);
  crossPath.addRoundedRect(crossMargin,
                           rect.center().y() - (crossLineWidth * 0.5),
                           crossLength, crossLineWidth, rounding, rounding);

  auto const center = rect.center();

  auto transform = QTransform{};
  transform.translate(center.x(), center.y());
  transform.rotate(45.0);
  transform.translate(-center.x(), -center.y());

  return transform.map(crossPath);
}

}  // namespace mjendruk
