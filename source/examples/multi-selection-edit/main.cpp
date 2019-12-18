
#include <qtadvwidgets/DarkStyle.h>

#include <QApplication>
#include <QStyleFactory>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
  auto application = QApplication{argc, argv};
  //    application.setStyle(new DarkStyle{});
  
  application.setStyle(QStyleFactory::create("Fusion"));

  auto mainWindow = MainWindow{};
  mainWindow.show();

  return application.exec();
}
