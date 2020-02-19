#pragma once

#include <QAbstractListModel>

class SelectedCustomersModel : public QAbstractListModel {
  Q_OBJECT

 public:
  SelectedCustomersModel(QObject* parent = nullptr);

  Qt::DropActions supportedDropActions() const override;

  QStringList mimeTypes() const override;

  QMimeData* mimeData(QModelIndexList const& indexes) const override;

  bool canDropMimeData(QMimeData const* data, Qt::DropAction action, int row,
                       int column, QModelIndex const& parent) const override;

  bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row,
                    int column, QModelIndex const& parent) override;

  int rowCount(QModelIndex const& parent = QModelIndex()) const override;

  QVariant data(QModelIndex const& index,
                int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(QModelIndex const& index) const override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  bool removeRows(int row, int count,
                  QModelIndex const& parent = QModelIndex()) override;

  void insert(int row, QString const& name, QString const& email);
  
  void append(QString const& name, QString const& email);

 private:
  QVector<QString> _names;
  QVector<QString> _emails;

  QString _mimeType;
};
