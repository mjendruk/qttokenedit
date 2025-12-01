#include "MainWindow.h"

#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>

#include <qttokenedit/qttokenedit_version.h>

#include <QSortFilterProxyModel>

#include "CsvItemModel.h"
#include "ColumnSortModel.h"
#include "ui_MainWindow.h"

#include <qttokenedit/TokenEdit.h>

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);
  connect(m_ui->aboutAction, &QAction::triggered, this, &MainWindow::on_about);

  auto model = new CsvItemModel{"data/customer-table.csv"};
  
  auto titles = model->titles();
  
  auto selectedColumns = new ColumnSortModel{{}, titles, this};
  
  auto selectedColumnEdit = new mjendruk::TokenEdit{this};
  selectedColumnEdit->setModel(selectedColumns);
  selectedColumnEdit->setMaxLineCount(2);
  selectedColumnEdit->setDragEnabled(true);
  selectedColumnEdit->setDragDropMode(QAbstractItemView::DragDrop);
  selectedColumnEdit->setShowLineEdit(mjendruk::ShowLineEdit::Never);
  
  connect(selectedColumns, &QAbstractItemModel::rowsInserted, [=]() {
    model->sortByColumns(selectedColumns->validSelectedColumn());
  });
  connect(selectedColumns, &QAbstractItemModel::rowsRemoved, [=]() {
     model->sortByColumns(selectedColumns->validSelectedColumn());
   });
  connect(selectedColumns, &QAbstractItemModel::dataChanged, [=]() {
     model->sortByColumns(selectedColumns->validSelectedColumn());
   });
  
  m_ui->formLayout_2->addRow("Selected", selectedColumnEdit);
  
  auto columnIndexes = QVector<int>(titles.size());
  std::iota(columnIndexes.begin(), columnIndexes.end(), 0);
  
  auto availableColumns = new ColumnSortModel{columnIndexes, titles, this};
  
  auto availableColumnEdit = new mjendruk::TokenEdit{this};
  availableColumnEdit->setModel(availableColumns);
  availableColumnEdit->setMaxLineCount(2);
  availableColumnEdit->setDragEnabled(true);
  availableColumnEdit->setDragDropMode(QAbstractItemView::DragDrop);
  availableColumnEdit->setShowLineEdit(mjendruk::ShowLineEdit::Never);
  
  m_ui->formLayout_2->addRow("Available", availableColumnEdit);
  
  m_ui->formLayout_2->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  
  auto tableView = m_ui->customerTableView;
  tableView->setModel(model);
  tableView->verticalHeader()->hide();
}

MainWindow::~MainWindow() {}

void MainWindow::on_about() {
  QString about =
      QString() + "Version:   " + QTTOKENEDIT_VERSION + "\n";

  QMessageBox::about(this, "template project", about);
}
