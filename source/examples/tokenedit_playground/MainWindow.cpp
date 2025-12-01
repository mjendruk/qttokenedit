#include "MainWindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QFormLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTableView>

#include <qttokenedit/qttokenedit_version.h>
#include <qttokenedit/TokenEdit.h>

#include "StopItemModel.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);
  connect(m_ui->aboutAction, &QAction::triggered, this, &MainWindow::on_about);

  {
    auto tokenEdit = new mjendruk::TokenEdit{this};

    tokenEdit->setModelColumn(1);

    auto listView = new QTableView{this};

    auto const longNames = QStringList{
        "S+U Potsdamer Platz",
        "Stendaler Str.",
        "Walther-Schreiber-Platz",
        "S+U Berlin Hbf",
        "U Spichernstr.",
        "U Berliner Str.",
        "S+U Bundesallee",
        "U Birkenstraße",
        "U Osloer Straße",
        "S+U Zoologischer Garten",
        "U Güntzelstraße",
        "S+U Rathaus Steglitz",
        "Schloßstraße",
    };

    auto model = new StopItemModel{longNames, this};

    // model->setStringList(longNames);

    tokenEdit->setModel(model);

    listView->setModel(model);
    listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    listView->setDragDropMode(QAbstractItemView::DragDrop);
    listView->setDefaultDropAction(Qt::MoveAction);
    listView->setDragDropOverwriteMode(false);
    listView->setDragEnabled(true);

    listView->verticalHeader()->hide();
    auto hHeader = listView->horizontalHeader();

    hHeader->setSectionsMovable(true);

    m_ui->formLayout->addRow("TokenEdit", tokenEdit);

    connect(m_ui->resetButton, &QPushButton::clicked, [=]() {
      tokenEdit->setModel(nullptr);
      tokenEdit->setModel(model);
    });

    connect(
        m_ui->dragEnabledCheckBox, &QCheckBox::stateChanged,
        [=](auto state) { tokenEdit->setDragEnabled(state == Qt::Checked); });

    m_ui->dragDropModeComboBox->addItems(
        {"None", "Drag Only", "Drop Only", "Drag Drop", "Internal Move"});

    connect(m_ui->dragDropModeComboBox, qOverload<int>(&QComboBox::activated),
            [=](int index) {
              auto dragDropMode =
                  static_cast<QAbstractItemView::DragDropMode>(index);
              tokenEdit->setDragDropMode(dragDropMode);
            });
    
    m_ui->showLineEditComboBox->addItems({"Never", "WhenEmpty", "Always"});
    m_ui->showLineEditComboBox->setCurrentIndex(static_cast<int>(tokenEdit->showLineEdit()));
    
    connect(m_ui->showLineEditComboBox, qOverload<int>(&QComboBox::activated),
            [=](int index) {
              auto showLineEdit =
                  static_cast<mjendruk::ShowLineEdit>(index);
              tokenEdit->setShowLineEdit(showLineEdit);
            });

    m_ui->maxLineCountSpinBox->setValue(3);
    connect(m_ui->maxLineCountSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [=](auto value) { tokenEdit->setMaxLineCount(value); });

    connect(m_ui->enabledCheckBox, &QCheckBox::stateChanged,
            [=](auto state) { tokenEdit->setEnabled(state == Qt::Checked); });

    connect(m_ui->removableCheckBox, &QCheckBox::stateChanged,
            [=](auto state) { tokenEdit->setRemovable(state == Qt::Checked); });

    m_ui->formLayout->addRow("TableView", listView);

    auto lineEdit = tokenEdit->lineEdit();

    auto completer = new QCompleter{longNames, this};
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->popup()->setMinimumWidth(200);
    completer->setFilterMode(Qt::MatchContains);
    lineEdit->setCompleter(completer);

    connect(
        completer, QOverload<QString const&>::of(&QCompleter::activated), this,
        [=](QString const& text) {
          auto const row = model->rowCount();
          model->insertRow(row);
          model->setData(model->index(row, 0), text);
          lineEdit->clear();
        },
        Qt::QueuedConnection);

    //     auto otherTokenEdit = new TokenEdit{};
    //     otherTokenEdit->setModel(model);
    //     otherTokenEdit->setDragEnabled(true);
    //
    //     otherTokenEdit->show();
  }

  {
    auto const longNames = QStringList{
        "S+U Potsdamer Platz",
    };

    auto model = new StopItemModel{longNames, this};

    auto tokenEdit = new mjendruk::TokenEdit{this};
    tokenEdit->setModelColumn(0);
    tokenEdit->setDragEnabled(true);
    tokenEdit->setDragDropMode(QAbstractItemView::DragDrop);
    tokenEdit->setModel(model);

    m_ui->formLayout->addRow("2nd TokenEdit", tokenEdit);
  }

  auto tokenEdit = new mjendruk::TokenEdit{this};
  m_ui->formLayout->addRow("Empty TokenEdit", tokenEdit);
}

MainWindow::~MainWindow() {}

void MainWindow::on_about() {
  QString about =
      QString() + "Version:   " + QTTOKENEDIT_VERSION + "\n";

  QMessageBox::about(this, "template project", about);
}
