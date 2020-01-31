#pragma once

#include <qtadvwidgets/TokenEditFrame.h>
#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QModelIndex>
#include <QScrollArea>
#include <QVariant>
#include <QVector>
#include <cstdint>

class QLineEdit;
class QStackedWidget;
class QAbstractItemModel;
class QScrollArea;

class TokenLineEdit;
class TokenEditView;
class TokenEditMode;
class TokenEditEditingMode;
class TokenEditDisplayMode;

class QTADVWIDGETS_API TokenEdit : public TokenEditFrame,
                                       public TokenEditModeAccess {
  Q_OBJECT

 public:
  TokenEdit(QWidget* parent = nullptr);
  ~TokenEdit();

  int maxLineCount() const override;
  void setMaxLineCount(int count);

  bool dragEnabled() const;
  void setDragEnabled(bool enable);

  bool removable() const;
  void setRemovable(bool enable);

  QLineEdit* lineEdit() const;

  QAbstractItemModel* model() const;
  void setModel(QAbstractItemModel* model);

  int modelColumn() const;
  void setModelColumn(int column);

  QModelIndex rootIndex() const;
  void setRootIndex(QModelIndex const& index);

  int count() const override;
  Token* createToken(QString const& text, QWidget* parent) override;
  QString text(int index) const override;

 signals:
  void dragStateChanged(bool enabled);
  void removableStateChanged(bool enabled);
                                         
 protected:
  bool eventFilter(QObject* object, QEvent* event) override;

 private:
  void init();
  void clear();

  void onRowsInserted(QModelIndex const& parent, int first, int last);
  void onRowsRemoved(QModelIndex const& parent, int first, int last);
  void onRowsMoved(QModelIndex const& parent, int first, int last,
                   QModelIndex const& destination, int row);
  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                     const QVector<int>& roles);
  void onModelReset();

  void onItemDragged(Token* source, Token* target, Token::DropHint hint);

  void onFocusChanged(QWidget* prev, QWidget* now);

  void setActiveMode(TokenEditMode* mode);
  void updateHeight();
  void ensureVisible(QWidget* widget);

 private:
  TokenEditView* _view;
  TokenEditMode* _activeMode;
  TokenEditEditingMode* _editingMode;
  TokenEditDisplayMode* _displayMode;

  QScrollArea* _scrollArea;

  int _maxLineCount;
  bool _dragEnabled;
  bool _removable;

  // model members
  QAbstractItemModel* _model;
  QModelIndex _rootModelIndex;
  int _modelColumn;
};
