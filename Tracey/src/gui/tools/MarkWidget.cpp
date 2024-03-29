#include "src/gui/tools/MarkWidget.h"

#include "src/gui/tools/MarkItem.h"
#include "src/gui/widgets/list/List.h"

#include <QDebug>
#include <QLabel>
#include <QScrollBar>
#include <QVBoxLayout>

//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
MarkWidget::MarkWidget(QWidget* parent) :
  QFrame(parent)
{
  setFixedHeight(270);
  setFont(QFont("Courier New", 8));

  markList = new List(this);
  markList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  markList->setVerticalScrollBar(new QScrollBar(Qt::Vertical, markList));

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(2);
  layout->addWidget(markList);

  connect(markList, &QListWidget::currentRowChanged, this, &MarkWidget::onMarkChanged);
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkWidget::setMarks(const QVector<Mark>& marks, int newIndex)
{
  markList->clear();

  QListWidgetItem* currentItem = nullptr;

  if (marks.isEmpty())
  {
    return;
  }

  int indexSize = fontMetrics().horizontalAdvance(QString::number(marks.last().index)) + 10;

  for (const Mark& mark : marks)
  {
    QListWidgetItem* item = new QListWidgetItem(markList);
    MarkItem* markItem = new MarkItem(mark.index, mark.text, indexSize, this);
     
    item->setData(Qt::UserRole, mark.index);
    item->setToolTip(mark.text);
    markList->addItem(item);
    markList->setItemWidget(item, markItem);

    if (mark.index == newIndex)
    {
      currentItem = item;
    }
  }

  if (currentItem != nullptr)
  {
    markList->setCurrentItem(currentItem);
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkWidget::onMarkChanged(int index)
{
  if (index != -1)
  {
    emit markRequested(markList->currentItem()->data(Qt::UserRole).toInt());

    markList->setFocus();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkWidget::goToNextMark()
{
  if (markList->count() > 0)
  {
    int currentRow = markList->currentRow();

    if (currentRow == -1 || currentRow == markList->count() - 1)
    {
      markList->setCurrentRow(0);
    }
    else
    {
      markList->setCurrentRow(++currentRow);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkWidget::goToPreviousMark()
{
  if (markList->count() > 0)
  {
    int currentRow = markList->currentRow();

    if (currentRow == -1 || currentRow == 0)
    {
      markList->setCurrentRow(markList->count() - 1);
    }
    else
    {
      markList->setCurrentRow(--currentRow);
    }
  }
}