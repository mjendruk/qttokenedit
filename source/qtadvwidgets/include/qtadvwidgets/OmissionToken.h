#pragma once

#include <qtadvwidgets/BaseToken.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API OmissionToken : public BaseToken {
  Q_OBJECT

 public:
  OmissionToken(QWidget* parent = nullptr);

  int count() const;
  void setCount(int count);

 private:
  int _count;
};
