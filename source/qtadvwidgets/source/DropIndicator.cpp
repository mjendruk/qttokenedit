#include "DropIndicator.h"

#include <QPainter>
#include <QPaintDevice>

void DropIndicator::draw(DropIndicatorPosition position, QRect const& rect,
                         QBrush const& brush, QPainter* painter) const {
  if (position == DropIndicatorPosition::None) {
    return;
  }

  painter->save();

  auto const rectf = QRectF{rect};

#ifdef Q_OS_MACOS
  auto scaling = 1.0;
#else
  auto scaling = std::round(painter->device()->logicalDpiX() / 96.0);
#endif

  auto const width = scaling * 1.0;
  auto const height = rectf.height();
  auto const size = QSizeF{width, height};

  auto pos = QPointF{0.0, 0.0};
  if (position == DropIndicatorPosition::Before) {
    pos.setX(0.0);
  } else {
    pos.setX(rectf.width() - size.width());
  }

  painter->setBrush(brush);
  painter->setPen(Qt::NoPen);

  painter->drawRect(QRectF{pos, size});

  painter->restore();
}
