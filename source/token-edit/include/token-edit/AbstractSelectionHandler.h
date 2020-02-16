#pragma once

#include <QtCore/QObject>

namespace mjendruk {

class Token;

class AbstractSelectionHandler : public QObject {
  Q_OBJECT
  
 public:
  using QObject::QObject;

  virtual void select(Token const* token, Qt::MouseButtons buttons,
                      Qt::KeyboardModifiers modifiers) = 0;
};
}  // namespace mjendruk
