#include "TokenDragDropHandler.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QScopedPointer>
#include <QtCore/QScopedValueRollback>
#include <QtGui/QDrag>
#include <QtWidgets/QAbstractItemView>

#include <qttokenedit/Token.h>
#include <qttokenedit/TokenEdit.h>
#include <qttokenedit/TokenEditView.h>

namespace mjendruk {

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

void TokenDragDropHandler::execDrag(Token* source, QPoint const& mousePos) {
  Q_ASSERT(canDrag(source));

  auto tokens = selectedTokens();
  Q_ASSERT((std::any_of(tokens.cbegin(), tokens.cend(),
                        [=](auto token) { return token == source; })));

  auto rect = enclosingRect(tokens);
  auto pixmap = renderPixmap(rect, tokens);

  auto hotSpot = source->mapToParent(mousePos) - rect.topLeft();

  auto indexes = selectedIndexes();
  auto mimeData = model()->mimeData(indexes);

  auto persistentIndexes = persistent(indexes);
  
  auto drag = QScopedPointer<QDrag>{new QDrag{source}};
  drag->setPixmap(pixmap);
  drag->setHotSpot(hotSpot);
  drag->setMimeData(mimeData);

  _tokenEdit->blockModeChange();

  if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
    auto success = _tokenEdit->remove(nonpersistent(persistentIndexes), UpdateFocus::No);
    Q_ASSERT(success);
  }

  _tokenEdit->unblockModeChange();
}

bool TokenDragDropHandler::canDropMimeData(const Token* target,
                                           const QMimeData* data,
                                           QObject* source,
                                           TokenDropHint dropHint) const {
  auto targetRow = _tokenEdit->indexOf(target);

  Q_ASSERT(targetRow >= 0);

  if (dropHint == TokenDropHint::After) {
    ++targetRow;
  }

  return canDropMimeData(targetRow, data, source);
}

bool TokenDragDropHandler::canDropMimeData(int row, QMimeData const* data,
                                           QObject* source) const {
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

  if (row == -1) {
    row = _tokenEdit->visibleCount();
  }

  return model()->canDropMimeData(data, Qt::MoveAction, row,
                                  _tokenEdit->modelColumn(),
                                  _tokenEdit->rootIndex());
}

bool TokenDragDropHandler::dropMimeData(const Token* target,
                                        const QMimeData* data, QObject* source,
                                        TokenDropHint dropHint) {
  auto targetRow = _tokenEdit->indexOf(target);

  Q_ASSERT(targetRow >= 0);

  if (dropHint == TokenDropHint::After) {
    ++targetRow;
  }

  return dropMimeData(targetRow, data, source);
}

bool TokenDragDropHandler::dropMimeData(int row, QMimeData const* data,
                                        QObject* source) {
  Q_UNUSED(source);
  Q_ASSERT(canDropMimeData(row, data, source));

  if (row == -1) {
    row = _tokenEdit->visibleCount();
  }

  auto updateFocusEnabled = _tokenEdit->enableUpdateFocus();
  return model()->dropMimeData(data, Qt::MoveAction, row,
                               _tokenEdit->modelColumn(),
                               _tokenEdit->rootIndex());
}

QModelIndexList TokenDragDropHandler::selectedIndexes() const {
  return _tokenEdit->selectionModel()->selectedRows(_tokenEdit->modelColumn());
}

QVector<Token*> TokenDragDropHandler::selectedTokens() const {
  auto indexes = selectedIndexes();
  auto result = QVector<Token*>(indexes.size());
  std::transform(indexes.cbegin(), indexes.cend(), result.begin(),
                 [=](auto const& i) { return _tokenEdit->at(i.row()); });
  return result;
}

QVector<QPersistentModelIndex> TokenDragDropHandler::persistent(
    QModelIndexList const& indexes) const {
  auto result = QVector<QPersistentModelIndex>{};
  for (auto const& index : indexes) {
    result.append(index);
  }
  return result;
}

QModelIndexList TokenDragDropHandler::nonpersistent(QVector<QPersistentModelIndex> const& indexes) const
{
  auto result = QModelIndexList{};
  for (auto const& index : indexes) {
    result.append(index);
  }
  return result;
}

QRect TokenDragDropHandler::enclosingRect(QVector<Token*> const& tokens) const {
  return std::accumulate(tokens.cbegin(), tokens.cend(), QRect{},
                         [](QRect const& rect, Token* token) {
                           return rect.united(token->geometry());
                         });
}

QPixmap TokenDragDropHandler::renderPixmap(QRect const& enclosingRect,
                                           QVector<Token*> tokens) const {
  auto dpr = _tokenEdit->devicePixelRatio();

  auto pixmap = QPixmap{dpr * enclosingRect.size()};
  pixmap.setDevicePixelRatio(dpr);
  pixmap.fill(Qt::transparent);

  for (auto token : tokens) {
    auto offset = token->geometry().topLeft() - enclosingRect.topLeft();
    token->render(&pixmap, offset, QRegion{}, QWidget::DrawChildren);
  }

  return pixmap;
}

QAbstractItemModel* TokenDragDropHandler::model() const {
  return _tokenEdit->model();
}

}  // namespace mjendruk
