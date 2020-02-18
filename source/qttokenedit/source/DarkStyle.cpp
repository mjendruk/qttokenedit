#include <qttokenedit/DarkStyle.h>

DarkStyle::DarkStyle() : DarkStyle(styleBase()) {}

DarkStyle::DarkStyle(QStyle* style) : QProxyStyle(style) {}

QStyle* DarkStyle::styleBase(QStyle* style) const {
  static QStyle* base =
      !style ? QStyleFactory::create(QStringLiteral("Fusion")) : style;
  return base;
}

QStyle* DarkStyle::baseStyle() const { return styleBase(); }

void DarkStyle::polish(QPalette& palette) {
  // modify palette to dark
  
  // dark (Polar Night)
  auto nord_2 = QColor(36, 51, 51);
  auto nord_1 = QColor(40, 45, 56);
  
  auto nord0 = QColor(46, 52, 64);
  auto nord1 = QColor(59, 66, 82);
  auto nord2 = QColor(67, 76, 94);
  auto nord3 = QColor(76, 86, 106);
  
  // bright (Snow Storm)
  auto nord4 = QColor(216, 222, 233);
  auto nord5 = QColor(229, 233, 240);
  auto nord6 = QColor(236, 239, 244);
  
  // blue (Frost)
  auto nord8 = QColor(136, 192, 208);
  
  auto nord9 = QColor(129, 161, 193);
  auto nord10 = QColor(94, 129, 172);
  
  palette.setColor(QPalette::Window, nord0);
  palette.setColor(QPalette::WindowText, nord5);
  palette.setColor(QPalette::Disabled, QPalette::WindowText, nord4);
  palette.setColor(QPalette::Base, nord_1);
  palette.setColor(QPalette::AlternateBase, QColor(76, 86, 106));
  palette.setColor(QPalette::ToolTipBase, QColor(236, 239, 244));
  palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
  palette.setColor(QPalette::Text, nord5);
  palette.setColor(QPalette::Disabled, QPalette::Text, nord4);
  palette.setColor(QPalette::Dark, nord0);
  palette.setColor(QPalette::Shadow, nord_2);
  palette.setColor(QPalette::Button, nord1);
  palette.setColor(QPalette::Midlight, nord1);
  palette.setColor(QPalette::Light, nord2);
  palette.setColor(QPalette::ButtonText, nord5);
  palette.setColor(QPalette::Disabled, QPalette::ButtonText,
                   QColor(127, 127, 127));
  palette.setColor(QPalette::BrightText, nord6);
  palette.setColor(QPalette::Link, QColor(42, 130, 218));
  palette.setColor(QPalette::Highlight, nord10);
  palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(129, 161, 193));
  palette.setColor(QPalette::HighlightedText, nord6);
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                   QColor(127, 127, 127));
}
