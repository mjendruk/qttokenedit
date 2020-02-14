#pragma once

#include <QtCore/QObject>

#include <token-edit/Global.h>

namespace mjendruk {

class Token;
class TokenEditView;
class AbstractTokenDragDropHandler;

class AbstractTokenEditModeAccess {
 public:
  virtual ~AbstractTokenEditModeAccess() = default;

  virtual int maxLineCount() const = 0;
  virtual int count() const = 0;

  virtual Token* createToken(int index, QWidget* parent = nullptr) const = 0;
  virtual void updateToken(int index, Token* token,
                           QVector<int> const& roles) const = 0;

  virtual AbstractTokenDragDropHandler* dragDropHandler() const = 0;
};

class TokenEditMode : public QObject {
  Q_OBJECT
 public:
  TokenEditMode(TokenEditView* view, AbstractTokenEditModeAccess* access,
                QObject* parent = nullptr);

  virtual ~TokenEditMode() = default;

  virtual void inserted(int first, int last, UpdateFocus uf) = 0;
  virtual void removed(int first, int last, UpdateFocus uf) = 0;
  virtual void moved(int first, int last, int to) = 0;
  virtual void changed(int first, int last, QVector<int> const& roles) = 0;

  virtual void invalidate() = 0;
  virtual int heightHint() const = 0;

  virtual void activate() = 0;
  virtual void deactivate() = 0;

 protected:
  TokenEditView* view() const;
  AbstractTokenEditModeAccess* access() const;

 private:
  TokenEditView* _view;
  AbstractTokenEditModeAccess* _access;
};

}  // namespace mjendruk
