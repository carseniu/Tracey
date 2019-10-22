#include "src/gui/main/TracePanel.h"

#include "src/gui/core/HighlightingData.h"
#include "src/gui/core/HighlightingGroup.h"
#include "src/gui/main/TraceWidget.h"
#include "src/gui/widgets/combobox/ComboBox.h"

#include <QLabel>
#include <QHBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TracePanel::TracePanel(HighlightingData* highlightingData, TraceWidget* parent) :
  QFrame(parent),
  highlightingData(highlightingData)
{
  QLabel* highlightingLabel = new QLabel(this);
  highlightingLabel->setPixmap(QPixmap(":/Application/img/Highlighting.png"));
  highlightingLabel->setToolTip(tr("Highlighting"));

  highlightingComboBox = new ComboBox(this);
  highlightingComboBox->setMinimumWidth(150);

  highlightingWidget = new QWidget(this);
  QHBoxLayout* highlightingLayout = new QHBoxLayout(highlightingWidget);
  highlightingLayout->setMargin(0);
  highlightingLayout->setSpacing(5);
  highlightingLayout->addWidget(highlightingLabel);
  highlightingLayout->addWidget(highlightingComboBox);

  setHighlightingGroups(highlightingData->getGroups());

  connect(highlightingData, &HighlightingData::groupsChanged, this, &TracePanel::setHighlightingGroups);
  connect(highlightingData, &HighlightingData::highlightingChanged, this, &TracePanel::processHighlightingChanged);
  connect(highlightingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TracePanel::processHighlightingGroupChanged, Qt::QueuedConnection);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QWidget* TracePanel::getHighlightingWidget() const
{
  return highlightingWidget;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString TracePanel::getHighlightingGroupId() const
{
  return highlightingComboBox->currentData().toString();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TracePanel::setHighlightingGroupId(const QString& highlightingGroupId)
{
  int groupIndex = highlightingComboBox->findData(highlightingGroupId);

  setHighlightingGroupIndex(groupIndex);
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TracePanel::setHighlightingGroupIndex(int index)
{
  if (index == -1)
  {
    highlightingComboBox->setCurrentIndex(0);
  }
  else
  {
    highlightingComboBox->setCurrentIndex(index);
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TracePanel::processHighlightingChanged(const QString& groupId)
{
  QString currentGroupId = highlightingComboBox->currentData().toString();

  if (currentGroupId == groupId)
  {
    emit highlightingGroupChanged(groupId);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TracePanel::processHighlightingGroupChanged(int index)
{
  emit highlightingGroupChanged(highlightingComboBox->currentData().toString());
  emit saveRequested();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TracePanel::setHighlightingGroups(const QVector<HighlightingGroup*>& groups)
{
  QString oldGroupId = highlightingComboBox->currentData().toString();

  highlightingComboBox->clear();
  highlightingComboBox->addItem("");

  for (HighlightingGroup* group : groups)
  {
    highlightingComboBox->addItem(group->getName(), group->getId());
  }

  int oldGroupIndex = highlightingComboBox->findData(oldGroupId);

  setHighlightingGroupIndex(oldGroupIndex);
}