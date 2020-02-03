#include <qtadvwidgets/OmissionToken.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEditDisplayMode.h>
#include <qtadvwidgets/TokenEditView.h>

#include <QBoxLayout>
#include <QVariant>

TokenEditDisplayMode::TokenEditDisplayMode(TokenEditView* view,
                                           AbstractTokenEditModeAccess* access,
                                           QObject* parent)
    : TokenEditMode{view, access, parent},
      _omissionToken{new OmissionToken{}} {}

TokenEditDisplayMode::~TokenEditDisplayMode() {
  if (_omissionToken->parent() == nullptr) {
    delete _omissionToken;
  }
}

void TokenEditDisplayMode::inserted(int first, int last) {
  if (view()->count() < first) {
    return;
  }

  for (auto index = first; index <= last; ++index) {
    auto token = access()->createToken(index);

    view()->insert(index, token);
  }

  invalidate();
}

void TokenEditDisplayMode::removed(int first, int last) {
  if (view()->count() <= first) {
    return;
  }

  for (int index = last; index >= first; --index) {
    view()->remove(index);
  }

  invalidate();
}

void TokenEditDisplayMode::moved(int first, int last, int to) {
  auto const realTo = to - 1;

  if (view()->count() <= first && view()->count() <= realTo) {
    return;
  }

  if (view()->count() <= first) {
    inserted(realTo, realTo + (last - first));
  } else if (view()->count() <= realTo) {
    removed(first, last);
  } else {
    for (int from = last; from >= first; --from) {
      view()->move(from, to);
    }

    invalidate();
  }
}

void TokenEditDisplayMode::changed(int first, int last,
                                   QVector<int> const& roles) {
  if (view()->count() <= first) {
    return;
  }

  for (auto index = first; index <= last; ++index) {
    auto token = view()->at(index);
    access()->updateToken(index, token, roles);
  }

  invalidate();
}

void TokenEditDisplayMode::clear() {
  while (!view()->isEmpty()) {
    view()->remove(0);
  }
}

void TokenEditDisplayMode::invalidate() {
  view()->freezeLayout();

  _omissionToken->hide();

  auto allTokensShown = [=]() { return view()->count() == access()->count(); };
  auto lineCountConstraintMet = [=]() {
    return view()->lineCount() <= access()->maxLineCount();
  };

  while (!allTokensShown() && lineCountConstraintMet()) {
    auto index = view()->count();
    auto token = access()->createToken(index);
    view()->add(token);
  }

  while (!lineCountConstraintMet()) {
    view()->remove(view()->count() - 1);
  }

  if (omittedTokens() > 0) {
    _omissionToken->show();

    while (!lineCountConstraintMet()) {
      view()->remove(view()->count() - 1);
    }

    _omissionToken->setCount(omittedTokens());
  }

  view()->unfreezeLayout();
}

int TokenEditDisplayMode::heightHint() const {
  return view()->heightForWidth(view()->width());
}

void TokenEditDisplayMode::activate() {
  view()->setFinalWidget(_omissionToken);
  invalidate();
}

void TokenEditDisplayMode::deactivate() {
  view()->takeFinalWidget();
  _omissionToken->hide();
}

QWidget* TokenEditDisplayMode::omissionToken() const { return _omissionToken; }

int TokenEditDisplayMode::omittedTokens() const {
  return access()->count() - view()->count();
}
