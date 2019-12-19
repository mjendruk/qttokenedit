
#include "MainWindow.h"

#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>

#include "ui_MainWindow.h"

#include <qtadvwidgets/MultiSelectionEdit.h>


MainWindow::MainWindow()
: m_ui(new Ui::MainWindow)
{
    // Setup UI
    m_ui->setupUi(this);

    auto multiSelectionEdit = new MultiSelectionEdit{this};
    m_ui->formLayout->addRow("MultiSelectionEdit", multiSelectionEdit);

    multiSelectionEdit->addItems({
        "S+U Potsdamer Platz",
        "Stendaler Str.",
        "Walther-Schreiber-Platz",
        "S+U Berlin Hbf",
        "U Spichernstr.",
        "U Berliner Str.",
        "S+U Bundesallee",
    });

    auto lineEdit = multiSelectionEdit->lineEdit();

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
        multiSelectionEdit->addItem(lineEdit->text());
        lineEdit->clear();
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_about()
{
    QString about = QString() +
        "Version:   " + QT_WIDGETS_EXTENSIONS_VERSION + "\n";

    QMessageBox::about(this, "template project", about);
}
