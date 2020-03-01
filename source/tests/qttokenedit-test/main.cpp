
#include <gmock/gmock.h>

#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication application{argc, argv};

  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
