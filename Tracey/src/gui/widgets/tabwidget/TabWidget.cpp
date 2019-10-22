#include "src/gui/widgets/tabwidget/TabWidget.h"

#include "src/gui/widgets/tabwidget/TabBar.h"
#include "src/gui/widgets/tabwidget/TabItem.h"

#include <QDebug>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStackedLayout>
#include <QVBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TabWidget::TabWidget(QWidget* parent) :
  QFrame(parent)
{
  setObjectName("TabWidget");

  setStyleSheet("TabWidget {background:#f8f8f8}");

  tabBar = new TabBar(this);

  contentWidget = new QFrame(this);
  contentWidget->setObjectName("TabWidgetContentWidget");
  contentWidget->setStyleSheet("QFrame#TabWidgetContentWidget {border:1px solid #727a7e; border-top:transparent; background: #f8f8f8; padding:3px}");

  contentLayout = new QStackedLayout(contentWidget);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);  
  mainLayout->setMargin(0);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(tabBar);
  mainLayout->addWidget(contentWidget);

  connect(tabBar, &TabBar::contextMenuRequested, this, &TabWidget::contextMenuRequested, Qt::DirectConnection);
  connect(tabBar, &TabBar::currentIndexChanged, this, &TabWidget::onCurrentIndexChanged, Qt::DirectConnection);
  connect(tabBar, &TabBar::tabSwapped, this, &TabWidget::onTabSwapped, Qt::DirectConnection);
  connect(tabBar, &TabBar::tabMoved, this, &TabWidget::tabMoved, Qt::DirectConnection);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TabWidget::addTab(QWidget* widget, const QString& label, bool fixed)
{
  for (int i = 0; i < contentLayout->count(); ++i)
  {
    QLayoutItem* item = contentLayout->itemAt(i);

    if (item->widget() == widget)
    {
      return -1;
    }
  }

  contentLayout->addWidget(widget);

  TabItem* tab = tabBar->addTab(label, fixed);

  currentIndex = contentLayout->count() - 1;

  return currentIndex;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TabWidget::getCount() const
{
  return tabBar->getCount();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TabWidget::getCurrentIndex() const
{
  return currentIndex;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QWidget* TabWidget::getCurrentWidget() const
{
  return contentLayout->currentWidget();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int	TabWidget::getIndexOf(QWidget* widget) const
{
  return contentLayout->indexOf(widget);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int	TabWidget::getTabAt(const QPoint& position) const
{
  return tabBar->getTabAt(position);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString TabWidget::getTabText(int index) const
{
  return tabBar->getTabText(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QWidget* TabWidget::getWidget(int index) const
{
  return contentLayout->widget(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabWidget::removeTab(int index)
{
  if (index < 0 || index > tabBar->getCount() - 1)
  {
    return;
  }

  contentLayout->removeWidget(contentLayout->widget(index));
  tabBar->removeTab(index);

  int newCurrentIndex = -1;

  if (index == currentIndex)
  {
    newCurrentIndex = index;
    if (newCurrentIndex > tabBar->getCount() - 1)
    {
      newCurrentIndex = tabBar->getCount() - 1;
    }
  }
  else
  {
    if (currentIndex > index)
    {
      newCurrentIndex = currentIndex - 1;
    }
    else
    {
      newCurrentIndex = currentIndex;
    }
  }

  setCurrentIndex(newCurrentIndex);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabWidget::setTabBarBackground(const QString& color)
{
  tabBar->setBackground(color);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabWidget::setTabText(int index, const QString& text)
{
  if (index < 0 || index > tabBar->getCount() - 1)
  {
    return;
  }

  tabBar->getTab(index)->setText(text);
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabWidget::setCurrentIndex(int index)
{
  TabItem* tab = tabBar->getTab(index);

  if (tab != nullptr)
  {
    tabBar->selectTab(tab);

    currentIndex = index;
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabWidget::onCurrentIndexChanged(int index)
{
  if (index != currentIndex)
  {
    contentLayout->setCurrentIndex(index);

    currentIndex = index;

    emit currentIndexChanged(currentIndex);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TabWidget::onTabSwapped(int newIndex, int oldIndex)
{
  QList<QWidget*> widgetList;

  for (int i = 0; i < contentLayout->count(); ++i)
  {
    widgetList.append(contentLayout->widget(i));
  }

  delete contentLayout;
  contentLayout = new QStackedLayout(contentWidget);

  widgetList.swap(oldIndex, newIndex);

  for (int i = 0; i < widgetList.size(); ++i)
  {
    contentLayout->addWidget(widgetList.at(i));
  }

  contentLayout->setCurrentIndex(newIndex);

  currentIndex = newIndex;

  emit tabSwapped(newIndex, oldIndex);
}