#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>

class Token;
class TokenEditView;

class AbstractTokenEditModeAccess {
 public:
  virtual ~AbstractTokenEditModeAccess() = default;
  
  virtual int maxLineCount() const = 0;
  virtual int count() const = 0;
  
  virtual Token* createToken(int index, QWidget* parent = nullptr) const = 0;
  virtual void updateToken(int index, Token* token, QVector<int> const& roles) const = 0;
};

class QTADVWIDGETS_API TokenEditMode : public QObject {
  Q_OBJECT
 public:
  TokenEditMode(TokenEditView* view, AbstractTokenEditModeAccess* access,
                QObject* parent = nullptr);
  
  virtual ~TokenEditMode() = default;

  virtual void inserted(int first, int last) = 0;
  virtual void removed(int first, int last) = 0;
  virtual void moved(int first, int last, int to) = 0;
  virtual void changed(int first, int last, QVector<int> const& roles) = 0;

  virtual void clear() = 0;
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
