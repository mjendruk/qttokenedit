#pragma once

#include <qtadvwidgets/FocusChainElement.h>
#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>
#include <cstdint>
#include <memory>

class FocusChainNavigation;

class QTADVWIDGETS_API FocusChain : public QObject {
  Q_OBJECT
  
 public:
  FocusChain(QObject* parent = nullptr);
  ~FocusChain();

  void add(QWidget* widget, FocusChainNavigation* navigation = nullptr);
  void insert(int index, QWidget* widget, FocusChainNavigation* navigation = nullptr);
  void move(int from, int to);
  void remove(QWidget* widget);
  
  int count() const;
  bool isEmpty() const;

 signals:
  void gotFocus(QWidget* widget);
  void lostFocus(QWidget* widget);

 private:
  enum class UpdateSignalConnection : std::int8_t { Yes, No };
  enum class UpdateFocus : std::int8_t { Yes, No };

  void insert(int index, FocusChainElement* element,
              UpdateSignalConnection updateConnection);

  FocusChainElement* takeAt(int index, UpdateSignalConnection updateConnection,
                            UpdateFocus updateFocus);

  int indexOf(QWidget* widget) const;

 private:
  QVector<FocusChainElement*> _elements;
};
