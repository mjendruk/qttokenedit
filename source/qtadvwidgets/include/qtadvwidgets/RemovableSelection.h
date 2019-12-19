#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QWidget>
#include <QString>
#include <QVariant>

class QFontMetrics;
class QLabel;

class RemoveButton;

class QTADVWIDGETS_API RemovableSelection : public QWidget {
  Q_OBJECT

 public:
  RemovableSelection(QString const& text, QWidget* parent = nullptr);
  RemovableSelection(QString const& text, QVariant const& userData, QWidget* parent = nullptr);

  QString const& text() const;
  void setText(QString const& text);

  QVariant const& userData() const;
  void setUserData(QVariant const& data);

 signals:
  void removeClicked();

 protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  
 private:
  QString _text;
  QVariant _userData;

  QLabel* _label;
  RemoveButton* _button;

  bool _selected;
  bool _pressed;
};
