#include <qtadvwidgets/TokenEditEditingMode.h>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>

#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEditView.h>
#include <qtadvwidgets/TokenLineEdit.h>

namespace mjendruk {

TokenEditEditingMode::TokenEditEditingMode(TokenEditView* view,
                                           AbstractTokenEditModeAccess* access,
                                           QObject* parent)
    : TokenEditMode{view, access, parent} {}

TokenEditEditingMode::~TokenEditEditingMode() {}

void TokenEditEditingMode::inserted(int first, int last, UpdateFocus uf) {
  for (auto index = first; index <= last; ++index) {
    auto token = access()->createToken(index);
    view()->insert(index, token, uf);
  }
}

void TokenEditEditingMode::removed(int first, int last, UpdateFocus uf) {
  for (int index = last; index >= first; --index) {
    view()->remove(index, uf);
  }
}

void TokenEditEditingMode::moved(int first, int last, int to) {
  for (int from = last; from >= first; --from) {
    view()->move(from, to);
  }
}

void TokenEditEditingMode::changed(int first, int last,
                                   QVector<int> const& roles) {
  for (auto index = first; index <= last; ++index) {
    auto token = view()->at(index);
    access()->updateToken(index, token, roles);
  }
}

void TokenEditEditingMode::invalidate() {}

int TokenEditEditingMode::heightHint() const {
  auto const actualRows =
      std::min(access()->maxLineCount(), view()->lineCount());

  auto spacing = view()->ySpacing();
  auto dummyToken = QScopedPointer{new Token{}};
  auto height =
      (dummyToken->sizeHint().height() + spacing) * actualRows + spacing;

  return height;
}

void TokenEditEditingMode::activate() {
  if (view()->count() < access()->count()) {
    inserted(view()->count(), access()->count() - 1, UpdateFocus::No);
  }
}

void TokenEditEditingMode::deactivate() {}

}  // namespace mjendruk