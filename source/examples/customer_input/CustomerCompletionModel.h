#pragma once

#include <QtCore/QAbstractListModel>

#include "LoadCsvFromFile.h"

class CustomerCompletionModel : public QAbstractListModel {
  Q_OBJECT

 public:
  enum UserRoles {
    NameRole = Qt::UserRole,
    EmailRole,
  };

  CustomerCompletionModel(QObject* parent = nullptr);

  int rowCount(QModelIndex const& parent = QModelIndex()) const override;

  QVariant data(QModelIndex const& index,
                int role = Qt::DisplayRole) const override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  CsvData _data;

  int _firstNameCol;
  int _lastNameCol;
  int _emailCol;
};
