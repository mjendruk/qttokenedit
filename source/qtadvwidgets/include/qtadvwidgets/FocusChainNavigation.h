#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>

class QKeyEvent;

class FocusChainNavigation : public QObject {
 public:
  using QObject::QObject;

  virtual bool focusPrevious(QKeyEvent* event) const = 0;
  virtual bool focusNext(QKeyEvent* event) const = 0;
};

class QTADVWIDGETS_API DefaultFocusChainNavigation
    : public FocusChainNavigation {
 public:
  using FocusChainNavigation::FocusChainNavigation;

  bool focusPrevious(QKeyEvent* event) const override;
  bool focusNext(QKeyEvent* event) const override;
};
