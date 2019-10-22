#include "src/gui/dialogs/InputDialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
InputDialog::InputDialog(QWidget* parent, const QString& title, const QString& label, const QString& value) :
  QDialog(parent)
{
  setWindowTitle(title);
  setFixedSize(250, 100);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  QLabel* inputLabel = new QLabel(label, this);

  inputLineEdit = new QLineEdit(this);
  inputLineEdit->setText(value);

  QPushButton* okButton = new QPushButton(tr("OK"), this);
  okButton->setFixedSize(70, 20);
  okButton->setFocus(Qt::TabFocusReason);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QPushButton*cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setFixedSize(70, 20);
  cancelButton->setFocus(Qt::TabFocusReason);
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);


  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(inputLabel);
  layout->addWidget(inputLineEdit);
  layout->addStretch();
  layout->addLayout(buttonLayout);

  inputLineEdit->setFocus();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString InputDialog::getTextValue() const
{
  return inputLineEdit->text();
}