#pragma once

#include <qtadvwidgets/Token.h>

class QAbstractItemModel;

class TokenEdit;

class TokenDragDropHandler : public AbstractTokenDragDropHandler {
 public:
  TokenDragDropHandler(TokenEdit* tokenEdit);

  bool canDrag(Token const* source) const override;

  QMimeData* mimeData(const Token* source) const override;

  bool canDropMimeData(const Token* target, const QMimeData* data,
                       QObject* source, TokenDropHint dropHint) const override;

  bool dropMimeData(const Token* target, const QMimeData* data, QObject* source,
                    TokenDropHint dropHint) override;

  bool dropAccepted(Token* token) override;

 private:
  QAbstractItemModel* model() const;

 private:
  TokenEdit* _tokenEdit;
};
