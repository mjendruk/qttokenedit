#pragma once

#include <qtadvwidgets/AbstractTokenButton.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API RemoveButton : public AbstractTokenButton {
  Q_OBJECT

 public:
  RemoveButton(int extent, QWidget* parent = nullptr);
  RemoveButton(int extent, QPalette::ColorRole colorRole,
               QWidget* parent = nullptr);

 private:
  QPainterPath createPath(int extent) const;
};
