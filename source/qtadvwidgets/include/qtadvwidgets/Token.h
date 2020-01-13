#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>
#include <qtadvwidgets/BaseToken.h>

#include <QString>
#include <memory>

class RemoveButton;

class QTADVWIDGETS_API Token : public BaseToken {
  Q_OBJECT

 public:
  enum class DropHint { Before, After };

  Token(QString const& text, QWidget* parent = nullptr);
  ~Token();

  bool dragEnabled() const;
  void setDragEnabled(bool enable);
  
  bool removable() const;
  void setRemovable(bool enable);

  QPixmap toPixmap() const override;

 signals:
  void removeClicked();
  void dragged(Token* target, DropHint hint);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void focusInEvent(QFocusEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;

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

  enum class DropIndicator { None, Before, After };
  
  void showDropIndicator(QPoint const& mousePos);
  void resetDropIndicator();
  
  void drawDropIndicator(QPainter* painter);
  
  int dragStartDistance() const;
  DropHint dropHint(QPoint const& mousePos) const;

 private:
  RemoveButton* _button;
  
  bool _removable;

  // drag members
  bool _dragEnabled;
  QPoint _mousePressedAt;
  DropIndicator _dropIndicator;
};

Q_DECLARE_METATYPE(Token*);
