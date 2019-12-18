#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QWidget>

class QTADVWIDGETS_API RemoveButton : public QWidget {
  Q_OBJECT

 public:
  RemoveButton(QWidget *parent = nullptr);

  QSize sizeHint() const override;
  
 signals:
  void clicked();

 protected:
  void paintEvent(QPaintEvent *event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  
private:
  void initSize();
  void initButtonShapePath();

private:
  bool _pressed;
  QSize _size;
  QPainterPath _path;
};
