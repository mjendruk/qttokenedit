#pragma once

#include <token-edit/Global.h>

class QObject;
class QMimeData;
class QPoint;

namespace mjendruk {

class Token;

class AbstractTokenDragDropHandler {
 public:
  virtual ~AbstractTokenDragDropHandler() = default;

  virtual bool canDrag(Token const* source) const = 0;
  
  virtual bool execDrag(Token* source, QPoint const& mousePos) = 0;

  virtual bool canDropMimeData(Token const* target, QMimeData const* data,
                               QObject* source,
                               TokenDropHint dropHint) const = 0;

  virtual bool canDropMimeData(int row, QMimeData const* data,
                               QObject* source) const = 0;

  virtual bool dropMimeData(Token const* target, QMimeData const* data,
                            QObject* source, TokenDropHint dropHint) = 0;

  virtual bool dropMimeData(int row, QMimeData const* data,
                            QObject* source) = 0;
};

}  // namespace mjendruk
