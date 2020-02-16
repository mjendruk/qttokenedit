#pragma once

#include <token-edit/TokenDropTarget.h>

#include <QtCore/QEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <token-edit/AbstractTokenDragDropHandler.h>
#include <token-edit/DropIndicator.h>

namespace mjendruk {

template <typename T>
TokenDropTarget<T>::TokenDropTarget(AbstractTokenDragDropHandler* handler,
                                    QWidget* parent)
    : T{parent}, _handler{handler}, _drawIndicator{false} {
  this->setAcceptDrops(true);
}

template <typename T>
void TokenDropTarget<T>::dragEnterEvent(QDragEnterEvent* event) {
  if (acceptsDrop(event)) {
    event->acceptProposedAction();
    event->accept(this->rect());
    showIndicator();
  } else {
    event->ignore(this->rect());
  }
}

template <typename T>
void TokenDropTarget<T>::dragMoveEvent(QDragMoveEvent* event) {
  if (acceptsDrop(event)) {
    event->acceptProposedAction();
    event->accept(this->rect());
    showIndicator();
  } else {
    event->ignore(this->rect());
  }
}

template <typename T>
void TokenDropTarget<T>::dragLeaveEvent(QDragLeaveEvent* event) {
  Q_UNUSED(event);
  hideIndicator();
}

template <typename T>
void TokenDropTarget<T>::dropEvent(QDropEvent* event) {
  if (acceptsDrop(event)) {
    event->acceptProposedAction();
    _handler->dropMimeData(-1, event->mimeData(), event->source());
    hideIndicator();
  }
}

template <typename T>
bool TokenDropTarget<T>::acceptsDrop(QDropEvent* event) const {
  Q_ASSERT(_handler);
  return _handler->canDropMimeData(-1, event->mimeData(), event->source());
}

template <typename T>
void TokenDropTarget<T>::drawIndicator(QPainter* painter) const {
  auto brush = this->palette().brush(QPalette::ButtonText);
  auto rect = this->rect();

  auto position = _drawIndicator ? DropIndicatorPosition::Before
                                 : DropIndicatorPosition::None;

  DropIndicator{}.draw(position, rect, brush, painter);
}

template <typename T>
void TokenDropTarget<T>::showIndicator() {
  _drawIndicator = true;
  this->update();
}
template <typename T>
void TokenDropTarget<T>::hideIndicator() {
  _drawIndicator = false;
  this->update();
}

}  // namespace mjendruk
