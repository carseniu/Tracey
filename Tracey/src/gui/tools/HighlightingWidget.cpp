#include "src/gui/tools/HighlightingWidget.h"

#include "src/Tracey.h"
#include "src/gui/core/Application.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/core/HighlightingData.h"
#include "src/gui/core/HighlightingGroup.h"
#include "src/gui/core/HighlightingRule.h"
#include "src/gui/dialogs/InputDialog.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/widgets/colorbutton/ColorButton.h"
#include "src/gui/widgets/combobox/ComboBox.h"
#include "src/gui/widgets/iconbutton/IconButton.h"
#include "src/gui/widgets/list/List.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QScrollBar>
#include <QVBoxLayout>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingWidget::HighlightingWidget(Application* application) :
  QFrame(application),
  highlightingData(application->getHighlightingData())
{
  setFixedHeight(280);  

  // HIGHLIGHTING GROUP
  addGroupButton = new IconButton(QIcon(":/Application/img/Add.png"), this);
  addGroupButton->setFocusPolicy(Qt::NoFocus);
  addGroupButton->setToolTip(tr("Add Group"));

  deleteGroupButton = new IconButton(QIcon(":/Application/img/Delete.png"), this);
  deleteGroupButton->setFocusPolicy(Qt::NoFocus);
  deleteGroupButton->setToolTip(tr("Delete Group"));
  deleteGroupButton->setEnabled(false);

  QHBoxLayout* groupToolBarLayout = new QHBoxLayout;
  groupToolBarLayout->setMargin(0);
  groupToolBarLayout->setSpacing(1);
  groupToolBarLayout->addWidget(addGroupButton);
  groupToolBarLayout->addWidget(deleteGroupButton);
  groupToolBarLayout->addStretch();

  groupComboBox = new ComboBox(this);
  groupComboBox->setEnabled(false);


  // HIGHLIGHTING ITEM
  addRuleButton = new IconButton(QIcon(":/Application/img/Add.png"), this);
  addRuleButton->setFocusPolicy(Qt::NoFocus);
  addRuleButton->setToolTip(tr("Add Rule"));
  addRuleButton->setEnabled(false);

  deleteRuleButton = new IconButton(QIcon(":/Application/img/Delete.png"), this);
  deleteRuleButton->setFocusPolicy(Qt::NoFocus);
  deleteRuleButton->setToolTip(tr("Delete Rule"));

  enabledCheckBox = new QCheckBox(this);
  enabledCheckBox->setToolTip(tr("Enabled"));

  foregroundColorButton = new ColorButton(this);
  foregroundColorButton->setToolTip(tr("Foreground Color"));
  foregroundColorButton->setFixedSize(16, 16);

  backgroundColorButton = new ColorButton(this);
  backgroundColorButton->setToolTip(tr("Background Color"));
  backgroundColorButton->setFixedSize(16, 16);

  QIcon matchCaseIcon;
  matchCaseIcon.addFile(":/Application/img/MatchCaseOff.png", QSize(), QIcon::Normal, QIcon::Off);
  matchCaseIcon.addFile(":/Application/img/MatchCaseOn.png", QSize(), QIcon::Normal, QIcon::On);
  matchCaseButton = new IconButton(matchCaseIcon, this);
  matchCaseButton->setFocusPolicy(Qt::NoFocus);
  matchCaseButton->setToolTip(tr("Match Case"));
  matchCaseButton->setCheckable(true);

  QIcon wholeWordIcon;
  wholeWordIcon.addFile(":/Application/img/WholeWordOff.png", QSize(), QIcon::Normal, QIcon::Off);
  wholeWordIcon.addFile(":/Application/img/WholeWordOn.png", QSize(), QIcon::Normal, QIcon::On);
  wholeWordButton = new IconButton(wholeWordIcon, this);
  wholeWordButton->setFocusPolicy(Qt::NoFocus);
  wholeWordButton->setToolTip(tr("Whole Word"));
  wholeWordButton->setCheckable(true);

  findPreviousButton = new IconButton(QIcon(":/Application/img/FindPrevious.png"), this);
  findPreviousButton->setFocusPolicy(Qt::NoFocus);
  findPreviousButton->setToolTip(tr("Find Previous"));

  findNextButton = new IconButton(QIcon(":/Application/img/FindNext.png"), this);
  findNextButton->setFocusPolicy(Qt::NoFocus);
  findNextButton->setToolTip(tr("Find Next"));

  QHBoxLayout* itemToolBarLayout = new QHBoxLayout;
  itemToolBarLayout->setMargin(0);
  itemToolBarLayout->setSpacing(1);
  itemToolBarLayout->addWidget(addRuleButton);
  itemToolBarLayout->addWidget(deleteRuleButton);
  itemToolBarLayout->addStretch();
  itemToolBarLayout->addWidget(enabledCheckBox);
  itemToolBarLayout->addSpacing(10);
  itemToolBarLayout->addWidget(foregroundColorButton);
  itemToolBarLayout->addWidget(backgroundColorButton);
  itemToolBarLayout->addStretch();
  itemToolBarLayout->addWidget(matchCaseButton);
  itemToolBarLayout->addWidget(wholeWordButton);
  itemToolBarLayout->addSpacing(10);
  itemToolBarLayout->addWidget(findNextButton);
  itemToolBarLayout->addWidget(findPreviousButton);

  ruleList = new List(this);
  ruleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ruleList->setVerticalScrollBar(new QScrollBar(Qt::Vertical, ruleList));
  ruleList->setDragEnabled(true);
  ruleList->setDragDropMode(QAbstractItemView::InternalMove);
  ruleList->setAcceptDrops(true);
  ruleList->setFont(QFont("Courier New", 9));
  ruleList->setEnabled(false);


  // MAIN LAYOUT
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(2);
  layout->setSpacing(2);
  layout->addLayout(groupToolBarLayout);
  layout->addWidget(groupComboBox);
  layout->addSpacing(3);
  layout->addLayout(itemToolBarLayout);
  layout->addWidget(ruleList);

  disableRuleToolbar();

  connect(groupComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HighlightingWidget::processGroupChanged, Qt::QueuedConnection);
  connect(ruleList, &List::currentRowChanged, this, &HighlightingWidget::processRuleChanged, Qt::QueuedConnection);
  connect(ruleList, &List::itemRowChanged, this, &HighlightingWidget::onItemRowChanged, Qt::QueuedConnection);
  connect(addGroupButton, &IconButton::clicked, this, &HighlightingWidget::processAddGroup);
  connect(deleteGroupButton, &IconButton::clicked, this, &HighlightingWidget::processDeleteGroup);
  connect(addRuleButton, &IconButton::clicked, this, &HighlightingWidget::processAddRule);
  connect(deleteRuleButton, &IconButton::clicked, this, &HighlightingWidget::processDeleteRule);
  connect(enabledCheckBox, &IconButton::toggled, this, &HighlightingWidget::setEnable);
  connect(foregroundColorButton, &IconButton::clicked, this, &HighlightingWidget::setForegroundColor);
  connect(backgroundColorButton, &IconButton::clicked, this, &HighlightingWidget::setBackgroundColor);
  connect(matchCaseButton, &IconButton::clicked, this, &HighlightingWidget::setMatchCase);
  connect(wholeWordButton, &IconButton::clicked, this, &HighlightingWidget::setWholeWord);
  connect(findNextButton, &IconButton::clicked, this, &HighlightingWidget::findNext);
  connect(findPreviousButton, &IconButton::clicked, this, &HighlightingWidget::findPrevious);

  setGroups();
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::disableRuleToolbar()
{
  deleteRuleButton->setEnabled(false);
  enabledCheckBox->setEnabled(false);
  foregroundColorButton->setEnabled(false);
  backgroundColorButton->setEnabled(false);
  matchCaseButton->setEnabled(false);
  wholeWordButton->setEnabled(false);
  findPreviousButton->setEnabled(false);
  findNextButton->setEnabled(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::enableRuleToolbar()
{
  deleteRuleButton->setEnabled(true);
  enabledCheckBox->setEnabled(true);
  foregroundColorButton->setEnabled(true);
  backgroundColorButton->setEnabled(true);
  matchCaseButton->setEnabled(true);
  wholeWordButton->setEnabled(true);
  findPreviousButton->setEnabled(true);
  findNextButton->setEnabled(true);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QColor HighlightingWidget::getColor(const QString& currentColor, const QString& title)
{
  QColorDialog* colorDialog = new QColorDialog(QColor(currentColor), this);
  colorDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  colorDialog->setWindowTitle(title);
  resizeDialogButton(colorDialog, QDialogButtonBox::Ok, 80, 20);
  resizeDialogButton(colorDialog, QDialogButtonBox::Cancel, 80, 20);

  colorDialog->exec();

  return colorDialog->selectedColor();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingRule* HighlightingWidget::getCurrentRule() const
{
  return highlightingData->getRule(ruleList->currentItem()->data(Qt::UserRole).toString(), groupComboBox->currentData().toString());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::updateRule(HighlightingRule* rule)
{
  QString groupId = groupComboBox->currentData().toString();

  highlightingData->updateRule(rule, groupId);
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::findNext()
{
  HighlightingRule* rule = getCurrentRule();

  emit findNextRequested(createRegexp(rule->getItem(), NormalSearch, rule->isWholeWord(), rule->isMatchCase()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::findPrevious()
{
  HighlightingRule* rule = getCurrentRule();

  emit findPreviousRequested(createRegexp(rule->getItem(), NormalSearch, rule->isWholeWord(), rule->isMatchCase()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::onItemRowChanged(int oldPosition, int newPosition)
{
  highlightingData->moveRule(oldPosition, newPosition, groupComboBox->currentData().toString());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::processAddGroup()
{
  InputDialog dialog(this, tr("Add Group"), tr("Name"));
  bool ok = dialog.exec();
  QString name = dialog.getTextValue();

  if (ok && !name.isEmpty())
  {
    QString groupId = highlightingData->addGroup(name);

    groupComboBox->addItem(name, groupId);
    groupComboBox->setCurrentIndex(groupComboBox->count() - 1);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::processAddRule()
{
  InputDialog dialog(this, tr("Add Rule"), tr("Name"));
  bool ok = dialog.exec();
  QString text = dialog.getTextValue();

  if (ok && !text.isEmpty())
  {
    QString groupId = groupComboBox->currentData().toString();
    QString ruleId = highlightingData->addRule(text, groupId);

    QListWidgetItem* item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, ruleId);
    ruleList->addItem(item);

    ruleList->setEnabled(true);
    ruleList->setCurrentRow(ruleList->count() - 1);
    ruleList->setFocus();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::processDeleteGroup()
{
  bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_GROUP);
  bool ok = true;

  if (showDialog)
  {
    MessageDialog dialog(this, tr("Do you want to delete the highlighting group <b><font color=#ff0000>") + groupComboBox->currentText() + "</font></b>?", MessageDialog::Warning, true);
    ok = dialog.exec();

    Config().setProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_GROUP, QString::number(!dialog.getDontAskAgain()));
  }

  if (ok)
  {
    QString groupId = groupComboBox->currentData().toString();
    highlightingData->deleteGroup(groupId);

    groupComboBox->removeItem(groupComboBox->currentIndex());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::processDeleteRule()
{
  bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_RULE);
  bool ok = true;

  if (showDialog)
  {
    MessageDialog dialog(this, tr("Do you want to delete the highlighting rule  <b><font color=#ff0000>") + ruleList->currentItem()->text() + "</font></b>?", MessageDialog::Warning, true);
    ok = dialog.exec();

    Config().setProperty(SETTINGS_DIALOG_DELETE_HIGHLIGHTING_RULE, QString::number(!dialog.getDontAskAgain()));
  }

  if (ok)
  {
    QString groupId = groupComboBox->currentData().toString();
    highlightingData->deleteRule(ruleList->currentItem()->data(Qt::UserRole).toString(), groupId);

    delete ruleList->currentItem();

    if (ruleList->count() == 0)
    {
      ruleList->setEnabled(false);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::processGroupChanged(int index)
{
  ruleList->clear();

  if (index != -1)
  {
    deleteGroupButton->setEnabled(true);
    addRuleButton->setEnabled(true);
    groupComboBox->setEnabled(true);

    QString groupId = groupComboBox->currentData().toString();

    QVector<HighlightingRule*> rules = highlightingData->getRules(groupId);

    if (rules.size() > 0)
    {
      for (HighlightingRule* rule : rules)
      {
        QListWidgetItem* item = new QListWidgetItem(rule->getItem());
        item->setData(Qt::UserRole, rule->getId());
        ruleList->addItem(item);
      }

      ruleList->setEnabled(true);
      ruleList->setCurrentRow(0);
    }
    else
    {
      ruleList->setEnabled(false);
    }
  }
  else
  {
    deleteGroupButton->setEnabled(false);
    addRuleButton->setEnabled(false);
    groupComboBox->setEnabled(false);
    ruleList->setEnabled(false);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::processRuleChanged(int index)
{
  if (index != -1)
  {
    enableRuleToolbar();

    HighlightingRule* rule = getCurrentRule();

    enabledCheckBox->setChecked(rule->isEnabled());

    foregroundColorButton->setColor(rule->getForegroundColor());
    backgroundColorButton->setColor(rule->getBackgroundColor());

    matchCaseButton->setChecked(rule->isMatchCase());
    wholeWordButton->setChecked(rule->isWholeWord());
  }
  else
  {
    disableRuleToolbar();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::setBackgroundColor()
{
  HighlightingRule* rule = getCurrentRule();

  QColor color = getColor(rule->getBackgroundColor(), tr("Background Color"));

  if (color.isValid())
  {
    backgroundColorButton->setColor(color.name());

    rule->setBackgroundColor(color.name());

    updateRule(rule);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::setEnable(bool enabled)
{
  HighlightingRule* rule = getCurrentRule();

  rule->setEnabled(enabled);

  updateRule(rule);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::setForegroundColor()
{
  HighlightingRule* rule = getCurrentRule();

  QColor color = getColor(rule->getForegroundColor(), tr("Foreground Color"));

  if (color.isValid())
  {
    foregroundColorButton->setColor(color.name());

    rule->setForegroundColor(color.name());

    updateRule(rule);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::setGroups()
{
  QVector<HighlightingGroup*> groups = highlightingData->getGroups();

  for (HighlightingGroup* group : groups)
  {
    groupComboBox->addItem(group->getName(), group->getId());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::setMatchCase(bool enabled)
{
  HighlightingRule* rule = getCurrentRule();

  rule->setMatchCase(enabled);

  updateRule(rule);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingWidget::setWholeWord(bool enabled)
{
  HighlightingRule* rule = getCurrentRule();

  rule->setWholeWord(enabled);

  updateRule(rule);
}