#pragma once

#include <token-edit/Global.h>
#include <token-edit/token-edit-api.h>

class QPainter;
class QRect;
class QBrush;

namespace mjendruk {

class TOKEN_EDIT_API DropIndicator {
 public:
  void draw(DropIndicatorPosition position, QRect const& rect,
            QBrush const& brush, QPainter* painter) const;
};

}  // namespace mjendruk
