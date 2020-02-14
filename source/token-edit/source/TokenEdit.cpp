#include <token-edit/TokenEdit.h>

#include <algorithm>

#include <QtCore/QScopedValueRollback>
#include <QtCore/QTimer>
#include <QtCore/QtGlobal>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>

#include <token-edit/Token.h>
#include <token-edit/TokenEditView.h>
#include <token-edit/TokenLineEdit.h>

#include "TokenDragDropHandler.h"
#include "TokenEditDisplayMode.h"
#include "TokenEditEditingMode.h"

namespace mjendruk {

class TokenEditModeAccess : public AbstractTokenEditModeAccess {
 public:
  explicit TokenEditModeAccess(TokenEdit* tokenEdit) : _tokenEdit{tokenEdit} {}

  int maxLineCount() const override { return _tokenEdit->maxLineCount(); }

  int count() const override {
    if (auto model = _tokenEdit->model()) {
      return model->rowCount();
    }
    return -1;
  }

  Token* createToken(int index, QWidget* parent) const override {
    auto token = new Token{_tokenEdit->dragDropHandler(), parent};
    token->setRemovable(_tokenEdit->removable());

    QObject::connect(token, &Token::removeClicked, [=]() {
      auto index = _tokenEdit->indexOf(token);
      _tokenEdit->remove(index, UpdateFocus::Yes);
      _tokenEdit->updateHeight();
    });

    QObject::connect(_tokenEdit, &TokenEdit::removableStateChanged, token,
                     &Token::setRemovable);

    updateToken(index, token, {});

    return token;
  }

  void updateToken(int index, Token* token,
                   QVector<int> const& roles) const override {
    auto updateText = roles.empty() || roles.contains(Qt::DisplayRole);
    auto updateToolTip = roles.empty() || roles.contains(Qt::ToolTipRole);

    if (updateText) {
      token->setText(data(index, Qt::DisplayRole).toString());
    }

    if (updateToolTip) {
      token->setToolTip(data(index, Qt::ToolTipRole).toString());
    }
  }

  AbstractTokenDragDropHandler* dragDropHandler() const override {
    return _tokenEdit->dragDropHandler();
  }

 private:
  QVariant data(int index, int role) const {
    auto const modelIndex = _tokenEdit->model()->index(
        index, _tokenEdit->modelColumn(), _tokenEdit->rootIndex());
    return modelIndex.data(role);
  }

 private:
  TokenEdit* _tokenEdit;
};

TokenEdit::TokenEdit(QWidget* parent)
    : TokenEditFrame{parent},
      _access{new TokenEditModeAccess{this}},
      _dragDropHandler{new TokenDragDropHandler{this}},
      _view{new TokenEditView{this}},
      _lineEdit{new TokenLineEdit{dragDropHandler(), _view}},
      _activeMode{nullptr},
      _nextActiveMode{nullptr},
      _modeChangedBlocked{false},
      _editingMode{new TokenEditEditingMode{_view, _access.get(), this}},
      _displayMode{new TokenEditDisplayMode{_view, _access.get(), this}},
      _scrollArea{new QScrollArea{this}},
      _maxLineCount{3},
      _dragEnabled{false},
      _dragDropMode{QAbstractItemView::NoDragDrop},
      _removable{false},
      _updateFocus{UpdateFocus::No},
      _model{nullptr},
      _rootModelIndex{QModelIndex{}},
      _modelColumn{0} {
  auto dummyToken = QScopedPointer{new Token{}};

  _lineEdit->setFixedHeight(dummyToken->sizeHint().height());

  _view->setDefaultFinalWidget(_lineEdit,
                               new LineEditFocusChainNavigation{_lineEdit});

  setWidget(_scrollArea);

  _view->setFocusPolicy(Qt::StrongFocus);
  _view->installEventFilter(this);

  _scrollArea->setFrameShape(QFrame::NoFrame);
  _scrollArea->setWidgetResizable(true);
  _scrollArea->setFocusPolicy(Qt::ClickFocus);
  _scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  auto singleStep = dummyToken->sizeHint().height() + _view->ySpacing();
  _scrollArea->verticalScrollBar()->setSingleStep(singleStep);

  _scrollArea->setWidget(_view);

  connect(_view, &TokenEditView::gotFocus,
          [=](auto widget) { ensureVisible(widget); });

  connect(_view, &TokenEditView::sizeChanged, [=]() {
    if (!_model && !_activeMode) {
      return;
    }
    _activeMode->invalidate();
    ensureVisible(focusWidget());
    updateHeight();
  });

  connect(_lineEdit, &TokenLineEdit::backspaceAtBeginning, [=]() {
    if (_model && !_view->isEmpty() && removable()) {
      this->remove(_view->count() - 1, UpdateFocus::No);
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

QAbstractItemView::DragDropMode TokenEdit::dragDropMode() const {
  return _dragDropMode;
}

void TokenEdit::setDragDropMode(QAbstractItemView::DragDropMode behavior) {
  _dragDropMode = behavior;
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

QLineEdit* TokenEdit::lineEdit() const { return _lineEdit; }

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

bool TokenEdit::eventFilter(QObject* object, QEvent* event) {
  Q_ASSERT(_view == object);

  if (event->type() == QEvent::FocusIn) {
    setNextActiveMode(_editingMode);
    lineEdit()->setFocus();
    return true;
  }

  return false;
}

TokenEditView* TokenEdit::view() const { return _view; }

AbstractTokenDragDropHandler* TokenEdit::dragDropHandler() const {
  return _dragDropHandler.get();
}

int TokenEdit::indexOf(Token const* token) const {
  return _view->indexOf(token);
}

QModelIndex TokenEdit::index(int row) const {
  Q_ASSERT(_model);
  return _model->index(row, _modelColumn, _rootModelIndex);
}

QModelIndex TokenEdit::index(Token const* token) const {
  return index(indexOf(token));
}

QScopedValueRollback<UpdateFocus> TokenEdit::enableUpdateFocus() {
  return QScopedValueRollback{_updateFocus, UpdateFocus::Yes};
}

bool TokenEdit::remove(int row, UpdateFocus uf) {
  Q_ASSERT(_model);
  QScopedValueRollback svr{_updateFocus, uf};
  return _model->removeRow(row);
}

void TokenEdit::init() {
  if (!_model) {
    return;
  }
  setActiveMode(_displayMode);
  _displayMode->activate();
}

void TokenEdit::clear() {
  while (!_view->isEmpty()) {
    _view->remove(0, UpdateFocus::No);
  }
}

void TokenEdit::onRowsInserted(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  _activeMode->inserted(first, last, _updateFocus);
  updateHeight();
}

void TokenEdit::onRowsRemoved(QModelIndex const& parent, int first, int last) {
  if (parent != _rootModelIndex) {
    return;
  }

  _activeMode->removed(first, last, _updateFocus);
  updateHeight();
}

void TokenEdit::onRowsMoved(QModelIndex const& parent, int first, int last,
                            QModelIndex const& destination, int to) {
  if (parent != _rootModelIndex) {
    return;
  }

  if (parent == destination) {
    _activeMode->moved(first, last, to);
    updateHeight();
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

  _activeMode->changed(topLeft.row(), bottomRight.row(), roles);
  updateHeight();
}

void TokenEdit::onModelReset() {
  clear();
  init();
}

void TokenEdit::onFocusChanged(QWidget* prev, QWidget* now) {
  auto isChild = [=](QWidget* widget) {
    return widget && (widget->parent() == _view || widget == _scrollArea ||
                      widget == _view);
  };

  auto prevIsChild = isChild(prev);
  auto nowIsChild = isChild(now);

  setShownAsFocused(nowIsChild);

  if (prevIsChild && !nowIsChild) {
    setNextActiveMode(_displayMode);
  } else if (!prevIsChild && nowIsChild) {
    setNextActiveMode(_editingMode);
  }
}

void TokenEdit::blockModeChange() { _modeChangedBlocked = true; }

void TokenEdit::unblockModeChange() {
  _modeChangedBlocked = false;
  updateActiveMode();
}

void TokenEdit::setNextActiveMode(TokenEditMode* mode) {
  _nextActiveMode = mode;
  updateActiveMode();
}

void TokenEdit::updateActiveMode() {
  if (_nextActiveMode && !_modeChangedBlocked) {
    setActiveMode(_nextActiveMode);
    _nextActiveMode = nullptr;
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

}  // namespace mjendruk
