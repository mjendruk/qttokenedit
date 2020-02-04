#include "TokenDragDropHandler.h"

#include <qtadvwidgets/TokenEdit.h>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QScopedValueRollback>

TokenDragDropHandler::TokenDragDropHandler(TokenEdit* tokenEdit)
    : _tokenEdit{tokenEdit} {}

bool TokenDragDropHandler::canDrag(Token const* source) const {
  if (!_tokenEdit->dragEnabled()) {
    return false;
  }

  if (auto mode = _tokenEdit->dragDropMode();
      mode == QAbstractItemView::NoDragDrop ||
      mode == QAbstractItemView::DropOnly) {
    return false;
  }

  if (!model()->supportedDragActions().testFlag(Qt::MoveAction)) {
    return false;
  }

  auto index = _tokenEdit->index(source);

  if (!model()->flags(index).testFlag(Qt::ItemIsDragEnabled)) {
    return false;
  }

  return true;
}

QMimeData* TokenDragDropHandler::mimeData(const Token* source) const {
  auto index = _tokenEdit->index(source);
  return model()->mimeData({index});
}

bool TokenDragDropHandler::canDropMimeData(const Token* target,
                                           const QMimeData* data,
                                           QObject* source,
                                           TokenDropHint dropHint) const {
  auto mode = _tokenEdit->dragDropMode();

  if (mode == QAbstractItemView::NoDragDrop ||
      mode == QAbstractItemView::DragOnly) {
    return false;
  }

  if (auto token = qobject_cast<Token*>(source);
      mode == QAbstractItemView::InternalMove &&
      !(token && _tokenEdit->indexOf(token) != -1)) {
    return false;
  }

  if (!model()->supportedDropActions().testFlag(Qt::MoveAction)) {
    return false;
  }

  auto targetRow = _tokenEdit->indexOf(target);

  Q_ASSERT(targetRow >= 0);

  if (dropHint == TokenDropHint::After) {
    ++targetRow;
  }

  return model()->canDropMimeData(data, Qt::MoveAction, targetRow,
                                  _tokenEdit->modelColumn(),
                                  _tokenEdit->rootIndex());
}

bool TokenDragDropHandler::dropMimeData(const Token* target,
                                        const QMimeData* data, QObject* source,
                                        TokenDropHint dropHint) {
  Q_UNUSED(source);
  Q_ASSERT(canDropMimeData(target, data, source, dropHint));

  auto targetRow = _tokenEdit->indexOf(target);

  Q_ASSERT(targetRow >= 0);

  if (dropHint == TokenDropHint::After) {
    ++targetRow;
  }

  auto updateFocusEnabled = _tokenEdit->enableUpdateFocus();
  return model()->dropMimeData(data, Qt::MoveAction, targetRow,
                               _tokenEdit->modelColumn(),
                               _tokenEdit->rootIndex());
}

bool TokenDragDropHandler::dropAccepted(Token* token) {
  return _tokenEdit->remove(_tokenEdit->indexOf(token), UpdateFocus::No);
}

QAbstractItemModel* TokenDragDropHandler::model() const {
  return _tokenEdit->model();
}
