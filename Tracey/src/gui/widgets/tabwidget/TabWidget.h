#pragma once

#include <QFrame>

class TabBar;
class TabItem;
class QStackedLayout;


class TabWidget : public QFrame
{
  Q_OBJECT

public:
  TabWidget(QWidget* parent = 0);

  int	addTab(QWidget* widget, const QString& label, bool fixed = false);
  int getCount() const;
  int getCurrentIndex() const;
  QWidget* getCurrentWidget() const;
  int getIndexOf(QWidget* widget) const;
  int	getTabAt(const QPoint& position) const;
  QString getTabText(int index) const;
  QWidget* getWidget(int index) const;
  void removeTab(int index);
  void setTabBarBackground(const QString& color);
  void setTabText(int index, const QString& text);

private:
  TabBar* tabBar;
  QFrame* contentWidget;
  QStackedLayout* contentLayout;

  int currentIndex = -1;

public slots:
  void setCurrentIndex(int index);

private slots:
  void onCurrentIndexChanged(int index);
  void onTabSwapped(int newIndex, int oldIndex);

signals:
  void contextMenuRequested(const QPoint& point);
  void currentIndexChanged(int index);
  void tabMoved(int newIndex, int oldIndex);
  void tabSwapped(int newIndex, int oldIndex);

};