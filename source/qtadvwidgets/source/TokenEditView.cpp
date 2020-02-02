#include <qtadvwidgets/FlexLayout.h>
#include <qtadvwidgets/FocusChain.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEditView.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <QResizeEvent>
#include <cmath>

TokenEditView::TokenEditView(QWidget* parent)
    : QWidget{parent},
      _focusChain{new FocusChain{this}},
      _layout{new FlexLayout{margin(), xSpacing(), ySpacing(), this}},
      _finalWidget{nullptr} {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setBackgroundRole(QPalette::Base);
  setFocusPolicy(Qt::NoFocus);
  connect(_focusChain, &FocusChain::gotFocus, this,
          qOverload<QWidget*>(&TokenEditView::gotFocus));
  connect(_focusChain, &FocusChain::lostFocus, this,
          qOverload<QWidget*>(&TokenEditView::lostFocus));
}

Token* TokenEditView::at(int index) const { return _tokens.at(index); }

int TokenEditView::indexOf(Token* token) const {
  return _tokens.indexOf(token);
}

void TokenEditView::add(Token* token) { insert(_tokens.size(), token); }

void TokenEditView::insert(int index, Token* token) {
  Q_ASSERT(0 <= index && index <= count());

  _tokens.insert(index, token);
  _layout->insertWidget(index, token);
  _focusChain->insert(index, token);

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

  _focusChain->move(from, insertAt);
}

void TokenEditView::remove(int index) {
  Q_ASSERT(0 <= index && index < count());

  auto item = _tokens.takeAt(index);
  auto layoutItem = _layout->takeAt(index);
  _focusChain->remove(item);
  delete layoutItem;
  item->hide();
  item->deleteLater();
}

int TokenEditView::count() const { return _tokens.size(); }

int TokenEditView::isEmpty() const { return count() == 0; }

QVector<Token*> const& TokenEditView::tokens() const { return _tokens; }

QWidget* TokenEditView::takeFinalWidget() {
  if (!_finalWidget) {
    return nullptr;
  }

  auto widget = _finalWidget;

  _focusChain->remove(widget);
  _layout->removeWidget(widget);

  _finalWidget = nullptr;
  return widget;
}

void TokenEditView::setFinalWidget(QWidget* widget,
                                   FocusChainNavigation* navigation) {
  if (_finalWidget == widget) {
    return;
  }

  if (_finalWidget) {
    auto widget = takeFinalWidget();
    delete widget;
  }

  if (widget) {
    _focusChain->add(widget, navigation);
    _layout->addWidget(widget);
  }

  _finalWidget = widget;
}

int TokenEditView::xSpacing() const {
  return qRound(fontMetrics().height() * 0.4);
}

int TokenEditView::ySpacing() const {
  return qRound(fontMetrics().height() * 0.25);
}

int TokenEditView::margin() const { return ySpacing(); }

int TokenEditView::lineCount() const { return _layout->lineCountForWidth(width()); }

int TokenEditView::lineCountForWidth(int width) const {
  return _layout->lineCountForWidth(width);
}

void TokenEditView::freezeLayout() { _layout->freeze(); }

void TokenEditView::unfreezeLayout() { _layout->unfreeze(); }

void TokenEditView::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);

  emit sizeChanged(event->size());
}
