#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QAbstractButton>

class QTADVWIDGETS_API RemoveButton : public QAbstractButton {
  Q_OBJECT

 public:
  RemoveButton(QPalette::ColorRole colorRole, int diameter, QWidget* parent = nullptr);

  QSize sizeHint() const override;
  
  QPalette::ColorRole colorRole() const;
  void setColorRole(QPalette::ColorRole role);

  void draw(QPainter* painter) const;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void initSize();
  void initButtonShapePath();

 private:
  QPalette::ColorRole _colorRole;
  int _diameter;
  QSize _size;
  QPainterPath _path;
};
