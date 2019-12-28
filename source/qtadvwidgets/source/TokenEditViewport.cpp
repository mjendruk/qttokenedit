#include <qtadvwidgets/Token.h>
#include <qtadvwidgets/TokenEditViewport.h>

#include <QBoxLayout>
#include <QChildEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFocusRect>

class TokenEditViewportFrame : public QWidget {
 public:
  TokenEditViewportFrame(TokenEditViewport* parent = nullptr)
      : QWidget{parent}, _parent{parent} {
    setAttribute(Qt::WA_TransparentForMouseEvents);
  }

 protected:
  void paintEvent(QPaintEvent* event) override {
    auto painter = QPainter{this};
    painter.setClipRect(event->rect());

    auto option = QStyleOptionFrame{};
    option.initFrom(this);
    option.frameShape = QFrame::StyledPanel;

    if (_parent->underMouse()) {
      option.state |= QStyle::State_MouseOver;
    }

    if (_parent->shownAsFocused()) {
      option.state |= QStyle::State_HasFocus;
    }

    style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, &painter);
  }

 private:
  TokenEditViewport* _parent;
};

TokenEditViewport::TokenEditViewport(QWidget* parent)
    : QWidget{parent},
      _widget{nullptr},
      _layout{new QBoxLayout{QBoxLayout::TopToBottom}},
      _frame{new TokenEditViewportFrame{this}} {
  setFocusPolicy(Qt::StrongFocus);
  _layout->setContentsMargins(contentMargins());
  setLayout(_layout);
}

void TokenEditViewport::setWidget(QWidget* widget) {
  if (_widget) {
    _layout->removeWidget(_widget);
    if (_widget->parent() == this) {
      delete _widget;
    }
  }

  _widget = widget;
  _layout->addWidget(_widget);
  _frame->raise();
}

bool TokenEditViewport::shownAsFocused() const { return _shownAsFocused; }

void TokenEditViewport::setShownAsFocused(bool value) {
  _shownAsFocused = value;
  update();
}

void TokenEditViewport::paintEvent(QPaintEvent* event) {
  auto painter = QPainter{this};
  painter.setClipRect(event->rect());

  painter.save();

  painter.setBrush(palette().brush(QPalette::Base));
  painter.setPen(Qt::NoPen);
  painter.drawRect(contentRect());

  painter.restore();

  QWidget::paintEvent(event);
}

void TokenEditViewport::resizeEvent(QResizeEvent* event) {
  _frame->resize(size());

  QWidget::resizeEvent(event);
}

QRect TokenEditViewport::contentRect() const {
  auto option = QStyleOptionFrame{};
  option.initFrom(this);
  option.frameShape = QFrame::StyledPanel;

  return style()->subElementRect(QStyle::SE_ShapedFrameContents, &option);
}

QMargins TokenEditViewport::contentMargins() const {
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
