#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>
#include <qtadvwidgets/FocusChainNavigation.h>

#include <QLineEdit>

class QTADVWIDGETS_API TokenLineEdit : public QLineEdit {
  Q_OBJECT
 public:
  TokenLineEdit(QWidget* parent = nullptr);

 signals:
  void backspaceAtBeginning();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  QSize sizeHint() const override;

 private:
  void onTextChanged(QString const& text);
 
 private:
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
