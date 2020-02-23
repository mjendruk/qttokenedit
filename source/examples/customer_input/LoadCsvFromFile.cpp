#include "LoadCsvFromFile.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

CsvData loadCsvFromFile(QString const& filePath) {
  auto result = CsvData{};

  QFile file{filePath};

  Q_ASSERT(file.exists());

  file.open(QFile::ReadOnly | QFile::Text);

  QTextStream in{&file};

  auto headerLine = in.readLine();
  for (auto title : headerLine.split(",")) {
    result.headerRow.append(title.remove('"'));
  }

  while (!in.atEnd()) {
    auto const line = in.readLine();
    auto row = CsvData::Row{};
    for (auto value : line.split(",")) {
      row.append(value.remove('"'));
    }

    if (row.size() < result.headerRow.size()) {
      auto diff = result.headerRow.size() - row.size();
      row.insert(row.size(), diff, QString{});
    }

    result.rows.append(row);
  }

  return result;
}
