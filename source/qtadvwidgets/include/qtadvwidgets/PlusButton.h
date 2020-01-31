#pragma once

#include <qtadvwidgets/AbstractTokenButton.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API PlusButton : public AbstractTokenButton {
  Q_OBJECT

 public:
  PlusButton(QWidget* parent = nullptr);
  PlusButton(QPalette::ColorRole colorRole,
             QWidget* parent = nullptr);

 private:
  QPainterPath createPath(int extent) const;
};
