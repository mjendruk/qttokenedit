#include "StopItemModel.h"

#include <QRegularExpression>

StopItemModel::StopItemModel(QStringList const& stops, QObject* parent)
    : QAbstractTableModel{parent}, _stops{stops} {}

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
  auto flags = QAbstractTableModel::flags(index);

  if (index.isValid() && index.column() == 0) {
    flags.setFlag(Qt::ItemIsEditable);
  }

  return flags;
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
