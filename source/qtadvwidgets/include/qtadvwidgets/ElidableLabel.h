#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QWidget>

class QTADVWIDGETS_API ElidableLabel : public QWidget {
  Q_OBJECT

 public:
  ElidableLabel(QWidget* parent = nullptr);
  ElidableLabel(QString const& text, QWidget* parent = nullptr);

  QString const& text() const;
  void setText(QString const& text);

  int minVisibleCharacters() const;
  void setMinVisibleCharacters(int count);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  QPalette::ColorRole textColorRole() const;
  void setTextColorRole(QPalette::ColorRole role);

  void draw(QPainter* painter) const;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void updateTextSize();
  void updateMinimumTextSize();

  QString elidedText() const;

 private:
  QString _text;
  int _minVisibleCharacters;
  QPalette::ColorRole _colorRole;

  QSize _textSize;
  QSize _minTextSize;
};
