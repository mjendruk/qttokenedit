
#include "MainWindow.h"

#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>

#include "ui_MainWindow.h"

#include <qtadvwidgets/TokenEdit.h>
#include <qtadvwidgets/Token.h>


MainWindow::MainWindow()
: m_ui(new Ui::MainWindow)
{
    // Setup UI
    m_ui->setupUi(this);

    auto tokenEdit = new TokenEdit{this};
    m_ui->formLayout->addRow("TokenEdit", tokenEdit);

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

    connect(lineEdit, &QLineEdit::returnPressed, [=]() {
        tokenEdit->addItem(lineEdit->text());
        lineEdit->clear();
    });

    m_ui->formLayout->addRow("Token", new Token{"S+U Potsdamer Platz"});
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
