
#include "MainWindow.h"

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>
#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEdit.h>

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>

#include "ui_MainWindow.h"

MainWindow::MainWindow() : m_ui(new Ui::MainWindow) {
  // Setup UI
  m_ui->setupUi(this);

  {
    auto tokenEdit = new TokenEdit{TokenEditMode::Multiple, this};
    m_ui->formLayout->addRow("MultipleTokenEdit", tokenEdit);

    tokenEdit->addItems({
        "S+U Potsdamer Platz",
        "Stendaler Str.",
        "Walther-Schreiber-Platz",
        "S+U Berlin Hbf",
        "U Spichernstr.",
        "U Berliner Str.",
        "S+U Bundesallee",
    });

    auto lineEdit = tokenEdit->lineEdit();

    lineEdit->setPlaceholderText("Halt hinzufügen");

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
      tokenEdit->addItem(lineEdit->text());
      lineEdit->clear();
    });
  }
  {
    auto tokenEdit = new TokenEdit{TokenEditMode::Single, this};
    m_ui->formLayout->addRow("SingleTokenEdit", tokenEdit);

    auto lineEdit = tokenEdit->lineEdit();

    lineEdit->setPlaceholderText("Halt auswählen");

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
      tokenEdit->addItem(lineEdit->text());
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
