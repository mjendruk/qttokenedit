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

TokenEdit::TokenEdit(QWidget* parent)
    : TokenEditViewport{parent},
      _lineEdit{new TokenLineEdit{this}},
      _tokenChain{new TokenChain{_lineEdit, this}},
      _maxLineCount{-1},
      _spacing{4},
      _scrollArea(new QScrollArea{this}) {
  connect(_tokenChain, &TokenChain::gotFocus,
          [=](auto element) { setShownAsFocused(true); });

  connect(_tokenChain, &TokenChain::lostFocus,
          [=](auto element) { setShownAsFocused(false); });

  setWidget(_scrollArea);

  auto mainWidget = new QWidget{};

  _layout = new FlexLayout{4, _spacing, _spacing};

  connect(_layout, &FlexLayout::linesChanged, this, &TokenEdit::updateHeight);

  _lineEdit->setFrame(false);
  _lineEdit->setPlaceholderText("Halt hinzufuegen");
  _lineEdit->setTextMargins(QMargins{});

  auto dummyItem = QScopedPointer{new Token{"dummy"}};
  _lineEdit->setFixedHeight(dummyItem->sizeHint().height());

  connect(_lineEdit, &TokenLineEdit::backspaceAtBeginning, [=]() {
    if (!_items.empty()) {
      removeItem(_items.size() - 1);
    }
  });

  connect(_lineEdit, &TokenLineEdit::focused, [=]() {
    _scrollArea->ensureWidgetVisible(_lineEdit);
  });

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

  setFocusProxy(_lineEdit);

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
  auto item = new Token{text, userData, this};
  auto index = std::max(0, _items.size());

  _items.append(item);
  _tokenChain->add(item->chainElement());
  _layout->insertWidget(index, item);

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
