#include <qtadvwidgets/FlexLayout.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenChain.h>
#include <qtadvwidgets/TokenEdit.h>
#include <qtadvwidgets/TokenEditViewport.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <QLineEdit>
#include <QPainter>
#include <QScrollBar>
#include <QtGlobal>

TokenEdit::TokenEdit(TokenEditMode mode, QWidget* parent)
    : TokenEditViewport{parent},
      _lineEdit{new TokenLineEdit{this}},
      _tokenChain{new TokenChain{mode, _lineEdit, this}},
      _maxLineCount{-1},
      _spacing{3},
      _mode{mode},
      _scrollArea(new QScrollArea{this}),
      _model{nullptr},
      _rootModelIndex{QModelIndex{}},
      _modelColumn{0} {
  _scrollArea->setFocusPolicy(Qt::ClickFocus);

  connect(_tokenChain, &TokenChain::gotFocus,
          [=](auto element) { setShownAsFocused(true);
          _scrollArea->ensureWidgetVisible(element->widget());
          });

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
    if (_model && !isEmpty()) {
      _model->removeRow(count() - 1);
    }
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

int TokenEdit::count() const { return _items.size(); }

int TokenEdit::isEmpty() const { return _items.isEmpty(); }

QLineEdit* TokenEdit::lineEdit() { return _lineEdit; }

QAbstractItemModel* TokenEdit::model() const
{
  return _model;
}

void TokenEdit::setModel(QAbstractItemModel* model)
{
  if (_model == model) {
    return;
  }
  
  clear();
  
  if (_model) {
    _model->disconnect(this);
    
    if (_model->parent() == this) {
      delete _model;
    }
  }
  
  _model = model;
  
  connect(_model, &QAbstractItemModel::rowsInserted, this,
          &TokenEdit::onRowsInserted);

  connect(_model, &QAbstractItemModel::rowsRemoved, this,
          &TokenEdit::onRowsRemoved);

  connect(_model, &QAbstractItemModel::rowsMoved, this, &TokenEdit::onRowsMoved);
  connect(_model, &QAbstractItemModel::dataChanged, this, &TokenEdit::onDataChanged);
  connect(_model, &QAbstractItemModel::modelReset, this, &TokenEdit::onModelReset);
  
  init();
}

int TokenEdit::modelColumn() const
{
  return _modelColumn;
}

void TokenEdit::setModelColumn(int column)
{
  if (_modelColumn == column) {
    return;
  }
  
  _modelColumn = column;
  
  onModelReset();
}

void TokenEdit::addItem(QString const& text) {
  auto const index = _items.size();
  insertItem(index, text);
}

void TokenEdit::insertItem(int index, QString const& text) {
  Q_ASSERT(_mode == Mode::Multiple ||
           (_mode == Mode::Single && _items.empty() && index == 0));

  auto item = new Token{text, this};

  if (_mode == Mode::Single) {
    _lineEdit->hide();
    _scrollArea->setFocusProxy(item);
  }

  _items.insert(index, item);
  _layout->insertWidget(index, item);
  _tokenChain->insert(index, item->chainElement());

  connect(item, &Token::removeClicked, [=]() {
    auto index = _items.indexOf(item);
    _model->removeRow(index);
  });
}

void TokenEdit::setItemText(int index, QString const& text) {
  auto item = _items.at(index);
  item->setText(text);
  _layout->invalidate();
  updateHeight();
}

void TokenEdit::removeItem(int index) {
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

void TokenEdit::init() {
  if (auto rows = _model->rowCount(_rootModelIndex); rows > 0) {
    onRowsInserted(_rootModelIndex, 0, (rows - 1));
  }
}

void TokenEdit::clear() {
  if (!isEmpty()) {
    onRowsRemoved(_rootModelIndex, 0, count() - 1);
  }
}

void TokenEdit::onRowsInserted(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  for (auto row = first; row <= last; ++row) {
    auto const index = _model->index(row, _modelColumn, parent);
    auto const text = index.data(Qt::DisplayRole).toString();
    insertItem(row, text);
  }
}

void TokenEdit::onRowsRemoved(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  auto index = first;
  for (auto i = first; i <= last; ++i) {
    removeItem(index);
  }
}

void TokenEdit::onRowsMoved(QModelIndex const& parent, int first, int last,
                   QModelIndex const& destination, int row)
{
  if (parent != _rootModelIndex) {
    return;
  }

  onRowsRemoved(parent, first, last);

  if (parent == destination) {
    onRowsInserted(parent, row, row + (last - first));
  }
}

void TokenEdit::onDataChanged(const QModelIndex& topLeft,
                              const QModelIndex& bottomRight,
                              const QVector<int>& roles) {
  if (topLeft.parent() != _rootModelIndex) {
    return;
  }

  if (topLeft.column() > _modelColumn || bottomRight.column() < _modelColumn) {
    return;
  }

  if (!roles.contains(Qt::DisplayRole) && !roles.contains(Qt::UserRole)) {
    return;
  }

  auto const parent = topLeft.parent();

  if (roles.contains(Qt::DisplayRole)) {
    for (auto row = topLeft.row(); row <= bottomRight.row(); ++row) {
      auto const index = _model->index(row, _modelColumn, parent);

      setItemText(row, _model->data(index, Qt::DisplayRole).toString());
    }
  }
}

void TokenEdit::onModelReset()
{
  clear();
  init();
}
