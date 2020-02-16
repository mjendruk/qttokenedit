#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QItemSelectionModel>

#include <token-edit/AbstractSelectionHandler.h>

namespace mjendruk {

class Token;
class TokenEdit;
class TokenEditView;

class SelectionHandler : public AbstractSelectionHandler {
  Q_OBJECT
  
 public:
  SelectionHandler(TokenEdit* tokenEdit);
  
  void select(Token const* token, Qt::MouseButtons buttons,
              Qt::KeyboardModifiers modifiers) override;
  
  QItemSelectionModel* selectionModel() const;

  void updateModel();

 public:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  QAbstractItemModel* model() const;
  TokenEditView* view() const;
  
  void onSelectionChanged(QItemSelection const& selected,
                          QItemSelection const& deselected);

  void updateSelection(QModelIndexList const& indexes, bool selected);

 private:
  TokenEdit* const _tokenEdit;

  QItemSelectionModel* _selectionModel;
};
}  // namespace mjendruk
