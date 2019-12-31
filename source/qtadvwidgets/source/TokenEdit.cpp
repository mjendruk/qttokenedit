#include <qtadvwidgets/FlexLayout.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenChain.h>
#include <qtadvwidgets/TokenEdit.h>
#include <qtadvwidgets/TokenEditViewport.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOptionFrame>
#include <QtGlobal>

TokenEdit::TokenEdit(TokenEditMode mode, QWidget* parent)
    : TokenEditViewport{parent},
      _lineEdit{new TokenLineEdit{this}},
      _tokenChain{new TokenChain{mode, _lineEdit, this}},
      _maxLineCount{-1},
      _spacing{3},
      _mode{mode},
      _scrollArea(new QScrollArea{this}) {
  _scrollArea->setFocusPolicy(Qt::ClickFocus);

  connect(_tokenChain, &TokenChain::gotFocus,
          [=](auto element) { setShownAsFocused(true); });

  connect(_tokenChain, &TokenChain::lostFocus,
          [=](auto element) { setShownAsFocused(false); });

  setWidget(_scrollArea);

  auto mainWidget = new QWidget{};
  mainWidget->setFocusPolicy(Qt::NoFocus);

  _layout = new FlexLayout{3, _spacing, _spacing};

  connect(_layout, &FlexLayout::linesChanged, this, &TokenEdit::updateHeight);

  _lineEdit->setFrame(false);
  _lineEdit->setTextMargins(QMargins{});

  auto dummyItem = QScopedPointer{new Token{"dummy"}};
  _lineEdit->setFixedHeight(dummyItem->sizeHint().height());

  connect(_lineEdit, &TokenLineEdit::backspaceAtBeginning, [=]() {
    if (!_items.empty()) {
      removeItem(_items.size() - 1);
    }
  });

  connect(_lineEdit, &TokenLineEdit::focused,
          [=]() { _scrollArea->ensureWidgetVisible(_lineEdit); });

  _layout->addWidget(_lineEdit);

  mainWidget->setLayout(_layout);

  auto customPalette = mainWidget->palette();
  customPalette.setColor(
      QPalette::Disabled, QPalette::Window,
      customPalette.color(QPalette::Disabled, QPalette::Base));
  customPalette.setColor(QPalette::Active, QPalette::Window,
                         customPalette.color(QPalette::Active, QPalette::Base));
  customPalette.setColor(
      QPalette::Inactive, QPalette::Window,
      customPalette.color(QPalette::Inactive, QPalette::Base));
  mainWidget->setPalette(customPalette);

  _scrollArea->setWidget(mainWidget);

  _scrollArea->setFocusProxy(_lineEdit);

  _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  _scrollArea->setWidgetResizable(true);
  _scrollArea->setFrameShape(QFrame::NoFrame);

  _scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  updateHeight();
}

TokenEdit::~TokenEdit() = default;

int TokenEdit::maxLineCount() const { return _maxLineCount; }

void TokenEdit::setMaxLineCount(int count) {
  _maxLineCount = count;
  updateHeight();
}

void TokenEdit::addItem(QString const& text, QVariant const& userData) {
  Q_ASSERT(_mode == Mode::Multiple ||
           (_mode == Mode::Single && _items.empty()));

  auto item = new Token{text, userData, this};
  auto index = std::max(0, _items.size());

  if (_mode == Mode::Single) {
    _lineEdit->hide();
    _scrollArea->setFocusProxy(item);
  }

  _items.append(item);
  _layout->insertWidget(index, item);
  _tokenChain->add(item->chainElement());

  connect(item, &Token::removeClicked, [=]() {
    auto index = _items.indexOf(item);
    removeItem(index);
  });

  connect(item, &Token::focused,
          [=]() { _scrollArea->ensureWidgetVisible(item); });
}

void TokenEdit::addItems(QStringList const& texts) {
  for (auto const& text : texts) {
    addItem(text);
  }
}

void TokenEdit::setItemData(int index, QVariant const& value) {
  auto item = _items.at(index);
  item->setUserData(value);
}

void TokenEdit::setItemText(int index, QString const& text) {
  auto item = _items.at(index);
  item->setText(text);
  _layout->activate();
  updateHeight();
}

void TokenEdit::removeItem(int index) {
  emit itemAboutToBeRemoved(index);
  auto item = _items.takeAt(index);
  auto layoutItem = _layout->takeAt(index);

  if (_mode == Mode::Single) {
    _lineEdit->show();
    _scrollArea->setFocusProxy(item);
  }

  _tokenChain->remove(item->chainElement());
  delete layoutItem;
  delete item;
}

QString TokenEdit::itemText(int index) const {
  auto item = _items.at(index);
  return item->text();
}

QVariant TokenEdit::itemData(int index) const {
  auto item = _items.at(index);
  return item->userData();
}

int TokenEdit::count() const { return _items.size(); }

QLineEdit* TokenEdit::lineEdit() { return _lineEdit; }

void TokenEdit::updateHeight() {
  auto const actualMaxRows = _maxLineCount <= 0 ? 3 : _maxLineCount;
  auto const actualRows = std::min(actualMaxRows, _layout->lineCount());

  auto height = _spacing;

  for (auto i = 0; i < actualRows; ++i) {
    height += _layout->lineHeight(i);
    height += _spacing;
  }

  height += 2;

  setFixedHeight(height);
  _scrollArea->verticalScrollBar()->triggerAction(
      QAbstractSlider::SliderToMaximum);
}
