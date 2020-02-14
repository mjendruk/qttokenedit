#pragma once

#include <token-edit/AbstractTokenButton.h>

namespace mjendruk {

class RemoveButton : public AbstractTokenButton {
  Q_OBJECT

 public:
  explicit RemoveButton(QWidget* parent = nullptr);
  explicit RemoveButton(QPalette::ColorRole colorRole,
                        QWidget* parent = nullptr);

 private:
  QPainterPath createPath(int extent) const;
};

}  // namespace mjendruk
