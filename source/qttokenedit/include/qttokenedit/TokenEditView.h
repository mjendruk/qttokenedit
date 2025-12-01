#pragma once

#include <QtCore/QVector>
#include <QtWidgets/QWidget>

#include <qttokenedit/Global.h>
#include <qttokenedit/qttokenedit_api.h>

namespace mjendruk {

class TokenLineEdit;
class Token;
class FlexLayout;

class QTTOKENEDIT_API TokenEditView : public QWidget {
  Q_OBJECT
 public:
  TokenEditView(QWidget* parent = nullptr);

  /**
   * @name token interface
   * @{
   */

  Token* at(int index) const;

  //! returns nullptr if index exceeds count
  Token* find(int index) const;
  
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
  void setFinalWidget(QWidget* widget);

  QWidget* takeDefaultFinalWidget();
  void setDefaultFinalWidget(QWidget* widget);

  /**
   * @name layout interface
   * @{
   */

  int xSpacing() const;
  int ySpacing() const;
  int margin() const;

  int heightForWidth(int width) const override;

  int lineCount() const;
  int lineCountForWidth(int width) const;

  void freezeLayout();
  void unfreezeLayout();

  /** @} */

 signals:
  void sizeChanged(QSize size);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private:
  QWidget* takeFinalWidget(bool setDefault);

 private:
  QVector<Token*> _tokens;
  FlexLayout* _layout;
  Token* _dummyToken;

  QWidget* _finalWidget;
  QWidget* _defaultFinalWidget;
};

}  // namespace mjendruk
