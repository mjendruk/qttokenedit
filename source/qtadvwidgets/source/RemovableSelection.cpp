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

RemovableSelection::RemovableSelection(QString const& text, QWidget* parent) : QWidget{parent}, _selected{false} {
  
  auto layout = new QBoxLayout{QBoxLayout::LeftToRight};
  
  auto label = new QLabel{text};
  label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  auto smallerFont = label->font();
  smallerFont.setPointSizeF(label->fontInfo().pointSizeF() * 0.8);
  label->setFont(smallerFont);
  
  auto button = new RemoveButton{this};
  
  layout->addWidget(label, 1, Qt::AlignVCenter);
  layout->addWidget(button, 0, Qt::AlignVCenter);
  layout->setContentsMargins(4, 2, 4, 2);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setLayout(layout);
  setCursor(Qt::ArrowCursor);
  
  connect(button, &RemoveButton::clicked, this, &RemovableSelection::removeClicked);
}

void RemovableSelection::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};
  
  auto rect = QRectF{QPointF{0.0,0.0}, QSizeF{size()}};

  auto path = QPainterPath{};
  path.addRect(rect);

  auto painter = QPainter{this};
  
  auto palette = this->palette();
  
  auto const penRole = _selected ? QPalette::WindowText : QPalette::Shadow;
  painter.setBrush(palette.brush(QPalette::Button));
  painter.setPen(QPen{palette.color(penRole)});
  
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
