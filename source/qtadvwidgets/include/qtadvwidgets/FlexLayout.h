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
  
  int numRows() const;
  int rowHeight(int index) const;

 private:
  int doLayout(const QRect &rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pm) const;

  QList<QLayoutItem *> _itemList;
  int _hSpacing;
  int _vSpacing;
  mutable QVector<int> _rowHeights;
};
