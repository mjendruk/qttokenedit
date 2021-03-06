#pragma once

#include <QAbstractTableModel>

class StopItemModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  StopItemModel(QStringList const& stops, QObject* parent = nullptr);

  Qt::DropActions supportedDropActions() const override;

  QStringList mimeTypes() const override;

  QMimeData* mimeData(QModelIndexList const& indexes) const override;

  bool canDropMimeData(QMimeData const* data, Qt::DropAction action, int row,
                       int column, QModelIndex const& parent) const override;

  bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row,
                    int column, QModelIndex const& parent) override;

  int rowCount(QModelIndex const& parent = QModelIndex()) const override;
  int columnCount(QModelIndex const& parent = QModelIndex()) const override;

  QVariant data(QModelIndex const& index,
                int role = Qt::DisplayRole) const override;

  bool setData(QModelIndex const& index, QVariant const& value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(QModelIndex const& index) const override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  bool insertRows(int row, int count,
                  QModelIndex const& parent = QModelIndex()) override;

  bool moveRows(QModelIndex const& sourceParent, int sourceRow, int count,
                QModelIndex const& destinationParent,
                int destinationChild) override;

  bool removeRows(int row, int count,
                  QModelIndex const& parent = QModelIndex()) override;

 private:
  QString abbreviation(QString const& str) const;

 private:
  QStringList _stops;
};
