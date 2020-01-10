#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QString>
#include <QVariant>
#include <QWidget>
#include <memory>

class QFontMetrics;

class RemoveButton;
class TokenChainElement;

class QTADVWIDGETS_API Token : public QWidget {
  Q_OBJECT

 public:
  enum class DropHint { Before, After };

  Token(QString const& text, QWidget* parent = nullptr);
  ~Token();

  QString const& text() const;
  void setText(QString const& text);

  bool dragEnabled() const;
  void setDragEnabled(bool enable); 

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  TokenChainElement* chainElement() const;

  QPixmap toPixmap() const;

 signals:
  void removeClicked();
  void dragged(Token* target, DropHint hint);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void focusInEvent(QFocusEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;

  void leaveEvent(QEvent* event) override;
  void enterEvent(QEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  void drawBackground(QPainter* painter, QBrush brush) const;
  void drawText(QPainter* painter, QPen pen) const;
  
  int contentHeight() const;
  int horizontalTextMargin() const;
  int margin() const;
  int spacing() const;
  QSize textSize() const;

  void updateElidedText(QSize size);
  void updateButtonPosition(QSize size);

  QSize elidedTextSize() const;
  QString elidedText() const;

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
  QString _text;
  std::unique_ptr<TokenChainElement> _chainElement;
  RemoveButton* _button;

  int _elidedTextWidth;
  QString _elidedText;

  // drag members
  bool _dragEnabled;
  QPoint _mousePressedAt;
  DropIndicator _dropIndicator;
};

Q_DECLARE_METATYPE(Token*);
