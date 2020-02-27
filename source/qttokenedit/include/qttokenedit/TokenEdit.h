#pragma once

#include <cstdint>
#include <memory>

#include <QtCore/QModelIndex>
#include <QtCore/QScopedValueRollback>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QScrollArea>

#include <qttokenedit/Global.h>
#include <qttokenedit/TokenEditFrame.h>
#include <qttokenedit/TokenEditPasskey.h>
#include <qttokenedit/qttokenedit_api.h>

class QLineEdit;
class QStackedWidget;
class QAbstractItemModel;
class QScrollArea;

namespace mjendruk {
class AbstractTokenDragDropHandler;
class AbstractTokenEditModeAccess;
class AbstractTokenSelectionHandler;
class FinalWidgetPlaceholder;
class Token;
class TokenEditDisplayMode;
class TokenEditEditingMode;
class TokenEditMode;
class TokenEditView;
class TokenLineEdit;
class TokenSelectionHandler;

class QTTOKENEDIT_API TokenEdit : public TokenEditFrame {
  Q_OBJECT

 public:
  TokenEdit(QWidget* parent = nullptr);
  ~TokenEdit();

  int maxLineCount() const;
  void setMaxLineCount(int count);

  ShowLineEdit showLineEdit() const;
  void setShowLineEdit(ShowLineEdit dragStateChanged);

  bool dragEnabled() const;
  void setDragEnabled(bool enable);

  QAbstractItemView::DragDropMode dragDropMode() const;
  void setDragDropMode(QAbstractItemView::DragDropMode behavior);

  bool removable() const;
  void setRemovable(bool enable);

  QLineEdit* lineEdit() const;

  QAbstractItemModel* model() const;
  void setModel(QAbstractItemModel* model);

  int modelColumn() const;
  void setModelColumn(int column);

  QModelIndex rootIndex() const;
  void setRootIndex(QModelIndex const& index);

  QItemSelectionModel* selectionModel() const;
  
  ActiveTokenEditMode activeMode() const;
  
  TokenEditView const* view(TokenEditPasskey const&) const;

 signals:
  void dragStateChanged(bool enabled);
  void removableStateChanged(bool enabled);

 protected:
  bool eventFilter(QObject* object, QEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

 private:
  friend class TokenEditModeAccess;
  friend class TokenSelectionHandler;
  friend class TokenDragDropHandler;

  TokenEditView* view() const;
  AbstractTokenSelectionHandler* selectionHandler() const;
  AbstractTokenDragDropHandler* dragDropHandler() const;

  int visibleCount() const;
  Token* at(int row) const;
  Token* find(int row) const;
  int indexOf(Token const* token) const;
  QModelIndex index(int row) const;
  QModelIndex index(Token const* token) const;

  std::unique_ptr<QScopedValueRollback<UpdateFocus>> enableUpdateFocus();
  bool remove(int row, UpdateFocus uf);
  bool remove(QModelIndexList const& indexes, UpdateFocus uf);

  void init();
  void clear();

  void onRowsInserted(QModelIndex const& parent, int first, int last);
  void onRowsRemoved(QModelIndex const& parent, int first, int last);
  void onRowsMoved(QModelIndex const& parent, int first, int last,
                   QModelIndex const& destination, int row);
  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                     const QVector<int>& roles);
  void onModelReset();

  void onFocusChanged(QWidget* prev, QWidget* now);

  void onCurrentChanged(QModelIndex const& current, QModelIndex const& prev);

  void blockModeChange();
  void unblockModeChange();
  void setNextActiveMode(TokenEditMode* mode);
  void updateActiveMode();
  void setActiveMode(TokenEditMode* mode);
  void updateHeight();
  void updateDefaultFinalWidget();
  void ensureVisible(QWidget* widget);

 private:
  QScopedPointer<AbstractTokenEditModeAccess> _access;
  TokenSelectionHandler* _selectionHandler;
  QScopedPointer<AbstractTokenDragDropHandler> _dragDropHandler;

  TokenEditView* _view;
  
  TokenLineEdit* _lineEdit;
  FinalWidgetPlaceholder *_placeholder;

  TokenEditMode* _activeMode;
  TokenEditMode* _nextActiveMode;
  bool _modeChangedBlocked;
  TokenEditEditingMode* _editingMode;
  TokenEditDisplayMode* _displayMode;

  QScrollArea* _scrollArea;

  int _maxLineCount;
  ShowLineEdit _showLineEdit;
  bool _dragEnabled;
  QAbstractItemView::DragDropMode _dragDropMode;
  bool _removable;

  UpdateFocus _updateFocus;

  // model members
  QAbstractItemModel* _model;
  QModelIndex _rootModelIndex;
  int _modelColumn;
};

}  // namespace mjendruk
