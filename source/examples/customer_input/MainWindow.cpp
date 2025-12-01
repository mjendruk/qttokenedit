#include "MainWindow.h"

#include <QCompleter>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QStringListModel>
#include <QTableView>

#include <qttokenedit/qttokenedit_version.h>

#include "CustomerCompletionModel.h"
#include "SelectedCustomersModel.h"
#include "ui_MainWindow.h"

#include <qttokenedit/TokenEdit.h>

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);

  connect(m_ui->aboutAction, &QAction::triggered, this, &MainWindow::on_about);

  auto selectedCustomerModel = new SelectedCustomersModel{this};

  auto customerInput = new mjendruk::TokenEdit{this};
  customerInput->setModel(selectedCustomerModel);
  customerInput->setMaxLineCount(3);
  customerInput->setDragEnabled(true);
  customerInput->setDragDropMode(QAbstractItemView::DragDrop);
  customerInput->setRemovable(true);
  customerInput->setModelColumn(0);
  customerInput->setShowLineEdit(mjendruk::ShowLineEdit::Always);

  auto customerModel = new CustomerCompletionModel{this};

  auto completer = new QCompleter{customerModel, this};
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->popup()->setMinimumWidth(300);
  completer->setCompletionColumn(0);
  completer->setFilterMode(Qt::MatchContains);

  customerInput->lineEdit()->setCompleter(completer);

  connect(
      completer, QOverload<QModelIndex const&>::of(&QCompleter::activated),
      this,
      [=](QModelIndex index) {
        selectedCustomerModel->append(
            completer->completionModel()
                ->data(index, CustomerCompletionModel::NameRole)
                .toString(),
            completer->completionModel()
                ->data(index, CustomerCompletionModel::EmailRole)
                .toString());

        customerInput->lineEdit()->clear();
      },
      Qt::QueuedConnection);

  m_ui->formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  m_ui->formLayout->addRow("Recipients", customerInput);
}

MainWindow::~MainWindow() {}

void MainWindow::on_about() {
  QString about = QString() + "Version:   " + QTTOKENEDIT_VERSION + "\n";

  QMessageBox::about(this, "template project", about);
}
