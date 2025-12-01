#include "ColumnSortModel.h"

#include <unordered_set>

#include <QBinaryJson>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMimeData>
#include <QRegularExpression>

ColumnSortModel::ColumnSortModel(QVector<int> const& selectedColumns,
                                 QStringList const& availableColumns,
                                 QObject* parent)
    : QAbstractTableModel{parent},
      _selectedColumns{selectedColumns},
      _availableColumns{availableColumns},
      _mimeType{"application/prs.columnindex"} {}

Qt::DropActions ColumnSortModel::supportedDropActions() const {
  return Qt::MoveAction;
}

QStringList ColumnSortModel::mimeTypes() const { return {_mimeType}; }

QMimeData* ColumnSortModel::mimeData(QModelIndexList const& indexes) const {
  auto rows = std::unordered_set<int>{};
  for (auto const& index : indexes) {
    rows.insert(index.row());
  }

  auto selectedIndexes = QVariantList{};
  std::transform(indexes.cbegin(), indexes.cend(),
                 std::back_inserter(selectedIndexes),
                 [=](auto index) { return _selectedColumns.at(index.row()); });

  auto jsonData = QBinaryJson::toBinaryData(QJsonDocument{QJsonArray::fromVariantList(selectedIndexes)});

  auto mimeData = new QMimeData{};
  mimeData->setData(_mimeType, jsonData);
  return mimeData;
}

bool ColumnSortModel::canDropMimeData(QMimeData const* data,
                                      Qt::DropAction action, int row,
                                      int column,
                                      QModelIndex const& parent) const {
  Q_UNUSED(action);
  Q_UNUSED(row);
  Q_UNUSED(column);
  Q_UNUSED(parent);

  if (!data->hasFormat(_mimeType)) {
    return false;
  }

  if (parent.isValid()) {
    return false;
  }

  if (!(0 <= row && row <= _selectedColumns.size())) {
    return false;
  }

  return true;
}

bool ColumnSortModel::dropMimeData(QMimeData const* data, Qt::DropAction action,
                                   int row, int column,
                                   QModelIndex const& parent) {
  if (!canDropMimeData(data, action, row, column, parent)) {
    return false;
  }

  if (action == Qt::IgnoreAction) {
    return true;
  }

  QByteArray encodedData = data->data(_mimeType);

  auto json = QBinaryJson::fromBinaryData(encodedData);

  if (!json.isArray()) {
    return false;
  }

  auto jsonArray = json.array();

  auto currentRow = row;

  insertRows(currentRow, jsonArray.count());

  for (auto jsonValue : json.array()) {
    auto index = this->index(currentRow, 0);
    if (!setData(index, jsonValue.toVariant(), Qt::EditRole)) {
      return false;
    }
    ++currentRow;
  }

  return true;
}

int ColumnSortModel::rowCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _selectedColumns.size();
}

int ColumnSortModel::columnCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return 1;
}

QVariant ColumnSortModel::data(QModelIndex const& index, int role) const {
  if (!index.isValid()) {
    return QVariant{};
  }

  if (index.column() != 0) {
    return QVariant{};
  }

  if (role == Qt::DisplayRole) {
    auto column = _selectedColumns.at(index.row());
    if (column == -1) {
      return QString{"invalid"};
    }
    return _availableColumns.at(column);
  }

  if (role == Qt::ToolTipRole) {
    return QString{"Index: %1"}.arg(_selectedColumns.at(index.row()));
  }

  if (role == Qt::EditRole) {
    return _selectedColumns.at(index.row());
  }

  return QVariant{};
}

bool ColumnSortModel::setData(QModelIndex const& index, QVariant const& value,
                              int role) {
  if (role != Qt::EditRole) {
    return false;
  }

  if (index.isValid() && index.column() == 0) {
    _selectedColumns[index.row()] = value.toInt();

    auto topLeftIndex = index;
    auto bottomRightIndex = index;
    emit dataChanged(topLeftIndex, bottomRightIndex,
                     {Qt::DisplayRole, Qt::EditRole, Qt::ToolTipRole});
    return true;
  }

  return false;
}

Qt::ItemFlags ColumnSortModel::flags(const QModelIndex& index) const {
  auto defaultFlags = QAbstractTableModel::flags(index);

  if (index.isValid() && index.column() == 0) {
    return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  }

  return defaultFlags;
}

QVariant ColumnSortModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
  return QVariant{};
}

bool ColumnSortModel::insertRows(int row, int count,
                                 QModelIndex const& parent) {
  if (parent.isValid()) {
    return false;
  }

  auto firstRow = row;
  auto lastRow = row + (count - 1);

  beginInsertRows(parent, firstRow, lastRow);

  for (auto insertRow = firstRow; insertRow <= lastRow; ++insertRow) {
    _selectedColumns.insert(insertRow, -1);
  }

  endInsertRows();
  
  return true;
}

bool ColumnSortModel::removeRows(int row, int count,
                                 QModelIndex const& parent) {
  if (parent.isValid()) {
    return false;
  }

  auto firstRow = row;
  auto lastRow = row + (count - 1);

  beginRemoveRows(parent, firstRow, lastRow);

  for (auto removeRow = lastRow; removeRow >= firstRow; --removeRow) {
    _selectedColumns.removeAt(removeRow);
  }

  endRemoveRows();

  return true;
}

QVector<int> ColumnSortModel::validSelectedColumn() const {
  auto result = _selectedColumns;
  result.removeAll(-1);
  return result;
}
