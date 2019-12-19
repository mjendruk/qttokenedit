#include <qtadvwidgets/FlexLayout.h>
#include <qtadvwidgets/MultiSelectionEdit.h>
#include <qtadvwidgets/RemovableSelection.h>

#include <QBoxLayout>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFrame>

MultiSelectionEdit::MultiSelectionEdit(QWidget* parent) : QScrollArea{parent} {
  auto mainWidget = new QWidget{};

  auto entries = std::vector{
      "S+U Potsdamer Platz", "Stendaler Str.", "Walther-Schreiber-Platz",
      "S+U Berlin Hbf",      "U Spichernstr.", "U Berliner Str.",
      "S+U Bundesallee",
  };

  _layout = new FlexLayout{4, 4, 4};

  for (auto entry : entries) {
    auto selection = new RemovableSelection{entry, this};
    _layout->addWidget(selection);

    connect(selection, &RemovableSelection::removeClicked,
            [=]() { delete selection; updateHeight(); });

    _selections.append(selection);
  }

  _lineEdit = new QLineEdit{this};
  _lineEdit->setFrame(false);
  _lineEdit->setMinimumSize(QSize{120, 0});
  _lineEdit->setPlaceholderText("Halt hinzufuegen");
  _lineEdit->setTextMargins(QMargins{});
  _lineEdit->setFixedHeight(_selections.front()->sizeHint().height());

  _layout->addWidget(_lineEdit);

  mainWidget->setLayout(_layout);

  mainWidget->setStyleSheet("#mainWidget { background-color: white; }");
  mainWidget->setObjectName("mainWidget");

  setWidget(mainWidget);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setWidgetResizable(true);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  setMaxRows(3);
}

int MultiSelectionEdit::maxRows() const { return _maxRows; }

void MultiSelectionEdit::setMaxRows(int rows) {
  _maxRows = rows;

  updateHeight();
}

void MultiSelectionEdit::resizeEvent(QResizeEvent* event) {
  QScrollArea::resizeEvent(event);
  updateHeight();
}

void MultiSelectionEdit::updateHeight() {
  auto const actualMaxRows = _maxRows <= 0 ? 3 : _maxRows;
  auto const actualRows = std::min(actualMaxRows, _layout->numRows());

  auto const spacing = 4;

  auto height = spacing;

  for (auto i = 0; i < actualRows; ++i) {
    height += _layout->rowHeight(i);
    height += spacing;
  }
  
  height += 2;

  setFixedHeight(height);
}
