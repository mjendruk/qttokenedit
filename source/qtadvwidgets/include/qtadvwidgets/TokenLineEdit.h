#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QLineEdit>

class QTADVWIDGETS_API TokenLineEdit : public QLineEdit {
  Q_OBJECT
 public:
  TokenLineEdit(QWidget* parent = nullptr);

 signals:
  void backspaceAtBeginning();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
};
