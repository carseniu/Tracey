#include "ShortcutDialog.h"

#include "src/gui/core/Configuration.h"

#include <QAction>
#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>

QKeySequence ShortcutDialog::s_shortcut;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QKeySequence ShortcutDialog::getShortcut(const QList<QAction*>& actionList, int actionIndex, QWidget* parent)
{
  ShortcutDialog* dialog = new ShortcutDialog(actionList, actionIndex, parent);
  dialog->exec();

  return s_shortcut;
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ShortcutDialog::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_unknown)
  {
    return;
  }

  if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift || event->key() == Qt::Key_Alt)
  {
    return;
  }

  int keySequence = event->key();

  QList<Qt::Key> modifiersList;

  if (event->modifiers() & Qt::ControlModifier)
  {
    keySequence += Qt::CTRL;
  }
  if (event->modifiers() & Qt::ShiftModifier)
  {
    keySequence += Qt::SHIFT;
  }
  if (event->modifiers() & Qt::AltModifier)
  {
    keySequence += Qt::ALT;
  }

  shortcut = QKeySequence(keySequence);

  actionTextLabel->setText(actionString.arg(QKeySequence(keySequence).toString(QKeySequence::NativeText)));

  checkDuplicate();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ShortcutDialog::ShortcutDialog(const QList<QAction*>& actionList, int actionIndex, QWidget* parent) :
  QDialog(parent),
  actionList(actionList),
  actionIndex(actionIndex)
{
  setFixedSize(300, 150);
  setWindowIcon(QIcon(":/Application/img/Icon.png"));
  setWindowTitle(tr("Shortcut"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  shortcut = actionList.at(actionIndex)->shortcut();
  s_shortcut = shortcut;

  QLabel* changeShortcutLabel = new QLabel(tr("Change shortcut for action:"), this);
  changeShortcutLabel->setAlignment(Qt::AlignCenter);


  QAction* action = actionList.at(actionIndex);
  actionString = QString("%1 (%2)").arg(action->text());

  actionIconLabel = new QLabel(this);
  actionIconLabel->setPixmap(action->icon().pixmap(16, 16));

  actionTextLabel = new QLabel(actionString.arg(action->shortcut().toString(QKeySequence::NativeText)), this);
  actionTextLabel->setAlignment(Qt::AlignCenter);
  actionTextLabel->setStyleSheet("font-weight:bold");

  QHBoxLayout* actionLayout = new QHBoxLayout;
  actionLayout->setMargin(0);
  actionLayout->setSpacing(3);
  actionLayout->addStretch();
  actionLayout->addWidget(actionIconLabel);
  actionLayout->addWidget(actionTextLabel);
  actionLayout->addStretch();


  actionExistsLabel = new QLabel(tr("Shortcut in use for action"), this);
  actionExistsLabel->setAlignment(Qt::AlignCenter);
  actionExistsLabel->setStyleSheet("color:red");
  actionExistsLabel->setVisible(false);

  existingActionIconLabel = new QLabel(this);
  existingActionIconLabel->setVisible(false);

  existingActionTextLabel = new QLabel(this);
  existingActionTextLabel->setAlignment(Qt::AlignCenter);
  existingActionTextLabel->setStyleSheet("color:red; font-weight:bold");
  existingActionTextLabel->setVisible(false);

  QHBoxLayout* existingActionLayout = new QHBoxLayout;
  existingActionLayout->setMargin(0);
  existingActionLayout->setSpacing(3);
  existingActionLayout->addStretch();
  existingActionLayout->addWidget(existingActionIconLabel);
  existingActionLayout->addWidget(existingActionTextLabel);
  existingActionLayout->addStretch();


  // Buttons
  QPushButton* okButton = new QPushButton(tr("OK"), this);
  okButton->setFocusPolicy(Qt::NoFocus);
  okButton->setFixedWidth(80);
  connect(okButton, &QPushButton::clicked, this, &ShortcutDialog::accept);

  QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setFocusPolicy(Qt::NoFocus);
  cancelButton->setFixedWidth(80);
  connect(cancelButton, &QPushButton::clicked, this, &ShortcutDialog::reject);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->setMargin(0);
  buttonLayout->setSpacing(20);
  buttonLayout->addStretch();
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
  buttonLayout->addStretch();

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setContentsMargins(15, 15, 15, 15);
  mainLayout->setSpacing(8);
  mainLayout->addWidget(changeShortcutLabel);
  mainLayout->addLayout(actionLayout);
  mainLayout->addWidget(actionExistsLabel);
  mainLayout->addLayout(existingActionLayout);
  mainLayout->addStretch();
  mainLayout->addLayout(buttonLayout);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ShortcutDialog::checkDuplicate()
{
  actionExistsLabel->setVisible(false);
  existingActionIconLabel->setVisible(false);
  existingActionTextLabel->setVisible(false);

  for (int i = 0; i < actionList.size(); ++i)
  {
    if (actionList.at(i)->shortcut().matches(shortcut) == QKeySequence::ExactMatch && i != actionIndex)
    {
      existingActionIconLabel->setPixmap(actionList.at(i)->icon().pixmap(16, 16));
      existingActionTextLabel->setText(actionList.at(i)->text());

      actionExistsLabel->setVisible(true);
      existingActionIconLabel->setVisible(true);
      existingActionTextLabel->setVisible(true);

      break;
    }
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ShortcutDialog::accept()
{
  s_shortcut = shortcut;

  QDialog::accept();
}