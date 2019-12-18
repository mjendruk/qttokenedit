#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QLineEdit>

#include <QVector>
#include <QScrollArea>

class RemovableSelection;

class QTADVWIDGETS_API MultiSelectionEdit : public QLineEdit {
  Q_OBJECT

 public:
  MultiSelectionEdit(QWidget* parent = nullptr);
  
private:
  QVector<RemovableSelection *> _selections;
  QLineEdit* _lineEdit;
};
