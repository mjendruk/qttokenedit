#include <qtadvwidgets/FlexLayout.h>

#include <QWidget>

FlexLayout::FlexLayout(QWidget *parent) : FlexLayout{-1, -1, -1, parent} {}

FlexLayout::FlexLayout(int margin, int hSpacing, int vSpacing, QWidget *parent)
: QLayout{parent}, _hSpacing{hSpacing}, _vSpacing{vSpacing} {
  setContentsMargins(margin, margin, margin, margin);
}

FlexLayout::~FlexLayout() {
  QLayoutItem *item{};
  while ((item = takeAt(0))) delete item;
}

void FlexLayout::addItem(QLayoutItem *item) { _itemList.append(item); }

int FlexLayout::horizontalSpacing() const {
  if (_hSpacing >= 0) {
    return _hSpacing;
  } else {
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
  }
}

int FlexLayout::verticalSpacing() const {
  if (_vSpacing >= 0) {
    return _vSpacing;
  } else {
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
  }
}

int FlexLayout::count() const { return _itemList.size(); }

QLayoutItem *FlexLayout::itemAt(int index) const {
  return _itemList.value(index);
}

QLayoutItem *FlexLayout::takeAt(int index) {
  if (index >= 0 && index < _itemList.size()) { 
    auto item = _itemList.takeAt(index);
    activate();
    return item;
  }
  return nullptr;
}

Qt::Orientations FlexLayout::expandingDirections() const {
  return Qt::Horizontal;
}

bool FlexLayout::hasHeightForWidth() const { return true; }

int FlexLayout::heightForWidth(int width) const {
  int height = doLayout(QRect(0, 0, width, 0), true);
  return height;
}

void FlexLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlexLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlexLayout::minimumSize() const {
  QSize size;
  for (const auto item : qAsConst(_itemList))
    size = size.expandedTo(item->minimumSize());

  auto margins = contentsMargins();
  size +=
      QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
  return size;
}

int FlexLayout::doLayout(const QRect &rect, bool testOnly) const {
  int left, top, right, bottom;
  getContentsMargins(&left, &top, &right, &bottom);
  QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
  int x = effectiveRect.x();
  int y = effectiveRect.y();
  
  int rowHeight = 0;
  _rowHeights.clear();

  for (QLayoutItem *item : qAsConst(_itemList)) {
    const QWidget *wid = item->widget();
    int spaceX = horizontalSpacing();
    if (spaceX == -1)
      spaceX = wid->style()->layoutSpacing(
          QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
    int spaceY = verticalSpacing();
    if (spaceY == -1)
      spaceY = wid->style()->layoutSpacing(
          QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

    int nextX = x + item->sizeHint().width() + spaceX;
    if (nextX - spaceX > effectiveRect.right() && rowHeight > 0) {
      x = effectiveRect.x();
      y = y + rowHeight + spaceY;
      nextX = x + item->sizeHint().width() + spaceX;
      _rowHeights.append(rowHeight);
      rowHeight = 0;
    }

    if (!testOnly) item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

    x = nextX;
    rowHeight = qMax(rowHeight, item->sizeHint().height());
  }
  
  _rowHeights.append(rowHeight);
  
  return y + rowHeight - rect.y() + bottom;
}

int FlexLayout::smartSpacing(QStyle::PixelMetric pm) const {
  QObject *parent = this->parent();
  if (!parent) {
    return -1;
  } else if (parent->isWidgetType()) {
    QWidget *pw = static_cast<QWidget *>(parent);
    return pw->style()->pixelMetric(pm, nullptr, pw);
  } else {
    return static_cast<QLayout *>(parent)->spacing();
  }
}

int FlexLayout::numRows() const
{
  return _rowHeights.size();
}
int FlexLayout::rowHeight(int index) const
{
  return _rowHeights.at(index);
}
