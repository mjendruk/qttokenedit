#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>

class Token;
class TokenEditView;

class TokenEditModeAccess {
 public:
  virtual int maxLineCount() const = 0;
  virtual int count() const = 0;
  virtual Token* createToken(QString const& text, QWidget* parent = nullptr) = 0;
  virtual QString text(int index) const = 0;
};

class QTADVWIDGETS_API TokenEditMode : public QObject {
  Q_OBJECT
 public:
  TokenEditMode(TokenEditView* view, TokenEditModeAccess* access,
                QObject* parent = nullptr);
  
  virtual ~TokenEditMode() = default;

  virtual void inserted(int first, int last) = 0;
  virtual void removed(int first, int last) = 0;
  virtual void moved(int first, int last, int to) = 0;
  virtual void changed(int first, int last) = 0;

  virtual void clear() = 0;
  virtual void invalidate() = 0;
  virtual int heightHint() const = 0;
  
  virtual void activate() = 0;
  virtual void deactivate() = 0;

 protected:
  TokenEditView* view() const;
  TokenEditModeAccess* access() const;

 private:
  TokenEditView* _view;
  TokenEditModeAccess* _access;
};
