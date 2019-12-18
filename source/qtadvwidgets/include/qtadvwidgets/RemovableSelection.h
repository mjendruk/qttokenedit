#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QWidget>

class QTADVWIDGETS_API RemovableSelection : public QWidget {
  Q_OBJECT

 public:
  RemovableSelection(QString const& text, QWidget* parent = nullptr);
  
 signals:
  void removeClicked();

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  
 private:
  bool _selected;
  bool _pressed;
};
