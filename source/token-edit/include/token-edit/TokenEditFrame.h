#pragma once

#include <QtWidgets/QWidget>

#include <token-edit/token-edit-api.h>

class QBoxLayout;

namespace mjendruk {

class TokenEditFrameOverlay;

class TOKEN_EDIT_API TokenEditFrame : public QWidget {
  Q_OBJECT

 public:
  TokenEditFrame(QWidget* parent = nullptr);

  QWidget* takeWidget();
  void setWidget(QWidget* widget);

  bool shownAsFocused() const;
  void setShownAsFocused(bool value);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void enterEvent(QEvent* event) override;

 private:
  QRect contentRect() const;
  QMargins contentMargins() const;

 private:
  QWidget* _widget;
  QBoxLayout* _layout;
  TokenEditFrameOverlay* _frame;
  bool _shownAsFocused;
};

}  // namespace mjendruk