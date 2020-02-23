#include <qttokenedit/AbstractTokenButton.h>

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QStyle>

namespace mjendruk {

AbstractTokenButton::AbstractTokenButton(QWidget* parent)
    : QAbstractButton{parent} {
  setCheckable(false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::ClickFocus);
}

QSize AbstractTokenButton::sizeHint() const {
  auto extent = this->extent();
  return QSize{extent, extent};
}

void AbstractTokenButton::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};

  QPainter painter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(clippingRect);

  auto color = palette().color(foregroundRole());
  color.setAlpha(50);

  painter.setBrush(color);
  painter.setPen(Qt::NoPen);

  painter.drawPath(_path);
}

void AbstractTokenButton::setPath(QPainterPath const& path) { _path = path; }

int AbstractTokenButton::extent() const { return fontMetrics().height(); }

}  // namespace mjendruk
