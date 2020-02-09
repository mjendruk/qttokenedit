#pragma once

#include <cstdint>
#include <memory>

#include <QtCore/QObject>

#include <token-edit/FocusChainElement.h>
#include <token-edit/Global.h>
#include <token-edit/TokenEditMode.h>
#include <token-edit/token-edit-api.h>

namespace mjendruk {

class FocusChainNavigation;

class TOKEN_EDIT_API FocusChain : public QObject {
  Q_OBJECT

 public:
  FocusChain(QObject* parent = nullptr);
  ~FocusChain();

  void add(QWidget* widget, UpdateFocus updateFocus,
           FocusChainNavigation* navigation = nullptr);

  void insert(int index, QWidget* widget, UpdateFocus updateFocus,
              FocusChainNavigation* navigation = nullptr);

  void move(int from, int to);

  void remove(QWidget* widget, UpdateFocus updateFocus);

  int count() const;

  bool isEmpty() const;

 signals:
  void gotFocus(QWidget* widget);
  void lostFocus(QWidget* widget);

 private:
  enum class UpdateSignalConnection : std::int8_t { Yes, No };

  void insert(int index, FocusChainElement* element,
              UpdateSignalConnection updateConnection);

  FocusChainElement* takeAt(int index, UpdateSignalConnection updateConnection,
                            UpdateFocus updateFocus);

  int indexOf(QWidget* widget) const;

 private:
  QVector<FocusChainElement*> _elements;
};

}  // namespace mjendruk
