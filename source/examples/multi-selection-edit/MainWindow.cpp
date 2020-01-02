
#include "MainWindow.h"

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEdit.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QStringListModel>
#include <QListView>
#include <QTableView>
#include <QStandardItemModel>
#include <QRegularExpression>

#include "ui_MainWindow.h"

namespace
{
  QString abbreviation(QString const& str)
  {
    auto result = str;
    result.remove(QRegularExpression{"\\W"});
    
    constexpr auto abbrSize = 6;
    if (result.size() > abbrSize) {
      result.truncate(abbrSize);
    }
    
    return result.toUpper();
  }
}

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);

  {
    auto tokenEdit = new TokenEdit{TokenEditMode::Multiple, this};
    tokenEdit->setModelColumn(0);
    
    auto listView = new QListView{this};
    listView->setMovement(QListView::Snap);
    
    auto const longNames = QStringList{
      "S+U Potsdamer Platz",
      "Stendaler Str.",
      "Walther-Schreiber-Platz",
      "S+U Berlin Hbf",
      "U Spichernstr.",
      "U Berliner Str.",
      "S+U Bundesallee",
    };
    
    auto model = new QStringListModel{this};
    
    model->setStringList(longNames);

    tokenEdit->setModel(model);
    listView->setModel(model);
    
    m_ui->formLayout->addRow("MultipleTokenEdit", tokenEdit);
    m_ui->formLayout->addRow("ListView", listView);

    auto lineEdit = tokenEdit->lineEdit();

    lineEdit->setPlaceholderText("Halt hinzufügen");

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
      auto const row = model->rowCount();
      auto const text = lineEdit->text();
      model->insertRow(row);
      model->setData(model->index(row), text);
      lineEdit->clear();
    });
  }
  {
    auto tokenEdit = new TokenEdit{TokenEditMode::Single, this};
    
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
