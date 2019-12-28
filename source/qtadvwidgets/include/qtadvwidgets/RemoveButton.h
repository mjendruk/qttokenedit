#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QAbstractButton>

class QTADVWIDGETS_API RemoveButton : public QAbstractButton {
  Q_OBJECT

 public:
  RemoveButton(QColor const& color, int diameter, QWidget* parent = nullptr);

  QSize sizeHint() const override;

  QColor color() const;
  void setColor(QColor const& color);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void initSize();
  void initButtonShapePath();

 private:
  int _diameter;
  QSize _size;
  QPainterPath _path;
  QColor _color;
};
