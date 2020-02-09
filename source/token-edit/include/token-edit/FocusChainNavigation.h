#pragma once

#include <QtCore/QObject>

#include <token-edit/token-edit-api.h>

class QKeyEvent;

namespace mjendruk {

class FocusChainNavigation : public QObject {
 public:
  using QObject::QObject;

  virtual bool focusPrevious(QKeyEvent* event) const = 0;
  virtual bool focusNext(QKeyEvent* event) const = 0;
};

class TOKEN_EDIT_API DefaultFocusChainNavigation
    : public FocusChainNavigation {
 public:
  using FocusChainNavigation::FocusChainNavigation;

  bool focusPrevious(QKeyEvent* event) const override;
  bool focusNext(QKeyEvent* event) const override;
};

}  // namespace mjendruk