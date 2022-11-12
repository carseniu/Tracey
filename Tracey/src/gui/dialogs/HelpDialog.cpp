#include "src/gui/dialogs/HelpDialog.h"

#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/widgets/helpbrowser/HelpBrowser.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QHelpSearchEngine>
#include <QHelpSearchResultWidget>
#include <QLineEdit>
#include <QSplitter>
#include <QTabWidget>
#include <QTextDocument>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HelpDialog::HelpDialog()
{
  setMinimumSize(1200, 800);
  setWindowIcon(QIcon(":/Application/img/Icon.png"));
  setWindowTitle(tr("Help"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowMinMaxButtonsHint);

  helpEngine = new QHelpEngine("Tracey.qhc");
  connect(helpEngine->searchEngine(), &QHelpSearchEngine::searchingFinished, this, &HelpDialog::onSearchFinished);
  connect(helpEngine->contentWidget(), &QHelpContentWidget::linkActivated, this, &HelpDialog::onLinkActivated);
  connect(helpEngine->indexWidget(), &QHelpIndexWidget::linkActivated, this, &HelpDialog::onLinkActivated);

  QHelpContentModel* contentModel = qobject_cast<QHelpContentModel*>(helpEngine->contentModel());
  connect(contentModel, &QHelpContentModel::contentsCreated, this, &HelpDialog::onContentsCreated);

  bool ok = helpEngine->setupData();

  if (!ok)
  {
    MessageDialog dialog(this, tr("Could not initialize help. Please check if the help files Tracey.qch and Tracey.qhc are in the application folder."), MessageDialog::Error);
    dialog.exec();
  }
  
  searchLineEdit = new QLineEdit(this);
  connect(searchLineEdit, &QLineEdit::returnPressed, this, &HelpDialog::onSearch);

  resultWidget = new QTextBrowser(this);
  resultWidget->setContextMenuPolicy(Qt::NoContextMenu);
  resultWidget->setOpenLinks(false);
  connect(resultWidget, &QTextBrowser::anchorClicked, this, &HelpDialog::onLinkActivated);

  QWidget* searchWidget = new QWidget(this);
  QVBoxLayout* searchLayout = new QVBoxLayout(searchWidget);
  searchLayout->setContentsMargins(0, 0, 0, 0);
  searchLayout->setSpacing(5);
  searchLayout->addWidget(searchLineEdit);
  searchLayout->addWidget(resultWidget);
 
  QTabWidget* tabWidget = new QTabWidget;
  tabWidget->setMaximumWidth(250);
  tabWidget->addTab(helpEngine->contentWidget(), tr("Contents"));
  tabWidget->addTab(helpEngine->indexWidget(), tr("Index"));
  tabWidget->addTab(searchWidget, tr("Search"));

  HelpBrowser* helpBrowser = new HelpBrowser(helpEngine);
  helpBrowser->setSource(QUrl("qthelp://zelorra.tracey/doc/1.1.what_is_tracey.html"));
  connect(this, &HelpDialog::linkActivated, helpBrowser, &HelpBrowser::setSource);

  QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
  splitter->insertWidget(0, tabWidget);
  splitter->insertWidget(1, helpBrowser);

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(5, 5, 5, 5);
  mainLayout->addWidget(splitter);
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HelpDialog::onContentsCreated()
{
  QHelpContentModel* contentModel = qobject_cast<QHelpContentModel*>(helpEngine->contentModel());

  if (contentModel)
  {
    QModelIndex	index = contentModel->index(0, 0, contentModel->index(0, 0));
    helpEngine->contentWidget()->setCurrentIndex(index);

    helpEngine->searchEngine()->reindexDocumentation();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HelpDialog::onLinkActivated(const QUrl& url)
{
  emit linkActivated(url, QTextDocument::UnknownResource);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HelpDialog::onSearch()
{
  helpEngine->searchEngine()->search(searchLineEdit->text());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HelpDialog::onSearchFinished(int count)
{
  QString htmlFile = QString("<html><body>");

  if (count > 0)
  {
    QVector<QHelpSearchResult> results = helpEngine->searchEngine()->searchResults(1, count);

    for (const QHelpSearchResult& result : results)
    {
      htmlFile += QString(QLatin1String("<div style=\"text-align:left; font-weight:bold\"><a href=\"%1\">%2</a><div style=\"color:green; font-weight:normal; margin:5px; margin-bottom:15px;\">%3</div></div><p></p>")).arg(result.url().toDisplayString()).arg(result.title()).arg(result.snippet());
    }
  }

  htmlFile += QLatin1String("</body></html>");

  resultWidget->setHtml(htmlFile);
}