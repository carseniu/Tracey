#include "src/gui/widgets/tabwidget/TabBar.h"

#include "src/gui/widgets/tabwidget/TabWidget.h"
#include "src/gui/widgets/tabwidget/TabItem.h"

#include <QDebug>
#include <QPropertyAnimation>
#include <QSignalMapper>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TabBar::TabBar(TabWidget* parent) :
  QFrame(parent)
{
  setObjectName("TabBar");
  setFixedHeight(28);

  styleSheet = "TabBar {background-color:%1}";
  styleSheetEmpty = "TabBar {background-color:#f8f8f8; border:1px solid #727a7e; border-bottom:transparent}";

  bottomWidget = new QFrame(this);
  bottomWidget->setStyleSheet("background-color:#f8f8f8; border:1px solid #727a7e; border-bottom:transparent");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TabItem* TabBar::addTab(const QString& text, bool fixed)
{
  TabItem* tab = new TabItem(text, this);
  tab->setFixed(fixed);
  tab->setVisible(true);

  items.append(tab);

  setLook();

  connect(tab, &TabItem::contextMenuRequested, this, &TabBar::contextMenuRequested, Qt::DirectConnection);
  connect(tab, &TabItem::itemMoved, this, &TabBar::onItemMoved, Qt::DirectConnection);
  connect(tab, &TabItem::itemPressed, this, &TabBar::onItemPressed, Qt::DirectConnection);
  connect(tab, &TabItem::itemReleased, this, &TabBar::onItemReleased, Qt::DirectConnection);
  connect(tab, &TabItem::itemSelected, this, &TabBar::onItemSelected, Qt::DirectConnection);

  selectTab(tab);

  return tab;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TabBar::getCount() const
{
  return items.size();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TabItem* TabBar::getTab(int index) const
{
  if (index < 0 || index > items.size() - 1)
  {
    return nullptr;
  }

  return items.at(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int	TabBar::getTabAt(const QPoint& position) const
{
  for (int i = 0; i < items.size(); ++i)
  {
    TabItem* tab = items.at(i);

    if (tab->geometry().contains(mapFromGlobal(position)))
    {
      return i;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString TabBar::getTabText(int index) const
{
  return items.at(index)->getText();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::removeTab(int index)
{
  delete items.takeAt(index);

  setLook();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::selectTab(TabItem* tab)
{
  for (TabItem* item : items)
  {
    item->setSelected(false);
  }

  tab->setSelected(true);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::setBackground(const QString& color)
{
  background = color;

  setLook();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::setLook()
{
  itemWidth = defaultItemWidth;

  // Resize the items
  while (itemWidth > minimumItemWidth && (float)geometry().width() / (items.size() * (itemWidth + itemGap)) < 1)
  {
    itemWidth -= 1;
  }

  // Display the items
  for (int i = 0; i < items.size(); ++i)
  {
    TabItem* button = items[i];

    int x = i * (itemWidth + itemGap);

    // Set the item geometry
    if (x + itemWidth < geometry().width())
    {
      button->setGeometry(x, 0, itemWidth, itemHeight);
    }
    // The item doesn't have enough space, don't display it and get the fuck out for the rest of the items
    else
    {
      button->setGeometry(0, 0, 0, 0);
      break;
    }
  }

  if (items.isEmpty())
  {
    bottomWidget->setVisible(false);
    setStyleSheet(styleSheetEmpty);
  }
  else
  {
    bottomWidget->setVisible(true);
    setStyleSheet(styleSheet.arg(background));

    int bottomWidgetHeight = geometry().height() - itemHeight + 1;
    bottomWidget->setGeometry(0, geometry().height() - bottomWidgetHeight, geometry().width(), bottomWidgetHeight);
  }
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
 void TabBar::resizeEvent(QResizeEvent* event)
 {
   setLook();
 }


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::finishTransition()
{
   QPropertyAnimation* animation = qobject_cast<QPropertyAnimation*>(sender());

   // Remove the transition item and delete the animation object
   if (animation)
   {
     TabItem* item = qobject_cast<TabItem*>(animation->targetObject());
     transitionItems.removeAt(transitionItems.indexOf(item));
     animation->deleteLater();
   }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::onItemMoved(const QPoint& localPosition, const QPoint& position)
{
  TabItem* item = qobject_cast<TabItem*>(sender());

  const int MOVING_ITEM_MARGIN = 20;    // This is the same value as the margin set in the item's styleSheetMoving

  if (mouseDrag)
  {
    movingItemX = position.x() - movingItemPressX;
    if (movingItemX < 0)
    {
      movingItemX = 0;
    }
    else if (movingItemX > width() - item->width() + MOVING_ITEM_MARGIN * 2)
    {
      movingItemX = width() - item->width() + MOVING_ITEM_MARGIN * 2;
    }

    itemMoved = true;

    item->raise();

    // Add some margins on left and right so the movement animation does not suck ass
    item->setGeometry(movingItemX - MOVING_ITEM_MARGIN, 0, itemWidth + MOVING_ITEM_MARGIN * 2, itemHeight);

    // Check if the cursor is over anothet item and mark it for transition
    for (int i = 0; i < items.size(); ++i)
    {
      QRect checkedGeometry = items.at(i)->geometry();
      checkedGeometry.moveLeft(checkedGeometry.x() + MOVING_ITEM_MARGIN);
      checkedGeometry.setWidth(checkedGeometry.width() - MOVING_ITEM_MARGIN * 2);

      if (items[i] != movingItem && !transitionItems.contains(items[i]) && !items[i]->isFixed() && checkedGeometry.x() < position.x() && checkedGeometry.x() + checkedGeometry.width() > position.x())
      {
        // Set the new transition geometry as the location of the currently transitioned item
        QRect moveToGeometry = transitionGeometry;
        transitionGeometry = items[i]->geometry();

        // Save the transition item and swap items
        transitionItem = items[i];
        transitionItems.append(transitionItem);

        newIndex = i;
        int swapIndex = items.indexOf(movingItem);
        items.swap(i, swapIndex);

        emit tabSwapped(i, swapIndex);

        // Move the transition item to the new location
        QPropertyAnimation* animation = new QPropertyAnimation(transitionItem, "geometry");
        connect(animation, SIGNAL(finished()), this, SLOT(finishTransition()));
        animation->setDuration(animationDuration);
        animation->setStartValue(transitionItem->geometry());
        animation->setEndValue(moveToGeometry);
        animation->start();

        break;
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::onItemPressed(const QPoint& localPosition, const QPoint& position)
{
  TabItem* item = qobject_cast<TabItem*>(sender());

  selectTab(item);

  if (!item->isFixed())
  {
    mouseDrag = true;

    movingItem = item;
    oldIndex = items.indexOf(movingItem);

    movingItemPressX = localPosition.x();
    transitionGeometry = item->geometry();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::onItemReleased(const QPoint& position)
{
  TabItem* item = qobject_cast<TabItem*>(sender());

  if (itemMoved)
  {
    // Remove the left/right margins
    item->setGeometry(movingItemX, 0, itemWidth, itemHeight);

    // Move the selected item to its place
    QPropertyAnimation* animation = new QPropertyAnimation(movingItem, "geometry");
    connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
    animation->setDuration(animationDuration);
    animation->setStartValue(movingItem->geometry());
    animation->setEndValue(transitionGeometry);

    animation->start();

    mouseDrag = false;
    itemMoved = false;

    if (oldIndex != -1 && newIndex != -1 && oldIndex != newIndex)
    {
      emit tabMoved(newIndex, oldIndex);

      oldIndex = -1;
      newIndex = -1;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabBar::onItemSelected()
{
  TabItem* item = qobject_cast<TabItem*>(sender());

  emit currentIndexChanged(items.indexOf(item));
}