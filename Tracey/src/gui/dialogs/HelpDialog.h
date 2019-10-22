#pragma once

#include <QDialog>

class QHelpEngine;
class QLineEdit;
class QTextBrowser;


class HelpDialog : public QDialog
{
  Q_OBJECT

public:
  HelpDialog();

private:
  QHelpEngine* helpEngine;
  QLineEdit* searchLineEdit;
  QTextBrowser* resultWidget;

private slots:
  void onContentsCreated();
  void onSearch();
  void onSearchFinished(int count);

};