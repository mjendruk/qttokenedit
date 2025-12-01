#include "SelectedCustomersModel.h"

#include <unordered_set>

#include <QBinaryJson>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QRegularExpression>

SelectedCustomersModel::SelectedCustomersModel(QObject* parent)
    : QAbstractListModel{parent},
      _mimeType{"application/prs.customer.data"} {}

Qt::DropActions SelectedCustomersModel::supportedDropActions() const {
  return Qt::MoveAction;
}

QStringList SelectedCustomersModel::mimeTypes() const { return {_mimeType}; }

QMimeData* SelectedCustomersModel::mimeData(
    QModelIndexList const& indexes) const {
  auto customers = QJsonArray{};

  for (auto index : indexes) {
    auto row = index.row();

    customers.append(
        QJsonObject{{QStringLiteral("name"), _names.at(row)},
                   {QStringLiteral("email"), _emails.at(row)}});
  }

  auto jsonData = QBinaryJson::toBinaryData(QJsonDocument{customers});

  auto mimeData = new QMimeData{};
  mimeData->setData(_mimeType, jsonData);
  return mimeData;
}

bool SelectedCustomersModel::canDropMimeData(QMimeData const* data,
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

  if (!(0 <= row && row <= _names.size())) {
    return false;
  }

  return true;
}

bool SelectedCustomersModel::dropMimeData(QMimeData const* data,
                                          Qt::DropAction action, int row,
                                          int column,
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
  for (auto jsonValue : jsonArray) {
    auto const& customer = jsonValue.toObject();

    insert(currentRow, customer["name"].toString(),
           customer["email"].toString());

    ++currentRow;
  }

  return true;
}

int SelectedCustomersModel::rowCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _names.size();
}

QVariant SelectedCustomersModel::data(QModelIndex const& index,
                                      int role) const {
  if (!index.isValid()) {
    return QVariant{};
  }

  if (index.column() != 0) {
    return QVariant{};
  }

  auto row = index.row();

  if (role == Qt::DisplayRole) {
    return _names.at(row);
  } else if (role == Qt::ToolTipRole) {
    return _emails.at(row);
  }

  return QVariant{};
}

Qt::ItemFlags SelectedCustomersModel::flags(const QModelIndex& index) const {
  auto defaultFlags = QAbstractListModel::flags(index);

  if (index.isValid() && index.column() == 0) {
    return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  }

  return defaultFlags;
}

QVariant SelectedCustomersModel::headerData(int section,
                                            Qt::Orientation orientation,
                                            int role) const {
  return QVariant{};
}

bool SelectedCustomersModel::removeRows(int row, int count,
                                        QModelIndex const& parent) {
  if (parent.isValid()) {
    return false;
  }

  auto firstRow = row;
  auto lastRow = row + (count - 1);

  beginRemoveRows(parent, firstRow, lastRow);

  for (auto removeRow = lastRow; removeRow >= firstRow; --removeRow) {
    _names.removeAt(removeRow);
    _emails.removeAt(removeRow);
  }

  endRemoveRows();

  return true;
}

void SelectedCustomersModel::insert(int row, QString const& name,
                                    QString const& email) {
  beginInsertRows(QModelIndex{}, row, row);
  _names.insert(row, name);
  _emails.insert(row, email);
  endInsertRows();
}

void SelectedCustomersModel::append(QString const& name, QString const& email)
{
  insert(_names.size(), name, email);
}
