#pragma once

#include <QtCore/QFile>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QProxyStyle>
#include <QtWidgets/QStyleFactory>

#include <qttokenedit/qttokenedit_api.h>

class QTTOKENEDIT_API DarkStyle : public QProxyStyle {
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
