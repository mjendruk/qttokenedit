#pragma once

#include <qtadvwidgets/Global.h>

class QObject;
class QMimeData;
class Token;

class AbstractTokenDragDropHandler {
 public:
  virtual ~AbstractTokenDragDropHandler() = default;

  virtual bool canDrag(Token const* source) const = 0;

  virtual QMimeData* mimeData(Token const* source) const = 0;

  virtual bool dropAccepted(Token* source) = 0;

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