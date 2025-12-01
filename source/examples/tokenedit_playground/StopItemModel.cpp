#include "StopItemModel.h"

#include <unordered_set>

#include <QBinaryJson>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMimeData>
#include <QRegularExpression>

StopItemModel::StopItemModel(QStringList const& stops, QObject* parent)
    : QAbstractTableModel{parent}, _stops{stops} {}

Qt::DropActions StopItemModel::supportedDropActions() const {
  return Qt::MoveAction;
}

QStringList StopItemModel::mimeTypes() const { return {"application/json"}; }

QMimeData* StopItemModel::mimeData(QModelIndexList const& indexes) const {
  auto rows = std::unordered_set<int>{};
  for (auto const& index : indexes) {
    rows.insert(index.row());
  }

  auto stopList = QStringList{};
  std::transform(rows.cbegin(), rows.cend(), std::back_inserter(stopList),
                 [=](auto rows) { return _stops.at(rows); });

  auto jsonData =
      QBinaryJson::toBinaryData(QJsonDocument{QJsonArray::fromStringList(stopList)});

  auto mimeData = new QMimeData{};
  mimeData->setData("application/json", jsonData);
  return mimeData;
}

bool StopItemModel::canDropMimeData(QMimeData const* data,
                                    Qt::DropAction action, int row, int column,
                                    QModelIndex const& parent) const {
  Q_UNUSED(action);
  Q_UNUSED(row);
  Q_UNUSED(column);
  Q_UNUSED(parent);

  if (!data->hasFormat("application/json")) {
    return false;
  }

  if (parent.isValid()) {
    return false;
  }

  if (!(0 <= row && row <= _stops.size())) {
    return false;
  }

  return true;
}

bool StopItemModel::dropMimeData(QMimeData const* data, Qt::DropAction action,
                                 int row, int column,
                                 QModelIndex const& parent) {
  if (!canDropMimeData(data, action, row, column, parent)) {
    return false;
  }

  if (action == Qt::IgnoreAction) {
    return true;
  }

  QByteArray encodedData = data->data("application/json");

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

int StopItemModel::rowCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  auto result = _stops.count();
  return result;
}

int StopItemModel::columnCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return 2;
}

QVariant StopItemModel::data(QModelIndex const& index, int role) const {
  if (!index.isValid()) {
    return QVariant{};
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if (index.column() == 0) {
      return _stops.at(index.row());
    } else if (index.column() == 1) {
      return abbreviation(_stops.at(index.row()));
    }
  }

  if (role == Qt::ToolTipRole) {
    if (index.column() == 1) {
      return _stops.at(index.row());
    } else if (index.column() == 0) {
      return abbreviation(_stops.at(index.row()));
    }
  }

  return QVariant{};
}

bool StopItemModel::setData(QModelIndex const& index, QVariant const& value,
                            int role) {
  if (role != Qt::EditRole) {
    return false;
  }

  if (index.isValid() && index.column() == 0) {
    auto stop = value.toString();
    _stops.replace(index.row(), stop);

    auto topLeftIndex = index;
    auto bottomRightIndex = this->index(index.row(), 1);
    emit dataChanged(topLeftIndex, bottomRightIndex,
                     {Qt::DisplayRole, Qt::EditRole, Qt::ToolTipRole});
    return true;
  }

  return false;
}

Qt::ItemFlags StopItemModel::flags(const QModelIndex& index) const {
  auto defaultFlags = QAbstractTableModel::flags(index);

  if (index.isValid()) {
    if (index.column() == 0) {
      return defaultFlags | Qt::ItemIsEditable | Qt::ItemIsDragEnabled |
             Qt::ItemIsDropEnabled;
    } else {
      return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
  }

  return defaultFlags | Qt::ItemIsDropEnabled;
}

QVariant StopItemModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation != Qt::Horizontal) {
    return {};
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if (section == 0) {
      return QString{"Description"};
    } else if (section == 1) {
      return QString{"Abbreviation"};
    }
  }

  return QVariant{};
}

bool StopItemModel::insertRows(int row, int count, QModelIndex const& parent) {
  if (parent.isValid()) {
    return false;
  }

  auto firstRow = row;
  auto lastRow = row + (count - 1);

  beginInsertRows(parent, firstRow, lastRow);

  for (auto insertRow = firstRow; insertRow <= lastRow; ++insertRow) {
    _stops.insert(insertRow, {});
  }

  endInsertRows();

  return true;
}

bool StopItemModel::moveRows(QModelIndex const& sourceParent, int sourceRow,
                             int count, QModelIndex const& destinationParent,
                             int destinationChild) {
  if (sourceParent.isValid() || destinationParent.isValid()) {
    return false;
  }

  auto firstRow = sourceRow;
  auto lastRow = sourceRow + (count - 1);

  Q_ASSERT(!(firstRow <= destinationChild && destinationChild < lastRow + 1));

  beginMoveRows(sourceParent, firstRow, lastRow, destinationParent,
                destinationChild);

  auto insertionIndex =
      lastRow > destinationChild ? destinationChild : (destinationChild - 1);

  for (auto row = lastRow; row >= firstRow; --row) {
    _stops.move(lastRow, insertionIndex);
  }

  endMoveRows();

  return true;
}

bool StopItemModel::removeRows(int row, int count, QModelIndex const& parent) {
  if (parent.isValid()) {
    return false;
  }

  auto firstRow = row;
  auto lastRow = row + (count - 1);

  beginRemoveRows(parent, firstRow, lastRow);

  for (auto removeRow = lastRow; removeRow >= firstRow; --removeRow) {
    _stops.removeAt(removeRow);
  }

  endRemoveRows();

  return true;
}

QString StopItemModel::abbreviation(QString const& stop) const {
  auto result = stop;
  result.remove(QRegularExpression{"\\W"});

  constexpr auto abbrSize = 7;
  if (result.size() > abbrSize) {
    result.truncate(abbrSize);
  }

  return result.toUpper();
}
