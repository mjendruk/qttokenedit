#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>
#include <QWidget>

class QTADVWIDGETS_API TokenChainElement : public QObject {
  Q_OBJECT
 public:
  TokenChainElement(QWidget* widget, bool isToken = true);

  TokenChainElement* previousElement() const;
  void setPreviousElement(TokenChainElement* element);

  TokenChainElement* nextElement() const;
  void setNextElement(TokenChainElement* element);

  QWidget* widget() const;

 signals:
  void gotFocus(TokenChainElement* element);
  void lostFocus(TokenChainElement* element);

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  TokenChainElement* _previousElement = nullptr;
  TokenChainElement* _nextElement = nullptr;
  QWidget* _widget = nullptr;
};
