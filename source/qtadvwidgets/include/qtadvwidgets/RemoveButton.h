#pragma once

#include <qtadvwidgets/AbstractTokenButton.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

namespace mjendruk {

class QTADVWIDGETS_API RemoveButton : public AbstractTokenButton {
  Q_OBJECT

 public:
  RemoveButton(QWidget* parent = nullptr);
  RemoveButton(QPalette::ColorRole colorRole, QWidget* parent = nullptr);

 private:
  QPainterPath createPath(int extent) const;
};

}  // namespace mjendruk