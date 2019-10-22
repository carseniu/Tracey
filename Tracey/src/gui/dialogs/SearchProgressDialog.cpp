#include "src/gui/dialogs/SearchProgressDialog.h"

#include <QDebug>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchProgressDialog::SearchProgressDialog(QWidget* parent) :
  QDialog(parent)
{
  setFixedSize(300, 120);
  setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
  setAttribute(Qt::WA_DeleteOnClose);

  titleLabel = new QLabel("Searching...", this);

  messageLabel = new QLabel(this);
  messageLabel->setVisible(false);

  progressBar = new QProgressBar(this);
  progressBar->setObjectName("SearchProgressDialogProgressBar");
  progressBar->setTextVisible(false);
  progressBar->setFixedSize(250, 20);
  progressBar->setMaximum(100);
  progressBar->setMinimum(0);
  progressBar->setValue(0);

  cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setFixedSize(80, 20);
  cancelButton->setFocus(Qt::TabFocusReason);
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  okButton = new QPushButton(tr("OK"), this);
  okButton->setFixedSize(80, 20);
  okButton->setFocus(Qt::TabFocusReason);
  okButton->setVisible(false);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(15);
  mainLayout->setSpacing(5);
  mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
  mainLayout->addWidget(progressBar, 0, Qt::AlignCenter);
  mainLayout->addWidget(messageLabel, 0, Qt::AlignCenter);
  mainLayout->addStretch();
  mainLayout->addWidget(cancelButton, 0, Qt::AlignCenter);
  mainLayout->addWidget(okButton, 0, Qt::AlignCenter);
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchProgressDialog::processMatchNotFound()
{
  okButton->setVisible(true);
  cancelButton->setVisible(false);
  messageLabel->setVisible(true);
  messageLabel->setText(tr("No match found."));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchProgressDialog::setCount(int count)
{
  if (count > 0)
  {
    messageLabel->setVisible(true);
    messageLabel->setText(tr("Count: ") + QString::number(count));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchProgressDialog::setStep(int step)
{
  progressBar->setValue(step);

  if (step == 100)
  {
    okButton->setVisible(true);
    cancelButton->setVisible(false);
  }
}
