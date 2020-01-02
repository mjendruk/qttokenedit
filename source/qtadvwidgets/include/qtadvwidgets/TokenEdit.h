#pragma once

#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/TokenEditViewport.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QScrollArea>
#include <QVariant>
#include <QVector>
#include <cstdint>
#include <QModelIndex>

class QLineEdit;
class QAbstractItemModel;

class Token;
class FlexLayout;
class TokenLineEdit;
class TokenChain;

class QTADVWIDGETS_API TokenEdit : public TokenEditViewport {
  Q_OBJECT

 public:
  using Mode = TokenEditMode;

  TokenEdit(TokenEditMode mode = TokenEditMode::Multiple,
            QWidget* parent = nullptr);
  ~TokenEdit();

  int maxLineCount() const;
  void setMaxLineCount(int count);

  int count() const;
  int isEmpty() const;

  QLineEdit* lineEdit();

  QAbstractItemModel* model() const;
  void setModel(QAbstractItemModel* model);
  
  int modelColumn() const;
  void setModelColumn(int column);

 signals:
  void itemAboutToBeRemoved(int index);

 private:
  void addItem(QString const& text);
  void addItems(QStringList const& texts);

  void insertItem(int index, QString const& text);

  void setItemText(int index, QString const& text);

  void removeItem(int index);
  
  void updateHeight();
  void init();
  void clear();

  void onRowsInserted(QModelIndex const& parent, int first, int last);
  void onRowsRemoved(QModelIndex const& parent, int first, int last);
  void onRowsMoved(QModelIndex const& parent, int first, int last,
                   QModelIndex const& destination, int row);
  void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                     const QVector<int>& roles);
  void onModelReset();

 private:
  QVector<Token*> _items;
  TokenLineEdit* _lineEdit;
  TokenChain* _tokenChain;
  FlexLayout* _layout;
  int _maxLineCount;
  int _spacing;
  TokenEditMode _mode;
  QScrollArea* _scrollArea;
  QAbstractItemModel* _model;
  QModelIndex _rootModelIndex;
  int _modelColumn;
};
