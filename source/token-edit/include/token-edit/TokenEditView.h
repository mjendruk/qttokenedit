#pragma once

#include <QtCore/QVector>
#include <QtWidgets/QWidget>

#include <token-edit/Global.h>
#include <token-edit/token-edit-api.h>

namespace mjendruk {

class TokenLineEdit;
class Token;
class FocusChain;
class FlexLayout;
class FocusChainNavigation;

class TOKEN_EDIT_API TokenEditView : public QWidget {
  Q_OBJECT
 public:
  TokenEditView(QWidget* parent = nullptr);

  /**
   * @name token interface
   * @{
   */

  Token* at(int index) const;
  int indexOf(Token const* token) const;
  void add(Token* token, UpdateFocus uf);
  void insert(int index, Token* token, UpdateFocus uf);
  void move(int from, int to);
  void remove(int index, UpdateFocus uf);
  int count() const;
  int isEmpty() const;

  QVector<Token*> const& tokens() const;

  /** @} */

  QWidget* takeFinalWidget();
  void setFinalWidget(QWidget* widget,
                      FocusChainNavigation* navigation = nullptr);

  QWidget* takeDefaultFinalWidget();
  void setDefaultFinalWidget(QWidget* widget,
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
  QWidget* takeFinalWidget(bool setDefault);

 private:
  QVector<Token*> _tokens;
  FocusChain* _focusChain;
  FlexLayout* _layout;

  QWidget* _finalWidget;

  QWidget* _defaultFinalWidget;
  FocusChainNavigation* _defaultNavigation;
};

}  // namespace mjendruk
