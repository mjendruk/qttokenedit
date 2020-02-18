#include <qttokenedit/TokenEditFrame.h>

#include <QtCore/QChildEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QStyleOptionFocusRect>

namespace mjendruk {

class TokenEditFrameOverlay : public QWidget {
 public:
  TokenEditFrameOverlay(TokenEditFrame* parent = nullptr)
      : QWidget{parent}, _parent{parent} {
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents);
  }

 protected:
  void paintEvent(QPaintEvent* event) override {
    auto painter = QPainter{this};
    painter.setClipRect(event->rect());

    auto option = QStyleOptionFrame{};
    option.initFrom(_parent);
    option.frameShape = QFrame::StyledPanel;
    option.state.setFlag(QStyle::State_MouseOver, _parent->underMouse());
    option.state.setFlag(QStyle::State_HasFocus, _parent->shownAsFocused());

    style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, &painter);
  }

 private:
  TokenEditFrame* _parent;
};

TokenEditFrame::TokenEditFrame(QWidget* parent)
    : QWidget{parent},
      _widget{nullptr},
      _layout{new QBoxLayout{QBoxLayout::TopToBottom}},
      _frame{new TokenEditFrameOverlay{this}},
      _shownAsFocused{false} {
  setAttribute(Qt::WA_Hover);
  setBackgroundRole(QPalette::Base);
  _layout->setContentsMargins(contentMargins());
  setLayout(_layout);
}

QWidget* TokenEditFrame::takeWidget() {
  if (!_widget) {
    return nullptr;
  }

  auto widget = _widget;
  _layout->removeWidget(widget);
  widget->setParent(nullptr);

  _widget = nullptr;
  return widget;
}

void TokenEditFrame::setWidget(QWidget* widget) {
  if (_widget) {
    auto old = takeWidget();
    delete old;
  }

  _layout->addWidget(widget);
  _frame->raise();
  _frame->update();

  _widget = widget;
}

bool TokenEditFrame::shownAsFocused() const { return _shownAsFocused; }

void TokenEditFrame::setShownAsFocused(bool value) {
  _shownAsFocused = value;
  _frame->update();
}

void TokenEditFrame::paintEvent(QPaintEvent* event) {
  auto painter = QPainter{this};
  painter.setClipRect(event->rect());

  painter.save();

  painter.setBrush(palette().brush(backgroundRole()));
  painter.setPen(Qt::NoPen);
  painter.drawRect(contentRect());

  painter.restore();

  QWidget::paintEvent(event);
}

void TokenEditFrame::resizeEvent(QResizeEvent* event) {
  _frame->resize(size());

  QWidget::resizeEvent(event);
}

void TokenEditFrame::leaveEvent(QEvent* event) {
  update();
  QWidget::leaveEvent(event);
}

void TokenEditFrame::enterEvent(QEvent* event) {
  update();
  QWidget::enterEvent(event);
}

QRect TokenEditFrame::contentRect() const {
  auto option = QStyleOptionFrame{};
  option.initFrom(this);
  option.frameShape = QFrame::StyledPanel;

  return style()->subElementRect(QStyle::SE_ShapedFrameContents, &option);
}

QMargins TokenEditFrame::contentMargins() const {
  auto rect = QWidget::rect();
  auto contentRect = this->contentRect();

  Q_ASSERT(rect.contains(contentRect));

  return QMargins{
      contentRect.left() - rect.left(),
      contentRect.top() - rect.top(),
      rect.right() - contentRect.right(),
      rect.bottom() - contentRect.bottom(),
  };
}

}  // namespace mjendruk
