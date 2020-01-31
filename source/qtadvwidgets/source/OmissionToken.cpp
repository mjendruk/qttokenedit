#include <qtadvwidgets/OmissionToken.h>
#include <qtadvwidgets/PlusButton.h>

OmissionToken::OmissionToken(QWidget* parent)
    : BaseToken{parent},
      _count{-1} {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setCount(0);
}

int OmissionToken::count() const { return _count; }

void OmissionToken::setCount(int count) {
  if (_count == count) {
    return;
  }

  setText(QString{"+ %1 more"}.arg(count));

  _count = count;
}
