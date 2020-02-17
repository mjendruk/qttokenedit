#pragma once

#include <qttokenedit/BaseToken.h>
#include <qttokenedit/TokenDropTarget.h>
#include <qttokenedit/qttokenedit_api.h>

namespace mjendruk {

class QTTOKENEDIT_API OmissionToken : public TokenDropTarget<BaseToken> {
  Q_OBJECT

 public:
  explicit OmissionToken(AbstractTokenDragDropHandler* handler,
                         QWidget* parent = nullptr);

  int count() const;
  void setCount(int count);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  int _count;
};

}  // namespace mjendruk
