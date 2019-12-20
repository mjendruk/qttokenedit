#pragma once

#include <qtadvwidgets/TokenChainElement.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QLineEdit>

class QTADVWIDGETS_API LineEditChainElement : public TokenChainElement {
  Q_OBJECT
 public:
  LineEditChainElement(QLineEdit* lineEdit);

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

 private:
  QLineEdit* _lineEdit;
};
