#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEdit.h>
#include <qtadvwidgets/TokenEditDisplayMode.h>
#include <qtadvwidgets/TokenEditEditingMode.h>
#include <qtadvwidgets/TokenEditView.h>
#include <qtadvwidgets/TokenLineEdit.h>

#include <QLineEdit>
#include <QtDebug>
#include <QScrollBar>
#include <QApplication>
#include <QtGlobal>
#include <algorithm>
#include <QScrollArea>

TokenEdit::TokenEdit(QWidget* parent)
    : TokenEditFrame{parent},
      _view{new TokenEditView{this}},
      _activeMode{nullptr},
      _editingMode{new TokenEditEditingMode{_view, this, this}},
      _displayMode{new TokenEditDisplayMode{_view, this, this}},
      _scrollArea{new QScrollArea{this}},
      _maxLineCount{3},
      _dragEnabled{false},
      _removable{false},
      _model{nullptr},
      _rootModelIndex{QModelIndex{}},
      _modelColumn{0} {
  setWidget(_scrollArea);
        
  _view->setFocusPolicy(Qt::StrongFocus);
  _view->installEventFilter(this);
        
  _scrollArea->setFrameShape(QFrame::NoFrame);
  _scrollArea->setWidgetResizable(true);
  _scrollArea->setFocusPolicy(Qt::ClickFocus);
  _scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  auto dummyToken = QScopedPointer{new Token{"dummy"}};

  auto singleStep = dummyToken->sizeHint().height() + _view->ySpacing();
  _scrollArea->verticalScrollBar()->setSingleStep(singleStep);

  _scrollArea->setWidget(_view);
        
  connect(_view, &TokenEditView::gotFocus, [=](auto widget) {
    ensureVisible(widget);
  });
        
  connect(_view, &TokenEditView::sizeChanged, [=]() {
    _activeMode->invalidate();
    ensureVisible(focusWidget());
    updateHeight();
  });

  connect(_editingMode->lineEdit(), &TokenLineEdit::backspaceAtBeginning, [=]() {
    if (_model && !_view->isEmpty() && removable()) {
      _model->removeRow(_view->count() - 1);
    }
  });

  connect(qApp, &QApplication::focusChanged, this, &TokenEdit::onFocusChanged);
}

TokenEdit::~TokenEdit() = default;

int TokenEdit::maxLineCount() const { return _maxLineCount; }

void TokenEdit::setMaxLineCount(int count) {
  _maxLineCount = count;
  _activeMode->invalidate();
  updateHeight();
}

bool TokenEdit::dragEnabled() const { return _dragEnabled; }

void TokenEdit::setDragEnabled(bool enable) {
  if (_dragEnabled == enable) {
    return;
  }

  _dragEnabled = enable;
  emit dragStateChanged(enable);
}

bool TokenEdit::removable() const { return _removable; }

void TokenEdit::setRemovable(bool enable) {
  if (_removable == enable) {
    return;
  }

  _removable = enable;

  emit removableStateChanged(enable);

  _activeMode->invalidate();
  updateHeight();
}

QLineEdit* TokenEdit::lineEdit() const { return _editingMode->lineEdit(); }

QAbstractItemModel* TokenEdit::model() const { return _model; }

void TokenEdit::setModel(QAbstractItemModel* model) {
  if (_model == model) {
    return;
  }

  if (_model) {
    clear();

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

QModelIndex TokenEdit::rootIndex() const { return _rootModelIndex; }

void TokenEdit::setRootIndex(QModelIndex const& index) {
  if (_rootModelIndex == index) {
    return;
  }

  _rootModelIndex = index;

  onModelReset();
}

int TokenEdit::count() const { return _model->rowCount(); }

Token* TokenEdit::createToken(QString const& text, QWidget* parent) {
  auto token = new Token{text, parent};
  token->setDragEnabled(dragEnabled());
  token->setRemovable(removable());

  connect(token, &Token::removeClicked, [=]() {
    auto index = _view->indexOf(token);
    _model->removeRow(index);
    updateHeight();
  });

  connect(token, &Token::dragged,
          [=](auto target, auto hint) { onItemDragged(token, target, hint); });

  connect(this, &TokenEdit::dragStateChanged, token, &Token::setDragEnabled);
  connect(this, &TokenEdit::removableStateChanged, token, &Token::setRemovable);

  return token;
}

QString TokenEdit::text(int index) const {
  auto modelIndex = _model->index(index, _modelColumn, _rootModelIndex);
  return modelIndex.data(Qt::DisplayRole).toString();
}

bool TokenEdit::eventFilter(QObject* object, QEvent* event) {
  Q_ASSERT(_view == object);
  
  if (event->type() == QEvent::FocusIn) {
    setActiveMode(_editingMode);
    lineEdit()->setFocus();
    return true;
  }
  
  return false;
}

void TokenEdit::init() {
  setActiveMode(_displayMode);
  
  if (auto rows = _model->rowCount(_rootModelIndex); rows > 0) {
    onRowsInserted(_rootModelIndex, 0, (rows - 1));
  }
}

void TokenEdit::clear() {
  while (!_view->isEmpty()) {
    _view->remove(0);
  }
}

void TokenEdit::onRowsInserted(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  _activeMode->inserted(first, last);
}

void TokenEdit::onRowsRemoved(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  _activeMode->removed(first, last);
}

void TokenEdit::onRowsMoved(QModelIndex const& parent, int first, int last,
                            QModelIndex const& destination, int to) {
  if (parent != _rootModelIndex) {
    return;
  }

  if (parent == destination) {
    _activeMode->moved(first, last, to);
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

  if (roles.contains(Qt::DisplayRole)) {
    _activeMode->changed(topLeft.row(), bottomRight.row());
  }
}

void TokenEdit::onModelReset() {
  clear();
  init();
}

void TokenEdit::onItemDragged(Token* source, Token* target,
                              Token::DropHint hint) {
  auto const from = _view->indexOf(source);

  auto to = _view->indexOf(target);

  Q_ASSERT(to >= 0);

  if (hint == Token::DropHint::After) ++to;

  to = from > to ? to + 1 : to;  // wrong implementation of QStringListModel
  // https://code.woboq.org/qt5/qtbase/src/corelib/itemmodels/qstringlistmodel.cpp.html#34fromRow

  _model->moveRow(_rootModelIndex, from, _rootModelIndex, to);
}

void TokenEdit::onFocusChanged(QWidget* prev, QWidget* now) {
  auto isChild = [=](QWidget* widget) {
    return widget && (widget->parent() == _view || widget == _scrollArea ||
                      widget == _view);
  };

  auto prevIsChild = isChild(prev);
  auto nowIsChild = isChild(now);
    
  if (prevIsChild && !nowIsChild) {
    setActiveMode(_displayMode);
    setShownAsFocused(false);
  } else if (!prevIsChild && nowIsChild) {
    setActiveMode(_editingMode);
    setShownAsFocused(true);
  }
}

void TokenEdit::setActiveMode(TokenEditMode* mode) {
  Q_ASSERT(mode);
  
  if (_activeMode == mode) {
    return;
  }
  
  if (_activeMode) {
    _activeMode->deactivate();
  }
  
  mode->activate();
  _activeMode = mode;
}
        
void TokenEdit::updateHeight() {
  auto height = _activeMode->heightHint();
        
  if (_scrollArea->height() != height) {
    _scrollArea->setFixedHeight(height);
  }
}

void TokenEdit::ensureVisible(QWidget* widget) {
  auto ySpacing = _view->ySpacing();
  
  // y-margin is not properly resprected for line edit
  if (widget == lineEdit()) {
    ySpacing *= 2;
  }
  
  _scrollArea->ensureWidgetVisible(widget, _view->xSpacing(), ySpacing);
}
