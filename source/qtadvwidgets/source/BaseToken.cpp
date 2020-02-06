#include <qtadvwidgets/BaseToken.h>
#include <qtadvwidgets/ElidableLabel.h>

#include <QBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

BaseToken::BaseToken(QWidget* parent) : BaseToken{{}, parent} {}

BaseToken::BaseToken(QString const& text, QWidget* parent)
    : QWidget{parent},
      _layout{new QBoxLayout{QBoxLayout::LeftToRight, this}},
      _label{new ElidableLabel{text, this}},
      _rightWidget{nullptr} {
  _layout->addWidget(_label);
  _layout->setSpacing(spacing());
  _layout->setContentsMargins(horizontalTextMargin(), margin(),
                              horizontalTextMargin(), margin());
  
  setFocusPolicy(Qt::ClickFocus);

  _label->setTextColorRole(QPalette::ButtonText);
        
  setAttribute(Qt::WA_Hover);
        
  updateMargins();
}

BaseToken::~BaseToken() = default;

QString const& BaseToken::text() const { return _label->text(); }

void BaseToken::setText(QString const& text) { _label->setText(text); }

QWidget* BaseToken::rightWidget() const { return _rightWidget; }

void BaseToken::setRightWidget(QWidget* widget) {
  if (_rightWidget == widget) {
    return;
  }

  if (_rightWidget) {
    _layout->removeWidget(_rightWidget);
    if (_rightWidget->parent() == this) {
      delete _rightWidget;
    }
  }

  if (widget) {
    _layout->addWidget(widget, 0, Qt::AlignCenter);
  }

  _rightWidget = widget;
  updateMargins();
}

int BaseToken::contentHeight() const { return _label->sizeHint().height(); }

void BaseToken::updateMargins() { _layout->setContentsMargins(margins()); }

void BaseToken::drawBackground(QPainter* painter, QBrush brush) const {
  painter->save();

  auto rect = QRectF{QPointF{0.0, 0.0}, QSizeF{size()}};

  auto const rounding = contentHeight() / 4.0;

  painter->setBrush(brush);
  painter->setPen(Qt::NoPen);
  painter->drawRoundedRect(rect, rounding, rounding);

  painter->restore();
}

int BaseToken::horizontalTextMargin() const { return margin() * 2; }

int BaseToken::margin() const { return std::lround(contentHeight() / 6.0); }

int BaseToken::spacing() const { return std::lround(contentHeight() * 0.25); }

QMargins BaseToken::margins() const {
  auto const rightMargin = _rightWidget && _rightWidget->isVisibleTo(this)
                               ? margin()
                               : horizontalTextMargin();
  return QMargins{horizontalTextMargin(), margin(), rightMargin, margin()};
}

void BaseToken::paintEvent(QPaintEvent* event) {
  auto painter = QPainter{this};
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(QRectF{event->rect()});

  auto brushRole = QPalette::Button;

  if (hasFocus()) {
    brushRole = QPalette::Highlight;
  }
  
  auto brush = palette().brush(brushRole);
  
  if (underMouse() && isEnabled()) {
    auto color = brush.color();
    int grayValue = qGray(color.rgb());
    color = color.lighter(100 + qMax(2, (180 - grayValue)/6));
    brush.setColor(color);
  }

  drawBackground(&painter, brush);

  QWidget::paintEvent(event);
}

void BaseToken::focusInEvent(QFocusEvent* event) {
  _label->setTextColorRole(QPalette::HighlightedText);
  QWidget::focusInEvent(event);
}

void BaseToken::focusOutEvent(QFocusEvent* event) {
  _label->setTextColorRole(QPalette::ButtonText);
  QWidget::focusOutEvent(event);
}
