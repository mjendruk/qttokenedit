#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEditEditingMode.h>
#include <qtadvwidgets/TokenEditView.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <QBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

TokenEditEditingMode::TokenEditEditingMode(TokenEditView* view,
                                           AbstractTokenEditModeAccess* access,
                                           QObject* parent)
    : TokenEditMode{view, access, parent}, _lineEdit{new TokenLineEdit{}} {
  auto dummyToken = QScopedPointer{new Token{}};
  _lineEdit->setFixedHeight(dummyToken->sizeHint().height());
}

TokenEditEditingMode::~TokenEditEditingMode() {
  if (_lineEdit->parent() == nullptr) {
    delete _lineEdit;
  }
}

void TokenEditEditingMode::inserted(int first, int last) {
  for (auto index = first; index <= last; ++index) {
    auto token = access()->createToken(index);

    view()->insert(index, token);
  }
}

void TokenEditEditingMode::removed(int first, int last) {
  for (int index = last; index >= first; --index) {
    view()->remove(index);
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

void TokenEditEditingMode::clear() {
  while (!view()->isEmpty()) {
    view()->remove(0);
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
  view()->setFinalWidget(_lineEdit,
                         new LineEditFocusChainNavigation{_lineEdit});
  _lineEdit->show();

  if (view()->count() < access()->count()) {
    inserted(view()->count(), access()->count() - 1);
  }
}

void TokenEditEditingMode::deactivate() {
  view()->takeFinalWidget();
  _lineEdit->hide();
}

TokenLineEdit* TokenEditEditingMode::lineEdit() const { return _lineEdit; }
