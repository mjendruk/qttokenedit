#pragma once

#include <QtCore/QString>
#include <QtCore/QVector>

struct CsvData {
  using Row = QVector<QString>;

  Row headerRow;
  QVector<Row> rows;
};

CsvData loadCsvFromFile(QString const& filePath);