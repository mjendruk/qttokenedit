#pragma once

#include <cstdint>
#include <memory>

#include <QtCore/QObject>

#include <qtadvwidgets/FocusChainElement.h>
#include <qtadvwidgets/Global.h>
#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class FocusChainNavigation;

class QTADVWIDGETS_API FocusChain : public QObject {
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
