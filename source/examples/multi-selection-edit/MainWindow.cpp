
#include "MainWindow.h"

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEdit.h>
#include <qtadvwidgets/ElidableLabel.h>

#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTableView>
#include <QCompleter>

#include "ui_MainWindow.h"

namespace {
QString abbreviation(QString const& str) {
  auto result = str;
  result.remove(QRegularExpression{"\\W"});

  constexpr auto abbrSize = 6;
  if (result.size() > abbrSize) {
    result.truncate(abbrSize);
  }

  return result.toUpper();
}
}  // namespace

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);

  {
    auto tokenEdit = new TokenEdit{this};
    
    tokenEdit->setModelColumn(0);

    auto listView = new QTableView{this};

    auto const longNames = QStringList{
        "S+U Potsdamer Platz", "Stendaler Str.", "Walther-Schreiber-Platz",
        "S+U Berlin Hbf",      "U Spichernstr.", "U Berliner Str.",
        "S+U Bundesallee", "U Birkenstraße", "U Osloer Straße", "S+U Zoologischer Garten",
        "U Güntzelstraße", "S+U Rathaus Steglitz", "Schloßstraße",
    };

    auto model = new QStringListModel{this};

    model->setStringList(longNames);

    tokenEdit->setModel(model);
    listView->setModel(model);

    m_ui->formLayout->addRow("Token Edit", tokenEdit);

    connect(m_ui->resetButton, &QPushButton::clicked, [=]() {
      tokenEdit->setModel(nullptr);
      tokenEdit->setModel(model);
    });

    connect(
        m_ui->dragEnabledCheckBox, &QCheckBox::stateChanged,
        [=](auto state) { tokenEdit->setDragEnabled(state == Qt::Checked); });

    m_ui->maxLineCountSpinBox->setValue(3);
    connect(m_ui->maxLineCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](auto value) { tokenEdit->setMaxLineCount(value); });

    connect(m_ui->enabledCheckBox, &QCheckBox::stateChanged, [=](auto state) {
      tokenEdit->setEnabled(state == Qt::Checked);
    });

    connect(m_ui->removableCheckBox, &QCheckBox::stateChanged, [=](auto state) {
      tokenEdit->setRemovable(state == Qt::Checked);
    });
            
    m_ui->formLayout->addRow("TableView", listView);

   auto lineEdit = tokenEdit->lineEdit();
    
    auto completer = new QCompleter{longNames, this};
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->popup()->setMinimumWidth(200);
    completer->setFilterMode(Qt::MatchContains);
    lineEdit->setCompleter(completer);

   connect(completer, QOverload<QString const&>::of(&QCompleter::activated), this,[=](QString const& text) {
     auto const row = model->rowCount();
     model->insertRow(row);
     model->setData(model->index(row), text);
     lineEdit->clear();
   }, Qt::QueuedConnection);
    
    auto otherTokenEdit = new TokenEdit{};
    otherTokenEdit->setModel(model);
    otherTokenEdit->setDragEnabled(true);
    
    otherTokenEdit->show();
  }
  
  connect(m_ui->widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](auto value) {
    resize(value, height());
  });
  
  connect(m_ui->heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](auto value) {
    resize(width(), value);
  });
}

MainWindow::~MainWindow() {}

void MainWindow::on_about() {
  QString about =
      QString() + "Version:   " + QT_WIDGETS_EXTENSIONS_VERSION + "\n";

  QMessageBox::about(this, "template project", about);
}
