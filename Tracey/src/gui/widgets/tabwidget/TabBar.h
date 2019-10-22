#pragma once

#include <QFrame>

class TabWidget;
class TabItem;

class QSignalMapper;


class TabBar : public QFrame
{
  Q_OBJECT

public:
  TabBar(TabWidget* parent);

  TabItem* addTab(const QString& text, bool fixed = false);
  int getCount() const;
  TabItem* getTab(int index) const;
  int	getTabAt(const QPoint& position) const;
  QString getTabText(int index) const;
  void removeTab(int index);
  void selectTab(TabItem* tab);
  void setBackground(const QString& color);
  void setLook();

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  QList<TabItem*> items;
  QFrame* bottomWidget;

  QString background = "#f8f8f8";

  int minimumItemWidth = 50;
  int defaultItemWidth = 150;
  int itemWidth = 150;
  int itemHeight = 24;
  int itemGap = 1;

  // Move items around
  TabItem* movingItem;
  bool mouseDrag = false;
  bool itemMoved = false;
  int animationDuration = 100;
  int movingItemX = 0;
  int movingItemPressX = 0;
  int oldIndex = -1;
  int newIndex = -1;

  // The item to be replaced and the place it should animate to
  TabItem* transitionItem;
  QList<TabItem*> transitionItems;
  QRect transitionGeometry;

  QString styleSheet;
  QString styleSheetEmpty;

private slots:
  void finishTransition();
  void onItemMoved(const QPoint& localPosition, const QPoint& position);
  void onItemPressed(const QPoint& localPosition, const QPoint& position);
  void onItemReleased(const QPoint& position);
  void onItemSelected();

signals:
  void contextMenuRequested(const QPoint& point);
  void currentIndexChanged(int index);
  void tabMoved(int newIndex, int oldIndex);
  void tabSwapped(int newIndex, int oldIndex);

};