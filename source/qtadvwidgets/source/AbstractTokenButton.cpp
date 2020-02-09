#include <qtadvwidgets/AbstractTokenButton.h>

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QStyle>

namespace mjendruk {

AbstractTokenButton::AbstractTokenButton(QWidget* parent)
    : AbstractTokenButton{QPalette::Text, parent} {}

AbstractTokenButton::AbstractTokenButton(QPalette::ColorRole colorRole,
                                         QWidget* parent)
    : QAbstractButton{parent}, _colorRole{colorRole} {
  setCheckable(false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::ClickFocus);
}

QSize AbstractTokenButton::sizeHint() const {
  auto extent = this->extent();
  return QSize{extent, extent};
}

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

void AbstractTokenButton::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};

  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(clippingRect);

  auto color = palette().color(_colorRole);
  color.setAlpha(50);

  painter.setBrush(color);
  painter.setPen(Qt::NoPen);

  painter.drawPath(_path);
}

void AbstractTokenButton::setPath(QPainterPath const& path) { _path = path; }

int AbstractTokenButton::extent() const { return fontMetrics().height(); }

}  // namespace mjendruk
