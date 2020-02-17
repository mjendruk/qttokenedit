#pragma once

#include <memory>

#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include <qttokenedit/qttokenedit_api.h>

class QBoxLayout;

namespace mjendruk {

class ElidableLabel;

class QTTOKENEDIT_API BaseToken : public QWidget {
  Q_OBJECT

 public:
  explicit BaseToken(QWidget* parent = nullptr);
  explicit BaseToken(QString const& text, QWidget* parent = nullptr);
  ~BaseToken();

  QString const& text() const;
  void setText(QString const& text);

  QWidget* rightWidget() const;
  void setRightWidget(QWidget* widget);

  int contentHeight() const;

 protected:
  void updateMargins();

  void drawBackground(QPainter* painter, QBrush brush) const;

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  int horizontalTextMargin() const;
  int margin() const;
  int spacing() const;
  QMargins margins() const;

 private:
  QBoxLayout* _layout;
  ElidableLabel* _label;
  QWidget* _rightWidget;
};

}  // namespace mjendruk
