#pragma once

#include <QtCore/QFile>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QProxyStyle>
#include <QtWidgets/QStyleFactory>

#include <qtadvwidgets/qtadvwidgets_api.h>

class QTADVWIDGETS_API DarkStyle : public QProxyStyle {
  Q_OBJECT

 public:
  DarkStyle();
  explicit DarkStyle(QStyle* style);

  QStyle* baseStyle() const;

  void extracted(QPalette& palette);

  void polish(QPalette& palette) override;

 private:
  QStyle* styleBase(QStyle* style = nullptr) const;
};
