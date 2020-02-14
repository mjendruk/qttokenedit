#pragma once

#include <token-edit/BaseToken.h>
#include <token-edit/TokenDropTarget.h>
#include <token-edit/token-edit-api.h>

namespace mjendruk {

class TOKEN_EDIT_API OmissionToken : public TokenDropTarget<BaseToken> {
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
