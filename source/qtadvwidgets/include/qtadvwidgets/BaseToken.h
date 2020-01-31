#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QString>
#include <QWidget>
#include <memory>

class QBoxLayout;

class ElidableLabel;

class QTADVWIDGETS_API BaseToken : public QWidget {
  Q_OBJECT

 public:
  BaseToken(QWidget* parent = nullptr);
  BaseToken(QString const& text, QWidget* parent = nullptr);
  ~BaseToken();

  QString const& text() const;
  void setText(QString const& text);

  QWidget* rightWidget() const;
  void setRightWidget(QWidget* widget);
  
  virtual QPixmap toPixmap() const;
 
  int contentHeight() const;

 protected:
  void updateMargins();

  void drawBackground(QPainter* painter, QBrush brush) const;

 protected:
  void paintEvent(QPaintEvent* event) override;
  void focusInEvent(QFocusEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;

  void leaveEvent(QEvent* event) override;
  void enterEvent(QEvent* event) override;

 private:
  int horizontalTextMargin() const;
  int margin() const;
  int spacing() const;
  QMargins margins() const;

 private:
  QBoxLayout* _layout;
  ElidableLabel* _label;
  QWidget* _rightWidget;
};
