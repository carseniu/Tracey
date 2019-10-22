#include "src/gui/dialogs/SettingsDialog.h"

#include "src/Tracey.h"
#include "src/gui/core/ActionManager.h"
#include "src/gui/core/Application.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/dialogs/ShortcutDialog.h"
#include "src/gui/widgets/table/Table.h"
#include "src/gui/widgets/tabwidget/TabWidget.h"

#include <QAction>
#include <QCheckBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SettingsDialog::SettingsDialog(Application* application) :
  QDialog(application),
  application(application)
{
  setFixedSize(430, 430);
  setWindowIcon(QIcon(":/Application/img/Icon.png"));
  setWindowTitle(tr("Settings"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  minimizeToTrayCheckBox = new QCheckBox(tr("Minimize to tray"), this);
  closeToTrayCheckBox = new QCheckBox(tr("Close to tray"), this);

  QLabel* deleteLabel = new QLabel("Dialog confirmation", this);

  deleteTraceCheckBox = new QCheckBox(tr("Delete trace"), this);
  deleteTraceCheckBox->setStyleSheet("margin-left:30px");
  deleteFilterCheckBox = new QCheckBox(tr("Delete filter"), this);
  deleteFilterCheckBox->setStyleSheet("margin-left:30px");
  deleteSourceHistoryCheckBox = new QCheckBox(tr("Delete source history"), this);
  deleteSourceHistoryCheckBox->setStyleSheet("margin-left:30px");
  deleteSearchHistoryCheckBox = new QCheckBox(tr("Delete search history"), this);
  deleteSearchHistoryCheckBox->setStyleSheet("margin-left:30px");
  deleteHighlightingGroupCheckBox = new QCheckBox(tr("Delete highlighting group"), this);
  deleteHighlightingGroupCheckBox->setStyleSheet("margin-left:30px");
  deleteHighlightingRuleCheckBox = new QCheckBox(tr("Delete highlighting rule"), this);
  deleteHighlightingRuleCheckBox->setStyleSheet("margin-left:30px");
  exitApplicationCheckBox = new QCheckBox(tr("Exit application"), this);
  exitApplicationCheckBox->setStyleSheet("margin-left:30px");

  QWidget* generalWidget = new QWidget(this);
  QVBoxLayout* generalLayout = new QVBoxLayout(generalWidget);
  generalLayout->setContentsMargins(70, 30, 70, 30);
  generalLayout->setSpacing(10);
  generalLayout->addWidget(minimizeToTrayCheckBox);
  generalLayout->addWidget(closeToTrayCheckBox);
  generalLayout->addSpacing(20);
  generalLayout->addWidget(deleteLabel);
  generalLayout->addSpacing(5);
  generalLayout->addWidget(deleteTraceCheckBox);
  generalLayout->addWidget(deleteFilterCheckBox);
  generalLayout->addWidget(deleteSourceHistoryCheckBox);
  generalLayout->addWidget(deleteSearchHistoryCheckBox);
  generalLayout->addWidget(deleteHighlightingGroupCheckBox);
  generalLayout->addWidget(deleteHighlightingRuleCheckBox);
  generalLayout->addWidget(exitApplicationCheckBox);
  generalLayout->addStretch();


  actionList = application->getActionManager()->getActions();

  actionTable = new Table(this);
  actionTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  actionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  actionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  actionTable->setSelectionMode(QAbstractItemView::SingleSelection);
  actionTable->setColumnCount(2);
  actionTable->setRowCount(actionList.size());
  actionTable->setHorizontalHeaderLabels(QStringList() << tr("Action") << tr("Shortcut"));
  actionTable->horizontalHeader()->setDefaultSectionSize(200);
  actionTable->horizontalHeader()->setSectionsMovable(false);
  actionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  actionTable->verticalHeader()->setDefaultSectionSize(20);
  actionTable->verticalHeader()->setVisible(false);
  connect(actionTable, &Table::cellDoubleClicked, this, &SettingsDialog::setShortcut);


  QWidget* shortcutWidget = new QWidget(this);
  QVBoxLayout* shortcutLayout = new QVBoxLayout(shortcutWidget);
  shortcutLayout->setContentsMargins(3, 3, 3, 2);
  shortcutLayout->setSpacing(0);
  shortcutLayout->addWidget(actionTable);

  contentTabWidget = new TabWidget(this);
  contentTabWidget->addTab(generalWidget, tr("General"), true);
  contentTabWidget->addTab(shortcutWidget, tr("Shortcuts"), true);
  contentTabWidget->setCurrentIndex(0);


  okButton = new QPushButton(tr("OK"), this);
  okButton->setFixedSize(80, 20);
  okButton->setFocus(Qt::TabFocusReason);
  connect(okButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);

  cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setFixedSize(80, 20);
  connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);

  QWidget* buttonWidget = new QWidget(this);
  buttonWidget->setFixedHeight(30);

  QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
  buttonLayout->setMargin(0);
  buttonLayout->setSpacing(10);
  buttonLayout->addStretch();
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);


  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(5);
  mainLayout->setSpacing(5);
  mainLayout->addWidget(contentTabWidget);
  mainLayout->addWidget(buttonWidget);
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SettingsDialog::showEvent(QShowEvent* event)
{
  minimizeToTrayCheckBox->setChecked(Config().getBoolProperty(SETTINGS_MINIMIZE_TO_TRAY));
  closeToTrayCheckBox->setChecked(Config().getBoolProperty(SETTINGS_CLOSE_TO_TRAY));
  deleteTraceCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_DELETE_TRACE));
  deleteFilterCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_DELETE_FILTER));
  deleteSourceHistoryCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_DELETE_SOURCE_HISTORY));
  deleteSearchHistoryCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_DELETE_SEARCH_HISTORY));
  deleteHighlightingGroupCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_GROUP));
  deleteHighlightingRuleCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_RULE));
  exitApplicationCheckBox->setChecked(Config().getBoolProperty(SETTINGS_DIALOG_EXIT_APPLICATION));

  for (int i = 0; i < actionList.size(); ++i)
  {
    actionTable->setItem(i, 0, new QTableWidgetItem(actionList.at(i)->icon(), actionList.at(i)->text()));
    actionTable->setItem(i, 1, new QTableWidgetItem(actionList.at(i)->shortcut().toString(QKeySequence::NativeText)));
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SettingsDialog::saveSettings()
{
  Config().setProperty(SETTINGS_MINIMIZE_TO_TRAY, QString::number(minimizeToTrayCheckBox->isChecked()));
  Config().setProperty(SETTINGS_CLOSE_TO_TRAY, QString::number(closeToTrayCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_DELETE_TRACE, QString::number(deleteTraceCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_DELETE_FILTER, QString::number(deleteFilterCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_DELETE_SOURCE_HISTORY, QString::number(deleteSourceHistoryCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_DELETE_SEARCH_HISTORY, QString::number(deleteSearchHistoryCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_GROUP, QString::number(deleteHighlightingGroupCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_RULE, QString::number(deleteHighlightingRuleCheckBox->isChecked()));
  Config().setProperty(SETTINGS_DIALOG_EXIT_APPLICATION, QString::number(exitApplicationCheckBox->isChecked()));

  QStringList shortcutList;
  for (int i = 0; i < actionTable->rowCount(); ++i)
  {
    shortcutList.append(actionTable->item(i, 1)->text());
  }

  Config().saveShortcuts(shortcutList);
  application->getActionManager()->setShortcuts(shortcutList);

  accept();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SettingsDialog::setShortcut(int row, int column)
{
  QKeySequence shortcut = ShortcutDialog::getShortcut(application->getActionManager()->getActions(), row, this);

  for (int i = 0; i < actionTable->rowCount(); ++i)
  {
    if (actionTable->item(i, 1)->text() == shortcut.toString(QKeySequence::NativeText) && i != row)
    {
      actionTable->item(i, 1)->setText("");
      break;
    }
  }

  actionTable->item(row, 1)->setText(shortcut.toString(QKeySequence::NativeText));
}