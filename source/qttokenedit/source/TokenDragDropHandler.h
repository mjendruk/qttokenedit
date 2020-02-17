#pragma once

#include <QtCore/QVector>
#include <QtCore/QAbstractItemModel>

#include <qttokenedit/AbstractTokenDragDropHandler.h>

namespace mjendruk {

class TokenEdit;
class TokenEditView;

class TokenDragDropHandler : public AbstractTokenDragDropHandler {
 public:
  explicit TokenDragDropHandler(TokenEdit* tokenEdit);

  bool canDrag(Token const* source) const override;

  void execDrag(Token* source, QPoint const& mousePos) override;

  bool canDropMimeData(const Token* target, const QMimeData* data,
                       QObject* source, TokenDropHint dropHint) const override;

  bool canDropMimeData(int row, QMimeData const* data,
                       QObject* source) const override;

  bool dropMimeData(const Token* target, const QMimeData* data, QObject* source,
                    TokenDropHint dropHint) override;

  bool dropMimeData(int row, QMimeData const* data, QObject* source) override;

 private:
  QModelIndexList selectedIndexes() const;
  QVector<Token*> selectedTokens() const;
  QRect enclosingRect(QVector<Token*> const& tokens) const;
  QPixmap renderPixmap(QRect const& enclosingRect,
                       QVector<Token*> tokens) const;
  
 private:
  QAbstractItemModel* model() const;
  
 private:
  TokenEdit* const _tokenEdit;
};

}  // namespace mjendruk
