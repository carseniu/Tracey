#pragma once

#include <QDialog>

class Application;
class Table;
class TabWidget;

class QCheckBox;


class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(Application* application);

protected:
  void showEvent(QShowEvent* event);

private:
  Application* application;

  TabWidget* contentTabWidget;
  QPushButton* okButton;
  QPushButton* cancelButton;

  QCheckBox* minimizeToTrayCheckBox;
  QCheckBox* closeToTrayCheckBox;
  QCheckBox* deleteTraceCheckBox;
  QCheckBox* deleteFilterCheckBox;
  QCheckBox* deleteSourceHistoryCheckBox;
  QCheckBox* deleteSearchHistoryCheckBox;
  QCheckBox* deleteHighlightingGroupCheckBox;
  QCheckBox* deleteHighlightingRuleCheckBox;
  QCheckBox* exitApplicationCheckBox;

  Table* actionTable;

  QList<QAction*> actionList;

private slots:
  void saveSettings();
  void setShortcut(int row, int column);

};