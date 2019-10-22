#pragma once

#include <QMainWindow>

#include <QSystemTrayIcon>

class ActionManager;
class MarkWidget;
class HelpDialog;
class HighlightingData;
class TraceArea;

class QSplitter;


class Application : public QMainWindow
{
    Q_OBJECT

public:
  Application(QWidget* parent = nullptr);

  ActionManager* getActionManager() const;
  HighlightingData* getHighlightingData();

protected:
  void changeEvent(QEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void hideEvent(QHideEvent* event) override;
  void showEvent(QShowEvent* event) override;

private:
  void init();

  ActionManager* actionManager;
  HighlightingData* highlightingData;
  TraceArea* traceArea;
  QSplitter* splitter;
  HelpDialog* helpDialog;

  MarkWidget* markWidget;

public slots:
  void exitApplication();
  void goToNextMark();
  void goToPreviousMark();
  void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
  void setApplicationVisibility();
  void showAboutWindow();
  void showAboutQtWindow();
  void showApplication();
  void showHelpWindow();

};
