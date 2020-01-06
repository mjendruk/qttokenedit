#include <qtadvwidgets/FlexLayout.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenChain.h>
#include <qtadvwidgets/TokenEdit.h>
#include <qtadvwidgets/TokenEditViewport.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <algorithm>

#include <QDrag>
#include <QLineEdit>
#include <QMimeData>
#include <QMouseEvent>
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
      _modelColumn{0},
      _mousePressedPosition{},
      _draggedTokenIndex{-1},
      _tokenDragMimeType{"application/x-tokenedit-token"} {
  setAcceptDrops(true);
  _scrollArea->setFocusPolicy(Qt::ClickFocus);

  connect(_tokenChain, &TokenChain::gotFocus, [=](auto element) {
    setShownAsFocused(true);
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

QAbstractItemModel* TokenEdit::model() const { return _model; }

void TokenEdit::setModel(QAbstractItemModel* model) {
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
  
  if (_model != nullptr) {
    connect(_model, &QAbstractItemModel::rowsInserted, this,
            &TokenEdit::onRowsInserted);
    connect(_model, &QAbstractItemModel::rowsRemoved, this,
            &TokenEdit::onRowsRemoved);
    connect(_model, &QAbstractItemModel::rowsMoved, this,
            &TokenEdit::onRowsMoved);
    connect(_model, &QAbstractItemModel::dataChanged, this,
            &TokenEdit::onDataChanged);
    connect(_model, &QAbstractItemModel::modelReset, this,
            &TokenEdit::onModelReset);

    init();
  } else {
    
  }
}

int TokenEdit::modelColumn() const { return _modelColumn; }

void TokenEdit::setModelColumn(int column) {
  if (_modelColumn == column) {
    return;
  }

  _modelColumn = column;

  onModelReset();
}

void TokenEdit::mousePressEvent(QMouseEvent* event) {
  TokenEditViewport::mousePressEvent(event);
  
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }

  auto tokenIt = std::find_if(_items.cbegin(), _items.cend(), [=](Token* item) {
    auto pos = item->mapTo(this, QPoint{0, 0});
    auto rect = QRect(pos, item->size());
    
    return rect.contains(event->pos());
  });
  
  if (tokenIt != _items.cend()) {
    _mousePressedPosition = event->pos();
    _draggedTokenIndex = _items.indexOf(*tokenIt);
  }
}

void TokenEdit::mouseMoveEvent(QMouseEvent* event) {
  TokenEditViewport::mouseMoveEvent(event);
  
  if (_mousePressedPosition.isNull()) {
    return;
  }

  auto const distance = event->pos() - _mousePressedPosition;

  if (distance.manhattanLength() < 10) {
    return;
  }

  Q_ASSERT(_draggedTokenIndex >= 0);

  auto token = _items.at(_draggedTokenIndex);

  auto hotSpot = event->pos() - token->pos();

  Q_ASSERT(_draggedTokenIndex >= 0);

  auto mimeData = new QMimeData{};
  mimeData->setData(_tokenDragMimeType, QByteArray::number(_draggedTokenIndex));

  auto drag = new QDrag{this};
  drag->setMimeData(mimeData);
  drag->setHotSpot(hotSpot);
  drag->setPixmap(token->toPixmap());

  if (auto dragAction = drag->exec(Qt::MoveAction);
      dragAction == Qt::MoveAction) {
    _mousePressedPosition = QPoint{};
  }
}

void TokenEdit::mouseReleaseEvent(QMouseEvent* event) {
  _mousePressedPosition = QPoint{};
}

void TokenEdit::leaveEvent(QEvent* event) { _mousePressedPosition = QPoint{}; }

void TokenEdit::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasFormat(_tokenDragMimeType) &&
      event->source() == this) {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  } else {
    event->ignore();
  }
}

void TokenEdit::dragMoveEvent(QDragMoveEvent* event) {
  if (event->mimeData()->hasFormat(_tokenDragMimeType) &&
      event->source() == this) {    
    event->setDropAction(Qt::MoveAction);
    event->accept();
  } else {
    event->ignore();
  }
}

void TokenEdit::dropEvent(QDropEvent* event) {
  if (event->mimeData()->hasFormat(_tokenDragMimeType) &&
      event->source() == this) {
    auto const index = event->mimeData()->data(_tokenDragMimeType).toInt();
    
    auto tokenIt = std::find_if(_items.cbegin(), _items.cend(), [=](Token* item) {
      auto pos = item->mapTo(this, QPoint{0, 0});
      auto rect = QRect(pos, item->size());
      
      rect += QMargins{_spacing, _spacing, _spacing, _spacing};
      
      return rect.contains(event->pos());
    });
    
    if (tokenIt != _items.cend()) {
      
      auto droppedOverToken = *tokenIt;
      
      if (droppedOverToken) {
        auto const droppedOverIndex = _items.indexOf(droppedOverToken);
        
        auto const globalPos = mapToGlobal(event->pos());
        auto const pos = droppedOverToken->mapFromGlobal(globalPos);
        
        
        auto const from = index;
        
        auto to = droppedOverIndex;
        
        if (pos.x() > droppedOverToken->width() / 2.0) {
          to += 1;
        }
        
        to = from > to ? to + 1 : to;  // wrong implementation of QStringListModel
        // https://code.woboq.org/qt5/qtbase/src/corelib/itemmodels/qstringlistmodel.cpp.html#34fromRow
        
        auto success = _model->moveRow(_rootModelIndex, from, _rootModelIndex, to);
        
        if (success) {
          event->setDropAction(Qt::MoveAction);
          event->accept();
          return;
        }
      }
    }
  }
          
  event->ignore();
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

void TokenEdit::moveItem(int from, int to)
{
  Q_ASSERT(_mode == Mode::Multiple || (_mode == Mode::Single && from == to));
  
  if (from == to) {
    return;
  }
  
  auto insertAt = to - 1;
  
  if (to < _items.size()) {
    _items.move(from, insertAt);
  } else {
    _items.append(_items.takeAt(from));
  }
  
  auto layoutItem = _layout->takeAt(from);
  _layout->insertItem(insertAt, layoutItem);
  
  _tokenChain->move(from, insertAt);
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
  _lineEdit->show();
  
  if (auto rows = _model->rowCount(_rootModelIndex); rows > 0) {
    onRowsInserted(_rootModelIndex, 0, (rows - 1));
  }
}

void TokenEdit::clear() {
  if (!isEmpty()) {
    onRowsRemoved(_rootModelIndex, 0, count() - 1);
  }
  
  _lineEdit->hide();
}

void TokenEdit::onRowsInserted(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  for (auto row = first; row <= last; ++row) {
    auto const index = _model->index(row, _modelColumn, parent);
    auto const text = index.data(Qt::DisplayRole).toString();
    auto stdText = text.toStdString();
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
                            QModelIndex const& destination, int to) {
  if (parent != _rootModelIndex) {
    return;
  }
  
  if (parent == destination) {
    for (int from = last; from >= last; --from) {
      moveItem(from, to);
    }
  } else {
    onRowsRemoved(parent, first, last);
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

void TokenEdit::onModelReset() {
  clear();
  init();
}
