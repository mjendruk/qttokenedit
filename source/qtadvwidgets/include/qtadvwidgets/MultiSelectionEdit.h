#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QLineEdit>

#include <QVector>
#include <QScrollArea>

class QBoxLayout;

class RemovableSelection;

class QTADVWIDGETS_API MultiSelectionEdit : public QScrollArea {
  Q_OBJECT

 public:
  MultiSelectionEdit(QWidget* parent = nullptr);

  void fillRow(QVector<QString> const& entries, QBoxLayout* layout);

private:
  QVector<RemovableSelection *> _selections;
  QLineEdit* _lineEdit;
};
