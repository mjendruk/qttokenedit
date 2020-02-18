#include "MainWindow.h"

#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>
#include <QCompleter>
#include <QStringListModel>
#include <QLineEdit>

#include <qttokenedit/qttokenedit_version.h>

#include <QSortFilterProxyModel>

#include "CsvItemModel.h"
#include "ui_MainWindow.h"

#include <qttokenedit/TokenEdit.h>

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);

  auto customerModel = new CsvItemModel{"data/customer-table.csv"};
  
  auto selectedCustomerModel = new QStringListModel{{"Max", "Gabriele", "Stavros"}, this};
  
  auto customerInput = new mjendruk::TokenEdit{this};
  customerInput->setModel(selectedCustomerModel);
  customerInput->setMaxLineCount(3);
  customerInput->setDragEnabled(true);
  customerInput->setDragDropMode(QAbstractItemView::DragDrop);
  customerInput->setRemovable(true);
  customerInput->setModelColumn(0);
  customerInput->setShowLineEdit(mjendruk::ShowLineEdit::Always);

  auto completer = new QCompleter{customerModel, this};
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->popup()->setMinimumWidth(200);
  completer->setCompletionColumn(3);
  completer->setFilterMode(Qt::MatchContains);

  customerInput->lineEdit()->setCompleter(completer);
  
  connect(
  completer, QOverload<QModelIndex const&>::of(&QCompleter::activated), this,
  [=](QModelIndex const& index) {
    auto const row = selectedCustomerModel->rowCount();
    selectedCustomerModel->insertRow(row);
    
    auto text = QString{"%1 %2"}
      .arg(index.siblingAtColumn(1).data().toString())
      .arg(index.siblingAtColumn(2).data().toString())
      .arg(index.data().toString());
    
    auto toolTip = index.data().toString();
    
    auto newIndex = selectedCustomerModel->index(row, 0);
    
    selectedCustomerModel->setData(newIndex, text);
    selectedCustomerModel->setData(newIndex, toolTip, Qt::ToolTipRole);
    customerInput->lineEdit()->clear();
  },
  Qt::QueuedConnection);
 
  m_ui->formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  m_ui->formLayout->addRow("Customers", customerInput);
  
}

MainWindow::~MainWindow() {}

void MainWindow::on_about() {
  QString about =
      QString() + "Version:   " + QTTOKENEDIT_VERSION + "\n";

  QMessageBox::about(this, "template project", about);
}
