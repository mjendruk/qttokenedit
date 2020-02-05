#pragma once

#include <qtadvwidgets/TokenDropTarget.h>

#include <QWidget>

class FinalWidgetPlaceholder : public TokenDropTarget<QWidget> {
  Q_OBJECT
 public:
  FinalWidgetPlaceholder(AbstractTokenDragDropHandler* handler,
                         QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};
