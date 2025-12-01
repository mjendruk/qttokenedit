#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtCore/QItemSelectionModel>

#include <qttokenedit/AbstractTokenSelectionHandler.h>

namespace mjendruk {

class Token;
class TokenEdit;
class TokenEditView;

class TokenSelectionHandler : public AbstractTokenSelectionHandler {
  Q_OBJECT

 public:
  TokenSelectionHandler(TokenEdit* tokenEdit);

  void select(Token const* token, Qt::MouseButtons buttons,
              Qt::KeyboardModifiers modifiers) override;

  QItemSelectionModel* selectionModel() const;

  void updateModel();

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  QAbstractItemModel* model() const;

  void onSelectionChanged(QItemSelection const& selected,
                          QItemSelection const& deselected);
  
  void onCurrentChanged(QModelIndex const& index);

  void updateSelection(QModelIndexList const& indexes, bool selected);

  QItemSelectionModel::SelectionFlags defaultFlags() const;

  bool shouldExtendSelection(Qt::MouseButtons buttons,
                             Qt::KeyboardModifiers modifiers) const;

  void extendSelectionTo(QModelIndex const& index);

  bool shouldSelectSingle(Qt::MouseButtons buttons,
                          Qt::KeyboardModifiers modifiers) const;

  void selectSingle(QModelIndex const& index, Qt::KeyboardModifiers modifiers);

  bool shouldSelectPreviousNext(QEvent* event) const;
  bool selectPreviousNext(QEvent* event);

  bool shouldSelectFirst(QEvent* event) const;
  bool selectFirst();

  bool shouldSelectAll(QEvent* event) const;
  bool selectAll();

  bool shouldClear(QEvent* event) const;

 private:
  TokenEdit* const _tokenEdit;

  QItemSelectionModel* _selectionModel;
};
}  // namespace mjendruk
