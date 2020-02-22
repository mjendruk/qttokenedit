#include "CustomerCompletionModel.h"

CustomerCompletionModel::CustomerCompletionModel(QObject* parent)
    : QAbstractListModel{parent} {
  _data = loadCsvFromFile("data/customer-table.csv");

  _firstNameCol = _data.headerRow.indexOf("first_name");
  _lastNameCol = _data.headerRow.indexOf("last_name");
  _emailCol = _data.headerRow.indexOf("email");
}

int CustomerCompletionModel::rowCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  auto res = _data.rows.size();
  return res;
}

QVariant CustomerCompletionModel::data(QModelIndex const& index,
                                       int role) const {
  if (!index.isValid()) {
    return QVariant{};
  }

  auto const& row = _data.rows.at(index.row());

  if (role == Qt::DisplayRole) {
    return QString{"%1 %2 <%3>"}
        .arg(row.at(_firstNameCol))
        .arg(row.at(_lastNameCol))
        .arg(row.at(_emailCol));
  } else if (role == NameRole || role == Qt::EditRole) {
    return QString{"%1 %2"}
        .arg(row.at(_firstNameCol))
        .arg(row.at(_lastNameCol));
  } else if (role == EmailRole) {
    return row.at(_emailCol);
  }

  return QVariant{};
}

QVariant CustomerCompletionModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const {
  if (role == Qt::DisplayRole) {
    return QString{"Customer"};
  }
  
  return QVariant{};
}
