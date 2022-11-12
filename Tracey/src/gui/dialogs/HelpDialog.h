#pragma once

#include <QDialog>

#include <QTextDocument>

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
  void onLinkActivated(const QUrl& url);
  void onSearch();
  void onSearchFinished(int count);

signals:
  void linkActivated(const QUrl& name, QTextDocument::ResourceType type);

};