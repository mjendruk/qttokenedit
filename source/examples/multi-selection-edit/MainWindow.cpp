
#include "MainWindow.h"

#include <QMessageBox>
#include <QFormLayout>

#include <qt-widgets-extensions/qt-widgets-extensions-version.h>

#include "ui_MainWindow.h"

#include <qtadvwidgets/MultiSelectionEdit.h>


MainWindow::MainWindow()
: m_ui(new Ui::MainWindow)
{
    // Setup UI
    m_ui->setupUi(this);

    m_ui->formLayout->addRow("MultiSelectionEdit", new MultiSelectionEdit{this});
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
