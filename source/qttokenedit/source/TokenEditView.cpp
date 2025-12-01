#include <qttokenedit/TokenEditView.h>

#include <cmath>

#include <QtGui/QResizeEvent>

#include <qttokenedit/FlexLayout.h>
#include <qttokenedit/Token.h>
#include <qttokenedit/TokenLineEdit.h>

namespace mjendruk {

TokenEditView::TokenEditView(QWidget* parent)
    : QWidget{parent},
      _layout{new FlexLayout{margin(), xSpacing(), ySpacing(), this}},
      _dummyToken{new Token{this}},
      _finalWidget{nullptr},
      _defaultFinalWidget{nullptr} {
  _dummyToken->hide();

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::NoFocus);
}

Token* TokenEditView::at(int index) const { return _tokens.at(index); }

Token* TokenEditView::find(int index) const {
  if (!(0 <= index && index < count())) {
    return nullptr;
  }

  return at(index);
}

int TokenEditView::indexOf(Token const* token) const {
  auto _token = const_cast<Token*>(token);
  return _tokens.indexOf(_token);
}

void TokenEditView::add(Token* token, UpdateFocus uf) {
  insert(_tokens.size(), token, uf);
}

void TokenEditView::insert(int index, Token* token, UpdateFocus uf) {
  Q_ASSERT(0 <= index && index <= count());

  _tokens.insert(index, token);
  _layout->insertWidget(index, token);

  updateGeometry();
}

void TokenEditView::move(int from, int to) {
  Q_ASSERT(0 <= from && from < count());
  Q_ASSERT(0 <= to && to <= count());

  if (from == to) {
    return;
  }

  auto insertAt = from > to ? to : (to - 1);

  if (to < _tokens.size()) {
    _tokens.move(from, insertAt);
  } else {
    _tokens.append(_tokens.takeAt(from));
  }

  auto layoutItem = _layout->takeAt(from);
  _layout->insertItem(insertAt, layoutItem);
}

void TokenEditView::remove(int index, UpdateFocus uf) {
  Q_ASSERT(0 <= index && index < count());

  auto item = _tokens.takeAt(index);
  auto layoutItem = _layout->takeAt(index);
  delete layoutItem;
  item->deleteLater();
}

int TokenEditView::count() const { return _tokens.size(); }

int TokenEditView::isEmpty() const { return count() == 0; }

QVector<Token*> const& TokenEditView::tokens() const { return _tokens; }

QWidget* TokenEditView::takeFinalWidget() { return takeFinalWidget(true); }

void TokenEditView::setFinalWidget(QWidget* widget) {
  if (_finalWidget == widget) {
    return;
  }

  if (_finalWidget) {
    auto setDefault = (widget == nullptr);
    auto old = takeFinalWidget(setDefault);

    if (old != _defaultFinalWidget) {
      delete old;
    }
  }

  if (widget) {
    _layout->addWidget(widget);
    widget->show();
  }

  _finalWidget = widget;
}

QWidget* TokenEditView::takeDefaultFinalWidget() {
  if (!_defaultFinalWidget) {
    return nullptr;
  }

  if (_defaultFinalWidget == _finalWidget) {
    takeFinalWidget(false);
  }

  auto widget = _defaultFinalWidget;

  _defaultFinalWidget = nullptr;

  return widget;
}

void TokenEditView::setDefaultFinalWidget(QWidget* widget) {
  if (_defaultFinalWidget == widget) {
    return;
  }

  if (_defaultFinalWidget) {
    auto old = takeDefaultFinalWidget();
    delete old;
  }

  if (!_finalWidget) {
    setFinalWidget(widget);
  }

  _defaultFinalWidget = widget;
}

int TokenEditView::xSpacing() const {
  return qRound(fontMetrics().height() * 0.4);
}

int TokenEditView::ySpacing() const {
  return qRound(fontMetrics().height() * 0.25);
}

int TokenEditView::margin() const { return ySpacing(); }

int TokenEditView::heightForWidth(int width) const
{
  if (lineCount() == 0) {
    // The flex layout will only give us the margins, because there's nothing in it.
    int marginsHeight = QWidget::heightForWidth(width);
    int dummyTokenHeight = _dummyToken->sizeHint().height();
    return marginsHeight + dummyTokenHeight;
  }

  return QWidget::heightForWidth(width);
}

int TokenEditView::lineCount() const {
  return _layout->lineCountForWidth(width());
}

int TokenEditView::lineCountForWidth(int width) const {
  return _layout->lineCountForWidth(width);
}

void TokenEditView::freezeLayout() { _layout->freeze(); }

void TokenEditView::unfreezeLayout() { _layout->unfreeze(); }

void TokenEditView::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);

  emit sizeChanged(event->size());
}

QWidget* TokenEditView::takeFinalWidget(bool setDefault) {
  if (!_finalWidget) {
    return nullptr;
  }

  if (setDefault && _finalWidget == _defaultFinalWidget) {
    return nullptr;
  }

  auto widget = _finalWidget;

  _layout->removeWidget(widget);
  widget->hide();
  _finalWidget = nullptr;

  if (setDefault) {
    setFinalWidget(_defaultFinalWidget);
  }

  return widget;
}

}  // namespace mjendruk
