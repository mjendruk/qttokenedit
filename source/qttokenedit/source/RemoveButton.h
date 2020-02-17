#pragma once

#include <qttokenedit/AbstractTokenButton.h>

namespace mjendruk {

class RemoveButton : public AbstractTokenButton {
  Q_OBJECT

 public:
  explicit RemoveButton(QWidget* parent = nullptr);

 private:
  QPainterPath createPath(int extent) const;
};

}  // namespace mjendruk
