#include "CsvItemModel.h"

#include <algorithm>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtDebug>

CsvItemModel::CsvItemModel(QString const& filePath, QObject* parent)
    : QAbstractTableModel{parent} {
  loadFromFile(filePath);
}

int CsvItemModel::rowCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _sortedRows.size();
}

int CsvItemModel::columnCount(QModelIndex const& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _headerRow.size();
}

QVariant CsvItemModel::data(QModelIndex const& index, int role) const {
  if (!index.isValid()) {
    return QVariant{};
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return _sortedRows.at(index.row()).at(index.column());
  }

  return QVariant{};
}

QVariant CsvItemModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (orientation != Qt::Horizontal) {
    return {};
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return _headerRow.at(section);
  }

  return QVariant{};
}

void CsvItemModel::sortByColumns(QVector<int> const& columns) {
  if (_originalRows.empty() || _headerRow.empty()) {
    return;
  }

  _sortedRows = _originalRows;

  for (auto columnIt = columns.crbegin(); columnIt != columns.crend(); ++columnIt) {
    std::stable_sort(_sortedRows.begin(), _sortedRows.end(),
              [=](auto const& left, auto const& right) {
                return left.at(*columnIt) < right.at(*columnIt);
              });
  }

  auto topLeft = this->index(0, 0);
  auto bottomRight = this->index(_sortedRows.size() - 1, _headerRow.size() - 1);
  emit dataChanged(topLeft, bottomRight);
}

QStringList CsvItemModel::titles() const { return _headerRow.toList(); }

void CsvItemModel::loadFromFile(QString const& path) {
  QFile file{path};

  Q_ASSERT(file.exists());

  file.open(QFile::ReadOnly | QFile::Text);

  QTextStream in{&file};

  auto headerLine = in.readLine();
  for (auto title : headerLine.split(",")) {
    _headerRow.append(title.remove('"'));
  }

  while (!in.atEnd()) {
    auto const line = in.readLine();
    auto row = Row{};
    for (auto value : line.split(",")) {
      row.append(value.remove('"'));
    }

    if (row.size() < _headerRow.size()) {
      auto diff = _headerRow.size() - row.size();
      row.insert(row.size(), diff, QString{});
    }

    _originalRows.append(row);
  }

  _sortedRows = _originalRows;
}
