#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QLayout>
#include <QStyle>

class QTADVWIDGETS_API FlexLayout : public QLayout {
  Q_OBJECT
 public:
  explicit FlexLayout(QWidget *parent);
  explicit FlexLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1,
                      QWidget *parent = nullptr);
  ~FlexLayout();

  void addItem(QLayoutItem *item) override;

  Qt::Orientations expandingDirections() const override;
  bool hasHeightForWidth() const override;
  int heightForWidth(int width) const override;
  int count() const override;
  QLayoutItem *itemAt(int index) const override;
  QSize minimumSize() const override;
  void setGeometry(const QRect &rect) override;
  QSize sizeHint() const override;
  QLayoutItem *takeAt(int index) override;

  int horizontalSpacing() const;
  int verticalSpacing() const;

  void insertItem(int index, QLayoutItem *item);
  void insertLayout(int index, QLayout *layout);
  void insertWidget(int index, QWidget *widget);

  int lineCountForWidth(int width) const;
  
  void freeze();
  void unfreeze();
  
 signals:
  void geometryChanged();

 private:
  struct ItemMetrics {
    QSize size;
    int hSpacing;  //!< succeeding horizontal spacing
    QLayoutItem *item;
  };

  using LayoutItemConstIterator = QList<QLayoutItem *>::const_iterator;
  using RemainingWidth = int;

  int resultingHorizontalSpacing(QLayoutItem *leftItem,
                                 QLayoutItem *rightItem) const;

  int resultingVerticalSpacing(QLayoutItem *item) const;

  auto findNextLine(LayoutItemConstIterator begin, LayoutItemConstIterator end,
                    int width) const
      -> std::pair<LayoutItemConstIterator, RemainingWidth>;

  auto itemMetrics(LayoutItemConstIterator begin,
                   LayoutItemConstIterator end) const
      -> std::vector<ItemMetrics>;

  void tryRemoveOverflow(int overflow, QLayoutItem const *item,
                         QSize &size) const;

  auto expandingItemIndices(std::vector<ItemMetrics> const& itemMetrics) const
      -> std::vector<std::size_t>;

  void distributeRemainingWidth(int width,
                                std::vector<std::size_t> const &indices,
                                std::vector<ItemMetrics> &itemMetrics) const;

  auto metricsForLine(LayoutItemConstIterator begin,
                      LayoutItemConstIterator end, int width, bool testOnly) const
      -> std::pair<std::vector<ItemMetrics>, LayoutItemConstIterator>;

  struct LayoutProperties
  {
    int height;
    int lineCount;
  };
  
  LayoutProperties doLayout(const QRect &rect, bool testOnly) const;

  int smartSpacing(QStyle::PixelMetric pm) const;

 private:
  QList<QLayoutItem *> _itemList;
  int _hSpacing;
  int _vSpacing;
  mutable int _lineCount;

  enum class Frozen : std::int8_t {
    No,
    Yes,
    YesWithChanges,
  };

  Frozen _frozen;
};
