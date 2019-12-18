#include <qtadvwidgets/MultiSelectionEdit.h>
#include <qtadvwidgets/RemovableSelection.h>

#include <QBoxLayout>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionFrame>

MultiSelectionEdit::MultiSelectionEdit(QWidget* parent) : QScrollArea{parent} {

  auto mainWidget = new QWidget{};

  auto mainLayout = new QBoxLayout{QBoxLayout::TopToBottom};
  mainLayout->setMargin(2);
  mainLayout->setSpacing(2);

  _lineEdit = new QLineEdit{this};
  _lineEdit->setFrame(false);
  _lineEdit->setMinimumSize(QSize{100, 0});
  _lineEdit->setPlaceholderText("Weiteres Element ...");

  auto firstRowLayout = new QBoxLayout{QBoxLayout::LeftToRight};
  firstRowLayout->setMargin(0);
  firstRowLayout->setSpacing(2);
  firstRowLayout->setSizeConstraint(QLayout::SetMaximumSize);

  fillRow({"S+U Potsdamer Platz", "Stendaler Str.", "Walther-Schreiber-Platz"},
           firstRowLayout);
  firstRowLayout->addStretch();

  auto secondRowLayout = new QBoxLayout{QBoxLayout::LeftToRight};
  secondRowLayout->setMargin(0);
  secondRowLayout->setSpacing(2);
  secondRowLayout->setSizeConstraint(QLayout::SetMaximumSize);

  fillRow({"S+U Berlin Hbf", "U Spichernstr.", "U Berliner Str."},
          secondRowLayout);
  secondRowLayout->addStretch();

  auto fourthRowLayout = new QBoxLayout{QBoxLayout::LeftToRight};
  fourthRowLayout->setMargin(0);
  fourthRowLayout->setSpacing(2);
  fourthRowLayout->setSizeConstraint(QLayout::SetMaximumSize);

  fillRow({"S+U Potsdamer Platz", "Stendaler Str.", "Walther-Schreiber-Platz"},
          fourthRowLayout);
  fourthRowLayout->addStretch();

  auto thirdRowLayout = new QBoxLayout{QBoxLayout::LeftToRight};
  thirdRowLayout->setMargin(0);
  thirdRowLayout->setSpacing(2);
  thirdRowLayout->setSizeConstraint(QLayout::SetMaximumSize);

  fillRow({"S+U Bundesallee"}, thirdRowLayout);
  thirdRowLayout->addWidget(_lineEdit);

  mainLayout->addLayout(firstRowLayout);
  mainLayout->addLayout(secondRowLayout);
  mainLayout->addLayout(fourthRowLayout);
  mainLayout->addLayout(thirdRowLayout);
  mainLayout->addStretch();
  

  mainWidget->setLayout(mainLayout);

  mainWidget->setStyleSheet("#mainWidget { background-color: white; }");
  mainWidget->setObjectName("mainWidget");

  setWidget(mainWidget);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setWidgetResizable(true);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  setMaximumHeight(secondRowLayout->sizeHint().height() * 3 + 10);
}

void MultiSelectionEdit::fillRow(QVector<QString> const& entries, QBoxLayout* layout) 
{
  for (auto const& entry : entries) {
    auto selection = new RemovableSelection{entry, this};
    layout->addWidget(selection);

    connect(selection, &RemovableSelection::removeClicked,
            [=]() { delete selection; });
  }
}
