#include <qttokenedit/ElidableLabel.h>

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

namespace mjendruk {

ElidableLabel::ElidableLabel(QWidget* parent) : ElidableLabel{{}, parent} {}

ElidableLabel::ElidableLabel(QString const& text, QWidget* parent)
    : QWidget{parent},
      _text{text},
      _minVisibleCharacters{0} {
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

QSize ElidableLabel::sizeHint() const { return _textSize; }

QSize ElidableLabel::minimumSizeHint() const { return _minTextSize; }

void ElidableLabel::paintEvent(QPaintEvent* event) {
  QPainter painter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  painter.save();
  painter.setPen(palette().color(foregroundRole()));
  painter.drawText(rect(), Qt::TextSingleLine | Qt::AlignLeft, elidedText());
  painter.restore();
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

}  // namespace mjendruk
