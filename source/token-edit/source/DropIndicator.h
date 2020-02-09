#pragma once

#include <token-edit/Global.h>

class QPainter;
class QRect;
class QBrush;

namespace mjendruk {

class DropIndicator {
 public:
  void draw(DropIndicatorPosition position, QRect const& rect,
            QBrush const& brush, QPainter* painter) const;
};

}  // namespace mjendruk
