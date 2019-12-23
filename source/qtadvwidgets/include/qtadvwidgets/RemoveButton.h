#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QAbstractButton>

class QTADVWIDGETS_API RemoveButton : public QAbstractButton {
  Q_OBJECT

 public:
  RemoveButton(int diameter, QWidget *parent = nullptr);

  QSize sizeHint() const override;

 protected:
  void paintEvent(QPaintEvent *event) override;
  
private:
  void initSize();
  void initButtonShapePath();

private:
  int _diameter;
  QSize _size;
  QPainterPath _path;
};
