#pragma once

#include <type_traits>

#include <QtWidgets/QWidget>

namespace mjendruk {

class AbstractTokenDragDropHandler;

template <typename T>
class TokenDropTarget : public T {
  static_assert(std::is_base_of_v<QWidget, T>);

 public:
  TokenDropTarget(AbstractTokenDragDropHandler* handler,
                  QWidget* parent = nullptr);

 protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

  //! call in sub-class
  void drawIndicator(QPainter* painter) const;

 private:
  bool acceptsDrop(QDropEvent* event) const;

  void showIndicator();
  void hideIndicator();

 private:
  AbstractTokenDragDropHandler* _handler;
  bool _drawIndicator;
};

}  // namespace mjendruk
