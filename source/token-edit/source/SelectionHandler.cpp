#include "SelectionHandler.h"

#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include <token-edit/Token.h>
#include <token-edit/TokenEdit.h>
#include <token-edit/TokenEditView.h>

namespace mjendruk {

SelectionHandler::SelectionHandler(TokenEdit* tokenEdit)
    : AbstractSelectionHandler{tokenEdit},
      _tokenEdit{tokenEdit},
      _selectionModel{nullptr} {
  tokenEdit->installEventFilter(this);
}

void SelectionHandler::updateModel() {
  if (!model()) {
    delete _selectionModel;
    _selectionModel = nullptr;
    return;
  }

  if (!_selectionModel) {
    _selectionModel = new QItemSelectionModel{model()};

    connect(_selectionModel, &QItemSelectionModel::selectionChanged, this,
            &SelectionHandler::onSelectionChanged);
  } else {
    _selectionModel->setModel(model());
  }
}

void SelectionHandler::select(Token const* token, Qt::MouseButtons buttons,
                              Qt::KeyboardModifiers modifiers) {
  auto index = _tokenEdit->index(token);

  if (shouldExtendSelection(buttons, modifiers)) {
    return extendSelectionTo(index);
  }

  if (shouldSelectSingle(buttons, modifiers)) {
    return selectSingle(index, modifiers);
  }
}

QItemSelectionModel* SelectionHandler::selectionModel() const {
  return _selectionModel;
}

bool SelectionHandler::eventFilter(QObject* watched, QEvent* event) {
  Q_ASSERT(_tokenEdit == watched);

  if (shouldSelectPreviousNext(event)) {
    return selectPreviousNext(event);
  }

  if (shouldSelectFirst(event)) {
    return selectFirst();
  }

  if (shouldSelectAll(event)) {
    return selectAll();
  }

  if (shouldClear(event)) {
    _selectionModel->clear();
    return false;
  }

  return false;
}

QAbstractItemModel* SelectionHandler::model() const {
  return _tokenEdit->model();
}

void SelectionHandler::onSelectionChanged(QItemSelection const& selected,
                                          QItemSelection const& deselected) {
  updateSelection(deselected.indexes(), false);
  updateSelection(selected.indexes(), true);
}

void SelectionHandler::updateSelection(QModelIndexList const& indexes,
                                       bool selected) {
  for (auto const& index : indexes) {
    if (index.column() != _tokenEdit->modelColumn()) {
      continue;
    }

    if (auto token = _tokenEdit->find(index.row())) {
      token->setSelected(selected);
    }
  }
}

QItemSelectionModel::SelectionFlags SelectionHandler::defaultFlags() const {
  return QItemSelectionModel::Select | QItemSelectionModel::Rows;
}

bool SelectionHandler::shouldExtendSelection(
    Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers) const {
  return buttons.testFlag(Qt::LeftButton) &&
         modifiers.testFlag(Qt::ShiftModifier) &&
         _selectionModel->currentIndex().isValid();
}

void SelectionHandler::extendSelectionTo(QModelIndex const& index) {
  auto current = _selectionModel->currentIndex();

  auto itemSelection = QItemSelection{};
  if (current.row() < index.row()) {
    itemSelection.select(current, index);
  } else {
    itemSelection.select(index, current);
  }

  _selectionModel->select(itemSelection, defaultFlags());
  _selectionModel->setCurrentIndex(index, {});
}

bool SelectionHandler::shouldSelectSingle(
    Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers) const {
  Q_UNUSED(modifiers);
  return buttons.testFlag(Qt::LeftButton);
}

void SelectionHandler::selectSingle(QModelIndex const& index,
                                    Qt::KeyboardModifiers modifiers) {
  if (_selectionModel->selection().contains(index)) {
    _selectionModel->setCurrentIndex(index, {});
    return;
  }

  auto flags = defaultFlags();

  if (!modifiers.testFlag(Qt::ControlModifier)) {
    flags.setFlag(QItemSelectionModel::Clear);
  }

  _selectionModel->select(index, flags);
  _selectionModel->setCurrentIndex(index, {});
}

bool SelectionHandler::shouldSelectPreviousNext(QEvent* event) const {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
      return true;
  }

  return false;
}

bool SelectionHandler::selectPreviousNext(QEvent* event) {
  auto keyEvent = static_cast<QKeyEvent*>(event);
  bool isLeft = keyEvent->key() == Qt::Key_Left;

  auto currentIndex = _selectionModel->currentIndex();

  auto row = 0;

  if (currentIndex.isValid()) {
    auto adjacentShift = isLeft ? -1 : 1;
    row = currentIndex.row() + adjacentShift;
  } else {
    row = isLeft ? _tokenEdit->visibleCount() - 1 : 0;
  }

  auto newCurrentIndex = _tokenEdit->index(row);

  auto flags = defaultFlags();

  if (!keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
    flags.setFlag(QItemSelectionModel::Clear);
  }

  if (newCurrentIndex.isValid()) {
    _selectionModel->select(newCurrentIndex, flags);
    _selectionModel->setCurrentIndex(newCurrentIndex,
                                     QItemSelectionModel::Current);
  }

  return true;
}

bool SelectionHandler::shouldSelectFirst(QEvent* event) const {
  return event->type() == QEvent::FocusIn;
}

bool SelectionHandler::selectFirst() {
  auto index = _tokenEdit->index(0);
  _selectionModel->select(index, defaultFlags());
  _selectionModel->setCurrentIndex(index, {});

  return true;
}

bool SelectionHandler::shouldSelectAll(QEvent* event) const {
  if (event->type() != QEvent::KeyPress) return false;

  auto keyEvent = static_cast<QKeyEvent*>(event);
  return keyEvent->key() == Qt::Key_A &&
         keyEvent->modifiers().testFlag(Qt::ControlModifier);
}

bool SelectionHandler::selectAll() {
  if (model()->rowCount() == 0) {
    return false;
  }

  auto firstIndex = _tokenEdit->index(0);
  auto lastIndex = _tokenEdit->index(model()->rowCount() - 1);
  auto itemSelection = QItemSelection{firstIndex, lastIndex};
  _selectionModel->select(itemSelection, defaultFlags());
  _selectionModel->setCurrentIndex(lastIndex, {});

  return true;
}

bool SelectionHandler::shouldClear(QEvent* event) const {
  if (event->type() == QEvent::MouseButtonPress) {
    auto mouseEvent = static_cast<QMouseEvent*>(event);

    if (mouseEvent->buttons().testFlag(Qt::LeftButton)) {
      return true;
    }
  }

  if (event->type() == QEvent::FocusOut) {
    return true;
  }

  return false;
}

}  // namespace mjendruk
