#pragma once

#include <qttokenedit/Global.h>
#include <qttokenedit/qttokenedit_api.h>

class QPainter;
class QRect;
class QBrush;

namespace mjendruk {

class QTTOKENEDIT_API DropIndicator {
 public:
  void draw(DropIndicatorPosition position, QRect const& rect,
            QBrush const& brush, QPainter* painter) const;
};

}  // namespace mjendruk
