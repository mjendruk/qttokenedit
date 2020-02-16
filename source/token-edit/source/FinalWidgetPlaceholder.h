#pragma once

#include <QWidget>

#include <token-edit/TokenDropTarget.h>

namespace mjendruk {

class FinalWidgetPlaceholder : public TokenDropTarget<QWidget> {
  Q_OBJECT
 public:
  FinalWidgetPlaceholder(AbstractTokenDragDropHandler* handler,
                         QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};

}  // namespace mjendruk
