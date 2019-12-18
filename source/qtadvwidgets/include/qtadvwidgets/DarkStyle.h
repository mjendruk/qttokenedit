#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QProxyStyle>
#include <QStyleFactory>

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
