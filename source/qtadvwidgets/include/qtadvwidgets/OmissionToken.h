#pragma once

#include <qtadvwidgets/BaseToken.h>
#include <qtadvwidgets/TokenDropTarget.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

namespace mjendruk {

class QTADVWIDGETS_API OmissionToken : public TokenDropTarget<BaseToken> {
  Q_OBJECT

 public:
  OmissionToken(AbstractTokenDragDropHandler* handler,
                QWidget* parent = nullptr);

  int count() const;
  void setCount(int count);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  int _count;
};

}  // namespace mjendruk