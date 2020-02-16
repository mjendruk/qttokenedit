#pragma once

#include <memory>

#include <QtCore/QString>

#include <token-edit/BaseToken.h>
#include <token-edit/Global.h>
#include <token-edit/token-edit-api.h>

class QMimeData;

namespace mjendruk {

class AbstractTokenDragDropHandler;
class AbstractTokenSelectionHandler;
class RemoveButton;
class Token;

class TOKEN_EDIT_API Token : public BaseToken {
  Q_OBJECT

 public:
  Token(QWidget* parent = nullptr);

  Token(AbstractTokenDragDropHandler* dragDropHandler,
        AbstractTokenSelectionHandler* selectionHandler,
        QWidget* parent = nullptr);

  Token(QString const& text, AbstractTokenDragDropHandler* dragDropHandler,
        AbstractTokenSelectionHandler* selectionHandler,
        QWidget* parent = nullptr);
  ~Token();

  bool removable() const;
  void setRemovable(bool enable);

  bool selected() const;
  void setSelected(bool enable);

 signals:
  void pressed();
  void removeClicked();

 protected:
  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  bool shouldStartDrag(QPoint const& mousePos) const;
  void startDrag(QPoint const& mousePos);
  bool acceptsDrag(QDropEvent* event) const;
  void acceptDrag(QDragMoveEvent* event);
  void finishDrag(QDropEvent* event);

  void showDropIndicator(QPoint const& mousePos);
  void resetDropIndicator();

  void drawDropIndicator(QPainter* painter);

  int dragStartDistance() const;
  TokenDropHint dropHint(QPoint const& mousePos) const;

 private:
  AbstractTokenDragDropHandler* _dragDropHandler;
  AbstractTokenSelectionHandler* _selectionHandler;
  RemoveButton* _button;

  bool _removable;
  bool _selected;

  // drag members
  QPoint _mousePressedAt;
  DropIndicatorPosition _dropIndicatorPosition;
};

}  // namespace mjendruk
