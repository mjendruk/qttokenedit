#pragma once

#include <qtadvwidgets/AbstractTokenButton.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API PlusButton : public AbstractTokenButton {
  Q_OBJECT

 public:
  PlusButton(int extent, QWidget* parent = nullptr);
  PlusButton(int extent, QPalette::ColorRole colorRole,
               QWidget* parent = nullptr);

 private:
  QPainterPath createPath(int extent) const;
};
