#include <qtadvwidgets/RemovableSelection.h>

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
//  label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  auto smallerFont = label->font();
  smallerFont.setPointSizeF(label->fontInfo().pointSizeF() * 0.8);
  label->setFont(smallerFont);
  
  auto button = new RemoveButton{this};
  
  layout->addWidget(label, 1, Qt::AlignVCenter);
  layout->addWidget(button, 0, Qt::AlignVCenter);
  layout->setContentsMargins(4, 2, 4, 2);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  setLayout(layout);
  setCursor(Qt::ArrowCursor);
  
  connect(button, &RemoveButton::clicked, this, &RemovableSelection::removeClicked);
}

void RemovableSelection::paintEvent(QPaintEvent* event) {
  auto const clippingRect = QRectF{event->rect()};
  
  auto rect = QRectF{QPointF{0.0,0.0}, QSizeF{size()}};

  auto path = QPainterPath{};
  path.addRoundedRect(rect, 3.0, 3.0);

  auto painter = QPainter{this};
  
  auto palette = this->palette();
  
  auto const fillRole = _selected ? QPalette::Highlight : QPalette::Window;
  painter.setBrush(palette.brush(fillRole));
  painter.setPen(QPen{palette.color(QPalette::Dark)});
  
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
