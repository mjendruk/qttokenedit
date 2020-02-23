#include "FinalWidgetPlaceholder.h"

#include <QtCore/QScopedPointer>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenDropTarget.hpp>

namespace mjendruk {

FinalWidgetPlaceholder::FinalWidgetPlaceholder(
    AbstractTokenDragDropHandler* handler, QWidget* parent)
    : TokenDropTarget<QWidget>{handler, parent} {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMinimumWidth(1);
}

void FinalWidgetPlaceholder::paintEvent(QPaintEvent* event) {
  TokenDropTarget<QWidget>::paintEvent(event);

  QPainter painter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  drawIndicator(&painter);
}

}  // namespace mjendruk
