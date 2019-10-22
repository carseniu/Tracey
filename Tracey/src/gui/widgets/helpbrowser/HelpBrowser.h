#pragma once

#include <QTextBrowser>

class QHelpEngine;


class HelpBrowser : public QTextBrowser
{
public:
  HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = nullptr);

  QVariant loadResource(int type, const QUrl& name) override;

private:
  QHelpEngine* helpEngine;
};