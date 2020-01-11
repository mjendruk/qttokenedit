#include "qtadvwidgets/ElidableLabel.h"

#include <QPaintEvent>
#include <QPainter>

ElidableLabel::ElidableLabel(QWidget* parent) : ElidableLabel{{}, parent} {}

ElidableLabel::ElidableLabel(QString const& text, QWidget* parent)
    : QWidget{parent},
      _text{text},
      _minVisibleCharacters{0},
      _colorRole{QPalette::Text} {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  updateTextSize();
}

QString const& ElidableLabel::text() const { return _text; }

void ElidableLabel::setText(QString const& text) {
  if (_text == text) {
    return;
  }

  _text = text;
  updateTextSize();
  updateGeometry();
  update();
}

int ElidableLabel::minVisibleCharacters() const {
  return _minVisibleCharacters;
}

void ElidableLabel::setMinVisibleCharacters(int count) {
  if (_minVisibleCharacters == count) {
    return;
  }

  _minVisibleCharacters = count;
  updateMinimumTextSize();
  updateGeometry();
  update();
}

QPalette::ColorRole ElidableLabel::textColorRole() const { return _colorRole; }

void ElidableLabel::setTextColorRole(QPalette::ColorRole role) {
  if (_colorRole == role) {
    return;
  }

  _colorRole = role;
  update();
}

void ElidableLabel::draw(QPainter* painter) const {
  painter->save();
  painter->setPen(palette().color(_colorRole));
  painter->drawText(rect(), Qt::TextSingleLine | Qt::AlignLeft, elidedText());
  painter->restore();
}

QSize ElidableLabel::sizeHint() const { return _textSize; }

QSize ElidableLabel::minimumSizeHint() const { return _minTextSize; }

void ElidableLabel::paintEvent(QPaintEvent* event) {
  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});
  draw(&painter);
}

void ElidableLabel::updateTextSize() {
  _textSize = fontMetrics().size(Qt::TextSingleLine, _text);

  updateMinimumTextSize();
}

void ElidableLabel::updateMinimumTextSize() {
  static auto const ellipsis = QChar{0x2026};
  _minTextSize = fontMetrics().size(
      Qt::TextSingleLine, _text.left(_minVisibleCharacters).append(ellipsis));
}

QString ElidableLabel::elidedText() const {
  return fontMetrics().elidedText(_text, Qt::ElideRight, width());
}
