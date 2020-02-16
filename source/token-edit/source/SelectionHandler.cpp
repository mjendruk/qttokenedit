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
  if (!buttons.testFlag(Qt::LeftButton)) {
    return;
  }

  auto flags = QItemSelectionModel::Select | QItemSelectionModel::Rows;

  auto tokenIndex = _tokenEdit->index(token);
  
  if (_selectionModel->selection().contains(tokenIndex)) {
    _selectionModel->setCurrentIndex(tokenIndex, QItemSelectionModel::Current);
    return;
  }
  
  auto itemSelection = QItemSelection{};
  
  if (auto latest = _selectionModel->currentIndex();
      latest.isValid() && modifiers.testFlag(Qt::ShiftModifier)) {
    
    if (latest.row() < tokenIndex.row()) {
      itemSelection.select(latest, tokenIndex);
    } else {
      itemSelection.select(tokenIndex, latest);
    }
  } else {
    itemSelection.select(tokenIndex, tokenIndex);
    
    if (!modifiers.testFlag(Qt::ControlModifier)) {
       flags.setFlag(QItemSelectionModel::Clear);
    }
  }

  _selectionModel->select(itemSelection, flags);
  _selectionModel->setCurrentIndex(tokenIndex, QItemSelectionModel::Current);
}

QItemSelectionModel* SelectionHandler::selectionModel() const {
  return _selectionModel;
}

bool SelectionHandler::eventFilter(QObject* watched, QEvent* event) {
  Q_ASSERT(_tokenEdit == watched);

  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right) {
      auto latestSelectedIndex = _selectionModel->currentIndex();
        
      auto row = 0;
      
      if (latestSelectedIndex.isValid()) {
        auto siblingShift = (keyEvent->key() == Qt::Key_Left) ? -1 : +1;
        row = latestSelectedIndex.row() + siblingShift;
      } else {
        row = (keyEvent->key() == Qt::Key_Right) ? 0 : view()->count() - 1;
      }
      
      auto newCurrentIndex = _tokenEdit->index(row);

      auto flags = QItemSelectionModel::Select | QItemSelectionModel::Rows;

      if (!keyEvent->modifiers().testFlag(Qt::ShiftModifier)) {
        flags.setFlag(QItemSelectionModel::Clear);
      }

      if (newCurrentIndex.isValid()) {
        _selectionModel->select(newCurrentIndex, flags);
        _selectionModel->setCurrentIndex(newCurrentIndex, QItemSelectionModel::Current);
      }
      return true;
    }
    
    if (keyEvent->key() == Qt::Key_A &&
        keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
      auto itemSelection = QItemSelection{
          _tokenEdit->index(0), _tokenEdit->index(model()->rowCount() - 1)};
      _selectionModel->select(itemSelection, QItemSelectionModel::Select);
      return true;
    }
  }
  
  if (event->type() == QEvent::MouseButtonPress) {
    auto mouseEvent = static_cast<QMouseEvent*>(event);
    
    if (mouseEvent->buttons().testFlag(Qt::LeftButton)) {
      _selectionModel->clear();
      return true;
    }
  }
  
  if (event->type() == QEvent::FocusIn) {
    auto index = _tokenEdit->index(0);
    _selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    _selectionModel->setCurrentIndex(index, QItemSelectionModel::Current);
    return false;
  }
  
  if (event->type() == QEvent::FocusOut) {
    _selectionModel->clear();
    return false;
  }

  return false;
}

QAbstractItemModel* SelectionHandler::model() const {
  return _tokenEdit->model();
}

TokenEditView* SelectionHandler::view() const { return _tokenEdit->view(); }

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

    if (auto token = view()->find(index.row())) {
      token->setSelected(selected);
    }
  }
}

}  // namespace mjendruk
