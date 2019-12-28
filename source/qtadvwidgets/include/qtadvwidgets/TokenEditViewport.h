#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QWidget>

class QBoxLayout;

class TokenEditViewportFrame;

class QTADVWIDGETS_API TokenEditViewport : public QWidget {
  Q_OBJECT

 public:
  TokenEditViewport(QWidget* parent = nullptr);

  void setWidget(QWidget* widget);

  bool shownAsFocused() const;
  void setShownAsFocused(bool value);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private:
  QRect contentRect() const;
  QMargins contentMargins() const;

 private:
  QWidget* _widget;
  QBoxLayout* _layout;
  TokenEditViewportFrame* _frame;
  bool _shownAsFocused;
};
