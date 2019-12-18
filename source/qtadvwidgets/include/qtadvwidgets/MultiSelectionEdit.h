#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <cstdint>

#include <QLineEdit>

#include <QVector>
#include <QScrollArea>

class QBoxLayout;

class RemovableSelection;
class FlexLayout;

class QTADVWIDGETS_API MultiSelectionEdit : public QScrollArea {
  Q_OBJECT

 public:
  MultiSelectionEdit(QWidget* parent = nullptr);
  
  int maxRows() const;
  void setMaxRows(int rows);

protected:
  void resizeEvent(QResizeEvent* event) override;
  
private:
  void updateHeight();
  
private:
  QVector<RemovableSelection *> _selections;
  QLineEdit* _lineEdit;
  FlexLayout* _layout;
  int _maxRows;
};
