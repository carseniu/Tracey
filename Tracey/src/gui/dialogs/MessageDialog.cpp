#include "src/gui/dialogs/MessageDialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
MessageDialog::MessageDialog(QWidget* parent, const QString& message, Type type, bool showDontAskAgain) :
  QDialog(parent)
{
  setMinimumSize(350, 100);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  QLabel* iconLabel = new QLabel(this);
  iconLabel->setFixedSize(40, 40);

  switch (type)
  {
  case Error:
    iconLabel->setPixmap(QPixmap(":/Application/img/DialogError.png"));
    break;
  case Info:
    iconLabel->setPixmap(QPixmap(":/Application/img/DialogInfo.png"));
    break;
  case Question:
    iconLabel->setPixmap(QPixmap(":/Application/img/DialogQuestion.png"));
    break;
  case Warning:
    iconLabel->setPixmap(QPixmap(":/Application/img/DialogWarning.png"));
    break;
  }

  QLabel* messageLabel = new QLabel(message, this);
  messageLabel->setFixedHeight(50);
  messageLabel->setWordWrap(true);

  QHBoxLayout* messageLayout = new QHBoxLayout;
  messageLayout->setSpacing(20);
  messageLayout->addSpacing(10);
  messageLayout->addWidget(iconLabel);
  messageLayout->addWidget(messageLabel);

  if (showDontAskAgain)
  {
    dontAskAgainCheckBox = new QCheckBox(tr("Don't ask again"), this);
    dontAskAgainCheckBox->setStyleSheet("margin-left:70px");
  }

  QHBoxLayout* buttonLayout = new QHBoxLayout;

  if (type == Question || type == Warning)
  {
    QPushButton* yesButton = new QPushButton(tr("Yes"), this);
    yesButton->setFixedSize(70, 20);
    yesButton->setFocus(Qt::TabFocusReason);
    connect(yesButton, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton* noButton = new QPushButton(tr("No"), this);
    noButton->setFixedSize(70, 20);
    noButton->setFocus(Qt::TabFocusReason);
    connect(noButton, SIGNAL(clicked()), this, SLOT(reject()));

    buttonLayout->addStretch();
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
  }
  else if (type == Error || type == Info)
  {
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setFixedSize(70, 20);
    okButton->setFocus(Qt::TabFocusReason);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    buttonLayout->addWidget(okButton);
  }

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addLayout(messageLayout);
  if (showDontAskAgain)
  {
    layout->addWidget(dontAskAgainCheckBox);
    layout->addSpacing(10);
  }
  layout->addStretch();
  layout->addLayout(buttonLayout);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool MessageDialog::getDontAskAgain() const
{
  return dontAskAgainCheckBox->isChecked();
}