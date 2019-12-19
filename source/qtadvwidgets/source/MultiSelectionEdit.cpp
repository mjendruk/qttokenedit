#include <qtadvwidgets/FlexLayout.h>
#include <qtadvwidgets/MultiSelectionEdit.h>
#include <qtadvwidgets/RemovableSelection.h>
#include <qtadvwidgets/MultiSelectionLineEdit.h>

#include <QtGlobal>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFrame>

MultiSelectionEdit::MultiSelectionEdit(QWidget* parent) : QScrollArea{parent} {
  auto mainWidget = new QWidget{};

  _layout = new FlexLayout{4, 4, 4};

  _lineEdit = new MultiSelectionLineEdit{this};
  _lineEdit->setFrame(false);
  _lineEdit->setMinimumSize(QSize{120, 0});
  _lineEdit->setPlaceholderText("Halt hinzufuegen");
  _lineEdit->setTextMargins(QMargins{});

  connect(_lineEdit, &MultiSelectionLineEdit::backspaceAtBeginning, [=]() {
    if (!_items.empty()) {
      removeItem(_items.size() - 1);
    }
  });

  auto dummyItem = QScopedPointer{new RemovableSelection{"dummy"}};
  _lineEdit->setFixedHeight(dummyItem->sizeHint().height());

  _layout->addWidget(_lineEdit);

  mainWidget->setLayout(_layout);

  mainWidget->setStyleSheet("#mainWidget { background-color: white; }");
  mainWidget->setObjectName("mainWidget");

  setWidget(mainWidget);
  
  setFocusProxy(_lineEdit);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setWidgetResizable(true);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  updateHeight();
}

int MultiSelectionEdit::maxLineCount() const { return _maxLineCount; }

void MultiSelectionEdit::setMaxLineCount(int count) {
  _maxLineCount = count;
  updateHeight();
}

void MultiSelectionEdit::addItem(QString const& text,
                                 QVariant const& userData) {
  auto item = new RemovableSelection{text, userData, this};
  auto index = std::max(0, _items.size());

  _items.append(item);
  _layout->insertWidget(index, item);

  connect(item, &RemovableSelection::removeClicked, [=]() {
    auto index = _items.indexOf(item);
    removeItem(index);
  });

  updateHeight();
}

void MultiSelectionEdit::addItems(QStringList const& texts)
{
  for (auto const& text : texts) {
    addItem(text);
  }
}

void MultiSelectionEdit::setItemData(int index, QVariant const& value)
{
  auto item = _items.at(index);
  item->setUserData(value);
}

void MultiSelectionEdit::setItemText(int index, QString const& text)
{
  auto item = _items.at(index);
  item->setText(text);
  _layout->activate();
  updateHeight();
}

void MultiSelectionEdit::removeItem(int index)
{
  auto item = _items.takeAt(index);
  auto layoutItem = _layout->takeAt(index);
  emit itemAboutToBeRemoved(index);
  delete layoutItem;
  delete item;
  updateHeight();
}

QString MultiSelectionEdit::itemText(int index) const
{
  auto item = _items.at(index);
  return item->text();
}

QVariant MultiSelectionEdit::itemData(int index) const
{
  auto item = _items.at(index);
  return item->userData();
}

int MultiSelectionEdit::count() const
{
  return _items.size();
}

QLineEdit* MultiSelectionEdit::lineEdit() 
{
  return _lineEdit;
}

void MultiSelectionEdit::resizeEvent(QResizeEvent* event) {
  QScrollArea::resizeEvent(event);
  updateHeight();
}

void MultiSelectionEdit::updateHeight() {
  auto const actualMaxRows = _maxLineCount <= 0 ? 3 : _maxLineCount;
  auto const actualRows = std::min(actualMaxRows, _layout->lineCount());

  auto const spacing = 4;

  auto height = spacing;

  for (auto i = 0; i < actualRows; ++i) {
    height += _layout->lineHeight(i);
    height += spacing;
  }
  
  height += 2;

  setFixedHeight(height);
}
