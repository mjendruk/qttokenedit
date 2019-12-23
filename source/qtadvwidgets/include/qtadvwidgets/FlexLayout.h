#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QLayout>
#include <QStyle>

class QTADVWIDGETS_API FlexLayout : public QLayout {
 public:
  explicit FlexLayout(QWidget *parent);
  explicit FlexLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1,
                      QWidget *parent = nullptr);
  ~FlexLayout();

  void addItem(QLayoutItem *item) override;

  int horizontalSpacing() const;
  int verticalSpacing() const;
  Qt::Orientations expandingDirections() const override;
  bool hasHeightForWidth() const override;
  int heightForWidth(int) const override;
  int count() const override;
  QLayoutItem *itemAt(int index) const override;
  QSize minimumSize() const override;
  void setGeometry(const QRect &rect) override;
  QSize sizeHint() const override;
  QLayoutItem *takeAt(int index) override;

  void insertItem(int index, QLayoutItem *item);
  void insertLayout(int index, QLayout *layout);
  void insertWidget(int index, QWidget *widget);

  int lineCount() const;
  int lineHeight(int index) const;

 private:
  struct ItemMetrics {
    QSize size;
    int succHorizontalSpacing;
  };

  using LayoutItemConstIterator = QList<QLayoutItem *>::const_iterator;

  int resultingHorizontalSpacing(QLayoutItem *leftItem,
                                 QLayoutItem *rightItem) const;

  int resultingVerticalSpacing(QLayoutItem *item) const;

  void tryRemoveOverflow(int overflow, QLayoutItem const *item,
                         QSize &size) const;
  
  std::vector<std::size_t> expandingItemIndices(LayoutItemConstIterator begin,
                                                LayoutItemConstIterator end) const;
  
  void distributeRemainingWidth(
      int remainingWidth,
      std::vector<std::size_t> const& indices,
      std::vector<ItemMetrics>& itemMetrics) const;

  auto metricsForLine(LayoutItemConstIterator begin,
                      LayoutItemConstIterator end, int width) const
      -> std::pair<std::vector<ItemMetrics>, LayoutItemConstIterator>;

  int doLayout(const QRect &rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pm) const;

 private:
  QList<QLayoutItem *> _itemList;
  int _hSpacing;
  int _vSpacing;
  mutable QVector<int> _lineHeights;
};
