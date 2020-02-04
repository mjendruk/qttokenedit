#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QVector>
#include <QWidget>

class TokenLineEdit;
class Token;
class FocusChain;
class FocusChainNavigation;
class FlexLayout;

class QTADVWIDGETS_API TokenEditView : public QWidget {
  Q_OBJECT
 public:
  TokenEditView(QWidget* parent = nullptr);

  /**
   * @name token interface
   * @{
   */

  Token* at(int index) const;
  int indexOf(Token const* token) const;
  void add(Token* token);
  void insert(int index, Token* token);
  void move(int from, int to);
  void remove(int index);
  int count() const;
  int isEmpty() const;

  QVector<Token*> const& tokens() const;

  /** @} */

  QWidget* takeFinalWidget();
  void setFinalWidget(QWidget* widget,
                      FocusChainNavigation* navigation = nullptr);

  /**
   * @name layout interface
   * @{
   */
  
  int xSpacing() const;
  int ySpacing() const;
  int margin() const;

  int lineCount() const;
  int lineCountForWidth(int width) const;

  void freezeLayout();
  void unfreezeLayout();

  /** @} */

 signals:
  void gotFocus(QWidget* widget);
  void lostFocus(QWidget* widget);

  void sizeChanged(QSize size);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  QVector<Token*> _tokens;
  FocusChain* _focusChain;
  FlexLayout* _layout;
  QWidget* _finalWidget;
};
