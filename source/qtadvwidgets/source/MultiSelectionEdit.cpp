#include <qtadvwidgets/MultiSelectionEdit.h>
#include <qtadvwidgets/RemovableSelection.h>

#include <QBoxLayout>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFrame>

MultiSelectionEdit::MultiSelectionEdit(QWidget* parent) : QLineEdit{parent} {
  //  setReadOnly(true);

  auto scrollAreaLayout = new QBoxLayout{QBoxLayout::TopToBottom};
  scrollAreaLayout->setMargin(0);

  setLayout(scrollAreaLayout);
  setFixedHeight(32);

  auto scrollArea = new QScrollArea{this};
  scrollAreaLayout->addWidget(scrollArea);

  auto mainWidget = new QWidget{};

  _selections = {
      new RemovableSelection{"S+U Potsdamer Platz", this},
      new RemovableSelection{"Stendaler Str.", this},
      new RemovableSelection{"Walther-Schreiber-Platz", this},
      new RemovableSelection{"S+U Bundesallee", this},
  };

  _lineEdit = new QLineEdit{this};
  _lineEdit->setFrame(false);
  _lineEdit->setMinimumSize(QSize{100, 0});
  auto layout = new QBoxLayout{QBoxLayout::LeftToRight};

  layout->setMargin(4);

  for (auto selection : _selections) {
    layout->addWidget(selection);

    connect(selection, &RemovableSelection::removeClicked,
            [=]() { delete selection; });
  }

  layout->addWidget(_lineEdit);
  layout->setSizeConstraint(QLayout::SetMaximumSize);

  mainWidget->setLayout(layout);

  scrollArea->setWidget(mainWidget);

  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setWidgetResizable(true);

  scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}
