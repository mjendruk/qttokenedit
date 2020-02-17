#pragma once

#include <QtWidgets/QLineEdit>

#include <qttokenedit/TokenDropTarget.h>
#include <qttokenedit/qttokenedit_api.h>

namespace mjendruk {

class QTTOKENEDIT_API TokenLineEdit : public TokenDropTarget<QLineEdit> {
  Q_OBJECT
 public:
  explicit TokenLineEdit(AbstractTokenDragDropHandler* handler,
                         QWidget* parent = nullptr);

 signals:
  void backspaceAtBeginning();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

  QSize sizeHint() const override;

 private:
  void onTextChanged(QString const& text);

 private:
  int _widthHint;
};

}  // namespace mjendruk
