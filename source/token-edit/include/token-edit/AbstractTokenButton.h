#pragma once

#include <QtWidgets/QAbstractButton>

#include <token-edit/token-edit-api.h>

namespace mjendruk {

class TOKEN_EDIT_API AbstractTokenButton : public QAbstractButton {
  Q_OBJECT

 public:
  explicit AbstractTokenButton(QWidget* parent = nullptr);

  QSize sizeHint() const override;

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
};

}  // namespace mjendruk
