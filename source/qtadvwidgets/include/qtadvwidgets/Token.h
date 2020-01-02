#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QString>
#include <QVariant>
#include <QWidget>
#include <memory>

class QFontMetrics;

class RemoveButton;
class TokenChainElement;

class QTADVWIDGETS_API Token : public QWidget {
  Q_OBJECT

 public:
  Token(QString const& text, QWidget* parent = nullptr);
  ~Token();

  QString const& text() const;
  void setText(QString const& text);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  TokenChainElement* chainElement() const;

 signals:
  void removeClicked();
  void focused();

 protected:
  void paintEvent(QPaintEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void enterEvent(QEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void focusInEvent(QFocusEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;

  int contentHeight() const;
  int horizontalTextMargin() const;
  int margin() const;
  int spacing() const;
  QSize textSize() const;

  void updateElidedText(QSize size);
  void updateButtonPosition(QSize size);

  QSize elidedTextSize() const;
  QString elidedText() const;

 private:
  QString _text;

  std::unique_ptr<TokenChainElement> _chainElement;

  RemoveButton* _button;

  int _elidedTextWidth;
  QString _elidedText;

  bool _hovered;
};
