#pragma once

#include <QtWidgets/QWidget>

#include <qttokenedit/qttokenedit_api.h>

class QBoxLayout;

namespace mjendruk {

class QTTOKENEDIT_API TokenEditFrame : public QWidget {
  Q_OBJECT

 public:
  TokenEditFrame(QWidget* parent = nullptr);

  QWidget* takeWidget();
  void setWidget(QWidget* widget);

  bool shownAsFocused() const;
  void setShownAsFocused(bool value);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void drawBackground(QPainter* painter);
  void drawFrame(QPainter* painter);
  
  QRect contentRect() const;
  QMargins contentMargins() const;

 private:
  QWidget* _widget;
  QBoxLayout* _layout;
  bool _shownAsFocused;
};

}  // namespace mjendruk
