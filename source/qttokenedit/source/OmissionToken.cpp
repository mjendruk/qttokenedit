#include <qttokenedit/OmissionToken.h>

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <qttokenedit/TokenDropTarget.hpp>

namespace mjendruk {

OmissionToken::OmissionToken(AbstractTokenDragDropHandler* handler,
                             QWidget* parent)
    : TokenDropTarget<BaseToken>{handler, parent}, _count{-1} {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setCount(0);
  setFocusPolicy(Qt::NoFocus);
}

int OmissionToken::count() const { return _count; }

void OmissionToken::setCount(int count) {
  if (_count == count) {
    return;
  }

  setText(QString{"+ %1 more"}.arg(count));

  _count = count;
}

void OmissionToken::paintEvent(QPaintEvent* event) {
  BaseToken::paintEvent(event);

  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  this->drawIndicator(&painter);
}

}  // namespace mjendruk
