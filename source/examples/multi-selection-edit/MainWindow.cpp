
#include "MainWindow.h"

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEdit.h>

#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTableView>

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
    auto tokenEdit = new TokenEdit{TokenEditMode::AlwaysShowLineEdit, this};

    auto palette = tokenEdit->palette();
    palette.setColor(QPalette::Highlight, QColor(3, 158, 230));
    tokenEdit->setPalette(palette);

    tokenEdit->setModelColumn(0);

    auto listView = new QTableView{this};

    auto const longNames = QStringList{
        "S+U Potsdamer Platz", "Stendaler Str.", "Walther-Schreiber-Platz",
        "S+U Berlin Hbf",      "U Spichernstr.", "U Berliner Str.",
        "S+U Bundesallee",
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
            
    m_ui->formLayout->addRow("TableView", listView);

    auto lineEdit = tokenEdit->lineEdit();

    lineEdit->setPlaceholderText("Halt hinzufügen");

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
      auto const row = model->rowCount();
      auto const text = lineEdit->text();
      model->insertRow(row);
      model->setData(model->index(row), text);
      lineEdit->clear();
    });
    
    auto neverShowLineEditTokenEdit = new TokenEdit{TokenEditMode::NeverShowLineEdit, this};

    neverShowLineEditTokenEdit->setModel(model);

    m_ui->formLayout->addWidget(neverShowLineEditTokenEdit);
  }
  {
    auto tokenEdit = new TokenEdit{TokenEditMode::ShowLineEditIfEmpty, this};

    auto model = new QStringListModel{this};

    tokenEdit->setModel(model);

    m_ui->formLayout->addRow("SingleTokenEdit", tokenEdit);

    auto lineEdit = tokenEdit->lineEdit();

    lineEdit->setPlaceholderText("Halt auswählen");

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
      auto const row = model->rowCount();
      model->insertRow(row);
      model->setData(model->index(row), lineEdit->text());
      lineEdit->clear();
    });
  }
}

MainWindow::~MainWindow() {}

void MainWindow::on_about() {
  QString about =
      QString() + "Version:   " + QT_WIDGETS_EXTENSIONS_VERSION + "\n";

  QMessageBox::about(this, "template project", about);
}
