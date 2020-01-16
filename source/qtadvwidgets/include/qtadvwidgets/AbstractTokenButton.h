#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QAbstractButton>

class QTADVWIDGETS_API AbstractTokenButton : public QAbstractButton {
  Q_OBJECT

 public:
  AbstractTokenButton(QPainterPath path, QSize size, QWidget* parent = nullptr);
  AbstractTokenButton(QPainterPath path, QSize size,
                      QPalette::ColorRole colorRole, QWidget* parent = nullptr);

  QSize sizeHint() const override;

  QPalette::ColorRole colorRole() const;
  void setColorRole(QPalette::ColorRole role);

  void draw(QPainter* painter) const;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void initSize();

 private:
  QPainterPath _path;
  QSize _size;
  QPalette::ColorRole _colorRole;
};
