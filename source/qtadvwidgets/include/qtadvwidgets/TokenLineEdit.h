#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>
#include <qtadvwidgets/FocusChainNavigation.h>

#include <QLineEdit>


class AbstractTokenDragDropHandler;


class QTADVWIDGETS_API TokenLineEdit : public QLineEdit {
  Q_OBJECT
 public:
  TokenLineEdit(QWidget* parent = nullptr);

  void setDragDropHandler(AbstractTokenDragDropHandler* handler);
  
 signals:
  void backspaceAtBeginning();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  QSize sizeHint() const override;

  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  bool acceptsDrop(QDropEvent* event) const;

  void onTextChanged(QString const& text);
 
 private:
  AbstractTokenDragDropHandler* _dragDropHandler;
  int _widthHint;
};

class QTADVWIDGETS_API LineEditFocusChainNavigation
    : public FocusChainNavigation {
 public:
  LineEditFocusChainNavigation(QLineEdit* lineEdit, QObject * parent = nullptr);

  bool focusPrevious(QKeyEvent* event) const override;
  bool focusNext(QKeyEvent* event) const override;

 private:
  QLineEdit const* _lineEdit;
};
