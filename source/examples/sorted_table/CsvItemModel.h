#pragma once

#include <QtCore/QAbstractTableModel>

class CsvItemModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  CsvItemModel(QString const& filePath, QObject* parent = nullptr);

  int rowCount(QModelIndex const& parent = QModelIndex()) const override;

  int columnCount(QModelIndex const& parent = QModelIndex()) const override;

  QVariant data(QModelIndex const& index,
                int role = Qt::DisplayRole) const override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  
  void sortByColumns(QVector<int> const& columns);
  
  QStringList titles() const;
  
 private:
  void loadFromFile(QString const& path);

 private:
  using Row = QVector<QString>;
  Row _headerRow;
  
  QVector<Row> _originalRows;
  QVector<Row> _sortedRows;
};
