#pragma once

#include <QtWidgets/QLineEdit>

#include <qtadvwidgets/FocusChainNavigation.h>
#include <qtadvwidgets/TokenDropTarget.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API TokenLineEdit : public TokenDropTarget<QLineEdit> {
  Q_OBJECT
 public:
  TokenLineEdit(AbstractTokenDragDropHandler* handler,
                QWidget* parent = nullptr);

 signals:
  void backspaceAtBeginning();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

  QSize sizeHint() const override;

 private:
  void onTextChanged(QString const& text);

 private:
  int _widthHint;
};

class QTADVWIDGETS_API LineEditFocusChainNavigation
    : public FocusChainNavigation {
 public:
  LineEditFocusChainNavigation(QLineEdit* lineEdit);

  bool focusPrevious(QKeyEvent* event) const override;
  bool focusNext(QKeyEvent* event) const override;

 private:
  QLineEdit const* _lineEdit;
};
