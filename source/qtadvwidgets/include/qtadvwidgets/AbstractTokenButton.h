#pragma once

#include <QtWidgets/QAbstractButton>

#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API AbstractTokenButton : public QAbstractButton {
  Q_OBJECT

 public:
  AbstractTokenButton(QWidget* parent = nullptr);
  AbstractTokenButton(QPalette::ColorRole colorRole, QWidget* parent = nullptr);

  QSize sizeHint() const override;

  QPalette::ColorRole colorRole() const;
  void setColorRole(QPalette::ColorRole role);

 protected:
  void paintEvent(QPaintEvent* event) override;

 protected:
  void setPath(QPainterPath const& path);
  int extent() const;

 private:
  void initSize();

 private:
  QPainterPath _path;
  QSize _size;
  QPalette::ColorRole _colorRole;
};
