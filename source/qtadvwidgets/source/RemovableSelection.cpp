#include <qtadvwidgets/RemovableSelection.h>

#include <cmath>

#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMouseEvent>

#include <qtadvwidgets/RemoveButton.h>

RemovableSelection::RemovableSelection(QString const& text, QWidget* parent)
    : RemovableSelection{text, {}, parent} {}

RemovableSelection::RemovableSelection(QString const& text, QVariant const& userData,
                   QWidget* parent)
    : QWidget{parent}, _text{text}, _userData{userData}, _selected{false} {
  auto layout = new QBoxLayout{QBoxLayout::LeftToRight};

  _label = new QLabel{text};
  _label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  auto const labelHeight = _label->sizeHint().height();

  _button = new RemoveButton{labelHeight, this};

  auto const margin = std::lround(labelHeight / 3.0);
  auto const marginLeft = std::lround(labelHeight * 0.5);
  layout->setContentsMargins(marginLeft, margin, margin, margin);

  layout->addWidget(_label, 1, Qt::AlignVCenter);
  layout->addWidget(_button, 0, Qt::AlignVCenter);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setLayout(layout);
  setCursor(Qt::ArrowCursor);

  connect(_button, &RemoveButton::clicked, this,
          &RemovableSelection::removeClicked);
}

QString const& RemovableSelection::text() const
{
  return _text;
}

void RemovableSelection::setText(QString const& text)
{
  _text = text;
  _label->setText(_text);
}

QVariant const& RemovableSelection::userData() const
{
  return _userData;
}

void RemovableSelection::setUserData(QVariant const& data)
{
  _userData = data;
}

void RemovableSelection::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};
  
  auto rect = QRectF{QPointF{0.0,0.0}, QSizeF{size()}};

  auto const rounding = rect.height() / 2.0;

  auto path = QPainterPath{};
  path.addRoundedRect(rect, rounding, rounding);

  auto painter = QPainter{this};
  
  auto palette = this->palette();
  
  auto const penRole = _selected ? QPalette::WindowText : QPalette::Shadow;
  painter.setBrush(palette.brush(QPalette::Window));
  // painter.setPen(QPen{palette.color(penRole)});
  painter.setPen(Qt::NoPen);
  
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setClipRect(clippingRect);
  painter.drawPath(path);
  
  QWidget::paintEvent(event);
}

void RemovableSelection::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton) {
    _pressed = true;
    update();
  }
}

void RemovableSelection::mouseReleaseEvent(QMouseEvent *event)
{
  if (_pressed) {
    _selected = !_selected;
    _pressed = false;
    update();
  }
}

void RemovableSelection::leaveEvent(QEvent *event)
{
  _pressed = false;
  update();
}
