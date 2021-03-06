#include <qttokenedit/FlexLayout.h>

#include <cmath>

#include <QtWidgets/QLayoutItem>
#include <QtWidgets/QWidget>
#include <QtWidgets/QWidgetItem>

namespace mjendruk {

FlexLayout::FlexLayout(QWidget *parent) : FlexLayout{-1, -1, -1, parent} {}

FlexLayout::FlexLayout(int margin, int hSpacing, int vSpacing, QWidget *parent)
    : QLayout{parent},
      _hSpacing{hSpacing},
      _vSpacing{vSpacing},
      _frozen{Frozen::No} {
  setContentsMargins(margin, margin, margin, margin);
}

FlexLayout::~FlexLayout() {
  QLayoutItem *item{};
  while ((item = takeAt(0))) delete item;
}

void FlexLayout::addItem(QLayoutItem *item) {
  insertItem(_itemList.size(), item);
}

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

    if (_frozen != Frozen::No) {
      _frozen = Frozen::YesWithChanges;
    } else {
      invalidate();
    }

    return item;
  }
  return nullptr;
}

Qt::Orientations FlexLayout::expandingDirections() const {
  return Qt::Horizontal;
}

bool FlexLayout::hasHeightForWidth() const { return true; }

int FlexLayout::heightForWidth(int width) const {
  if (_frozen != Frozen::No) {
    return geometry().height();
  }
  auto properties = doLayout(QRect(0, 0, width, 0), true);
  return properties.height;
}

void FlexLayout::setGeometry(const QRect &rect) {
  QLayout::setGeometry(rect);

  if (_frozen != Frozen::No) {
    _frozen = Frozen::YesWithChanges;
    return;
  }

  doLayout(rect, false);
}

QSize FlexLayout::sizeHint() const { return minimumSize(); }

QSize FlexLayout::minimumSize() const {
  QSize size;
  for (const auto item : qAsConst(_itemList))
    size = size.expandedTo(item->minimumSize());

  auto margins = contentsMargins();
  size +=
      QSize(margins.left() + margins.right(), margins.top() + margins.bottom());

  return size;
}

void FlexLayout::insertItem(int index, QLayoutItem *item) {
  _itemList.insert(index, item);

  if (_frozen != Frozen::No) {
    _frozen = Frozen::YesWithChanges;
  } else {
    invalidate();
  }
}

void FlexLayout::insertLayout(int index, QLayout *layout) {
  insertItem(index, layout);
}

void FlexLayout::insertWidget(int index, QWidget *widget) {
  Q_ASSERT(widget);

  addChildWidget(widget);
  insertItem(index, new QWidgetItem{widget});
}

int FlexLayout::resultingHorizontalSpacing(QLayoutItem *leftItem,
                                           QLayoutItem *rightItem) const {
  auto leftWidget = leftItem->widget();
  auto rightWidget = rightItem->widget();

  Q_ASSERT(leftWidget);
  Q_ASSERT(rightWidget);

  auto hSpacing = horizontalSpacing();

  if (hSpacing == -1) {
    hSpacing = leftWidget->style()->layoutSpacing(
        leftWidget->sizePolicy().controlType(),
        rightWidget->sizePolicy().controlType(), Qt::Horizontal);
  }

  return hSpacing;
}

int FlexLayout::resultingVerticalSpacing(QLayoutItem *item) const {
  auto widget = item->widget();

  Q_ASSERT(widget);

  auto vSpacing = verticalSpacing();

  if (vSpacing == -1) {
    vSpacing = widget->style()->layoutSpacing(
        widget->sizePolicy().controlType(), widget->sizePolicy().controlType(),
        Qt::Vertical);
  }

  return vSpacing;
}

auto FlexLayout::findNextLine(LayoutItemConstIterator begin,
                              LayoutItemConstIterator end, int width) const
    -> std::pair<LayoutItemConstIterator, RemainingWidth> {
  auto remainingWidth = width;
  auto lastHSpacing = 0;

  auto it = begin;
  while (it != end) {
    auto const size = (*it)->sizeHint();

    if (size.isEmpty()) {
      ++it;
      continue;
    }

    auto nextIt = std::next(it);

    auto const hSpacing =
        (nextIt == end) ? 0 : resultingHorizontalSpacing((*it), (*nextIt));

    if (remainingWidth < size.width() && it != begin) {
      break;
    }

    remainingWidth -= (size.width() + hSpacing);

    it = nextIt;
    lastHSpacing = hSpacing;
  }

  remainingWidth += lastHSpacing;

  return std::make_pair(it, remainingWidth);
}

auto FlexLayout::itemMetrics(LayoutItemConstIterator begin,
                             LayoutItemConstIterator end) const
    -> std::vector<ItemMetrics> {
  auto result = std::vector<ItemMetrics>{};

  for (auto it = begin; it != end; ++it) {
    auto const size = (*it)->sizeHint();

    if (size.isEmpty()) {
      continue;
    }

    auto nextIt = std::next(it);

    auto hSpacing =
        (nextIt == end) ? 0 : resultingHorizontalSpacing((*it), (*nextIt));

    result.push_back({size, hSpacing, *it});
  }

  return result;
}

void FlexLayout::tryRemoveOverflow(int overflow, QLayoutItem const *item,
                                   QSize &size) const {
  auto const adjustedWidth = size.width() - overflow;

  if (adjustedWidth >= item->minimumSize().width()) {
    size.setWidth(adjustedWidth);

    if (item->hasHeightForWidth()) {
      size.setHeight(item->heightForWidth(adjustedWidth));
    }
  }
}

std::vector<std::size_t> FlexLayout::expandingItemIndices(
    std::vector<ItemMetrics> const &itemMetrics) const {
  auto result = std::vector<std::size_t>{};

  for (auto index = std::size_t{0}; index < itemMetrics.size(); ++index) {
    if (itemMetrics.at(index).item->expandingDirections().testFlag(
            Qt::Horizontal)) {
      result.push_back(index);
    }
  }

  return result;
}

void FlexLayout::distributeRemainingWidth(
    int width, std::vector<std::size_t> const &indices,
    std::vector<ItemMetrics> &itemMetrics) const {
  auto const additionalWidth = width / indices.size();
  auto remainingWidth = width - (additionalWidth * indices.size());

  for (auto indexIt = indices.cbegin(); indexIt != indices.cend(); ++indexIt) {
    auto const additionalPixel = (remainingWidth-- > 0) ? 1 : 0;

    auto &itemMetric = itemMetrics.at(*indexIt);

    itemMetric.size.rwidth() += (additionalWidth + additionalPixel);

    if (itemMetric.item->hasHeightForWidth()) {
      itemMetric.size.setHeight(
          itemMetric.item->heightForWidth(itemMetric.size.height()));
    }
  }
}

auto FlexLayout::metricsForLine(LayoutItemConstIterator begin,
                                LayoutItemConstIterator end, int width,
                                bool testOnly) const
    -> std::pair<std::vector<ItemMetrics>, LayoutItemConstIterator> {
  auto nextLineIt = LayoutItemConstIterator{};
  auto remainingWidth = 0;
  std::tie(nextLineIt, remainingWidth) = findNextLine(begin, end, width);
      
  auto itemMetrics = this->itemMetrics(begin, nextLineIt);

  if (testOnly) {
    return std::make_pair(std::move(itemMetrics), nextLineIt);
  }

  if (remainingWidth < 0) {
    Q_ASSERT(itemMetrics.size() == 1u);

    auto const overflow = -remainingWidth;
    tryRemoveOverflow(overflow, *begin, itemMetrics.front().size);
  } else if (remainingWidth > 0) {
    auto indices = expandingItemIndices(itemMetrics); 
    if (!indices.empty()) {
      distributeRemainingWidth(remainingWidth, indices, itemMetrics);
    }
  }

  return std::make_pair(std::move(itemMetrics), nextLineIt);
}

auto FlexLayout::doLayout(QRect const &rect, bool testOnly) const
    -> LayoutProperties {
  int left, top, right, bottom;
  getContentsMargins(&left, &top, &right, &bottom);
  QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);

  auto itemIt = _itemList.cbegin();
  auto const itemEnd = _itemList.cend();

  auto lineY = effectiveRect.y();

  auto lastVSpacing = 0;

  auto lineCount = 0;

  while (itemIt != itemEnd) {
    auto itemMetrics = std::vector<ItemMetrics>{};
    auto nextLineItemIt = LayoutItemConstIterator{};
    std::tie(itemMetrics, nextLineItemIt) =
        metricsForLine(itemIt, itemEnd, effectiveRect.width(), testOnly);

    auto x = effectiveRect.x();
    auto lineHeight = 0;

    auto const vSpacing = resultingVerticalSpacing(*itemIt);

    for (auto metric : itemMetrics) {
      if (!testOnly)
        metric.item->setGeometry(QRect{QPoint{x, lineY}, metric.size});

      x = x + metric.size.width() + metric.hSpacing;

      lineHeight = qMax(lineHeight, metric.size.height());
    }

    itemIt = nextLineItemIt;
    lineY = lineY + lineHeight + vSpacing;

    lastVSpacing = vSpacing;
    ++lineCount;
  }

  auto const height = lineY - lastVSpacing - rect.y() + bottom;
  return LayoutProperties{height, lineCount};
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

int FlexLayout::lineCountForWidth(int width) const {
  auto properties = doLayout(QRect{0, 0, width, 0}, true);
  return properties.lineCount;
}

void FlexLayout::freeze() { _frozen = Frozen::Yes; }

void FlexLayout::unfreeze() {
  if (_frozen == Frozen::YesWithChanges) {
    invalidate();
  }

  _frozen = Frozen::No;
}

}  // namespace mjendruk
