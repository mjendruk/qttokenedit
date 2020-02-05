#include "FinalWidgetPlaceholder.h"

#include <qtadvwidgets/Token.h>

#include <QPaintEvent>
#include <QPainter>

#include "TokenDropTarget.hpp"

FinalWidgetPlaceholder::FinalWidgetPlaceholder(
    AbstractTokenDragDropHandler* handler, QWidget* parent)
    : TokenDropTarget<QWidget>{handler, parent} {
  auto dummyToken = QScopedPointer{new Token{}};
  setFixedHeight(dummyToken->sizeHint().height());
  setMinimumWidth(1);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void FinalWidgetPlaceholder::paintEvent(QPaintEvent* event) {
  QWidget::paintEvent(event);

  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  this->drawIndicator(&painter);
}
