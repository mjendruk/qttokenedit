#include <qtadvwidgets/OmissionToken.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEditDisplayMode.h>
#include <qtadvwidgets/TokenEditView.h>

#include <QBoxLayout>
#include <QVariant>

#include "FinalWidgetPlaceholder.h"

TokenEditDisplayMode::TokenEditDisplayMode(TokenEditView* view,
                                           AbstractTokenEditModeAccess* access,
                                           QObject* parent)
    : TokenEditMode{view, access, parent},
      _omissionToken{new OmissionToken{access->dragDropHandler(), view}} {
        _omissionToken->hide();
      }

TokenEditDisplayMode::~TokenEditDisplayMode() {}

void TokenEditDisplayMode::inserted(int first, int last, UpdateFocus uf) {
  if (view()->count() < first) {
    return;
  }

  for (auto index = first; index <= last; ++index) {
    auto token = access()->createToken(index);
    view()->insert(index, token, uf);
  }

  invalidate();
}

void TokenEditDisplayMode::removed(int first, int last, UpdateFocus uf) {
  if (view()->count() <= last) {
    return;
  }

  for (int index = last; index >= first; --index) {
    view()->remove(index, uf);
  }

  invalidate();
}

void TokenEditDisplayMode::moved(int first, int last, int to) {
  auto const realTo = to - 1;

  if (view()->count() <= first && view()->count() <= realTo) {
    return;
  }

  if (view()->count() <= first) {
    inserted(realTo, realTo + (last - first), UpdateFocus::No);
  } else if (view()->count() <= realTo) {
    removed(first, last, UpdateFocus::No);
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

void TokenEditDisplayMode::invalidate() {
  view()->freezeLayout();

  view()->takeFinalWidget();

  auto allTokensShown = [=]() { return view()->count() == access()->count(); };
  auto lineCountConstraintMet = [=]() {
    return view()->lineCount() <= access()->maxLineCount();
  };

  while (!allTokensShown() && lineCountConstraintMet()) {
    auto index = view()->count();
    auto token = access()->createToken(index);
    view()->add(token, UpdateFocus::No);
  }

  while (!lineCountConstraintMet()) {
    view()->remove(view()->count() - 1, UpdateFocus::No);
  }

  if (omittedTokens() > 0) {
    view()->setFinalWidget(_omissionToken);

    while (!lineCountConstraintMet()) {
      view()->remove(view()->count() - 1, UpdateFocus::No);
    }

    _omissionToken->setCount(omittedTokens());
  }

  view()->unfreezeLayout();
}

int TokenEditDisplayMode::heightHint() const {
  return view()->heightForWidth(view()->width());
}

void TokenEditDisplayMode::activate() {
  invalidate();
}

void TokenEditDisplayMode::deactivate() {
  view()->takeFinalWidget();
}

QWidget* TokenEditDisplayMode::omissionToken() const { return _omissionToken; }

int TokenEditDisplayMode::omittedTokens() const {
  return access()->count() - view()->count();
}
